#ifndef DRANN_ENTRENAMIENTO_H
#define DRANN_ENTRENAMIENTO_H

#include "DRANN_vectorial.h"

#include <numeric>
#include <optional>
#include <string>

namespace vectorial {
struct Example {
  double x;
  Point ideal;
  double physicalInput = 0;
};
using Dataset = std::vector<Example>;

struct NeuronMetrics {
  double weightSum = 0, weightMax = 0, sentMagnitude = 0, receivedMagnitude = 0;
  double transmitterSteps = 0, receiverSteps = 0;
  Point distances;
};

struct Metrics {
  Point coordinates;
  double loss = 0, meanSteps = 0, worstPoint = 0;
  std::size_t equalModuleStops = 0, nearTies = 0;
  std::vector<NeuronMetrics> neurons;
};

inline Metrics evaluate(const Model &model, const Dataset &dataset, bool diagnostics = true) {
  if (dataset.empty())
    throw std::invalid_argument("Conjunto de datos vacío.");
  Metrics metrics;
  metrics.coordinates.resize(model.dimensions);
  const std::size_t n = model.input.size();
  if (diagnostics) {
    metrics.neurons.resize(n);
    for (auto &neuron : metrics.neurons)
      neuron.distances.resize(n);
    for (const auto &edge : model.edges)
      if (edge.active)
        for (std::size_t i : {edge.i, edge.j}) {
          metrics.neurons[i].weightSum += edge.w;
          metrics.neurons[i].weightMax = std::max(metrics.neurons[i].weightMax, edge.w);
        }
  }
  for (const auto &example : dataset) {
    const Trace trace = model.forwardRhizome(example.x);
    const Point losses = model.coordinateLoss(trace, example.ideal);
    for (std::size_t k = 0; k < model.dimensions; ++k)
      metrics.coordinates[k] += losses[k];
    metrics.meanSteps += trace.steps.size();
    metrics.equalModuleStops += trace.terminalTies;
    metrics.nearTies += trace.nearTies;
    for (std::size_t i = 0; i < n; ++i) {
      if (!trace.participating[i])
        continue;
      double distance = 0;
      for (std::size_t k = 0; k < model.dimensions; ++k)
        distance = std::hypot(distance, trace.final[i][k] - example.ideal[k]);
      metrics.worstPoint = std::max(metrics.worstPoint, distance);
      if (diagnostics)
        for (std::size_t j = i + 1; j < n; ++j)
          if (trace.participating[j]) {
            double pair = 0;
            for (std::size_t k = 0; k < model.dimensions; ++k)
              pair = std::hypot(pair, trace.final[i][k] - trace.final[j][k]);
            metrics.neurons[i].distances[j] += pair;
            metrics.neurons[j].distances[i] += pair;
          }
    }
    if (diagnostics)
      for (const auto &step : trace.steps) {
        for (std::size_t i = 0; i < n; ++i) {
          metrics.neurons[i].transmitterSteps += step.transmits[i];
          metrics.neurons[i].receiverSteps += step.receives[i];
        }
        for (const auto &message : step.messages) {
          metrics.neurons[message.source].sentMagnitude += message.magnitude;
          metrics.neurons[message.receiver].receivedMagnitude += message.magnitude;
        }
      }
  }
  for (double &value : metrics.coordinates)
    value /= dataset.size();
  metrics.loss = std::accumulate(metrics.coordinates.begin(), metrics.coordinates.end(), 0.0);
  requireFinite(metrics.loss);
  requireFinite(metrics.worstPoint);
  metrics.meanSteps /= dataset.size();
  for (auto &neuron : metrics.neurons) {
    neuron.sentMagnitude /= dataset.size();
    neuron.receivedMagnitude /= dataset.size();
    neuron.transmitterSteps /= dataset.size();
    neuron.receiverSteps /= dataset.size();
    for (double &distance : neuron.distances)
      distance /= dataset.size();
  }
  return metrics;
}

struct Measurements {
  Metrics training, validation;
  double regularizer = 0;
  std::vector<bool> participating;
  std::size_t connections = 0;

  std::size_t neurons() const {
    return std::count(participating.begin(), participating.end(), true);
  }
};

struct Config {
  double lambdaTarget = 0, minImprovement = 1e-8, relativeTolerance = 0.05;
  Point absoluteTolerance{1e-6};
  int warmup = 1000, patience = 200, ramp = 200, maxPressure = 2000;
  int maxRecovery = 500, maxAttempts = 3;
  ProxOptions prox;
};

enum class Phase { training, pressure, recovery, restoration, accepted };

inline const char *name(Phase phase) {
  switch (phase) {
    case Phase::training: return "entrenamiento";
    case Phase::pressure: return "presion";
    case Phase::recovery: return "recuperacion";
    case Phase::restoration: return "restauracion";
    case Phase::accepted: return "aceptacion";
  }
  return "";
}

struct Snapshot {
  Model model;
  Measurements measurements;
  int work, modelEpoch, lastImprovement;
  double plateau;
  Phase phase;
};

struct Epoch {
  int work = 0, modelEpoch = 0, attempt = 0, processed = 0, referenceWork = -1;
  Phase phase = Phase::training;
  double lambda = 0, gap = 0, appliedLambda = 0;
  int sweeps = 0;
  std::vector<std::size_t> newlyIsolated;
  std::string decision;
  std::optional<Measurements> trial;
};

struct Attempt {
  std::size_t referenceIndex;
  int pressureEpochs = 0, recoveryEpochs = 0;
  std::optional<Snapshot> bestReduction;
};

inline bool sameTopology(const Model &a, const Model &b) {
  for (std::size_t i = 0; i < a.edges.size(); ++i)
    if (a.edges[i].active != b.edges[i].active)
      return false;
  return true;
}

struct Training {
  Model model;
  const Config config;
  const Dataset trainData, validationData;
  int work = 0, modelEpoch = 0, lastImprovement = 0;
  double plateau;
  Measurements current;
  Snapshot bestPredictive;
  std::vector<Snapshot> references;
  std::vector<std::pair<int, Snapshot>> reductionStarts;
  std::optional<Attempt> attempt;
  Phase phase = Phase::training;

  Training(Model initial, Config settings, Dataset training, Dataset validation)
      : model(std::move(initial)), config(std::move(settings)), trainData(std::move(training)),
        validationData(std::move(validation)), plateau(0), current(measure()),
        bestPredictive(snapshot(Phase::training)) {
    for (double value : {config.lambdaTarget, config.minImprovement, config.relativeTolerance,
                         config.prox.zeroTolerance, config.prox.absTolerance, config.prox.relTolerance}) {
      requireFinite(value);
      if (value < 0)
        throw std::invalid_argument("Las tolerancias y lambda deben ser no negativas.");
    }
    if ((config.absoluteTolerance.size() != 1 && config.absoluteTolerance.size() != model.dimensions) ||
        config.warmup < 0 || config.patience < 0 || config.ramp < 1 || config.maxPressure < 1 ||
        config.maxRecovery < 0 || config.maxAttempts < 1 || config.prox.maxSweeps < 1)
      throw std::invalid_argument("Presupuesto o dimensión de tolerancias no válido.");
    for (double value : config.absoluteTolerance) {
      requireFinite(value);
      if (value < 0)
        throw std::invalid_argument("La tolerancia absoluta debe ser no negativa.");
    }
    plateau = current.training.loss;
    bestPredictive.plateau = plateau;
  }

  Measurements measure() const {
    return {evaluate(model, trainData, false), evaluate(model, validationData),
            model.regularizer(), model.participation(), model.activeConnections()};
  }

  Snapshot snapshot(Phase stage) const {
    return {model, current, work, modelEpoch, lastImprovement, plateau, stage};
  }

  bool canAttempt() const {
    if (config.lambdaTarget == 0 || current.neurons() < 3 || work < config.warmup ||
        work - lastImprovement < config.patience ||
        references.size() >= static_cast<std::size_t>(config.maxAttempts))
      return false;
    if (references.empty())
      return true;
    const auto &last = references.back();
    return !sameTopology(model, last.model) ||
           last.measurements.training.loss - current.training.loss > config.minImprovement;
  }

  void observePredictive(Phase stage) {
    if (current.validation.loss < bestPredictive.measurements.validation.loss)
      bestPredictive = snapshot(stage);
  }

  bool acceptable(const Measurements &reference) const {
    if (current.neurons() < 2 || current.neurons() >= reference.neurons())
      return false;
    for (std::size_t k = 0; k < model.dimensions; ++k) {
      const double absolute = config.absoluteTolerance[config.absoluteTolerance.size() == 1 ? 0 : k];
      for (auto data : {&Measurements::training, &Measurements::validation}) {
        const double prior = (reference.*data).coordinates[k];
        if ((current.*data).coordinates[k] - prior > absolute + config.relativeTolerance * prior)
          return false;
      }
    }
    return true;
  }

  void considerReduction() {
    if (acceptable(references[attempt->referenceIndex].measurements) &&
        (!attempt->bestReduction ||
         current.validation.loss < attempt->bestReduction->measurements.validation.loss))
      attempt->bestReduction = snapshot(Phase::recovery);
  }

  void settle(Epoch &epoch, const std::string &reason, bool invalid = false) {
    const auto &reference = references[attempt->referenceIndex];
    const bool accepted = !invalid && attempt->bestReduction.has_value();
    const auto &chosen = accepted ? *attempt->bestReduction : reference;
    epoch.phase = accepted ? Phase::accepted : Phase::restoration;
    epoch.decision += (epoch.decision.empty() ? "" : ";") +
                      std::string(accepted ? "aceptado:" : "restaurado:") + reason;
    model = chosen.model;
    current = chosen.measurements;
    modelEpoch = chosen.modelEpoch;
    plateau = accepted ? current.training.loss : chosen.plateau;
    lastImprovement = accepted ? work : chosen.lastImprovement;
    epoch.modelEpoch = modelEpoch;
    epoch.lambda = 0;
    phase = Phase::training;
    attempt.reset();
  }

  Epoch advance(int goal) {
    if (goal <= work)
      throw std::invalid_argument("Se agotó el presupuesto de trabajo.");
    Epoch epoch;
    if (!attempt && canAttempt()) {
      references.push_back(snapshot(Phase::training));
      attempt = Attempt{references.size() - 1, 0, 0, std::nullopt};
      phase = Phase::pressure;
      epoch.decision = "inicio_presion";
    }
    epoch.work = ++work;
    epoch.modelEpoch = ++modelEpoch;
    epoch.phase = phase;
    if (attempt) {
      epoch.attempt = static_cast<int>(attempt->referenceIndex + 1);
      epoch.referenceWork = references[attempt->referenceIndex].work;
    }
    if (phase == Phase::pressure) {
      ++attempt->pressureEpochs;
      epoch.lambda = config.lambdaTarget *
          std::min(1.0, static_cast<double>(attempt->pressureEpochs) / config.ramp);
      epoch.appliedLambda = epoch.lambda;
    }
    if (phase == Phase::recovery)
      ++attempt->recoveryEpochs;

    const auto before = current.participating;
    try {
      for (const auto &example : trainData) {
        const Trace trace = model.forwardRhizome(example.x);
        const auto gradient = model.backpropRhizome(example.x, trace, example.ideal);
        ++epoch.processed;
        const auto prox = model.parameterUpdate(gradient, epoch.lambda, config.prox);
        epoch.gap = std::max(epoch.gap, prox.gap);
        epoch.sweeps = std::max(epoch.sweeps, prox.sweeps);
        const auto after = model.participation();
        if (std::count(after.begin(), after.end(), true) < 2) {
          for (std::size_t i = 0; i < before.size(); ++i)
            if (before[i] && !after[i])
              epoch.newlyIsolated.push_back(i);
          throw std::runtime_error("menos de dos participantes");
        }
        if (phase == Phase::pressure && after != before)
          break; // La primera actualización que aísla fija el candidato inicial.
      }
      auto measured = measure();
      requireFinite(measured.training.loss + epoch.lambda * measured.regularizer);
      current = std::move(measured);
    } catch (const std::runtime_error &error) {
      if (!attempt)
        throw;
      settle(epoch, std::string("intento_invalido:") + error.what(), true);
      return epoch;
    }

    for (std::size_t i = 0; i < before.size(); ++i)
      if (before[i] && !current.participating[i])
        epoch.newlyIsolated.push_back(i);

    if (phase == Phase::training) {
      if (plateau - current.training.loss > config.minImprovement) {
        plateau = current.training.loss;
        lastImprovement = work;
      }
      observePredictive(phase);
      return epoch;
    }

    if (phase == Phase::pressure) {
      if (!epoch.newlyIsolated.empty()) {
        epoch.decision += (epoch.decision.empty() ? "" : ";") + std::string("aislamiento_recuperacion_0");
        phase = Phase::recovery;
        reductionStarts.emplace_back(epoch.attempt, snapshot(phase));
        considerReduction();
        observePredictive(phase);
      } else if (attempt->pressureEpochs >= config.maxPressure || work == goal) {
        observePredictive(phase);
        epoch.trial = current;
        settle(epoch, "sin_reduccion");
        return epoch;
      } else {
        observePredictive(phase);
      }
    } else {
      considerReduction();
      observePredictive(phase);
    }

    if (phase == Phase::recovery && (attempt->recoveryEpochs >= config.maxRecovery || work == goal)) {
      epoch.trial = current;
      settle(epoch, attempt->bestReduction ? "dentro_de_tolerancias" : "fuera_de_tolerancias");
    }
    return epoch;
  }

  std::optional<Epoch> closeAttempt() {
    if (!attempt)
      return std::nullopt;
    Epoch result;
    result.work = work;
    result.attempt = static_cast<int>(attempt->referenceIndex + 1);
    result.referenceWork = references[attempt->referenceIndex].work;
    result.trial = current;
    settle(result, "fin_de_sesion");
    return result;
  }
};
} // namespace vectorial
#endif
