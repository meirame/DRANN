#include "../entrenamiento.h"

#include <iostream>
#include <tuple>

using namespace vectorial;

void check(bool condition, const char *message) {
  if (!condition)
    throw std::runtime_error(message);
}

void near(double actual, double expected, double tolerance = 1e-9) {
  if (!std::isfinite(actual) || std::abs(actual - expected) > tolerance)
    throw std::runtime_error("Valor distinto. Esperado: " + std::to_string(expected) +
                            ", obtenido: " + std::to_string(actual));
}

double loss(const Model &model, double x, const Point &y) {
  const auto v = model.coordinateLoss(model.forwardRhizome(x), y);
  return std::accumulate(v.begin(), v.end(), 0.0);
}

void sameModel(const Model &a, const Model &b) {
  check(a.input == b.input && a.bias == b.bias && a.rng == b.rng &&
        a.dimensions == b.dimensions && a.lr == b.lr && a.edges.size() == b.edges.size(),
        "Snapshot incompleto o parámetros modificados.");
  for (std::size_t e = 0; e < a.edges.size(); ++e)
    check(std::tie(a.edges[e].i, a.edges[e].j, a.edges[e].w, a.edges[e].active) ==
          std::tie(b.edges[e].i, b.edges[e].j, b.edges[e].w, b.edges[e].active),
          "Se modificó la identidad, el peso o la máscara de una arista.");
}

void forwardManual() {
  Model m(4, 2, 0.01, 42);
  m.input = {{3, -5}, {0.8, 1}, {-1, 4}, {9, 9}};
  m.bias[1] = {0.2, -0.3};
  m.edges = {{0, 1, 0.2, true}, {1, 2, 0.4, true},
             {0, 2, 0, false}, {0, 3, 0, false}, {1, 3, 0, false}, {2, 3, 0, false}};
  const auto tr = m.forwardRhizome(1);
  check(tr.steps.size() == 1 && tr.participants == 3 && !tr.participating[3], "Ruta incorrecta.");
  const auto &step = tr.steps[0];
  check(step.transmits[0] && step.transmits[2] && !step.transmits[1] &&
        step.receives[1] && step.messages.size() == 2, "Falló la recepción compartida.");
  near(tr.final[0][0], 3);
  near(tr.final[0][1], -0.05);
  near(tr.final[1][0], 1.196);
  near(tr.final[1][1], 1.57);
  near(tr.final[2][0], -0.01);
  near(tr.final[2][1], 4);
  near(m.coordinateLoss(tr, {0, 0})[0], (9 + 1.196 * 1.196 + 0.0001) / 3);
  const auto gradient = m.backpropRhizome(1, tr, {0, 0});
  check(gradient.input[3] == Point({0, 0}) && gradient.bias[3] == Point({0, 0}),
        "La aislada produjo gradientes.");
  check(gradient.input[0][0] != 0 && gradient.input[2][1] != 0,
        "Se excluyó una transmisora retirada.");
  const Model saved = m;
  const auto metrics = evaluate(m, {{1, {0, 0}}, {0.5, {0, 0}}});
  check(metrics.neurons[1].receivedMagnitude > 0 && metrics.neurons[3].weightSum == 0,
        "Diagnóstico por neurona incorrecto.");
  sameModel(m, saved);

  Model tie(2, 2, 0.01, 42);
  tie.input = {{3, 4}, {4, 3}};
  auto equal = tie.forwardRhizome(1);
  check(equal.steps.empty() && equal.terminalTies == 1, "Empate de módulo y direcciones distintas.");
  tie.input[0][0] += 1e-13;
  const auto nearTie = tie.forwardRhizome(1);
  check(nearTie.nearTies == 1 && nearTie.steps.size() == 1 &&
        nearTie.steps[0].transmits[0], "La tolerancia cambió la selección.");
  near(sigma(-2), -0.02);
  near(sigmaDerivative(0), 1);
}

bool sameRoute(const Trace &a, const Trace &b) {
  if (a.participating != b.participating || a.steps.size() != b.steps.size())
    return false;
  for (std::size_t t = 0; t < a.steps.size(); ++t) {
    if (a.steps[t].transmits != b.steps[t].transmits || a.steps[t].receives != b.steps[t].receives)
      return false;
    for (std::size_t i = 0; i < a.participating.size(); ++i)
      if (a.steps[t].receives[i])
        for (std::size_t k = 0; k < a.final[i].size(); ++k)
          if ((a.steps[t].preactivation[i][k] >= 0) != (b.steps[t].preactivation[i][k] >= 0))
            return false;
  }
  for (std::size_t i = 0; i < a.participating.size(); ++i)
    if (a.participating[i])
      for (std::size_t k = 0; k < a.final[i].size(); ++k)
        if ((a.initialPreactivation[i][k] >= 0) != (b.initialPreactivation[i][k] >= 0))
          return false;
  return true;
}

int gradients(Model m, double x, const Point &y) {
  const auto reference = m.forwardRhizome(x);
  const auto expected = m.backpropRhizome(x, reference, y);
  const auto compare = [&](auto access, double analytical) {
    Model plus = m, minus = m;
    const double h = 1e-6;
    access(plus) += h;
    access(minus) -= h;
    check(sameRoute(reference, plus.forwardRhizome(x)) && sameRoute(reference, minus.forwardRhizome(x)) &&
          sameTopology(m, plus) && sameTopology(m, minus), "Perturbación fuera del recorrido.");
    near(analytical, (loss(plus, x, y) - loss(minus, x, y)) / (2 * h), 3e-8);
  };
  int tested = 0;
  for (std::size_t i = 0; i < m.input.size(); ++i)
    for (std::size_t k = 0; k < m.dimensions; ++k) {
      compare([=](Model &p) -> double & { return p.input[i][k]; }, expected.input[i][k]);
      compare([=](Model &p) -> double & { return p.bias[i][k]; }, expected.bias[i][k]);
      tested += 2;
    }
  for (std::size_t e = 0; e < m.edges.size(); ++e) {
    compare([=](Model &p) -> double & { return p.edges[e].w; }, expected.edges[e]);
    ++tested;
  }
  return tested;
}

void backward() {
  Model chain(3, 2, 0.01, 42);
  chain.input = {{3, -2}, {2, -0.7}, {0.5, -0.9}};
  chain.bias = {{0.1, -0.2}, {-0.1, -0.05}, {0.25, -0.17}};
  chain.edges = {{0, 1, 0.25, true}, {0, 2, 0, false}, {1, 2, 0.3, true}};
  check(chain.forwardRhizome(1).steps.size() == 2, "La prueba necesita dos pasos.");
  int tested = gradients(chain, 1, {1.2, -0.9});
  tested += gradients(chain, -0.8, {-0.4, 0.8});
  Model shared(4, 2, 0.02, 42);
  shared.input = {{3, -5}, {0.8, 1}, {-1, 4}, {9, 9}};
  shared.bias = {{0.1, 0.1}, {0.2, -0.3}, {-0.1, -0.1}, {0.1, 0.1}};
  shared.edges = {{0, 1, 0.2, true}, {1, 2, 0.4, true}, {0, 2, 0, false},
                  {0, 3, 0, false}, {1, 3, 0, false}, {2, 3, 0, false}};
  tested += gradients(shared, 0.8, {1.1, -0.3});
  std::cout << tested << " derivadas comprobadas por diferencias finitas.\n";
}

void proximal() {
  const ProxOptions options;
  for (double v : {0.01, 0.1, 0.8}) {
    const auto z = groupProx({v}, {{0}, {0}}, 0.05, options);
    near(z.weights[0], std::max(v - 0.1, 0.0));
    check(z.gap <= 1e-10 && z.sweeps > 0, "Brecha incorrecta en una sola arista.");
  }
  const auto triangle = groupProx({1, 0.01, 0.01}, {{0, 1}, {0, 2}, {1, 2}}, 0.05, options);
  near(triangle.weights[0], 0.9, 1e-8);
  check(triangle.weights[1] == 0 && triangle.weights[2] == 0 && triangle.gap <= 1e-10,
        "El proximal del triángulo no aisló la tercera neurona.");
  Model m(3, 2, 0.1, 42);
  m.edges = {{0, 1, 1, true}, {0, 2, 0.01, true}, {1, 2, 0.01, true}};
  Gradients zero{Points(3, Point(2)), Points(3, Point(2)), Point(3)};
  ProxOptions fail = options;
  fail.maxSweeps = 1;
  bool rejected = false;
  try {
    m.parameterUpdate(zero, 0.5, fail);
  } catch (const std::runtime_error &) {
    rejected = true;
  }
  check(rejected && m.activeConnections() == 3 && m.edges[1].w == 0.01,
        "Un solver no convergente modificó las máscaras.");
  m.parameterUpdate(zero, 0.5, options);
  check(m.activeConnections() == 1 && m.participation() == std::vector<bool>({true, true, false}),
        "Trasladar el proximal al modelo debe anular las aristas finales.");
  zero.edges[1] = -100;
  m.parameterUpdate(zero, 0, options);
  check(!m.edges[1].active && m.edges[1].w == 0, "Se reintegró una arista.");
  zero.edges[0] = 100;
  m.parameterUpdate(zero, 0.5, options);
  check(!m.edges[0].active, "El proximal abrió una arista anulada por SGD.");
}

Config immediatePressure() {
  Config c;
  c.lambdaTarget = 0.5;
  c.warmup = c.patience = c.maxRecovery = 0;
  c.ramp = c.maxPressure = 1;
  c.absoluteTolerance = {0, 0};
  c.relativeTolerance = 0;
  return c;
}

void attempts() {
  Model fixture(3, 2, 0.1, 42);
  fixture.input = fixture.bias = Points(3, Point(2));
  fixture.edges = {{0, 1, 1, true}, {0, 2, 0.01, true}, {1, 2, 0.01, true}};
  const Dataset zero{{1, {0, 0}}};
  const Dataset all = {{1, {0, 0}}, {0.5, {0, 0}}};
  Config c = immediatePressure();
  c.maxRecovery = 2;
  Training accepts(fixture, c, all, zero);
  const Epoch pressure = accepts.advance(3);
  check(pressure.phase == Phase::pressure && pressure.processed == 1 &&
        pressure.newlyIsolated == std::vector<std::size_t>({2}) &&
        accepts.phase == Phase::recovery && accepts.reductionStarts.size() == 1,
        "La presión no se detuvo en el candidato 0.");
  check(accepts.advance(3).phase == Phase::recovery, "Falta una época de recuperación.");
  const auto accepted = accepts.advance(3);
  check(accepted.phase == Phase::accepted && accepts.current.neurons() == 2 &&
        accepts.modelEpoch == 1 && accepts.work == 3,
        "No se aceptó el mejor candidato reducido, incluido el candidato 0.");
  sameModel(accepts.model, accepts.reductionStarts.front().second.model);

  Model regression = fixture;
  regression.lr = 0.01;
  regression.bias = {{2, 2}, {2, 2}, {0, 0}};
  c.lambdaTarget = 5;
  Training rejects(regression, c, zero, zero);
  rejects.advance(3);
  rejects.advance(3);
  const Epoch rejected = rejects.advance(3);
  check(rejected.phase == Phase::restoration && rejected.trial &&
        rejected.trial->neurons() == 2 && rejects.current.neurons() == 3 &&
        rejects.reductionStarts.size() == 1 && rejects.work == 3 && rejects.modelEpoch == 0,
        "No se restauró el estado tras perder precisión.");
  sameModel(regression, rejects.model);
  check(rejects.advance(4).phase == Phase::training && rejects.references.size() == 1,
        "El mismo estado provocó un reintento inmediato.");

  c = immediatePressure();
  c.lambdaTarget = 1e-4;
  Training noReduction(fixture, c, zero, zero);
  const auto no = noReduction.advance(1);
  check(no.phase == Phase::restoration && no.decision.find("sin_reduccion") != std::string::npos,
        "Reducir pesos sin aislar no debe aceptarse.");
  sameModel(fixture, noReduction.model);
  c.lambdaTarget = 50;
  Training invalid(fixture, c, zero, zero);
  const auto bad = invalid.advance(1);
  check(bad.phase == Phase::restoration && bad.newlyIsolated.size() == 3 &&
        bad.decision.find("intento_invalido") != std::string::npos, "Aislamiento total no invalidado.");
  sameModel(fixture, invalid.model);

  c.lambdaTarget = 0;
  c.prox.zeroTolerance = 10;
  Training control(fixture, c, zero, zero);
  check(control.advance(1).sweeps == 0 && control.references.empty(),
        "El control lambda=0 entró al solver.");
  sameModel(fixture, control.model);

  const Dataset drift{{1, {0.25, 0.4}}, {0.4, {0.1, 0.1}}};
  Training whole(fixture, c, drift, drift), split(fixture, c, drift, drift);
  for (int e = 0; e < 4; ++e)
    whole.advance(4);
  for (int goal : {2, 4})
    while (split.work < goal)
      split.advance(goal);
  sameModel(whole.model, split.model);
}

int main() {
  try {
    forwardManual();
    backward();
    proximal();
    attempts();
    std::cout << "Forward, proximal, control, aceptación y restauración comprobados.\n";
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
