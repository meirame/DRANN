#include <algorithm> // fijar un mínimo en los intervalos de salida
#include <chrono>    // tiempos de entrenamiento y estimación de lo que queda
#include <cstdlib>   // TERM y NO_COLOR
#include <cmath>     // comprobación de valores finitos
#include <cstddef>   // índices de los vectores
#include <fstream>   // historial del entrenamiento
#include <functional> // notificar las evaluaciones sin depender de la interfaz
#include <iomanip>   // precisión de la salida
#include <iostream>  // salida por terminal
#include <limits>    // límite del contador de épocas
#include <optional>  // época desde la que se reanuda el entrenamiento
#include <random>    // inicialización de los pesos
#include <sstream>   // formar los logs sin perder precisión
#include <stdexcept> // errores sin inventar una recuperación del modelo
#include <string>    // nombre del archivo de resultados
#include <vector>    // parámetros, estados y cachés

#if defined(__unix__) || defined(__APPLE__)
#include <sys/ioctl.h> // dimensiones de la terminal para fijar la barra abajo
#include <unistd.h>    // comprobar si la entrada y la salida usan una terminal
#elif defined(_WIN32)
#include <cstdio>      // stdin
#include <io.h>        // detectar la entrada interactiva en Windows
#endif

/* Filosofía
El rizoma no tiene capas ni una neurona de salida. Cada neurona participante
aporta su último potencial al cognifenómeno primitivo, aunque ya haya transmitido.
Los papeles se deciden simultáneamente al comienzo de cada paso tau.
El backward deriva el recorrido realizado; no deriva las decisiones discretas.
*/

// Presentación por terminal inspirada en satellite_CNN_HL5.cpp.
// Los colores y el movimiento del cursor solo se usan en terminales compatibles.
struct drannConsole {
  using Clock = std::chrono::steady_clock;
  std::ostream &out;
  bool terminal = false, colors = false;

  explicit drannConsole(std::ostream &output = std::cout) : out(output) {
#if defined(__unix__) || defined(__APPLE__)
    terminal = (&out == &std::cout && isatty(STDOUT_FILENO)) ||
               (&out == &std::cerr && isatty(STDERR_FILENO));
#elif defined(_WIN32)
    terminal = (&out == &std::cout && _isatty(_fileno(stdout))) ||
               (&out == &std::cerr && _isatty(_fileno(stderr)));
#endif
    const char *term = std::getenv("TERM");
    terminal = terminal && (!term || std::string(term) != "dumb");
    colors = terminal && !std::getenv("NO_COLOR");
  }

  const char *color(int value) const {
    if (!colors)
      return "";
    switch (value) {
      case 4: return "\033[31m";  // Rojo: errores.
      case 6: return "\033[33m";  // Amarillo: esperas y avisos.
      case 8: return "\033[90m";  // Gris: detalles y tiempos.
      case 10: return "\033[32m"; // Verde: progreso y éxitos.
      case 15: return "\033[37m"; // Blanco: cabeceras y resultados.
      default: return "\033[0m";
    }
  }

  void line(int value, const std::string &message) const {
    out << color(value) << message << color(0) << '\n';
  }

  void prompt(int value, const std::string &message) const {
    out << color(value) << message << color(0) << std::flush;
  }

  static std::string duration(Clock::duration elapsed) {
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    std::ostringstream text;
    text << ms / 3600000 << "hrs " << (ms / 60000) % 60 << "min "
         << (ms / 1000) % 60 << "s " << ms % 1000 << "ms";
    return text.str();
  }

  std::string progress(int e, int goal, int first, Clock::duration elapsed,
                       bool stopped = false) const {
    const double ratio = static_cast<double>(e) / goal;
    const int width = 20;
    const int filled = static_cast<int>(ratio * width);
    std::ostringstream text;
    text << color(15) << "Epochs: [";
    for (int i = 0; i < width; i++)
      text << color(i < filled ? 10 : 6) << (i < filled ? '#' : '.');
    text << color(15) << "] " << color(10) << e << '/' << goal << " ("
         << std::fixed << std::setprecision(2) << ratio * 100.0 << "%)";
    if (stopped)
      text << color(6) << " Stopped.";
    else if (e == goal)
      text << " Completed.";
    else if (e > first) {
      // Al reanudar, estimar con las épocas ejecutadas en este tramo.
      const double seconds = std::chrono::duration<double>(elapsed).count();
      const double remaining = std::ceil(seconds * (goal - e) / (e - first));
      text << color(6) << " Remaining: " << std::setprecision(0)
           << std::floor(remaining / 3600.0) << "hrs, "
           << std::floor(std::fmod(remaining, 3600.0) / 60.0) << "min, "
           << std::fmod(remaining, 60.0) << "secs.";
    } else {
      text << color(6) << " Remaining: --";
    }
    text << color(0);
    return text.str();
  }
};

struct drannFunctions {
  /*=======ACTIVATION FUNCTIONS=======*/
  double identity(double z) const { return z; }
};

struct drannConnection {
  std::size_t i, j;
  double w;
  bool active;
};

struct drannTransmission {
  std::size_t connection, transmitter, receiver;
};

struct drannStep {
  std::vector<double> potential;
  std::vector<std::size_t> transmitters;
  std::vector<bool> receivers;
  std::vector<drannTransmission> transmissions;
};

struct drannGradients {
  std::vector<double> inputWeights, weights, bias;
};

struct drannMetrics {
  double loss, meanSteps;
};

struct drannEpoch {
  int e;
  drannMetrics training, validation;
  std::size_t connections, participants;
};

struct DRANN {
  drannFunctions drannFunction;

  std::vector<double> inputWeights, bias;
  // Una sola entrada por pareja: no hay copias dirigidas ni autoconexiones.
  std::vector<drannConnection> weights;

  std::vector<double> sigmaCache;
  std::vector<bool> participating;
  std::vector<drannStep> stepCache;
  double xCache = 0.0;
  double lr;

  DRANN(std::size_t neuronNumber, double learningRate)
      : inputWeights(neuronNumber, 0.0), bias(neuronNumber, 0.0),
        sigmaCache(neuronNumber, 0.0), participating(neuronNumber, false),
        lr(learningRate) {
    if (neuronNumber < 2 || !std::isfinite(lr) || lr <= 0.0)
      throw std::invalid_argument("Se requieren al menos 2 neuronas y lr positivo.");

    for (std::size_t i = 0; i < neuronNumber; i++) {
      for (std::size_t j = i + 1; j < neuronNumber; j++)
        weights.push_back({i, j, 0.0, false});
    }
  }

  void initWeights(unsigned int seed) {
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    auto positiveWeight = [&]() {
      double w;
      do {
        w = dist(gen);
      } while (w <= 0.0 || w >= 1.0);
      return w;
    };

    for (std::size_t i = 0; i < inputWeights.size(); i++) {
      inputWeights[i] = positiveWeight();
      bias[i] = 0.0;
    }
    for (drannConnection &connection : weights) {
      connection.w = positiveWeight();
      connection.active = true;
    }
    stepCache.clear();
    participating.assign(inputWeights.size(), false);
  }

  std::size_t participantCount() const {
    std::size_t count = 0;
    for (bool participant : participating)
      count += participant;
    return count;
  }

  std::size_t activeConnections() const {
    std::size_t count = 0;
    for (const drannConnection &connection : weights)
      count += connection.active && connection.w > 0.0;
    return count;
  }

  /*=======FORWARD=======*/
  void forwardRhizome(double x) {
    if (!std::isfinite(x))
      throw std::invalid_argument("La entrada debe ser finita.");

    xCache = x;
    stepCache.clear();
    participating.assign(inputWeights.size(), false);
    for (const drannConnection &connection : weights) {
      if (connection.active && connection.w > 0.0) {
        participating[connection.i] = true;
        participating[connection.j] = true;
      }
    }
    // No se define una pérdida cero ni una reintegración para el aislamiento total.
    if (participantCount() == 0)
      throw std::runtime_error("Aislamiento total: caso fuera del alcance de v1.");

    std::vector<bool> available = participating;
    for (std::size_t i = 0; i < inputWeights.size(); i++) {
      sigmaCache[i] = participating[i]
                          ? drannFunction.identity(inputWeights[i] * x + bias[i])
                          : 0.0;
      if (!std::isfinite(sigmaCache[i]))
        throw std::runtime_error("Potencial inicial no finito.");
    }

    while (true) {
      std::vector<bool> higher(inputWeights.size(), false);
      std::vector<bool> lower(inputWeights.size(), false);
      for (const drannConnection &connection : weights) {
        std::size_t i = connection.i, j = connection.j;
        if (!connection.active || connection.w <= 0.0 || !available[i] ||
            !available[j])
          continue;
        if (sigmaCache[i] > sigmaCache[j]) {
          lower[i] = true;
          higher[j] = true;
        } else if (sigmaCache[j] > sigmaCache[i]) {
          lower[j] = true;
          higher[i] = true;
        }
      }

      drannStep step;
      step.potential = sigmaCache;
      step.receivers.assign(inputWeights.size(), false);
      std::vector<bool> transmitter(inputWeights.size(), false);
      for (std::size_t i = 0; i < inputWeights.size(); i++) {
        if (available[i] && !higher[i] && lower[i]) {
          transmitter[i] = true;
          step.transmitters.push_back(i);
        }
      }
      if (step.transmitters.empty())
        break;

      // Todas las contribuciones usan el mismo estado anterior del paso.
      std::vector<double> signal(inputWeights.size(), 0.0);
      for (std::size_t k = 0; k < weights.size(); k++) {
        const drannConnection &connection = weights[k];
        std::size_t i = connection.i, j = connection.j;
        if (!connection.active || connection.w <= 0.0 || !available[i] ||
            !available[j])
          continue;
        if (transmitter[j]) {
          i = connection.j;
          j = connection.i;
        }
        if (transmitter[i] && step.potential[i] > step.potential[j]) {
          double deltaV = step.potential[i] - step.potential[j];
          signal[j] += connection.w * deltaV;
          step.receivers[j] = true;
          step.transmissions.push_back({k, i, j});
        }
      }

      for (std::size_t j = 0; j < inputWeights.size(); j++) {
        if (step.receivers[j]) {
          sigmaCache[j] = drannFunction.identity(step.potential[j] + signal[j] +
                                                bias[j]);
          if (!std::isfinite(sigmaCache[j]))
            throw std::runtime_error("Potencial no finito durante el forward.");
        }
      }
      for (std::size_t i : step.transmitters)
        available[i] = false;
      stepCache.push_back(step);
    }
  }

  /*=======LOSS FUNCTION=======*/
  double loss(double y) const {
    std::size_t count = participantCount();
    if (count == 0 || !std::isfinite(y))
      throw std::invalid_argument("La pérdida requiere participantes y un ideal finito.");
    double L = 0.0;
    for (std::size_t j = 0; j < sigmaCache.size(); j++) {
      if (participating[j]) {
        double difference = sigmaCache[j] - y;
        L += difference * difference;
      }
    }
    L /= static_cast<double>(count);
    if (!std::isfinite(L))
      throw std::runtime_error("Pérdida no finita.");
    return L;
  }

  double lossDerivative(double potential, double y) const {
    return 2.0 * (potential - y);
  }

  /*=======BACKPROP=======*/
  drannGradients backpropRhizome(double y) const {
    if (participantCount() == 0 || !std::isfinite(y))
      throw std::invalid_argument("El backward requiere un forward y un ideal finito.");
    drannGradients gradients;
    gradients.inputWeights.assign(inputWeights.size(), 0.0);
    gradients.weights.assign(weights.size(), 0.0);
    gradients.bias.assign(bias.size(), 0.0);

    std::vector<double> delta(inputWeights.size(), 0.0);
    for (std::size_t j = 0; j < inputWeights.size(); j++) {
      if (participating[j])
        delta[j] = lossDerivative(sigmaCache[j], y) /
                   static_cast<double>(participantCount());
    }

    for (std::size_t tau = stepCache.size(); tau > 0; tau--) {
      const drannStep &step = stepCache[tau - 1];
      // Paso identidad para los estados conservados. No modificar delta al leerlo.
      std::vector<double> previousDelta = delta;
      for (const drannTransmission &transmission : step.transmissions) {
        std::size_t i = transmission.transmitter, j = transmission.receiver;
        std::size_t k = transmission.connection;
        double deltaV = step.potential[i] - step.potential[j];
        gradients.weights[k] += delta[j] * deltaV;
        previousDelta[i] += delta[j] * weights[k].w;
        previousDelta[j] -= delta[j] * weights[k].w;
      }
      for (std::size_t j = 0; j < inputWeights.size(); j++) {
        if (step.receivers[j])
          gradients.bias[j] += delta[j];
      }
      delta = previousDelta;
    }

    for (std::size_t i = 0; i < inputWeights.size(); i++) {
      gradients.inputWeights[i] = delta[i] * xCache;
      gradients.bias[i] += delta[i]; // El mismo sesgo también se usó en la entrada.
    }
    return gradients;
  }

  /*=======PARAMETER UPDATE=======*/
  void parameterUpdate(const drannGradients &gradients) {
    if (gradients.inputWeights.size() != inputWeights.size() ||
        gradients.bias.size() != bias.size() ||
        gradients.weights.size() != weights.size())
      throw std::invalid_argument("Los gradientes no corresponden a esta arquitectura.");

    for (std::size_t i = 0; i < inputWeights.size(); i++) {
      inputWeights[i] -= lr * gradients.inputWeights[i];
      bias[i] -= lr * gradients.bias[i];
      if (!std::isfinite(inputWeights[i]) || !std::isfinite(bias[i]))
        throw std::runtime_error("Parámetro de entrada o sesgo no finito.");
    }
    for (std::size_t k = 0; k < weights.size(); k++) {
      if (!weights[k].active)
        continue;
      weights[k].w -= lr * gradients.weights[k];
      if (!std::isfinite(weights[k].w))
        throw std::runtime_error("Peso interno no finito.");
      if (weights[k].w <= 0.0)
        weights[k].active = false;
    }
  }

  /*=======EVALUATION=======*/
  drannMetrics evaluate(const std::vector<double> &X,
                       const std::vector<double> &Y) {
    if (X.empty() || X.size() != Y.size())
      throw std::invalid_argument("El dataset debe contener pares entrada-salida.");
    drannMetrics metrics{0.0, 0.0};
    for (std::size_t ej = 0; ej < X.size(); ej++) {
      forwardRhizome(X[ej]);
      metrics.loss += loss(Y[ej]);
      metrics.meanSteps += static_cast<double>(stepCache.size());
    }
    metrics.loss /= static_cast<double>(X.size());
    metrics.meanSteps /= static_cast<double>(X.size());
    return metrics;
  }

  /*=======TRAINING=======*/
  int train(const std::vector<double> &X, const std::vector<double> &Y,
             const std::vector<double> &Xvalidation,
             const std::vector<double> &Yvalidation, int epochs,
             const std::string &historyPath,
             const std::function<bool(const drannEpoch &)> &onEpoch = {},
             std::optional<int> resumeAfter = std::nullopt) {
    if (epochs < 1 || epochs == std::numeric_limits<int>::max())
      throw std::invalid_argument("El número de épocas debe ser positivo y representable.");
    if (resumeAfter && (*resumeAfter < 0 || *resumeAfter >= epochs))
      throw std::invalid_argument("La nueva meta debe superar la última época completada.");
    std::ofstream history(historyPath, resumeAfter ? std::ios::app : std::ios::trunc);
    if (!history)
      throw std::runtime_error("No se pudo abrir el historial: " + historyPath);
    history << std::setprecision(17);
    if (!resumeAfter) {
      history << "epoch,training_loss,validation_loss,training_mean_steps,"
                 "validation_mean_steps,active_connections,participating_neurons\n";
    }

    drannConsole console;
    using Clock = drannConsole::Clock;
    const auto started = Clock::now();
    auto lastProgress = started;
    const int firstEpoch = resumeAfter.value_or(0);
    const int logInterval = std::max(1, epochs / 10); // Frecuencia de los resúmenes.
    int terminalRows = 0;
    bool pinnedProgress = false;
#if defined(__unix__) || defined(__APPLE__)
    struct winsize terminalSize {};
    pinnedProgress = console.terminal &&
                     ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminalSize) == 0 &&
                     terminalSize.ws_row > 2 && terminalSize.ws_col >= 120;
    if (pinnedProgress) {
      terminalRows = terminalSize.ws_row;
      std::cout << "\033[1;" << terminalRows - 1 << "r"
                << "\033[" << terminalRows - 1 << ";1H" << std::flush;
    }
#endif
    struct ProgressTerminalReset {
      bool active;
      int rows;
      ~ProgressTerminalReset() {
        if (active)
          std::cout << "\033[r\033[" << rows << ";1H\r\n" << std::flush;
      }
    } terminalReset{pinnedProgress, terminalRows};

    auto printLog = [&](int color, const std::string &message) {
      if (pinnedProgress)
        std::cout << "\033[" << terminalRows - 1 << ";1H\r\033[2K";
      console.line(color, message);
    };
    auto printProgress = [&](int e, bool force, bool stopped = false) {
      if (!pinnedProgress)
        return;
      const auto now = Clock::now();
      if (!force && now - lastProgress < std::chrono::milliseconds(100))
        return;
      lastProgress = now;
      std::cout << "\033[" << terminalRows << ";1H\r"
                << console.progress(e, epochs, firstEpoch, now - started, stopped)
                << "\033[K\033[" << terminalRows - 1 << ";1H" << std::flush;
    };
    auto finishLog = [&](int e) {
      printProgress(e, true, e < epochs);
      printLog(8, "Total training duration: " + drannConsole::duration(Clock::now() - started));
      printLog(e == epochs ? 10 : 6, e == epochs ? "Training has finished."
                        : "Training stopped after epoch " + std::to_string(e) + '.');
    };
    printLog(15, "------------------------------------------------");
    printLog(10, resumeAfter ? "Training will continue from epoch " + std::to_string(firstEpoch + 1) + '.'
                            : "Training will start.");
    printLog(8, "Training samples: " + std::to_string(X.size()) +
                " | Validation samples: " + std::to_string(Xvalidation.size()));
    printProgress(firstEpoch, true);

    auto record = [&](int e) {
      drannMetrics training = evaluate(X, Y);
      drannMetrics validation = evaluate(Xvalidation, Yvalidation);
      const auto evaluated = Clock::now();
      history << e << ',' << training.loss << ',' << validation.loss << ','
              << training.meanSteps << ',' << validation.meanSteps << ','
              << activeConnections() << ',' << participantCount() << '\n';
      history.flush();
      if (!history)
        throw std::runtime_error("No se pudo escribir el historial.");
      const bool keepTraining = !onEpoch || onEpoch({e, training, validation, activeConnections(),
                                                     participantCount()});
      if (e == 0 || e == epochs || e % logInterval == 0 || !keepTraining) {
        printLog(8, "------------------------------------------------");
        printLog(15, "Epoch -> " + std::to_string(e) + " / " + std::to_string(epochs) + '.');
        std::ostringstream message;
        message << std::setprecision(10) << "Training Loss: " << training.loss
                << " | Validation Loss: " << validation.loss
                << "\nActive connections: " << activeConnections()
                << " | Participating neurons: " << participantCount()
                << " | Steps: " << validation.meanSteps;
        // Una línea por llamada: mantener la barra fijada debajo del resumen.
        std::istringstream lines(message.str());
        for (std::string line; std::getline(lines, line);)
          printLog(15, line);
        if (!pinnedProgress)
          printLog(15, console.progress(e, epochs, firstEpoch, evaluated - started, !keepTraining && e < epochs));
        printProgress(e, true, !keepTraining && e < epochs);
      }
      return keepTraining;
    };

    if (!resumeAfter && !record(0)) { // Referencia anterior a las actualizaciones.
      finishLog(0);
      return 0;
    }
    for (int e = firstEpoch + 1; e <= epochs; e++) {
      for (std::size_t ej = 0; ej < X.size(); ej++) {
        forwardRhizome(X[ej]);
        loss(Y[ej]);
        drannGradients gradients = backpropRhizome(Y[ej]);
        parameterUpdate(gradients);
      }
      if (!record(e)) { // Ambos conjuntos con los parámetros finales de la época.
        finishLog(e);
        return e;
      }
      printProgress(e, false);
    }
    finishLog(epochs);
    return epochs;
  }

  /*=======VALIDATION=======*/
  void validation(double x, double y) {
    forwardRhizome(x);
    drannConsole console;
    std::ostringstream message;
    message << std::setprecision(10)
            << "Input (C): " << x * 100.0 << " | Ideal (F): " << y * 100.0
            << "\nCognifenomeno primitivo (F): [";
    bool first = true;
    for (std::size_t j = 0; j < sigmaCache.size(); j++) {
      if (participating[j]) {
        if (!first)
          message << ", ";
        message << sigmaCache[j] * 100.0;
        first = false;
      }
    }
    message << "]\nGeometric loss (normalized): " << loss(y)
            << " | Steps: " << stepCache.size();
    console.line(15, message.str());
    console.line(8, "------------------------------------------------");
  }
};

#ifndef DRANN_NO_MAIN
#ifdef DRANN_GUI
#include "graphics/DRANN_plot.h"
#else
static int waitForConsoleContinuation(int completed) {
  bool interactive = false;
#if defined(__unix__) || defined(__APPLE__)
  interactive = isatty(STDIN_FILENO) != 0;
#elif defined(_WIN32)
  interactive = _isatty(_fileno(stdin)) != 0;
#endif
  if (!interactive)
    return 0;

  drannConsole console;
  const int maxAdditional = std::numeric_limits<int>::max() - 1 - completed;
  if (maxAdditional == 0) {
    console.line(6, "Se alcanzó el límite de épocas.");
    return 0;
  }
  while (true) {
    console.prompt(6, "¿Cuántas épocas más quieres entrenar? [0 o Intro para salir] ");
    std::string line;
    if (!std::getline(std::cin, line))
      return 0;
    std::istringstream input(line);
    input >> std::ws;
    if (input.eof())
      return 0;

    int additional = 0;
    if (input >> additional && (input >> std::ws).eof() &&
        additional >= 0 && additional <= maxAdditional)
      return additional == 0 ? 0 : completed + additional;

    console.line(4, "Introduce un número entero entre 0 y " + std::to_string(maxAdditional) + '.');
  }
}
#endif

int main(int argc, char **argv) {
  try {
    /*=======DATASET=======*/
    // Elección inicial documentada: 8 ejemplos de entrenamiento y 2 de validación.
    std::vector<double> celsius = {-40, -20, 0, 20, 40, 60, 80, 100};
    std::vector<double> validationCelsius = {10, 70};
    std::vector<double> X, Y, Xvalidation, Yvalidation;
    for (double c : celsius) {
      X.push_back(c / 100.0);
      Y.push_back((1.8 * c + 32.0) / 100.0);
    }
    for (double c : validationCelsius) {
      Xvalidation.push_back(c / 100.0);
      Yvalidation.push_back((1.8 * c + 32.0) / 100.0);
    }

    /*=======CONFIGURATION=======*/
    std::size_t neuronNumber = 3;
    double lr = 0.001;
    int epochs = 100000;
    unsigned int seed = 42;
    if (argc != 1) {
      if (argc != 3 || std::string(argv[1]) != "--epochs")
        throw std::invalid_argument("Uso: drann [--epochs numero_positivo]");
      std::size_t parsed = 0;
      std::string value = argv[2];
      epochs = std::stoi(value, &parsed);
      if (parsed != value.size() || epochs < 1 ||
          epochs == std::numeric_limits<int>::max())
        throw std::invalid_argument("El número de épocas debe ser positivo y representable.");
    }
    DRANN drann(neuronNumber, lr);
    drann.initWeights(seed);

    drannConsole console;
    console.line(15, "DRANN | Celsius -> Fahrenheit");
    console.line(8, "------------------------------------------------");
    std::ostringstream configuration;
    configuration << std::setprecision(10) << "Neurons: " << neuronNumber << " | Seed: " << seed
                  << " | lr: " << lr << " | Epochs: " << epochs;
    console.line(8, configuration.str());
    console.line(10, "Weights have been initialized.");
    std::function<bool(const drannEpoch &)> onEpoch;
#ifdef DRANN_GUI
    DRANNPlot plot(epochs);
    onEpoch = [&](const drannEpoch &epoch) { return plot.update(epoch); };
#endif
    std::optional<int> resumeAfter;
    while (epochs > 0) {
      const int completed = drann.train(X, Y, Xvalidation, Yvalidation, epochs,
                                        "training_history.csv", onEpoch, resumeAfter);
      console.line(15, "Validation results:");
      const auto validationStarted = drannConsole::Clock::now();
      for (std::size_t ej = 0; ej < Xvalidation.size(); ej++)
        drann.validation(Xvalidation[ej], Yvalidation[ej]);
      console.line(8, "Final validation duration: " +
                      drannConsole::duration(drannConsole::Clock::now() - validationStarted));
      console.line(10, "History saved in training_history.csv.");
#ifdef DRANN_GUI
      epochs = plot.waitForContinuation();
#else
      epochs = waitForConsoleContinuation(completed);
#endif
      resumeAfter = completed;
    }
    return 0;
  } catch (const std::exception &error) {
    drannConsole console(std::cerr);
    console.line(4, "DRANN: " + std::string(error.what()));
    return 1;
  }
}
#endif
