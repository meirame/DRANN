#include "DRANN.h"
#include "utils.h"
#include <filesystem>

/*=======SHARED CONFIGURATION (CONSOLE AND GODOT)=======*/
Config laboratoryConfig() {
  Config c;
  c.neurons = 5;
  c.outputDimension = 2;
  c.seed = 42;
  c.lr = .005;
  c.epochs = 1000;
  c.tanhActivation = false;
  c.radius = 0; // Cero: mitad de la mediana de distancias entre pozos de entrenamiento.
  c.radiusFallback = .5;
  c.zeroNormTolerance = 1e-12;
  c.membershipMinScore = .75;
  c.membershipMinMargin = .20;
  c.clusterMaxSpan = .05;
  c.initialDistanceMin = .01;
  c.initialDistanceMax = .05;
  c.initialWeightRange = .05;
  return c;
}
Point physicalTargets(double c) { return {1.8 * c + 32, c + 273.15}; }
Dataset laboratoryDataset(Config &c) {
  std::vector<double> samplesTraining = {-40, -20, 0, 20, 40, 60, 80, 100};
  std::vector<double> samplesValidation = {10, 70};
  c.scaleX = 0;
  c.scaleY = 0;
  for (double x : samplesTraining) {
    c.scaleX = std::max(c.scaleX, std::abs(x));
    for (double y : physicalTargets(x))
      c.scaleY = std::max(c.scaleY, y);
  }
  if (c.outputDimension != 2)
    throw std::runtime_error(
        "Este dataset define dos coordenadas; adapte las etiquetas para otro T.");
  Dataset d;
  for (double x : samplesTraining) {
    Point y = physicalTargets(x);
    for (double &v : y)
      v /= c.scaleY;
    d.training.push_back({x / c.scaleX, y});
  }
  for (double x : samplesValidation) {
    Point y = physicalTargets(x);
    for (double &v : y)
      v /= c.scaleY;
    d.validation.push_back({x / c.scaleX, y});
  }
  c.radiusUsedFallback = false;
  if (c.radius <= 0) {
    Point distances;
    for (const auto &ej : d.training)
      for (int k = 0; k < c.outputDimension; k++)
        for (int j = k + 1; j < c.outputDimension; j++) {
          double distance = std::hypot(ej.y[k], ej.y[j]);
          if (distance > 0)
            distances.push_back(distance);
        }
    if (distances.empty()) {
      c.radius = c.radiusFallback;
      c.radiusUsedFallback = true;
    } else {
      std::sort(distances.begin(), distances.end());
      std::size_t m = distances.size() / 2;
      c.radius = .5 * (distances.size() % 2 ? distances[m] : (distances[m - 1] + distances[m]) / 2);
    }
  }
  return d;
}
#ifdef DRANN_CONSOLE
int main(int argc, char **argv) {
  try {
    Config c = laboratoryConfig();
    std::string loadPath, outPath;
    int epochs = c.epochs;
    for (int i = 1; i < argc; i++) {
      std::string key = argv[i];
      if (i + 1 == argc)
        throw std::runtime_error("Falta valor del argumento.");
      std::string value = argv[++i];
      if (key == "--epochs")
        epochs = std::stoi(value);
      else if (key == "--load")
        loadPath = value;
      else if (key == "--out")
        outPath = value;
      else
        throw std::runtime_error("Uso: drann --epochs N [--load modelo] [--out carpeta_nueva]");
    }
    if (epochs < 1)
      throw std::runtime_error("Épocas positivas requeridas.");
    Dataset data = laboratoryDataset(c);
    Laboratory lab = loadPath.empty() ? Laboratory(c, data) : Laboratory::load(loadPath);
    c = lab.model.config;
    if (outPath.empty())
      outPath = "results/run_" +
                std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    if (!std::filesystem::create_directories(outPath))
      throw std::runtime_error("Use una carpeta de resultados nueva.");
    std::ofstream history(outPath + "/history.csv"), examples(outPath + "/examples.csv"),
        points(outPath + "/neurons.csv");
    history << std::setprecision(17);
    examples << std::setprecision(17);
    points << std::setprecision(17);
    history << "epoch,model_epoch,best_epoch,training_loss,validation_loss,attraction,coverage,"
               "mean_steps";
    for (int k = 0; k < c.outputDimension; k++)
      history << ",well_distance2_" << k << ",read_valid_" << k << ",read_total_" << k
              << ",read_sse_normalized_" << k;
    history << '\n';
    examples << "epoch,input_physical,axis,status,value_physical,target_physical,members,span_"
                "normalized\n";
    points << "epoch,input_physical,neuron,axis,coordinate,position_normalized,membership,"
              "specialization_changes,evidence_disagreement,impulse,orientation\n";
    utils util;
    auto started = utils::Clock::now();
    int start = lab.e, goal = start + epochs;
    util.line(15, "DRANN | Celsius -> pozos Fahrenheit / Kelvin");
    util.line(10, "Training will start.");
    std::vector<std::vector<int>> previousAxes(lab.data.validation.size(),
                                               std::vector<int>(c.neurons, -2));
    std::vector<std::vector<int>> changes(lab.data.validation.size(),
                                          std::vector<int>(c.neurons, 0));
    auto observations = [&]() {
      for (std::size_t ej = 0; ej < lab.data.validation.size(); ej++) {
        const auto &sample = lab.data.validation[ej];
        auto trace = lab.model.forward(sample.x);
        auto read = lab.model.interpret(trace);
        for (int k = 0; k < c.outputDimension; k++) {
          const auto &r = read.components[k];
          examples << lab.e << ',' << sample.x * c.scaleX << ',' << k << ',' << r.status << ',';
          if (r.status == "válido")
            examples << r.value * c.scaleY;
          examples << ',' << sample.y[k] * c.scaleY << ',';
          for (int id : r.members)
            examples << id << ';';
          examples << ',' << r.span << '\n';
        }
        for (int i = 0; i < c.neurons; i++) {
          if (previousAxes[ej][i] != -2 && previousAxes[ej][i] != read.axis[i])
            changes[ej][i]++;
          previousAxes[ej][i] = read.axis[i];
          int evidenceAxis = -1;
          bool disagreement = false;
          for (const auto *v : {&trace.q[i], &trace.biasT[i], &trace.weightsT[i]}) {
            double norm = 0;
            int axis = 0;
            for (int k = 0; k < c.outputDimension; k++) {
              norm += (*v)[k] * (*v)[k];
              if (std::abs((*v)[k]) > std::abs((*v)[axis]))
                axis = k;
            }
            if (norm <= c.zeroNormTolerance * c.zeroNormTolerance)
              continue;
            if (evidenceAxis != -1 && evidenceAxis != axis)
              disagreement = true;
            evidenceAxis = axis;
          }
          double impulse = 0;
          for (double w : trace.weightsT[i])
            impulse = std::hypot(impulse, w * trace.final[i]);
          for (int k = 0; k < c.outputDimension; k++) {
            points << lab.e << ',' << sample.x * c.scaleX << ',' << i << ',' << read.axis[i] << ','
                   << k << ',' << trace.q[i][k] << ',' << read.scores[i][k] << ',' << changes[ej][i]
                   << ',' << disagreement << ',' << impulse << ',';
            if (impulse > c.zeroNormTolerance)
              points << trace.weightsT[i][k] * trace.final[i] / impulse;
            points << '\n';
          }
        }
      }
    };
    auto record = [&]() {
      observations();
      auto &row = lab.history.back();
      history << lab.e << ',' << lab.modelEpoch << ',' << lab.bestEpoch << ',' << row.training.loss
              << ',' << row.validation.loss << ',' << row.validation.attraction << ','
              << row.validation.coverage << ',' << row.validation.meanSteps;
      for (int k = 0; k < c.outputDimension; k++)
        history << ',' << row.validation.distanceSquared[k] << ',' << row.validation.validCount[k]
                << ',' << row.validation.examples << ',' << row.validation.readSquaredError[k];
      history << '\n';
      if (lab.e == start || lab.e == goal || ((lab.e - start) % std::max(1, epochs / 10) == 0)) {
        util.line(8, "------------------------------------------------");
        util.line(15, "Epoch -> " + std::to_string(lab.e) + " / " + std::to_string(goal));
        std::ostringstream msg;
        msg << std::setprecision(10) << "Training Loss: " << row.training.loss
            << " | Validation Loss: " << row.validation.loss << "\nActive connections: "
            << std::count_if(lab.model.edges.begin(), lab.model.edges.end(),
                             [](const Connection &w) { return w.active; })
            << " | Participating neurons: " << c.neurons
            << " | Steps: " << row.validation.meanSteps;
        util.line(15, msg.str());
        util.line(15, util.progress(lab.e, goal, start, utils::Clock::now() - started));
      }
    };
    record();
    while (lab.e < goal)
      if (lab.advanceExample())
        record();
    lab.save(outPath + "/final.model");
    lab.save(outPath + "/best.model", true);
    const auto &metrics = lab.history.back().validation;
    for (const auto &ej : lab.data.validation) {
      auto trace = lab.model.forward(ej.x);
      auto reading = lab.model.interpret(trace);
      auto geometric = lab.model.loss(trace, ej.y);
      util.line(14, "Validation input: " + std::to_string(ej.x * c.scaleX) + " Celsius");
      for (int k = 0; k < c.outputDimension; k++) {
        const auto &r = reading.components[k];
        std::ostringstream text;
        text << "Axis " << k << " | " << r.status << " | prediction: ";
        if (r.status == "válido")
          text << r.value * c.scaleY;
        else
          text << "—";
        text << " | ideal: " << ej.y[k] * c.scaleY
             << " | E_k distance²: " << geometric.distanceSquared[k] << " | members: ";
        for (int id : r.members)
          text << id << ' ';
        util.line(15, text.str());
      }
    }
    std::ofstream metadata(outPath + "/metadata.txt");
    metadata << std::setprecision(17)
             << "Godot=4.5.1-stable\ngodot-cpp=e83fd0904c13356ed1d4c3d09f8bb9132bdc6b77\n"
             << "configuration_fields: neurons outputDimension epochs seed lr scaleX scaleY radius "
                "radiusFallback zeroNormTolerance membershipMinScore membershipMinMargin "
                "clusterMaxSpan initialDistanceMin initialDistanceMax initialWeightRange "
                "tanhActivation radiusUsedFallback\n";
    lab.model.write(metadata);
    metadata << "executed_epoch=" << lab.e << "\nmodel_epoch=" << lab.modelEpoch
             << "\nbest_epoch=" << lab.bestEpoch << "\nbest_model_epoch=" << lab.bestModelEpoch
             << "\nrequested_additional_epochs=" << epochs << "\n";
    for (int k = 0; k < c.outputDimension; k++) {
      std::ostringstream text;
      text << "Axis " << k << " | valid readings: " << metrics.validCount[k] << '/'
           << metrics.examples << " | RMSE physical: ";
      if (metrics.validCount[k])
        text << c.scaleY * std::sqrt(metrics.readSquaredError[k] / metrics.validCount[k]);
      else
        text << "—";
      util.line(15, text.str());
    }
    util.line(10, "Training has finished. Final y mejor modelo guardados en " + outPath);
    util.line(8, "Total training duration: " + utils::duration(utils::Clock::now() - started));
    return 0;
  } catch (const std::exception &e) {
    utils err(std::cerr);
    err.line(4, e.what());
    return 1;
  }
}
#endif
