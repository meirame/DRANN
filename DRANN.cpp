#include <cmath>     // comprobación de valores finitos
#include <cstddef>   // índices de los vectores
#include <fstream>   // historial del entrenamiento
#include <functional> // notificar las evaluaciones sin depender de la interfaz
#include <iomanip>   // precisión de la salida
#include <iostream>  // salida por terminal
#include <limits>    // límite del contador de épocas
#include <random>    // inicialización de los pesos
#include <stdexcept> // errores sin inventar una recuperación del modelo
#include <string>    // nombre del archivo de resultados
#include <vector>    // parámetros, estados y cachés

/* Filosofía
El rizoma no tiene capas ni una neurona de salida. Cada neurona participante
aporta su último potencial al cognifenómeno primitivo, aunque ya haya transmitido.
Los papeles se deciden simultáneamente al comienzo de cada paso tau.
El backward deriva el recorrido realizado; no deriva las decisiones discretas.
*/

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
             const std::function<bool(const drannEpoch &)> &onEpoch = {}) {
    if (epochs < 1)
      throw std::invalid_argument("El número de épocas debe ser positivo.");
    std::ofstream history(historyPath);
    if (!history)
      throw std::runtime_error("No se pudo abrir el historial: " + historyPath);
    history << std::setprecision(17);
    history << "epoch,training_loss,validation_loss,training_mean_steps,"
               "validation_mean_steps,active_connections,participating_neurons\n";

    auto record = [&](int e) {
      drannMetrics training = evaluate(X, Y);
      drannMetrics validation = evaluate(Xvalidation, Yvalidation);
      history << e << ',' << training.loss << ',' << validation.loss << ','
              << training.meanSteps << ',' << validation.meanSteps << ','
              << activeConnections() << ',' << participantCount() << '\n';
      history.flush();
      if (!history)
        throw std::runtime_error("No se pudo escribir el historial.");
      std::cout << "Epoch " << e << " | Training loss = " << training.loss
                << " | Validation loss = " << validation.loss
                << " | Active connections = " << activeConnections() << '\n';
      return !onEpoch || onEpoch({e, training, validation, activeConnections(),
                                 participantCount()});
    };

    if (!record(0)) // Referencia anterior a las actualizaciones.
      return 0;
    for (int e = 1; e <= epochs; e++) {
      for (std::size_t ej = 0; ej < X.size(); ej++) {
        forwardRhizome(X[ej]);
        loss(Y[ej]);
        drannGradients gradients = backpropRhizome(Y[ej]);
        parameterUpdate(gradients);
      }
      if (!record(e)) // Ambos conjuntos con los parámetros finales de la época.
        return e;
    }
    return epochs;
  }

  /*=======VALIDATION=======*/
  void validation(double x, double y) {
    forwardRhizome(x);
    std::cout << "Input (C): " << x * 100.0 << " | Ideal (F): " << y * 100.0
              << " | Cognifenomeno primitivo (F): [";
    bool first = true;
    for (std::size_t j = 0; j < sigmaCache.size(); j++) {
      if (participating[j]) {
        if (!first)
          std::cout << ", ";
        std::cout << sigmaCache[j] * 100.0;
        first = false;
      }
    }
    std::cout << "] | Geometric loss (normalized): " << loss(y)
              << " | Steps: " << stepCache.size() << '\n';
  }
};

#ifndef DRANN_NO_MAIN
#ifdef DRANN_GUI
#include "graphics/DRANN_plot.h"
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
    std::size_t neuronNumber = 5;
    double lr = 0.005;
    int epochs = 100;
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

    std::cout << std::setprecision(10);
    std::cout << "DRANN | Neurons: " << neuronNumber << " | Seed: " << seed
              << " | lr: " << lr << " | Epochs: " << epochs << '\n';
    std::function<bool(const drannEpoch &)> onEpoch;
#ifdef DRANN_GUI
    DRANNPlot plot(epochs);
    onEpoch = [&](const drannEpoch &epoch) { return plot.update(epoch); };
#endif
    int completed = drann.train(X, Y, Xvalidation, Yvalidation, epochs,
                               "training_history.csv", onEpoch);
    for (std::size_t ej = 0; ej < Xvalidation.size(); ej++)
      drann.validation(Xvalidation[ej], Yvalidation[ej]);
    std::cout << "Training stopped after " << completed << '/' << epochs
              << " epochs. History: training_history.csv\n";
#ifdef DRANN_GUI
    plot.finish();
#endif
    return 0;
  } catch (const std::exception &error) {
    std::cerr << "DRANN: " << error.what() << '\n';
    return 1;
  }
}
#endif
