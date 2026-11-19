#include "DRANN.h"
#include <cmath>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <stdexcept>
void check(bool b) {
  if (!b)
    throw std::runtime_error("Comprobación fallida.");
}
void near(double a, double b, double tol = 1e-6) {
  check(std::isfinite(a) && std::abs(a - b) < tol * (1 + std::abs(b)));
}
bool route(const Trace &a, const Trace &b) {
  if (a.steps.size() != b.steps.size())
    return false;
  for (std::size_t i = 0; i < a.steps.size(); i++)
    if (a.steps[i].transmitters != b.steps[i].transmitters ||
        a.steps[i].receivers != b.steps[i].receivers ||
        a.steps[i].connections != b.steps[i].connections)
      return false;
  return true;
}
std::string state(const Model &m) {
  std::ostringstream o;
  m.write(o);
  return o.str();
}
int main() {
  try {
    Config c;
    c.neurons = 3;
    c.outputDimension = 2;
    c.radius = .7;
    Model m(c);
    m.input = {10, 7, 2};
    m.bias = {0, 0, .4};
    m.edges = {{0, 1, 0, false}, {0, 2, .5, true}, {1, 2, .25, true}};
    auto t = m.forward(1);
    check(t.steps.size() == 1 && t.steps[0].transmitters.size() == 2);
    near(t.final[2], 7.75);
    near(t.final[0], 10);
    // Invariancia al renombrar neuronas y comparación por potencial firmado.
    Model permuted = m;
    std::vector<int> permutation = {2, 0, 1};
    for (int i = 0; i < 3; i++) {
      permuted.input[i] = m.input[permutation[i]];
      permuted.bias[i] = m.bias[permutation[i]];
    }
    for (auto &w : permuted.edges) {
      int a = permutation[w.i], b = permutation[w.j];
      for (const auto &old : m.edges)
        if ((old.i == a && old.j == b) || (old.i == b && old.j == a)) {
          w.w = old.w;
          w.active = old.active;
          break;
        }
    }
    auto renamed = permuted.forward(1);
    for (int i = 0; i < 3; i++)
      near(renamed.final[i], t.final[permutation[i]]);
    auto negative = m.forward(-1);
    check(negative.steps[0].transmitters == std::vector<int>{2});
    for (const auto &w : m.edges)
      check(w.i < w.j);
    // Una aislada conserva entrada y aprende; ninguna neurona se excluye.
    m.edges = {{0, 1, .3, true}, {0, 2, 0, false}, {1, 2, 0, false}};
    m.input = {.3, .7, .2};
    m.bias = {.01, -.02, .03};
    m.weightsT = {{.3, .1}, {.1, .5}, {.4, .2}};
    auto tr = m.forward(.6);
    auto g = m.backward(tr, {.2, .8});
    check(std::abs(g.input[2]) > 1e-12);
    check(std::abs(g.biasT[2][0]) > 1e-12);
    int checked = 0;
    for (bool tanh : {false, true}) {
      m.config.tanhActivation = tanh;
      tr = m.forward(.6);
      g = m.backward(tr, {.2, .8});
      for (int group = 0; group < 5; group++) {
        int size = group == 2 ? static_cast<int>(m.edges.size()) : group > 2 ? 6 : 3;
        for (int idx = 0; idx < size; idx++) {
          auto at = [&](Model &n) -> double & {
            if (group == 0)
              return n.input[idx];
            if (group == 1)
              return n.bias[idx];
            if (group == 2)
              return n.edges[idx].w;
            if (group == 3)
              return n.biasT[idx / 2][idx % 2];
            return n.weightsT[idx / 2][idx % 2];
          };
          double expected = group == 0   ? g.input[idx]
                            : group == 1 ? g.bias[idx]
                            : group == 2 ? g.edges[idx]
                            : group == 3 ? g.biasT[idx / 2][idx % 2]
                                         : g.weightsT[idx / 2][idx % 2];
          Model a = m, b = m;
          double h = 1e-6;
          at(a) += h;
          at(b) -= h;
          auto ta = a.forward(.6), tb = b.forward(.6);
          check(route(tr, ta) && route(tr, tb));
          near(expected, (a.loss(ta, {.2, .8}).total - b.loss(tb, {.2, .8}).total) / (2 * h));
          checked++;
        }
      }
    }
    // Sesgo reutilizado en dos recepciones sucesivas.
    m.config.tanhActivation = false;
    m.input = {3, 2, .5};
    m.bias = {.1, -.2, .3};
    m.edges = {{0, 1, .25, true}, {0, 2, 0, false}, {1, 2, .4, true}};
    tr = m.forward(.8);
    check(tr.steps.size() == 2);
    g = m.backward(tr, {.2, .8});
    for (int i = 0; i < 3; i++) {
      Model a = m, b = m;
      a.bias[i] += 1e-6;
      b.bias[i] -= 1e-6;
      near(g.bias[i],
           (a.loss(a.forward(.8), {.2, .8}).total - b.loss(b.forward(.8), {.2, .8}).total) / 2e-6);
    }
    m.edges[0].w = .01;
    g = m.backward(m.forward(.8), {.2, .8});
    g.edges[0] = 100;
    m.parameterUpdate(g);
    check(!m.edges[0].active);
    g.edges[0] = -100;
    m.parameterUpdate(g);
    check(!m.edges[0].active);
    // Lectura por dimensión, signos, ausencia y grupos sin encadenamiento.
    for (int T : {1, 2, 3}) {
      Config x;
      x.neurons = 6;
      x.outputDimension = T;
      Model n(x);
      auto q = n.forward(.2);
      for (int i = 0; i < 6; i++) {
        q.q[i] = Point(T);
        q.biasT[i] = Point(T);
        q.weightsT[i] = Point(T);
        int k = i % T;
        q.q[i][k] = -.3;
        q.biasT[i][k] = -.1;
      }
      auto read = n.interpret(q);
      for (int k = 0; k < T; k++) {
        check(read.components[k].status == "válido");
        near(read.components[k].value, -.3);
      }
      for (auto &v : q.q)
        std::fill(v.begin(), v.end(), 0);
      for (auto &v : q.biasT)
        std::fill(v.begin(), v.end(), 0);
      check(n.interpret(q).components[0].status == "sin representantes");
    }
    Config x;
    x.neurons = 4;
    x.outputDimension = 2;
    Model n(x);
    auto q = n.forward(.2);
    for (int i = 0; i < 4; i++) {
      q.q[i] = {.1 + i * .04, 0};
      q.biasT[i] = {.02, 0};
      q.weightsT[i] = {0, 0};
    }
    check(n.interpret(q).components[0].status == "ambiguo");
    // Dos ejes próximos no se mezclan; pertenencia se calcula en T completo.
    q.q = {{.01, 0}, {0, .01}, {.01, 0}, {0, .01}};
    q.biasT = q.q;
    auto read = n.interpret(q);
    check(read.components[0].members.size() == 2 && read.components[1].members.size() == 2);
    auto before = state(n);
    n.validation({{.1, {.2, .8}}});
    check(state(n) == before);
    auto a = n.forward(.1);
    n.loss(a, {8, 2});
    check(n.forward(.1).q == a.q);
    Config defaults = laboratoryConfig();
    auto data = laboratoryDataset(defaults);
    near(defaults.scaleX, 100);
    near(defaults.scaleY, 373.15);
    check(defaults.radius > 0);
    Laboratory lab(defaults, data);
    for (int i = 0; i < 11; i++)
      lab.advanceExample();
    lab.save("test.model");
    auto loaded = Laboratory::load("test.model");
    check(loaded.e == lab.e && loaded.cursor == lab.cursor &&
          state(loaded.model) == state(lab.model));
    check(loaded.model.forward(.1).q == lab.model.forward(.1).q);
    for (int i = 0; i < 25; i++) {
      lab.advanceExample();
      loaded.advanceExample();
    }
    check(state(loaded.model) == state(lab.model));
    std::remove("test.model");
    // Empates exactos reparten gradientes; pozos coincidentes se señalan.
    Config one;
    one.neurons = 2;
    one.outputDimension = 1;
    Model tied(one);
    auto same = tied.forward(0);
    same.q = {{.2}, {.2}};
    auto l = tied.loss(same, {.7});
    near(l.gradient[0][0], l.gradient[1][0]);
    auto coinc = n.loss(n.forward(.1), {0, 0});
    check(coinc.coincidentWells);
    // Cargar el mejor conserva el contador de trabajo y su propia cinta RNG/orden.
    lab.save("best-test.model", true);
    auto bestLoaded = Laboratory::load("best-test.model");
    check(bestLoaded.e == lab.e && bestLoaded.modelEpoch == lab.bestModelEpoch);
    check(state(bestLoaded.model) == state(lab.bestModel));
    Laboratory expected(defaults, data);
    expected.model = lab.bestModel;
    expected.order = lab.bestOrder;
    for (int j = 0; j < 17; j++) {
      expected.advanceExample();
      bestLoaded.advanceExample();
    }
    check(state(expected.model) == state(bestLoaded.model));
    std::remove("best-test.model");
    Config tieConfig;
    tieConfig.neurons = 2;
    tieConfig.outputDimension = 2;
    Model tieModel(tieConfig);
    auto tieTrace = tieModel.forward(0);
    tieTrace.q = {{0, 0}, {0, 0}};
    auto tiedLoss = tieModel.loss(tieTrace, {-1, 1});
    double coefficient = .5 * std::exp(-1 / (2 * tieConfig.radius * tieConfig.radius)) /
                         (tieConfig.radius * tieConfig.radius);
    near(tiedLoss.gradient[0][0], coefficient);
    near(tiedLoss.gradient[0][1], -coefficient);
    tieTrace.q[0] = {1, 0};
    tieTrace.biasT[0] = {0, 1};
    tieTrace.weightsT[0] = {0, 0};
    check(tieModel.interpret(tieTrace).axis[0] == -1);
    // Tres pozos separados: cada coordenada mantiene su identidad de eje.
    Config threeConfig;
    threeConfig.neurons = 3;
    threeConfig.outputDimension = 3;
    Model three(threeConfig);
    auto idealTrace = three.forward(0);
    idealTrace.q = {{-.2, 0, 0}, {0, .4, 0}, {0, 0, .8}};
    near(three.loss(idealTrace, {-.2, .4, .8}).total, 0);
    Config sparse = threeConfig;
    sparse.neurons = 1;
    check(Model(sparse).forward(0).q.front().size() == 3);
    std::cout << "Pruebas correctas: " << checked
              << " derivadas, lectura T=1/2/3, aislamiento, evaluación y reanudación exacta.\n";
    return 0;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
