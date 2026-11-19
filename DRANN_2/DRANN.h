#pragma once
#include <iosfwd>
#include <random>
#include <string>
#include <vector>

using Point = std::vector<double>;
using Points = std::vector<Point>;
struct Config {
  int neurons = 5, outputDimension = 2, epochs = 1000;
  unsigned seed = 42;
  double lr = .005, scaleX = 100, scaleY = 373.15, radius = .5, radiusFallback = .5;
  double zeroNormTolerance = 1e-12, membershipMinScore = .75, membershipMinMargin = .20,
         clusterMaxSpan = .05;
  double initialDistanceMin = .01, initialDistanceMax = .05, initialWeightRange = .05;
  bool tanhActivation = false, radiusUsedFallback = false;
};
struct Example {
  double x;
  Point y;
};
struct Dataset {
  std::vector<Example> training, validation;
};
struct Connection {
  int i, j;
  double w;
  bool active;
};
struct Step {
  Point before, after, z;
  std::vector<int> transmitters, receivers;
  std::vector<int> connections;
};
struct Trace {
  double x = 0;
  Point initial, initialZ, final;
  Points q, biasT, weightsT;
  std::vector<Step> steps;
  std::vector<Connection> edges;
};
struct Reading {
  std::string status = "sin representantes";
  double value = 0, span = 0;
  std::vector<int> members;
  std::vector<std::vector<int>> groups;
};
struct Interpretation {
  std::vector<int> axis;
  Points scores;
  std::vector<Reading> components;
};
struct Loss {
  double attraction = 0, coverage = 0, total = 0;
  Point distanceSquared;
  Points gradient;
  bool coincidentWells = false;
};
struct Gradients {
  Point input, bias, edges;
  Points biasT, weightsT;
};
struct Metrics {
  double loss = 0, attraction = 0, coverage = 0, meanSteps = 0;
  Point distanceSquared, readSquaredError;
  std::vector<int> validCount;
  int examples = 0, ambiguous = 0;
};
struct Model {
  Config config;
  Point input, bias;
  Points biasT, weightsT;
  std::vector<Connection> edges;
  std::mt19937 rng;
  explicit Model(Config c);
  void initWeights();
  double sigma(double z) const;
  double sigmaDerivative(double z) const;
  Trace forward(double x) const;
  Loss loss(const Trace &trace, const Point &target) const;
  Gradients backward(const Trace &trace, const Point &target) const;
  void parameterUpdate(const Gradients &g);
  Interpretation interpret(const Trace &trace) const;
  Metrics validation(const std::vector<Example> &samples) const;
  void write(std::ostream &out) const;
  static Model read(std::istream &in);
};
struct Epoch {
  int e;
  Metrics training, validation;
};
struct Laboratory {
  Model model, bestModel;
  Dataset data;
  int e = 0, modelEpoch = 0, bestEpoch = 0, bestModelEpoch = 0, cursor = 0;
  double bestLoss;
  std::vector<int> order, bestOrder;
  std::vector<Epoch> history;
  Trace inspected;
  Point inspectedTarget;
  bool reveal = false;
  explicit Laboratory(Config c, Dataset dataset);
  bool advanceExample();
  void inspect(double physicalInput, bool showIdeal = false);
  void save(const std::string &path, bool best = false) const;
  static Laboratory load(const std::string &path);
};
Config laboratoryConfig();
Dataset laboratoryDataset(Config &config);
Point physicalTargets(double c);
