#include "DRANN.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>
#include <numeric>
#include <stdexcept>

namespace {
void require(bool ok, const char *message) {
  if (!ok)
    throw std::runtime_error(message);
}
double squared(const Point &a, const Point &b) {
  require(a.size() == b.size(), "Dimensiones incompatibles.");
  double d = 0;
  for (std::size_t k = 0; k < a.size(); k++) {
    require(std::isfinite(a[k]) && std::isfinite(b[k]), "Valor no finito.");
    d += (a[k] - b[k]) * (a[k] - b[k]);
  }
  require(std::isfinite(d), "Distancia no finita.");
  return d;
}
void writePoint(std::ostream &o, const Point &v) {
  for (double x : v)
    o << x << ' ';
  o << '\n';
}
void readPoint(std::istream &i, Point &v) {
  for (double &x : v) {
    i >> x;
    require(std::isfinite(x), "Parámetro no finito.");
  }
}
void writeData(std::ostream &o, const std::vector<Example> &data) {
  o << data.size() << '\n';
  for (const auto &v : data) {
    o << v.x << ' ';
    writePoint(o, v.y);
  }
}
std::vector<Example> readData(std::istream &i, int T) {
  int n = 0;
  i >> n;
  require(n > 0 && n < 1000000, "Dataset inválido.");
  std::vector<Example> d;
  for (int j = 0; j < n; j++) {
    Example x{0, Point(T)};
    i >> x.x;
    readPoint(i, x.y);
    d.push_back(x);
  }
  return d;
}
} // namespace

/*=======INITIALIZATION=======*/
Model::Model(Config c)
    : config(c), input(c.neurons), bias(c.neurons), biasT(c.neurons, Point(c.outputDimension)),
      weightsT(biasT), rng(c.seed) {
  require(c.neurons > 0 && c.outputDimension > 0 && c.neurons < 10000,
          "Dimensiones de red inválidas.");
  for (double x : {c.lr, c.scaleX, c.scaleY, c.radius, c.radiusFallback, c.clusterMaxSpan})
    require(std::isfinite(x) && x > 0, "Escalas, radio y lr deben ser positivos.");
  require(c.zeroNormTolerance >= 0 && c.membershipMinScore >= 0 && c.membershipMinScore <= 1 &&
              c.membershipMinMargin >= 0 && c.membershipMinMargin <= 1,
          "Tolerancias de lectura inválidas.");
  require(c.initialDistanceMin > 0 && c.initialDistanceMax >= c.initialDistanceMin &&
              c.initialWeightRange >= 0,
          "Inicialización inválida.");
  initWeights();
}
void Model::initWeights() {
  rng.seed(config.seed);
  std::uniform_real_distribution<double> positive(0, 1),
      distance(config.initialDistanceMin, config.initialDistanceMax),
      trans(-config.initialWeightRange, config.initialWeightRange);
  auto weight = [&]() {
    double w;
    do {
      w = positive(rng);
    } while (w <= 0 || w >= 1);
    return w;
  };
  for (int i = 0; i < config.neurons; i++) {
    input[i] = weight();
    bias[i] = 0;
  }
  edges.clear();
  for (int i = 0; i < config.neurons; i++)
    for (int j = i + 1; j < config.neurons; j++)
      edges.push_back({i, j, weight(), true});
  std::vector<int> axis(config.neurons);
  for (int i = 0; i < config.neurons; i++)
    axis[i] = i % config.outputDimension;
  std::shuffle(axis.begin(), axis.end(), rng);
  for (int i = 0; i < config.neurons; i++) {
    std::fill(biasT[i].begin(), biasT[i].end(), 0);
    std::fill(weightsT[i].begin(), weightsT[i].end(), 0);
    biasT[i][axis[i]] = distance(rng);
    weightsT[i][axis[i]] = trans(rng);
  }
}
double Model::sigma(double z) const { return config.tanhActivation ? std::tanh(z) : z; }
double Model::sigmaDerivative(double z) const {
  double t = std::tanh(z);
  return config.tanhActivation ? 1 - t * t : 1;
}

/*=======FORWARD=======*/
Trace Model::forward(double x) const {
  require(std::isfinite(x), "Entrada no finita.");
  Trace trace;
  trace.x = x;
  trace.edges = edges;
  trace.biasT = biasT;
  trace.weightsT = weightsT;
  Point v(config.neurons);
  trace.initialZ = v;
  for (int i = 0; i < config.neurons; i++) {
    trace.initialZ[i] = input[i] * x + bias[i];
    v[i] = sigma(trace.initialZ[i]);
    require(std::isfinite(v[i]), "Potencial no finito.");
  }
  trace.initial = v;
  std::vector<bool> available(config.neurons, true);
  while (true) {
    std::vector<bool> higher(config.neurons, false), lower = higher;
    for (const auto &w : edges)
      if (w.active && available[w.i] && available[w.j]) {
        if (v[w.i] > v[w.j]) {
          lower[w.i] = true;
          higher[w.j] = true;
        } else if (v[w.j] > v[w.i]) {
          lower[w.j] = true;
          higher[w.i] = true;
        }
      }
    Step step;
    step.before = v;
    step.z = v;
    std::vector<bool> tx(config.neurons, false), rx = tx;
    for (int i = 0; i < config.neurons; i++)
      if (available[i] && !higher[i] && lower[i]) {
        tx[i] = true;
        step.transmitters.push_back(i);
      }
    if (step.transmitters.empty())
      break;
    require(trace.steps.size() < static_cast<std::size_t>(config.neurons),
            "Recorrido inconsistente: exceso de pasos.");
    for (std::size_t k = 0; k < edges.size(); k++) {
      const auto &w = edges[k];
      int i = w.i, j = w.j;
      if (!w.active || !available[i] || !available[j])
        continue;
      if (tx[j])
        std::swap(i, j);
      if (tx[i] && v[i] > v[j]) {
        require(!tx[j], "Emisión y recepción simultáneas.");
        step.z[j] += w.w * (v[i] - v[j]);
        rx[j] = true;
        step.connections.push_back(static_cast<int>(k));
      }
    }
    for (int j = 0; j < config.neurons; j++)
      if (rx[j]) {
        step.z[j] += bias[j];
        v[j] = sigma(step.z[j]);
        require(std::isfinite(v[j]), "Potencial no finito.");
        step.receivers.push_back(j);
      }
    for (int i : step.transmitters)
      available[i] = false;
    step.after = v;
    trace.steps.push_back(step);
  }
  trace.final = v;
  trace.q = biasT;
  for (int i = 0; i < config.neurons; i++)
    for (int k = 0; k < config.outputDimension; k++) {
      trace.q[i][k] += weightsT[i][k] * v[i];
      require(std::isfinite(trace.q[i][k]), "Posición no finita.");
    }
  return trace;
}

/*=======LOSS FUNCTION=======*/
Loss Model::loss(const Trace &trace, const Point &target) const {
  int N = config.neurons, T = config.outputDimension;
  require(target.size() == static_cast<std::size_t>(T), "Dimensión objetivo inválida.");
  require(trace.q.size() == static_cast<std::size_t>(N), "Traza inválida.");
  Points wells(T, Point(T));
  for (int k = 0; k < T; k++) {
    require(std::isfinite(target[k]), "Ideal no finito.");
    wells[k][k] = target[k];
  }
  Loss result;
  result.gradient = Points(N, Point(T));
  result.distanceSquared = Point(T, std::numeric_limits<double>::infinity());
  Points distances(N, Point(T));
  double r2 = config.radius * config.radius;
  for (int i = 0; i < N; i++)
    for (int k = 0; k < T; k++) {
      double d = squared(trace.q[i], wells[k]);
      distances[i][k] = d;
      result.distanceSquared[k] = std::min(result.distanceSquared[k], d);
    }
  for (int k = 0; k < T; k++)
    for (int j = k + 1; j < T; j++)
      if (squared(wells[k], wells[j]) == 0)
        result.coincidentWells = true;
  // Comparar distancias preserva el orden del mínimo incluso si la gaussiana satura.
  auto accumulate = [&](int i, int k, double factor) {
    double coefficient = std::exp(-distances[i][k] / (2 * r2)) * factor / r2;
    for (int j = 0; j < T; j++)
      result.gradient[i][j] += coefficient * (trace.q[i][j] - wells[k][j]);
  };
  for (int i = 0; i < N; i++) {
    double minimum = *std::min_element(distances[i].begin(), distances[i].end());
    int count = 0;
    for (double d : distances[i])
      if (d == minimum)
        count++;
    double value = -std::expm1(-minimum / (2 * r2));
    result.attraction += value / N;
    for (int k = 0; k < T; k++)
      if (distances[i][k] == minimum)
        accumulate(i, k, 1.0 / (N * count));
  }
  for (int k = 0; k < T; k++) {
    double minimum = result.distanceSquared[k];
    int count = 0;
    for (int i = 0; i < N; i++)
      if (distances[i][k] == minimum)
        count++;
    result.coverage += -std::expm1(-minimum / (2 * r2)) / T;
    for (int i = 0; i < N; i++)
      if (distances[i][k] == minimum)
        accumulate(i, k, 1.0 / (T * count));
  }
  result.total = result.attraction + result.coverage;
  return result;
}

/*=======BACKWARD=======*/
Gradients Model::backward(const Trace &trace, const Point &target) const {
  const auto geometric = loss(trace, target);
  Gradients g;
  int N = config.neurons, T = config.outputDimension;
  g.input = Point(N);
  g.bias = Point(N);
  g.edges = Point(edges.size());
  g.biasT = geometric.gradient;
  g.weightsT = Points(N, Point(T));
  Point delta(N);
  for (int i = 0; i < N; i++)
    for (int k = 0; k < T; k++) {
      g.weightsT[i][k] = trace.final[i] * g.biasT[i][k];
      delta[i] += weightsT[i][k] * g.biasT[i][k];
    }
  for (auto it = trace.steps.rbegin(); it != trace.steps.rend(); ++it) {
    const Step &step = *it;
    Point previous = delta;
    for (int j : step.receivers)
      previous[j] = 0;
    for (int j : step.receivers) {
      double d = delta[j] * sigmaDerivative(step.z[j]);
      g.bias[j] += d;
      double sum = 0;
      for (int idx : step.connections) {
        const auto &w = edges[idx];
        int i = w.i, dest = w.j;
        if (std::find(step.transmitters.begin(), step.transmitters.end(), dest) !=
            step.transmitters.end())
          std::swap(i, dest);
        if (dest != j)
          continue;
        sum += w.w;
        g.edges[idx] += d * (step.before[i] - step.before[j]);
        previous[i] += w.w * d;
      }
      previous[j] += (1 - sum) * d;
    }
    delta = previous;
  }
  for (int i = 0; i < N; i++) {
    double d = delta[i] * sigmaDerivative(trace.initialZ[i]);
    g.input[i] = d * trace.x;
    g.bias[i] += d;
  }
  return g;
}

/*=======PARAMETER UPDATE=======*/
void Model::parameterUpdate(const Gradients &g) {
  auto update = [&](double &v, double d) {
    require(std::isfinite(d), "Gradiente no finito.");
    v -= config.lr * d;
    require(std::isfinite(v), "Parámetro no finito.");
  };
  for (int i = 0; i < config.neurons; i++) {
    update(input[i], g.input.at(i));
    update(bias[i], g.bias.at(i));
    for (int k = 0; k < config.outputDimension; k++) {
      update(biasT[i][k], g.biasT.at(i).at(k));
      update(weightsT[i][k], g.weightsT.at(i).at(k));
    }
  }
  for (std::size_t k = 0; k < edges.size(); k++)
    if (edges[k].active) {
      update(edges[k].w, g.edges.at(k));
      if (edges[k].w <= 0)
        edges[k].active = false;
    }
}

/*=======READING WITHOUT TARGETS=======*/
Interpretation Model::interpret(const Trace &trace) const {
  int N = config.neurons, T = config.outputDimension;
  Interpretation result;
  result.axis = std::vector<int>(N, -1);
  result.scores = Points(N, Point(T));
  result.components.resize(T);
  for (int i = 0; i < N; i++) {
    int evidence = 0;
    for (const auto *v : {&trace.q[i], &trace.biasT[i], &trace.weightsT[i]}) {
      double norm = squared(*v, Point(T));
      if (norm <= config.zeroNormTolerance * config.zeroNormTolerance)
        continue;
      evidence++;
      for (int k = 0; k < T; k++)
        result.scores[i][k] += (*v)[k] * (*v)[k] / norm;
    }
    if (!evidence)
      continue;
    for (double &score : result.scores[i])
      score /= evidence;
    std::vector<int> axes(T);
    std::iota(axes.begin(), axes.end(), 0);
    std::sort(axes.begin(), axes.end(),
              [&](int a, int b) { return result.scores[i][a] > result.scores[i][b]; });
    int k = axes[0];
    double score = result.scores[i][k];
    if (score >= config.membershipMinScore &&
        (T == 1 || (score > result.scores[i][axes[1]] &&
                    score - result.scores[i][axes[1]] >= config.membershipMinMargin)))
      result.axis[i] = k;
  }
  for (int k = 0; k < T; k++) {
    std::vector<int> ids;
    for (int i = 0; i < N; i++)
      if (result.axis[i] == k)
        ids.push_back(i);
    std::sort(ids.begin(), ids.end(), [&](int a, int b) { return trace.q[a][k] < trace.q[b][k]; });
    auto &read = result.components[k];
    if (ids.empty())
      continue;
    for (int id : ids) {
      if (read.groups.empty() ||
          trace.q[id][k] - trace.q[read.groups.back().front()][k] > config.clusterMaxSpan)
        read.groups.push_back({});
      read.groups.back().push_back(id);
    }
    read.status = "ambiguo";
    for (const auto &group : read.groups)
      if (read.groups.size() == 1 || group.size() * 2 > ids.size()) {
        read.status = "válido";
        read.members = group;
        std::size_t mid = group.size() / 2;
        read.value = trace.q[group[mid]][k];
        if (group.size() % 2 == 0)
          read.value = (read.value + trace.q[group[mid - 1]][k]) / 2;
        read.span = trace.q[group.back()][k] - trace.q[group.front()][k];
        break;
      }
  }
  return result;
}
Metrics Model::validation(const std::vector<Example> &samples) const {
  require(!samples.empty(), "Dataset vacío.");
  Metrics m;
  m.examples = static_cast<int>(samples.size());
  int T = config.outputDimension;
  m.distanceSquared = Point(T);
  m.readSquaredError = Point(T);
  m.validCount = std::vector<int>(T);
  for (const auto &ej : samples) {
    auto trace = forward(ej.x);
    auto l = loss(trace, ej.y);
    auto read = interpret(trace);
    m.loss += l.total;
    m.attraction += l.attraction;
    m.coverage += l.coverage;
    m.meanSteps += trace.steps.size();
    if (l.coincidentWells)
      m.ambiguous++;
    for (int k = 0; k < T; k++) {
      m.distanceSquared[k] += l.distanceSquared[k];
      if (read.components[k].status == "válido") {
        m.validCount[k]++;
        double d = read.components[k].value - ej.y[k];
        m.readSquaredError[k] += d * d;
      }
    }
  }
  for (double *v : {&m.loss, &m.attraction, &m.coverage, &m.meanSteps})
    *v /= samples.size();
  for (double &v : m.distanceSquared)
    v /= samples.size();
  return m;
}

/*=======SAVE / LOAD=======*/
void Model::write(std::ostream &o) const {
  const auto &c = config;
  o << std::setprecision(17) << c.neurons << ' ' << c.outputDimension << ' ' << c.epochs << ' '
    << c.seed << ' ' << c.lr << ' ' << c.scaleX << ' ' << c.scaleY << ' ' << c.radius << ' '
    << c.radiusFallback << ' ' << c.zeroNormTolerance << ' ' << c.membershipMinScore << ' '
    << c.membershipMinMargin << ' ' << c.clusterMaxSpan << ' ' << c.initialDistanceMin << ' '
    << c.initialDistanceMax << ' ' << c.initialWeightRange << ' ' << c.tanhActivation << ' '
    << c.radiusUsedFallback << '\n';
  writePoint(o, input);
  writePoint(o, bias);
  for (const auto &v : biasT)
    writePoint(o, v);
  for (const auto &v : weightsT)
    writePoint(o, v);
  for (const auto &w : edges)
    o << w.w << ' ' << w.active << '\n';
  o << rng << '\n';
}
Model Model::read(std::istream &i) {
  Config c;
  i >> c.neurons >> c.outputDimension >> c.epochs >> c.seed >> c.lr >> c.scaleX >> c.scaleY >>
      c.radius >> c.radiusFallback >> c.zeroNormTolerance >> c.membershipMinScore >>
      c.membershipMinMargin >> c.clusterMaxSpan >> c.initialDistanceMin >> c.initialDistanceMax >>
      c.initialWeightRange >> c.tanhActivation >> c.radiusUsedFallback;
  require(bool(i), "Cabecera incompleta.");
  Model m(c);
  readPoint(i, m.input);
  readPoint(i, m.bias);
  for (auto &v : m.biasT)
    readPoint(i, v);
  for (auto &v : m.weightsT)
    readPoint(i, v);
  for (auto &w : m.edges) {
    i >> w.w >> w.active;
    require(std::isfinite(w.w) && (!w.active || w.w > 0), "Conexión inválida.");
  }
  i >> m.rng;
  require(bool(i), "Modelo incompleto.");
  return m;
}
Laboratory::Laboratory(Config c, Dataset dataset)
    : model(c), bestModel(model), data(std::move(dataset)), bestLoss(0) {
  require(!data.training.empty() && !data.validation.empty(), "Datasets vacíos.");
  for (const auto *set : {&data.training, &data.validation})
    for (const auto &v : *set) {
      require(std::isfinite(v.x) && v.y.size() == static_cast<std::size_t>(c.outputDimension),
              "Dataset incompatible.");
      for (double y : v.y)
        require(std::isfinite(y), "Objetivo no finito.");
    }
  order.resize(data.training.size());
  std::iota(order.begin(), order.end(), 0);
  bestOrder = order;
  auto a = model.validation(data.training), b = model.validation(data.validation);
  bestLoss = b.loss;
  history.push_back({0, a, b});
  inspect(data.validation.front().x * c.scaleX);
}
bool Laboratory::advanceExample() {
  if (cursor == 0)
    std::shuffle(order.begin(), order.end(), model.rng);
  const auto &ej = data.training[order.at(cursor)];
  inspected = model.forward(ej.x);
  inspectedTarget = ej.y;
  reveal = true;
  model.parameterUpdate(model.backward(inspected, ej.y));
  cursor++;
  if (cursor == static_cast<int>(order.size())) {
    cursor = 0;
    e++;
    modelEpoch++;
    auto a = model.validation(data.training), b = model.validation(data.validation);
    history.push_back({e, a, b});
    if (b.loss < bestLoss) {
      bestLoss = b.loss;
      bestModel = model;
      bestEpoch = e;
      bestModelEpoch = modelEpoch;
      bestOrder = order;
    }
    return true;
  }
  return false;
}
void Laboratory::inspect(double physicalInput, bool showIdeal) {
  auto candidate = model.forward(physicalInput / model.config.scaleX);
  Point target;
  if (showIdeal) {
    auto physical = physicalTargets(physicalInput);
    require(physical.size() == static_cast<std::size_t>(model.config.outputDimension),
            "Ideal no disponible para esta dimensión.");
    for (double value : physical)
      target.push_back(value / model.config.scaleY);
  }
  inspected = std::move(candidate);
  inspectedTarget = std::move(target);
  reveal = showIdeal;
}

void Laboratory::save(const std::string &path, bool best) const {
  std::ofstream o(path);
  require(bool(o), "No se pudo guardar el modelo.");
  o << "DRANN_LAB_1\n"
    << e << ' ' << (best ? bestModelEpoch : modelEpoch) << ' ' << bestEpoch << ' ' << bestModelEpoch
    << ' ' << std::setprecision(17) << bestLoss << ' ' << (best ? 0 : cursor) << '\n';
  (best ? bestModel : model).write(o);
  bestModel.write(o);
  writeData(o, data.training);
  writeData(o, data.validation);
  for (int n : (best ? bestOrder : order))
    o << n << ' ';
  o << '\n';
  for (int n : bestOrder)
    o << n << ' ';
  o << '\n';
  require(bool(o), "Error al guardar.");
}
Laboratory Laboratory::load(const std::string &path) {
  std::ifstream i(path);
  std::string version;
  i >> version;
  require(version == "DRANN_LAB_1", "Formato de modelo incompatible.");
  int e = 0, me = 0, be = 0, bme = 0, cursor = 0;
  double best = 0;
  i >> e >> me >> be >> bme >> best >> cursor;
  auto m = Model::read(i);
  auto b = Model::read(i);
  Dataset d;
  d.training = readData(i, m.config.outputDimension);
  d.validation = readData(i, m.config.outputDimension);
  Laboratory lab(m.config, d);
  lab.model = m;
  lab.bestModel = b;
  lab.e = e;
  lab.modelEpoch = me;
  lab.bestEpoch = be;
  lab.bestModelEpoch = bme;
  lab.bestLoss = best;
  lab.cursor = cursor;
  for (int &n : lab.order)
    i >> n;
  for (int &n : lab.bestOrder)
    i >> n;
  auto sorted = lab.order;
  std::sort(sorted.begin(), sorted.end());
  for (std::size_t n = 0; n < sorted.size(); n++)
    require(sorted[n] == static_cast<int>(n), "Orden inválido.");
  sorted = lab.bestOrder;
  std::sort(sorted.begin(), sorted.end());
  for (std::size_t n = 0; n < sorted.size(); n++)
    require(sorted[n] == static_cast<int>(n), "Orden del mejor modelo inválido.");
  require(b.config.neurons == m.config.neurons &&
              b.config.outputDimension == m.config.outputDimension,
          "Modelos incompatibles.");
  require(bool(i) && std::isfinite(best) && best >= 0 && bme >= 0 && cursor >= 0 &&
              cursor < static_cast<int>(sorted.size()) && e >= 0 && me >= 0 && be >= 0,
          "Estado inválido.");
  lab.history.clear();
  lab.history.push_back({e, m.validation(d.training), m.validation(d.validation)});
  lab.inspect(d.validation.front().x * m.config.scaleX);
  return lab;
}
