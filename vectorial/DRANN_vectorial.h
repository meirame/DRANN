#ifndef DRANN_VECTORIAL_H
#define DRANN_VECTORIAL_H

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

namespace vectorial {
using Point = std::vector<double>;
using Points = std::vector<Point>;

inline double norm(const Point &v) {
  double result = 0;
  for (double value : v)
    result = std::hypot(result, value);
  return result;
}

inline double sigma(double z) { return z >= 0 ? z : 0.01 * z; }
// En cero se elige la derivada del lado positivo.
inline double sigmaDerivative(double z) { return z >= 0 ? 1.0 : 0.01; }

inline void requireFinite(double value) {
  if (!std::isfinite(value))
    throw std::runtime_error("Valor no finito en el cálculo vectorial.");
}

struct Connection {
  std::size_t i, j;
  double w;
  bool active;
};

struct Transmission {
  std::size_t edge, source, receiver;
  double magnitude;
};

struct Step {
  Points previous, preactivation;
  std::vector<bool> transmits, receives;
  std::vector<Transmission> messages;
};

struct Trace {
  Points initialPreactivation, final;
  std::vector<bool> participating;
  std::vector<Step> steps;
  std::size_t participants = 0, terminalTies = 0, nearTies = 0;
};

struct Gradients {
  Points input, bias;
  Point edges;
};

struct ProxOptions {
  double absTolerance = 1e-10, relTolerance = 1e-10, zeroTolerance = 1e-12;
  int maxSweeps = 2000;
};

struct ProxResult {
  Point weights;
  double gap = 0;
  int sweeps = 0;
};

inline double penalty(const Point &weights, const std::vector<std::vector<std::size_t>> &groups) {
  double total = 0;
  for (const auto &group : groups) {
    double magnitude = 0;
    for (std::size_t edge : group)
      magnitude = std::hypot(magnitude, weights[edge]);
    total += magnitude;
  }
  return total;
}

/*=======PROXIMAL DE GRUPOS SOLAPADOS=======*/
inline ProxResult groupProx(const Point &v, const std::vector<std::vector<std::size_t>> &groups,
                            double tau, const ProxOptions &options) {
  requireFinite(tau);
  if (tau < 0)
    throw std::invalid_argument("El parámetro proximal debe ser no negativo.");
  if (tau == 0)
    return {v, 0, 0};
  ProxResult result{v, 0, 0};
  Points corrections;
  for (const auto &group : groups)
    corrections.emplace_back(group.size(), 0.0);
  for (double w : v) {
    requireFinite(w);
    if (w < 0)
      throw std::invalid_argument("El proximal requiere pesos no negativos.");
  }
  for (int sweep = 1; sweep <= options.maxSweeps; ++sweep) {
    const Point previous = result.weights;
    for (std::size_t i = 0; i < groups.size(); ++i) {
      const auto &group = groups[i];
      Point a(group.size());
      for (std::size_t k = 0; k < group.size(); ++k)
        a[k] = result.weights.at(group[k]) + corrections[i][k];
      const double magnitude = norm(a);
      requireFinite(magnitude);
      const double scale = magnitude > tau ? 1 - tau / magnitude : 0.0;
      for (std::size_t k = 0; k < group.size(); ++k) {
        const double next = scale * a[k];
        corrections[i][k] = a[k] - next;
        result.weights[group[k]] = next;
      }
    }

    Point dual(v.size(), 0.0);
    for (std::size_t i = 0; i < groups.size(); ++i) {
      if (norm(corrections[i]) > tau + options.absTolerance + options.relTolerance * tau)
        throw std::runtime_error("Dykstra produjo una corrección dual no factible.");
      for (std::size_t k = 0; k < groups[i].size(); ++k)
        dual[groups[i][k]] += corrections[i][k];
    }
    long double primal = static_cast<long double>(tau) * penalty(result.weights, groups);
    long double dualObjective = 0;
    bool stable = true;
    for (std::size_t k = 0; k < v.size(); ++k) {
      const double z = result.weights[k];
      requireFinite(z);
      requireFinite(dual[k]);
      if (z < 0)
        throw std::runtime_error("Dykstra produjo un peso negativo.");
      const double tolerance = options.absTolerance + options.relTolerance * std::abs(v[k]);
      if (std::abs(z + dual[k] - v[k]) > tolerance)
        throw std::runtime_error("Dykstra perdió el invariante primal-dual.");
      stable = stable && std::abs(z - previous[k]) <= tolerance;
      const long double difference = static_cast<long double>(z) - v[k];
      primal += 0.5L * difference * difference;
      dualObjective += static_cast<long double>(v[k]) * dual[k] - 0.5L * dual[k] * dual[k];
    }
    const long double gap = primal - dualObjective;
    const long double tolerance = options.absTolerance +
        options.relTolerance * std::max(std::abs(primal), std::abs(dualObjective));
    if (!std::isfinite(gap) || gap < -tolerance)
      throw std::runtime_error("Brecha primal-dual inválida.");
    if (stable && gap <= tolerance) {
      result.gap = static_cast<double>(std::max(0.0L, gap));
      result.sweeps = sweep;
      return result;
    }
  }
  throw std::runtime_error("Dykstra no convergió dentro del presupuesto de barridos.");
}

struct Model {
  std::size_t dimensions;
  double lr;
  Points input, bias;
  std::vector<Connection> edges;
  std::mt19937 rng;

  Model(std::size_t neurons, std::size_t d, double rate, unsigned seed)
      : dimensions(d), lr(rate), input(neurons, Point(d)), bias(neurons, Point(d)), rng(seed) {
    if (neurons < 2 || d == 0 || !std::isfinite(rate) || rate <= 0)
      throw std::invalid_argument("Se requieren al menos dos neuronas, una dimensión y lr positiva.");
    std::uniform_real_distribution<double> incoming(-1, 1), internal(0, 1);
    for (Point &row : input)
      for (double &w : row)
        do { w = incoming(rng); } while (w <= -1 || w >= 1);
    for (std::size_t i = 0; i < neurons; ++i)
      for (std::size_t j = i + 1; j < neurons; ++j) {
        double w;
        do { w = internal(rng); } while (w <= 0 || w >= 1);
        edges.push_back({i, j, w, true});
      }
  }

  std::vector<bool> participation() const {
    std::vector<bool> result(input.size(), false);
    for (const auto &edge : edges)
      if (edge.active)
        result[edge.i] = result[edge.j] = true;
    return result;
  }

  std::size_t activeConnections() const {
    return std::count_if(edges.begin(), edges.end(),
                         [](const Connection &edge) { return edge.active; });
  }

  double regularizer() const {
    Point magnitudes(input.size(), 0.0);
    for (const auto &edge : edges) {
      if (edge.active) {
        magnitudes[edge.i] = std::hypot(magnitudes[edge.i], edge.w);
        magnitudes[edge.j] = std::hypot(magnitudes[edge.j], edge.w);
      }
    }
    double total = 0;
    for (double value : magnitudes)
      total += value;
    requireFinite(total);
    return total;
  }

  /*=======FORWARD=======*/
  Trace forwardRhizome(double x) const {
    requireFinite(x);
    Trace trace;
    const std::size_t n = input.size();
    trace.initialPreactivation = Points(n, Point(dimensions));
    trace.final = trace.initialPreactivation;
    trace.participating = participation();
    trace.participants = std::count(trace.participating.begin(), trace.participating.end(), true);
    if (trace.participants < 2)
      throw std::runtime_error("Aislamiento total: se requieren dos participantes.");
    auto available = trace.participating;
    for (std::size_t i = 0; i < n; ++i)
      if (available[i])
        for (std::size_t k = 0; k < dimensions; ++k) {
          const double z = input[i][k] * x + bias[i][k];
          requireFinite(z);
          trace.initialPreactivation[i][k] = z;
          trace.final[i][k] = sigma(z);
        }
    for (;;) {
      Point radii(n);
      for (std::size_t i = 0; i < n; ++i)
        if (available[i]) {
          radii[i] = norm(trace.final[i]);
          requireFinite(radii[i]);
        }
      std::vector<bool> higher(n), lower(n);
      std::size_t differentDirectionTies = 0;
      for (const auto &edge : edges) {
        const auto i = edge.i, j = edge.j;
        if (!edge.active || !available[i] || !available[j])
          continue;
        if (radii[i] > radii[j])
          lower[i] = higher[j] = true;
        else if (radii[i] < radii[j])
          higher[i] = lower[j] = true;
        else if (trace.final[i] != trace.final[j])
          ++differentDirectionTies;
        const double difference = std::abs(radii[i] - radii[j]);
        if (difference > 0 && difference <= 1e-12 * std::max({1.0, radii[i], radii[j]}))
          ++trace.nearTies;
      }
      Step step{trace.final, Points(n, Point(dimensions)), std::vector<bool>(n),
                std::vector<bool>(n), {}};
      for (std::size_t i = 0; i < n; ++i)
        step.transmits[i] = available[i] && !higher[i] && lower[i];
      if (std::none_of(step.transmits.begin(), step.transmits.end(), [](bool v) { return v; })) {
        trace.terminalTies = differentDirectionTies;
        return trace;
      }

      for (std::size_t e = 0; e < edges.size(); ++e) {
        const auto &edge = edges[e];
        std::size_t i = edge.i, j = edge.j;
        if (!edge.active || !available[i] || !available[j])
          continue;
        if (step.transmits[j])
          std::swap(i, j);
        if (!step.transmits[i] || radii[i] <= radii[j])
          continue;
        double magnitude = 0;
        for (std::size_t k = 0; k < dimensions; ++k) {
          const double signal = edge.w * (step.previous[i][k] - step.previous[j][k]);
          step.preactivation[j][k] += signal;
          magnitude = std::hypot(magnitude, signal);
        }
        requireFinite(magnitude);
        step.receives[j] = true;
        step.messages.push_back({e, i, j, magnitude});
      }
      for (std::size_t i = 0; i < n; ++i) {
        if (step.receives[i])
          for (std::size_t k = 0; k < dimensions; ++k) {
            double &z = step.preactivation[i][k];
            z = step.previous[i][k] + z + bias[i][k];
            requireFinite(z);
            trace.final[i][k] = sigma(z);
          }
        if (step.transmits[i])
          available[i] = false;
      }
      trace.steps.push_back(std::move(step));
    }
  }

  void checkTarget(const Point &y) const {
    if (y.size() != dimensions)
      throw std::invalid_argument("Dimensión del objetivo incorrecta.");
    for (double value : y)
      requireFinite(value);
  }

  /*=======PÉRDIDA=======*/
  Point coordinateLoss(const Trace &trace, const Point &y) const {
    checkTarget(y);
    Point losses(dimensions);
    for (std::size_t i = 0; i < input.size(); ++i)
      if (trace.participating[i])
        for (std::size_t k = 0; k < dimensions; ++k) {
          const double difference = trace.final[i][k] - y[k];
          losses[k] += difference * difference;
        }
    for (double &value : losses) {
      value /= trace.participants;
      requireFinite(value);
    }
    return losses;
  }

  /*=======BACKWARD=======*/
  Gradients backpropRhizome(double x, const Trace &trace, const Point &y) const {
    checkTarget(y);
    const std::size_t n = input.size();
    Gradients gradients{Points(n, Point(dimensions)), Points(n, Point(dimensions)),
                        Point(edges.size())};
    Points g(n, Point(dimensions));
    for (std::size_t i = 0; i < n; ++i)
      if (trace.participating[i])
        for (std::size_t k = 0; k < dimensions; ++k)
          g[i][k] = 2 * (trace.final[i][k] - y[k]) / trace.participants;
    for (auto tau = trace.steps.rbegin(); tau != trace.steps.rend(); ++tau) {
      Points previous = g;
      Points delta(n, Point(dimensions));
      for (std::size_t j = 0; j < n; ++j)
        if (tau->receives[j])
          for (std::size_t k = 0; k < dimensions; ++k) {
            delta[j][k] = g[j][k] * sigmaDerivative(tau->preactivation[j][k]);
            previous[j][k] = delta[j][k];
            gradients.bias[j][k] += delta[j][k];
          }
      for (const auto &message : tau->messages) {
        const auto i = message.source, j = message.receiver, e = message.edge;
        for (std::size_t k = 0; k < dimensions; ++k) {
          gradients.edges[e] += delta[j][k] * (tau->previous[i][k] - tau->previous[j][k]);
          const double change = edges[e].w * delta[j][k];
          previous[j][k] -= change;
          previous[i][k] += change;
        }
      }
      g = std::move(previous);
    }
    for (std::size_t i = 0; i < n; ++i)
      if (trace.participating[i])
        for (std::size_t k = 0; k < dimensions; ++k) {
          const double delta = g[i][k] * sigmaDerivative(trace.initialPreactivation[i][k]);
          gradients.input[i][k] = delta * x;
          gradients.bias[i][k] += delta;
        }
    return gradients;
  }

  /*=======ACTUALIZACIÓN=======*/
  ProxResult parameterUpdate(const Gradients &gradient, double lambda, const ProxOptions &options) {
    for (std::size_t i = 0; i < input.size(); ++i)
      for (std::size_t k = 0; k < dimensions; ++k) {
        input[i][k] -= lr * gradient.input[i][k];
        bias[i][k] -= lr * gradient.bias[i][k];
        requireFinite(input[i][k]);
        requireFinite(bias[i][k]);
      }
    for (std::size_t e = 0; e < edges.size(); ++e)
      if (edges[e].active) {
        edges[e].w -= lr * gradient.edges[e];
        requireFinite(edges[e].w);
        if (edges[e].w <= 0) {
          edges[e].w = 0;
          edges[e].active = false;
        }
      }
    if (lambda == 0)
      return {};

    Point v;
    std::vector<std::size_t> selected;
    std::vector<std::vector<std::size_t>> groups(input.size());
    for (std::size_t e = 0; e < edges.size(); ++e)
      if (edges[e].active) {
        const auto &edge = edges[e];
        groups[edge.i].push_back(v.size());
        groups[edge.j].push_back(v.size());
        v.push_back(edge.w);
        selected.push_back(e);
      }
    auto result = groupProx(v, groups, lr * lambda, options);
    for (std::size_t j = 0; j < selected.size(); ++j) {
      auto &edge = edges[selected[j]];
      const double value = result.weights[j];
      edge.active = value > options.zeroTolerance;
      edge.w = edge.active ? value : 0.0;
    }
    return result;
  }
};

} // namespace vectorial
#endif
