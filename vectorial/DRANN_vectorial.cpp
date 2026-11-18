#include "entrenamiento.h"
#include "terminal.h"
#include "graphics/metrics.h"

#include <filesystem>
#include <fstream>

#ifdef DRANN_GUI
#include "graphics/DRANN_plot.h"
#endif

namespace fs = std::filesystem;
using namespace vectorial;

struct Options {
  Config training;
  std::string dataset;
  fs::path output;
  int epochs, neurons;
  unsigned seed;
  double lr;
  bool batch, help;
};

int parseInt(const std::string &s, int minimum) {
  std::size_t used = 0;
  const int n = std::stoi(s, &used);
  if (used != s.size() || n < minimum || n >= std::numeric_limits<int>::max())
    throw std::invalid_argument("Entero fuera de rango: " + s);
  return n;
}

double parseReal(const std::string &s) {
  std::size_t used = 0;
  const double value = std::stod(s, &used);
  if (used != s.size() || !std::isfinite(value) || value < 0)
    throw std::invalid_argument("Número no negativo requerido: " + s);
  return value;
}

// Sobrescribir solo las opciones indicadas; los valores por defecto están en main().
void parseOptions(int argc, char **argv, Options &opt) {
  Config &c = opt.training;
  for (int i = 1; i < argc; ++i) {
    const std::string key = argv[i];
    if (key == "--batch") {
      opt.batch = true;
      continue;
    }
    if (key == "--help") {
      opt.help = true;
      continue;
    }
    if (i + 1 == argc)
      throw std::invalid_argument("Falta un valor para " + key);
    const std::string value = argv[++i];
    if (key == "--dataset") opt.dataset = value;
    else if (key == "--epochs") opt.epochs = parseInt(value, 1);
    else if (key == "--seed") opt.seed = static_cast<unsigned>(parseInt(value, 0));
    else if (key == "--neurons") opt.neurons = parseInt(value, 2);
    else if (key == "--lr") opt.lr = parseReal(value);
    else if (key == "--out") opt.output = value;
    else if (key == "--lambda") c.lambdaTarget = parseReal(value);
    else if (key == "--warmup") c.warmup = parseInt(value, 0);
    else if (key == "--patience") c.patience = parseInt(value, 0);
    else if (key == "--ramp") c.ramp = parseInt(value, 1);
    else if (key == "--pressure") c.maxPressure = parseInt(value, 1);
    else if (key == "--recovery") c.maxRecovery = parseInt(value, 0);
    else if (key == "--attempts") c.maxAttempts = parseInt(value, 1);
    else if (key == "--min-improvement") c.minImprovement = parseReal(value);
    else if (key == "--relative-tolerance") c.relativeTolerance = parseReal(value);
    else if (key == "--prox-abs-tol") c.prox.absTolerance = parseReal(value);
    else if (key == "--prox-rel-tol") c.prox.relTolerance = parseReal(value);
    else if (key == "--zero-tolerance") c.prox.zeroTolerance = parseReal(value);
    else if (key == "--max-sweeps") c.prox.maxSweeps = parseInt(value, 1);
    else if (key == "--absolute-tolerance") {
      const auto comma = value.find(',');
      c.absoluteTolerance = {parseReal(value.substr(0, comma))};
      if (comma != std::string::npos)
        c.absoluteTolerance.push_back(parseReal(value.substr(comma + 1)));
    } else {
      throw std::invalid_argument("Opción desconocida: " + key);
    }
  }
}

struct Problem {
  Dataset train, validation;
  double outputScale;
  std::string labels;
};

std::ofstream openFile(const fs::path &path) {
  std::ofstream out(path);
  if (!out)
    throw std::runtime_error("No se pudo abrir " + path.string());
  out.exceptions(std::ios::badbit | std::ios::failbit);
  out << std::setprecision(17);
  return out;
}

void writeOptions(std::ostream &out, const Options &opt, const Problem &problem) {
  const Config &c = opt.training;
  out << "dataset=" << opt.dataset << "\ncoordinates=" << problem.labels
      << "\noutput_scale=" << problem.outputScale
      << "\ntraining_samples=" << problem.train.size()
      << "\nvalidation_samples=" << problem.validation.size()
      << "\ninitial_goal=" << opt.epochs << "\nneurons=" << opt.neurons
      << "\nseed=" << opt.seed << "\nlr=" << opt.lr << "\nlambda_target=" << c.lambdaTarget
      << "\nwarmup=" << c.warmup << "\npatience=" << c.patience << "\nramp=" << c.ramp
      << "\nmax_pressure=" << c.maxPressure << "\nmax_recovery=" << c.maxRecovery
      << "\nmax_attempts=" << c.maxAttempts << "\nmin_improvement=" << c.minImprovement
      << "\nrelative_tolerance=" << c.relativeTolerance << "\nabsolute_tolerance=";
  for (std::size_t i = 0; i < c.absoluteTolerance.size(); ++i)
    out << (i ? ";" : "") << c.absoluteTolerance[i];
  out << "\nprox_abs_tol=" << c.prox.absTolerance << "\nprox_rel_tol=" << c.prox.relTolerance
      << "\nzero_tolerance=" << c.prox.zeroTolerance << "\nmax_sweeps=" << c.prox.maxSweeps
      << "\npresupuesto=epocas_de_trabajo; sin_barajar; SGD_por_ejemplo\n";
}

std::string quote(const std::string &value) {
  std::string result = "\"";
  for (char c : value) {
    result += c;
    if (c == '"')
      result += c;
  }
  return result + '"';
}

std::string ids(const std::vector<std::size_t> &values) {
  std::ostringstream out;
  for (std::size_t i = 0; i < values.size(); ++i)
    out << (i ? ";" : "") << values[i];
  return out.str();
}

struct Journal {
  fs::path directory;
  const Options &options;
  const Problem &problem;
  std::ofstream history, neurons, events;
  std::size_t referencesSaved = 0, startsSaved = 0;

  Journal(const fs::path &path, const Options &settings, const Problem &dataset)
      : directory(path), options(settings), problem(dataset),
        history(openFile(path / "history.csv")), neurons(openFile(path / "neurons.csv")),
        events(openFile(path / "events.csv")) {
    history << "work_epoch,model_epoch,goal,phase,attempt,reference_work,processed,lambda,applied_lambda,"
               "R,training_objective,training_loss,validation_loss,training_loss_0,training_loss_1,"
               "validation_loss_0,validation_loss_1,training_steps,validation_steps,"
               "training_worst_point,validation_worst_point,active_edges,participants,"
               "training_terminal_ties,validation_terminal_ties,training_near_ties,validation_near_ties,"
               "prox_gap,prox_sweeps,newly_isolated,decision,elapsed_s\n";
    neurons << "work_epoch,neuron,participates,weight_sum,weight_max,sent_norm,received_norm,"
               "transmission_steps,reception_steps,distances_by_id\n";
    events << "work_epoch,phase,attempt,reference_work,decision,newly_isolated,"
              "trial_train,trial_val,trial_train_0,trial_train_1,trial_val_0,trial_val_1,"
              "trial_worst_train,trial_worst_val,trial_edges,trial_participants,"
              "result_train,result_val,result_edges,result_participants\n";
    auto meta = openFile(path / "metadata.txt");
    writeOptions(meta, options, problem);
  }

  void save(const Snapshot &snapshot, const std::string &filename,
            std::size_t attempts, int currentWork) const {
    auto out = openFile(directory / filename);
    writeOptions(out, options, problem);
    out << "checkpoint_work=" << snapshot.work << "\nmodel_epoch=" << snapshot.modelEpoch
        << "\nsession_work_when_saved=" << currentWork
        << "\nattempts_started=" << attempts << "\nphase=" << name(snapshot.phase)
        << "\nlast_improvement_work=" << snapshot.lastImprovement << "\nplateau_loss=" << snapshot.plateau
        << "\ntraining_loss=" << snapshot.measurements.training.loss
        << "\nvalidation_loss=" << snapshot.measurements.validation.loss
        << "\nmt19937=" << snapshot.model.rng << "\n";
    for (std::size_t i = 0; i < snapshot.model.input.size(); ++i) {
      out << "neuron " << i;
      for (double value : snapshot.model.input[i])
        out << ' ' << value;
      out << " |";
      for (double value : snapshot.model.bias[i])
        out << ' ' << value;
      out << '\n';
    }
    for (const auto &edge : snapshot.model.edges)
      out << "edge " << edge.i << ' ' << edge.j << ' ' << edge.w << ' ' << edge.active << '\n';
  }

  void saveAttempts(const Training &training) {
    while (referencesSaved < training.references.size()) {
      const std::size_t i = referencesSaved++;
      save(training.references[i], "pre_attempt_" + std::to_string(i + 1) + ".txt", i + 1, training.work);
    }
    while (startsSaved < training.reductionStarts.size()) {
      const auto &start = training.reductionStarts[startsSaved++];
      save(start.second, "recovery_0_" + std::to_string(start.first) + ".txt", start.first, training.work);
    }
  }

  void failure(const Training &training, const std::runtime_error &error) const {
    save(training.bestPredictive, "best_predictive.txt", training.references.size(), training.work);
    auto out = openFile(directory / "failure.txt");
    out << "work_epoch_fallida=" << training.work
        << "\nultima_epoca_registrada=" << training.work - 1
        << "\nmotivo=" << error.what() << '\n';
  }

  void event(const Epoch &epoch, const Measurements &current) {
    if (epoch.decision.empty())
      return;
    events << epoch.work << ',' << name(epoch.phase) << ',' << epoch.attempt << ','
           << epoch.referenceWork << ',' << quote(epoch.decision) << ','
           << ids(epoch.newlyIsolated) << ',';
    if (epoch.trial) {
      const auto &trial = *epoch.trial;
      events << trial.training.loss << ',' << trial.validation.loss << ','
             << trial.training.coordinates[0] << ',' << trial.training.coordinates[1] << ','
             << trial.validation.coordinates[0] << ',' << trial.validation.coordinates[1] << ','
             << trial.training.worstPoint << ',' << trial.validation.worstPoint << ','
             << trial.connections << ',' << trial.neurons() << ',';
    } else {
      events << ",,,,,,,,,,";
    }
    events << current.training.loss << ',' << current.validation.loss << ','
           << current.connections << ',' << current.neurons() << '\n' << std::flush;
  }

  void record(const Epoch &epoch, const Training &training, int goal, double seconds) {
    saveAttempts(training);
    const auto &m = training.current;
    history << epoch.work << ',' << epoch.modelEpoch << ',' << goal << ',' << name(epoch.phase)
            << ',' << epoch.attempt << ',' << epoch.referenceWork << ',' << epoch.processed
            << ',' << epoch.lambda << ',' << epoch.appliedLambda << ',' << m.regularizer
            << ',' << m.training.loss + epoch.lambda * m.regularizer
            << ',' << m.training.loss << ',' << m.validation.loss
            << ',' << m.training.coordinates[0] << ',' << m.training.coordinates[1]
            << ',' << m.validation.coordinates[0] << ',' << m.validation.coordinates[1]
            << ',' << m.training.meanSteps << ',' << m.validation.meanSteps
            << ',' << m.training.worstPoint << ',' << m.validation.worstPoint
            << ',' << m.connections << ',' << m.neurons() << ',' << m.training.equalModuleStops
            << ',' << m.validation.equalModuleStops << ',' << m.training.nearTies
            << ',' << m.validation.nearTies << ',' << epoch.gap << ',' << epoch.sweeps
            << ',' << ids(epoch.newlyIsolated) << ',' << quote(epoch.decision)
            << ',' << seconds << '\n' << std::flush;
    for (std::size_t i = 0; i < m.validation.neurons.size(); ++i) {
      const auto &n = m.validation.neurons[i];
      neurons << epoch.work << ',' << i << ',' << m.participating[i] << ',' << n.weightSum
              << ',' << n.weightMax << ',' << n.sentMagnitude << ',' << n.receivedMagnitude
              << ',' << n.transmitterSteps << ',' << n.receiverSteps << ',';
      for (std::size_t j = 0; j < n.distances.size(); ++j) {
        if (j != 0)
          neurons << ';';
        if (i != j && m.participating[i] && m.participating[j])
          neurons << n.distances[j];
        else
          neurons << '-';
      }
      neurons << '\n';
    }
    neurons.flush();
    event(epoch, m);
  }
};

fs::path makeOutput(const Options &options) {
  fs::path path = options.output;
  if (path.empty()) {
    const auto stamp = std::chrono::system_clock::now().time_since_epoch();
    path = fs::path("results") / (options.dataset + "_seed" + std::to_string(options.seed) + "_" +
           std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(stamp).count()));
  }
  if (!fs::create_directories(path))
    throw std::runtime_error("La carpeta de salida ya existe: " + path.string());
  return fs::absolute(path);
}

void reportFinal(const Training &training, const Problem &problem, const drannConsole &console) {
  for (const auto &example : problem.validation) {
    const Trace trace = training.model.forwardRhizome(example.x);
    const Point losses = training.model.coordinateLoss(trace, example.ideal);
    std::ostringstream text;
    text << std::setprecision(10) << "Input (" << (problem.outputScale == 100 ? "C" : "x")
         << "): " << example.physicalInput << " | Ideal (" << problem.labels << "): ("
         << example.ideal[0] * problem.outputScale << ", " << example.ideal[1] * problem.outputScale
         << ")\nCognifenomeno primitivo (physical): [";
    bool firstPoint = true;
    for (std::size_t i = 0; i < trace.final.size(); ++i) {
      if (!trace.participating[i])
        continue;
      text << (firstPoint ? "(" : ", (") << trace.final[i][0] * problem.outputScale
           << ", " << trace.final[i][1] * problem.outputScale << ')';
      firstPoint = false;
    }
    text << "]\nCoordinate Loss: (" << losses[0] << ", " << losses[1]
         << ") | Total Loss: " << losses[0] + losses[1] << " | Steps: " << trace.steps.size();
    console.line(15, text.str());
    console.line(8, "------------------------------------------------");
  }
}

void printHelp() {
  std::cout << "DRANN vectorial | --dataset celsius|sincos --epochs N --neurons N --seed N --lr X\n"
               "  --lambda X --warmup N --patience N --min-improvement X --ramp N\n"
               "  --pressure N --recovery N --attempts N --absolute-tolerance X[,Y]\n"
               "  --relative-tolerance X --zero-tolerance X --prox-abs-tol X --prox-rel-tol X\n"
               "  --max-sweeps N --out CARPETA_NUEVA --batch\n";
}

int main(int argc, char **argv) {
  try {
    /*=======CONFIGURATION=======*/
    // Editar aquí los valores usados al ejecutar con F6.
    int neuronNumber = 4;
    double lr = 0.001;
    int epochs = 50000;
    unsigned seed = 42;
    std::size_t outputDimension = 2;
    std::string dataset = "sincos"; // "celsius" o "sincos".
    fs::path output; // Vacío: crear una carpeta nueva dentro de results/.
    bool batch = true; // true: salir al agotar las épocas, sin preguntar.

    /*=======PRESSURE AND RECOVERY CONFIGURATION=======*/
    Config trainingConfig;
    trainingConfig.lambdaTarget = 0.0; // Cero desactiva la presión de reducción.
    trainingConfig.minImprovement = 1e-6;
    trainingConfig.relativeTolerance = 0.05;
    trainingConfig.absoluteTolerance = {1e-6};

    trainingConfig.warmup = 5000;
    trainingConfig.patience = 500;
    trainingConfig.ramp = 1000;
    trainingConfig.maxPressure = 10000;
    trainingConfig.maxRecovery = 5000;
    trainingConfig.maxAttempts = 1;

    trainingConfig.prox.absTolerance = 1e-10;
    trainingConfig.prox.relTolerance = 1e-10;
    trainingConfig.prox.zeroTolerance = 1e-12;
    trainingConfig.prox.maxSweeps = 2000;

    // Los argumentos de terminal tienen prioridad sobre los valores anteriores.
    Options opt{trainingConfig, dataset, output, epochs, neuronNumber, seed, lr, batch, false};
    parseOptions(argc, argv, opt);
    if (opt.help) {
      printHelp();
      return 0;
    }

    /*=======DATASET: INPUTS AND OUTPUTS=======*/
    Problem problem;
    if (opt.dataset == "celsius") {
      problem.outputScale = 100;
      problem.labels = "Fahrenheit, Kelvin";
      const auto example = [](double c) {
        return Example{c / 100, {(1.8 * c + 32) / 100, (c + 273.15) / 100}, c};
      };
      std::vector<double> trainingCelsius = {-40, -20, 0, 20, 40, 60, 80, 100};
      std::vector<double> validationCelsius = {10, 70};
      for (double c : trainingCelsius)
        problem.train.push_back(example(c));
      for (double c : validationCelsius)
        problem.validation.push_back(example(c));
    } else if (opt.dataset == "sincos") {
      problem.outputScale = 1;
      problem.labels = "seno, coseno";
      int sampleNumber = 64;
      const double pi = std::acos(-1.0);
      const auto example = [pi, sampleNumber](double t) {
        const double x = pi * (-1 + 2 * t / sampleNumber);
        return Example{x / pi, {std::sin(x), std::cos(x)}, x};
      };
      for (int i = 0; i < sampleNumber; ++i) {
        problem.train.push_back(example(i));
        problem.validation.push_back(example(i + 0.5));
      }
    } else {
      throw std::invalid_argument("Dataset desconocido. Usa celsius o sincos.");
    }

    /*=======INITIALIZATION=======*/
    Training training(Model(opt.neurons, outputDimension, opt.lr, opt.seed), opt.training,
                      problem.train, problem.validation);
    Journal journal(makeOutput(opt), opt, problem);
    drannConsole console;
    ConsoleProgress progress(console);
    using Clock = drannConsole::Clock;
    const auto started = Clock::now();
    const auto elapsed = [&]() { return std::chrono::duration<double>(Clock::now() - started).count(); };
#ifdef DRANN_GUI
    std::unique_ptr<DRANNPlot> plot;
    if (!opt.batch)
      plot = std::make_unique<DRANNPlot>(opt.epochs);
#endif
    /*=======TRAINING, VALIDATION AND CONTINUATION=======*/
    int goal = opt.epochs;
    int first = 0;
    auto segmentStarted = started;
    int lastSummary = -1;
    double displayedLambda = 0.0;
    // Mismo resumen que DRANN_C_F.cpp; lambda es el único campo añadido.
    const auto printSummary = [&](int work, double lambda) {
      const auto &m = training.current;
      progress.line(8, "------------------------------------------------");
      progress.line(15, "Epoch -> " + std::to_string(work) + " / " + std::to_string(goal) + '.');
      std::ostringstream text;
      text << std::setprecision(10) << "Training Loss: " << m.training.loss
           << " | Validation Loss: " << m.validation.loss
           << "\nActive connections: " << m.connections
           << " | Participating neurons: " << m.neurons()
           << " | Steps: " << m.validation.meanSteps << " | lambda: " << lambda
           << "\nCoordinate Loss (" << problem.labels << "): Training = ("
           << m.training.coordinates[0] << ", " << m.training.coordinates[1]
           << ") | Validation = (" << m.validation.coordinates[0] << ", "
           << m.validation.coordinates[1] << ')';
      std::istringstream lines(text.str());
      for (std::string line; std::getline(lines, line);)
        progress.line(15, line);
      lastSummary = work;
    };
    const auto publish = [&](const Epoch &epoch) {
      journal.record(epoch, training, goal, elapsed());
      displayedLambda = epoch.lambda;
      const bool summary = epoch.work == 0 || epoch.work == goal || !epoch.decision.empty() ||
          (epoch.work - first) % std::max(1, (goal - first) / 10) == 0;
      if (summary)
        printSummary(epoch.work, epoch.lambda);
      if (!epoch.decision.empty())
        progress.line(6, "Phase: " + std::string(name(epoch.phase)) + " | " + epoch.decision +
                         " | Isolated neurons: " + ids(epoch.newlyIsolated));
      progress.draw(epoch.work, goal, first, segmentStarted, summary);
#ifdef DRANN_GUI
      if (plot) {
        const auto &m = training.current;
        auto train = m.training;
        auto validation = m.validation;
        // La gráfica necesita métricas y fase, no las distancias por neurona.
        validation.neurons.clear();
        return plot->update({epoch.work, epoch.modelEpoch, std::move(train), std::move(validation),
                    m.connections, m.neurons(), epoch.lambda, epoch.phase, problem.labels});
      }
#endif
      return true;
    };
    console.line(15, "DRANN | " + opt.dataset + " -> (" + problem.labels + ')');
    console.line(8, "------------------------------------------------");
    std::ostringstream configuration;
    configuration << std::setprecision(10) << "Neurons: " << opt.neurons << " | Seed: " << opt.seed
                  << " | lr: " << opt.lr << " | Epochs: " << opt.epochs;
    console.line(8, configuration.str());
    console.line(10, "Weights have been initialized.");
    progress.open();
    progress.line(15, "------------------------------------------------");
    progress.line(10, "Training will start.");
    progress.line(8, "Training samples: " + std::to_string(problem.train.size()) +
                     " | Validation samples: " + std::to_string(problem.validation.size()));
    bool running = publish(Epoch{});
    while (goal > 0) {
      while (running && training.work < goal) {
        Epoch epoch;
        try {
          epoch = training.advance(goal);
        } catch (const std::runtime_error &error) {
          journal.failure(training, error);
          throw;
        }
        running = publish(epoch);
      }
      if (lastSummary != training.work)
        printSummary(training.work, displayedLambda);
      const bool stopped = training.work < goal;
      if (stopped)
        progress.draw(training.work, goal, first, segmentStarted, true, true);
      progress.line(8, "Total training duration: " + drannConsole::duration(Clock::now() - segmentStarted));
      progress.line(stopped ? 6 : 10, stopped ? "Training stopped after epoch " + std::to_string(training.work) + '.'
                                            : "Training has finished.");
      progress.close();
      if (opt.batch)
        break;
#ifdef DRANN_GUI
      goal = plot->waitForContinuation();
#else
      goal = waitForConsoleContinuation(training.work);
#endif
      if (goal > 0) {
        first = training.work;
        segmentStarted = Clock::now();
        progress.open();
        progress.line(15, "------------------------------------------------");
        progress.line(10, "Training will continue from epoch " + std::to_string(first + 1) + '.');
        progress.line(8, "Training samples: " + std::to_string(problem.train.size()) +
                         " | Validation samples: " + std::to_string(problem.validation.size()));
        running = true;
      }
    }
    if (const auto settled = training.closeAttempt()) {
      journal.event(*settled, training.current);
      console.line(6, settled->decision);
    }
    journal.save(training.bestPredictive, "best_predictive.txt", training.references.size(), training.work);
    journal.save(training.snapshot(training.phase), "final_model.txt", training.references.size(), training.work);
    auto summary = openFile(journal.directory / "summary.txt");
    summary << "work_epochs=" << training.work << "\nmodel_epoch=" << training.modelEpoch
            << "\nelapsed_s=" << elapsed() << "\nattempts=" << training.references.size()
            << "\nparticipants=" << training.current.neurons()
            << "\nconnections=" << training.current.connections
            << "\nvalidation_loss=" << training.current.validation.loss
            << "\nvalidation_worst_point=" << training.current.validation.worstPoint
            << "\nbest_predictive_work=" << training.bestPredictive.work
            << "\nbest_predictive_validation=" << training.bestPredictive.measurements.validation.loss << '\n';
    console.line(15, "Validation results:");
    const auto validationStarted = Clock::now();
    reportFinal(training, problem, console);
    console.line(8, "Final validation duration: " + drannConsole::duration(Clock::now() - validationStarted));
    console.line(10, "History saved in " + (journal.directory / "history.csv").string() + '.');
    return 0;
  } catch (const std::exception &error) {
    drannConsole(std::cerr).line(4, "DRANN vectorial: " + std::string(error.what()));
    return 1;
  }
}
