#define DRANN_NO_MAIN
#include "../DRANN.cpp"

#include <algorithm>
#include <cstdio>
#include <iterator>
#include <sstream>

void require(bool condition, const std::string &message) {
  if (!condition)
    throw std::runtime_error(message);
}

void near(double actual, double expected, const std::string &message,
          double tolerance = 1e-10) {
  require(std::isfinite(actual) &&
              std::abs(actual - expected) <= tolerance * (1.0 + std::abs(expected)),
          message + ": obtenido " + std::to_string(actual) +
              ", esperado " + std::to_string(expected));
}

drannConnection &connection(DRANN &network, std::size_t i, std::size_t j) {
  for (drannConnection &edge : network.weights) {
    if ((edge.i == i && edge.j == j) || (edge.i == j && edge.j == i))
      return edge;
  }
  throw std::runtime_error("La pareja no existe.");
}

void connect(DRANN &network, std::size_t i, std::size_t j, double w) {
  drannConnection &edge = connection(network, i, j);
  edge.w = w;
  edge.active = true;
}

void checkRoles(const DRANN &network) {
  std::vector<bool> retired(network.inputWeights.size(), false);
  for (const drannStep &step : network.stepCache) {
    for (std::size_t i : step.transmitters) {
      require(!retired[i], "Una neurona transmitió dos veces.");
      require(!step.receivers[i], "Una neurona recibió y transmitió en el mismo paso.");
      retired[i] = true;
    }
    for (const drannTransmission &transmission : step.transmissions) {
      require(!retired[transmission.receiver], "Una neurona retirada recibió señal.");
      require(step.potential[transmission.transmitter] >
                  step.potential[transmission.receiver],
              "La transmisión no respeta la diferencia positiva.");
    }
  }
}

void testSequentialPropagation() {
  DRANN network(3, 0.005);
  network.inputWeights = {10.0, 6.0, 2.0};
  connect(network, 0, 1, 0.5);
  connect(network, 0, 2, 0.5);
  connect(network, 1, 2, 0.5);
  network.forwardRhizome(1.0);
  require(network.stepCache.size() == 2, "El recorrido debe tener dos pasos.");
  require(network.stepCache[0].transmitters == std::vector<std::size_t>{0} &&
              network.stepCache[1].transmitters == std::vector<std::size_t>{1},
          "Orden incorrecto de las transmisoras.");
  near(network.sigmaCache[0], 10.0, "La primera transmisora se conserva");
  near(network.sigmaCache[1], 8.0, "La segunda transmisora se conserva");
  near(network.sigmaCache[2], 7.0, "La última receptora acumula la propagación");
  near(network.loss(2.0), 125.0 / 3.0, "Las retiradas siguen en la pérdida");
  checkRoles(network);
  network.forwardRhizome(1.0);
  near(network.sigmaCache[2], 7.0, "La disponibilidad se reinicia por ejemplo");
  require(network.stepCache.size() == 2, "La traza no acumula ejemplos anteriores.");

  network.forwardRhizome(0.0);
  require(network.stepCache.empty(), "Con potenciales iguales no hay transmisiones.");
  near(network.loss(0.32), 0.32 * 0.32, "Un paso vacío no elimina participantes");
  drannGradients gradients = network.backpropRhizome(0.32);
  for (double gradient : gradients.weights)
    near(gradient, 0.0, "No hubo transmisiones que derivasen pesos internos");
  for (double gradient : gradients.bias)
    near(gradient, -0.64 / 3.0, "Los sesgos aún pueden aprender desde la entrada");
}

void testSimultaneousLocalMaxima() {
  DRANN network(4, 0.005);
  network.inputWeights = {10.0, 7.0, 1.6, 100.0};
  network.bias = {0.0, 0.0, 0.4, 100.0};
  connect(network, 0, 2, 0.5);
  connect(network, 1, 2, 0.25);
  network.forwardRhizome(1.0);
  require(network.stepCache.size() == 1, "Ambos máximos locales transmiten juntos.");
  require(network.stepCache[0].transmitters == std::vector<std::size_t>({0, 1}),
          "No debe imponerse el máximo global.");
  near(network.sigmaCache[2], 7.65, "Recepción simultánea y sesgo una sola vez");
  near(network.sigmaCache[0], 10.0, "No se descuenta señal de la emisora");
  near(network.sigmaCache[1], 7.0, "No se reparte la señal como caudal");
  require(!network.participating[3] && network.participantCount() == 3,
          "La neurona aislada no interviene.");
  near(network.loss(0.0), (100.0 + 49.0 + 7.65 * 7.65) / 3.0,
       "La pérdida no incluye el potencial de una neurona aislada");
  drannGradients gradients = network.backpropRhizome(0.0);
  near(gradients.inputWeights[3], 0.0, "La entrada aislada no tiene gradiente");
  near(gradients.bias[3], 0.0, "El sesgo aislado no tiene gradiente");
  checkRoles(network);

  DRANN tied(3, 0.005);
  tied.inputWeights = {10.0, 10.0, 2.0};
  connect(tied, 0, 1, 0.4);
  connect(tied, 0, 2, 0.25);
  connect(tied, 1, 2, 0.75);
  tied.forwardRhizome(1.0);
  require(tied.stepCache.size() == 1 && tied.stepCache[0].transmitters.size() == 2,
          "Los empates no se resuelven por identificador o peso.");
  near(tied.sigmaCache[2], 10.0, "Los pesos distintos se suman en el mismo paso");
  near(tied.loss(10.0), 0.0, "Los puntos coincidentes cuentan como participantes");
  checkRoles(tied);
}

void testWaitingAndOvertaking() {
  DRANN chain(3, 0.005);
  chain.inputWeights = {10.0, 6.7, 1.6};
  chain.bias = {0.0, 0.3, 0.4};
  connect(chain, 0, 1, 0.5);
  connect(chain, 1, 2, 0.5);
  chain.forwardRhizome(1.0);
  require(chain.stepCache.size() == 2, "La cadena necesita dos pasos.");
  near(chain.stepCache[1].potential[2], 2.0,
       "Una neurona sin recepción conserva el estado y no suma sesgo");
  near(chain.sigmaCache[1], 8.8, "Primera recepción de la cadena");
  near(chain.sigmaCache[2], 5.8, "La receptora puede transmitir en el siguiente paso");
  checkRoles(chain);

  DRANN changed(3, 0.005);
  changed.inputWeights = {10.0, 7.0, -1.0};
  changed.bias = {0.0, 0.0, 3.0};
  connect(changed, 0, 1, 0.1);
  connect(changed, 0, 2, 0.9);
  connect(changed, 1, 2, 0.3);
  changed.forwardRhizome(1.0);
  require(changed.stepCache[1].transmitters == std::vector<std::size_t>{2},
          "Hay que recalcular el orden tras superar otro potencial.");
  near(changed.sigmaCache[2], 12.2, "La nueva transmisora conserva su potencial");
  near(changed.sigmaCache[1], 8.77, "Transmisión tras la inversión de potenciales");
  checkRoles(changed);
}

void testIrreversiblePruning() {
  DRANN network(3, 0.005);
  network.inputWeights = {0.05, 2.0, 3.0};
  connect(network, 0, 1, 0.1);
  connect(network, 0, 2, 0.4);
  connect(network, 1, 2, 0.2);
  require(&connection(network, 0, 1) == &connection(network, 1, 0),
          "Ambas direcciones deben compartir un único parámetro.");
  drannGradients gradients{{20.0, 0.0, 0.0}, {20.0, 0.0, 80.0},
                          {0.0, 0.0, 0.0}};
  network.parameterUpdate(gradients);
  require(!connection(network, 0, 1).active &&
              !connection(network, 1, 2).active && network.activeConnections() == 1,
          "Los pesos cero y negativos deben anularse.");
  double negativeWeight = connection(network, 1, 2).w;
  gradients.weights = {-1000.0, 0.0, -1000.0};
  gradients.inputWeights[0] = -20.0;
  network.parameterUpdate(gradients);
  near(connection(network, 0, 1).w, 0.0, "La conexión cero no se reintegra");
  near(connection(network, 1, 2).w, negativeWeight,
       "La conexión negativa no recibe actualizaciones posteriores");
  near(network.inputWeights[0], 0.05,
       "La anulación interna no se extiende a los pesos de entrada");
  network.forwardRhizome(1.0);
  require(!network.participating[1] && network.participantCount() == 2,
          "Aislamiento estructural después de la anulación.");

  DRANN empty(2, 0.005);
  bool rejected = false;
  try {
    empty.forwardRhizome(1.0);
  } catch (const std::runtime_error &) {
    rejected = true;
  }
  require(rejected, "El aislamiento total no puede presentarse como un acierto.");
}

bool sameRoute(const DRANN &a, const DRANN &b) {
  if (a.participating != b.participating || a.stepCache.size() != b.stepCache.size())
    return false;
  for (std::size_t tau = 0; tau < a.stepCache.size(); tau++) {
    const drannStep &sa = a.stepCache[tau], &sb = b.stepCache[tau];
    if (sa.transmitters != sb.transmitters || sa.receivers != sb.receivers ||
        sa.transmissions.size() != sb.transmissions.size())
      return false;
    for (std::size_t k = 0; k < sa.transmissions.size(); k++) {
      const drannTransmission &ta = sa.transmissions[k], &tb = sb.transmissions[k];
      if (ta.connection != tb.connection || ta.transmitter != tb.transmitter ||
          ta.receiver != tb.receiver)
        return false;
    }
  }
  return true;
}

double &parameter(DRANN &network, int group, std::size_t index) {
  if (group == 0)
    return network.inputWeights[index];
  if (group == 1)
    return network.bias[index];
  return network.weights[index].w;
}

std::size_t checkGradients(DRANN network, double x, double y) {
  network.forwardRhizome(x);
  drannGradients analytical = network.backpropRhizome(y);
  std::size_t checked = 0;
  for (int group = 0; group < 3; group++) {
    const std::vector<double> &expected = group == 0   ? analytical.inputWeights
                                          : group == 1 ? analytical.bias
                                                       : analytical.weights;
    for (std::size_t k = 0; k < expected.size(); k++) {
      DRANN plus = network, minus = network;
      double h = 1e-6 * std::max(1.0, std::abs(parameter(network, group, k)));
      parameter(plus, group, k) += h;
      parameter(minus, group, k) -= h;
      plus.forwardRhizome(x);
      minus.forwardRhizome(x);
      require(sameRoute(network, plus) && sameRoute(network, minus),
              "La diferencia finita cruzó una decisión discreta.");
      double numerical = (plus.loss(y) - minus.loss(y)) / (2.0 * h);
      near(expected[k], numerical,
           "Gradiente grupo " + std::to_string(group) + ", índice " +
               std::to_string(k),
           2e-7);
      checked++;
    }
  }
  return checked;
}

void testBackwardAndEvaluation() {
  DRANN network(5, 0.005);
  network.inputWeights = {3.0 / 0.7, 2.7 / 0.7, 0.95 / 0.7, 0.22 / 0.7, 100.0};
  network.bias = {0.0, 0.0, 0.05, -0.02, 10.0};
  connect(network, 0, 2, 0.2);
  connect(network, 0, 3, 0.15);
  connect(network, 1, 2, 0.35);
  connect(network, 1, 3, 0.25);
  connect(network, 2, 3, 0.1);
  std::size_t checked = checkGradients(network, 0.7, 1.8);
  checked += checkGradients(network, -0.3, -0.2);

  DRANN chain(3, 0.005);
  chain.inputWeights = {3.0, 2.0, 0.5};
  chain.bias = {0.1, -0.2, 0.3};
  connect(chain, 0, 1, 0.25);
  connect(chain, 1, 2, 0.4);
  checked += checkGradients(chain, 0.8, 1.2);

  DRANN before = network;
  drannMetrics metrics = network.evaluate({0.7, -0.3}, {1.8, -0.2});
  require(std::isfinite(metrics.loss), "La evaluación debe dar una pérdida finita.");
  require(network.inputWeights == before.inputWeights && network.bias == before.bias,
          "La evaluación modificó parámetros de entrada.");
  for (std::size_t k = 0; k < network.weights.size(); k++) {
    require(network.weights[k].w == before.weights[k].w &&
                network.weights[k].active == before.weights[k].active,
            "La evaluación modificó conexiones o anulaciones.");
  }

  DRANN initialized(5, 0.005), repeated(5, 0.005);
  initialized.initWeights(42);
  repeated.initWeights(42);
  require(initialized.inputWeights == repeated.inputWeights,
          "La semilla no reproduce la inicialización.");
  require(initialized.weights.size() == 10, "Cinco neuronas tienen diez parejas.");
  for (std::size_t k = 0; k < initialized.weights.size(); k++) {
    require(initialized.weights[k].w > 0.0 && initialized.weights[k].w < 1.0 &&
                initialized.weights[k].w == repeated.weights[k].w,
            "Inicialización interna fuera de (0,1) o no reproducible.");
  }
  for (std::size_t i = 0; i < initialized.inputWeights.size(); i++) {
    require(initialized.inputWeights[i] > 0.0 && initialized.inputWeights[i] < 1.0,
            "Inicialización de entrada fuera de (0,1).");
    near(initialized.bias[i], 0.0, "Los sesgos empiezan a cero");
  }
  std::cout << "Backward: " << checked << " derivadas comprobadas por diferencias finitas.\n";
}

void testTrainingNotifications() {
  std::vector<double> X = {-0.4, 0.0, 0.4, 0.8};
  std::vector<double> Y = {-0.4, 0.32, 1.04, 1.76};
  std::vector<double> Xvalidation = {0.1, 0.7}, Yvalidation = {0.5, 1.58};
  DRANN plain(5, 0.005), notified(5, 0.005), cancelled(5, 0.005), shortRun(5, 0.005);
  plain.initWeights(42);
  notified.initWeights(42);
  cancelled.initWeights(42);
  shortRun.initWeights(42);

  // Redirigir solo los mensajes de este experimento; restaurar incluso al fallar.
  struct SilentOutput {
    std::ostringstream log;
    std::streambuf *original;
    SilentOutput() : original(std::cout.rdbuf(log.rdbuf())) {}
    ~SilentOutput() { std::cout.rdbuf(original); }
  } silent;

  const std::string plainPath = "drann_test_plain.csv";
  const std::string notifiedPath = "drann_test_notified.csv";
  const std::string cancelledPath = "drann_test_cancelled.csv";
  const std::string shortPath = "drann_test_short.csv";
  int plainEpochs = plain.train(X, Y, Xvalidation, Yvalidation, 8, plainPath);
  std::vector<drannEpoch> records;
  int notifiedEpochs = notified.train(
      X, Y, Xvalidation, Yvalidation, 8, notifiedPath,
      [&](const drannEpoch &epoch) {
        require(epoch.e == static_cast<int>(records.size()),
                "Notificaciones duplicadas o fuera de orden.");
        require(std::isfinite(epoch.training.loss) && std::isfinite(epoch.validation.loss),
                "La interfaz debe recibir métricas finitas.");
        records.push_back(epoch);
        return true;
      });
  require(plainEpochs == 8 && notifiedEpochs == 8 && records.size() == 9,
          "Se debe notificar el estado inicial y cada época completada.");
  auto readFile = [](const std::string &path) {
    std::ifstream file(path);
    return std::string(std::istreambuf_iterator<char>(file),
                       std::istreambuf_iterator<char>());
  };
  require(readFile(plainPath) == readFile(notifiedPath),
          "Notificar a la interfaz cambió el historial numérico.");

  int completed = cancelled.train(
      X, Y, Xvalidation, Yvalidation, 8, cancelledPath,
      [](const drannEpoch &epoch) { return epoch.e < 3; });
  shortRun.train(X, Y, Xvalidation, Yvalidation, 3, shortPath);
  require(completed == 3 && readFile(cancelledPath) == readFile(shortPath),
          "Cerrar la interfaz debe detener las épocas en el punto notificado.");
  require(cancelled.inputWeights == shortRun.inputWeights &&
              cancelled.bias == shortRun.bias,
          "Se entrenaron ejemplos después de cancelar.");
  for (std::size_t k = 0; k < cancelled.weights.size(); k++) {
    require(cancelled.weights[k].w == shortRun.weights[k].w &&
                cancelled.weights[k].active == shortRun.weights[k].active,
            "La cancelación alteró las conexiones.");
  }
  std::remove(plainPath.c_str());
  std::remove(notifiedPath.c_str());
  std::remove(cancelledPath.c_str());
  std::remove(shortPath.c_str());
}

int main() {
  try {
    testSequentialPropagation();
    testSimultaneousLocalMaxima();
    testWaitingAndOvertaking();
    testIrreversiblePruning();
    testBackwardAndEvaluation();
    testTrainingNotifications();
    std::cout << "Todas las comprobaciones de DRANN han pasado.\n";
    return 0;
  } catch (const std::exception &error) {
    std::cerr << "DRANN tests: " << error.what() << '\n';
    return 1;
  }
}
