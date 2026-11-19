#include "DRANN.h"
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <memory>
using namespace godot;
namespace {
Array values(const Point &v) {
  Array a;
  for (double x : v)
    a.push_back(x);
  return a;
}
Array ids(const std::vector<int> &v) {
  Array a;
  for (int x : v)
    a.push_back(x);
  return a;
}
Array matrix(const Points &p) {
  Array a;
  for (const auto &v : p)
    a.push_back(values(v));
  return a;
}
} // namespace
class DrannLaboratory : public Node {
  GDCLASS(DrannLaboratory, Node)
  std::unique_ptr<Laboratory> lab;
  String error;

protected:
  static void _bind_methods() {
    ClassDB::bind_method(D_METHOD("defaults"), &DrannLaboratory::defaults);
    ClassDB::bind_method(D_METHOD("reset", "neurons", "lr", "seed", "activation"),
                         &DrannLaboratory::reset);
    ClassDB::bind_method(D_METHOD("set_goal", "epochs"), &DrannLaboratory::set_goal);
    ClassDB::bind_method(D_METHOD("advance"), &DrannLaboratory::advance);
    ClassDB::bind_method(D_METHOD("inspect", "input", "reveal"), &DrannLaboratory::inspect);
    ClassDB::bind_method(D_METHOD("snapshot"), &DrannLaboratory::snapshot);
    ClassDB::bind_method(D_METHOD("save_model", "path", "best"), &DrannLaboratory::save_model);
    ClassDB::bind_method(D_METHOD("load_model", "path"), &DrannLaboratory::load_model);
  }

public:
  Dictionary defaults() {
    Config c = laboratoryConfig();
    Dictionary d;
    d["neurons"] = c.neurons;
    d["lr"] = c.lr;
    d["seed"] = int(c.seed);
    d["epochs"] = c.epochs;
    d["activation"] = int(c.tanhActivation);
    return d;
  }
  bool reset(int neurons, double lr, int seed, int activation) {
    try {
      auto c = laboratoryConfig();
      c.neurons = neurons;
      c.lr = lr;
      c.seed = seed;
      c.tanhActivation = activation != 0;
      auto data = laboratoryDataset(c);
      lab = std::make_unique<Laboratory>(c, data);
      error = "";
      return true;
    } catch (const std::exception &e) {
      error = String::utf8(e.what());
      return false;
    }
  }
  bool set_goal(int epochs) {
    if (!lab || epochs < 1) {
      error = "Meta de épocas inválida.";
      return false;
    }
    lab->model.config.epochs = epochs;
    lab->bestModel.config.epochs = epochs;
    error = "";
    return true;
  }
  bool advance() {
    try {
      if (!lab)
        throw std::runtime_error("Reinicie el laboratorio.");
      lab->advanceExample();
      error = "";
      return true;
    } catch (const std::exception &e) {
      error = String::utf8(e.what());
      return false;
    }
  }
  bool inspect(double input, bool reveal) {
    try {
      if (!lab)
        throw std::runtime_error("Reinicie el laboratorio.");
      lab->inspect(input, reveal);
      error = "";
      return true;
    } catch (const std::exception &e) {
      error = String::utf8(e.what());
      return false;
    }
  }
  bool save_model(String path, bool best) {
    try {
      if (!lab)
        throw std::runtime_error("Reinicie el laboratorio.");
      lab->save(path.utf8().get_data(), best);
      error = "";
      return true;
    } catch (const std::exception &e) {
      error = String::utf8(e.what());
      return false;
    }
  }
  bool load_model(String path) {
    try {
      auto candidate = Laboratory::load(path.utf8().get_data());
      lab = std::make_unique<Laboratory>(std::move(candidate));
      error = "";
      return true;
    } catch (const std::exception &e) {
      error = String::utf8(e.what());
      return false;
    }
  }
  Dictionary snapshot() {
    Dictionary d;
    d["error"] = error;
    if (!lab)
      return d;
    const auto &t = lab->inspected;
    auto read = lab->model.interpret(t);
    d["epoch"] = lab->e;
    d["model_epoch"] = lab->modelEpoch;
    d["best_epoch"] = lab->bestEpoch;
    d["best_loss"] = lab->bestLoss;
    d["cursor"] = lab->cursor;
    d["dimension"] = lab->model.config.outputDimension;
    d["scaleY"] = lab->model.config.scaleY;
    d["radius"] = lab->model.config.radius;
    d["lr"] = lab->model.config.lr;
    d["goal"] = lab->model.config.epochs;
    d["seed"] = int(lab->model.config.seed);
    d["activation"] = int(lab->model.config.tanhActivation);
    d["neurons"] = lab->model.config.neurons;
    d["x"] = t.x * lab->model.config.scaleX;
    d["q"] = matrix(t.q);
    d["biasT"] = matrix(t.biasT);
    d["weightsT"] = matrix(t.weightsT);
    d["initial"] = values(t.initial);
    d["final"] = values(t.final);
    d["axis"] = ids(read.axis);
    d["scores"] = matrix(read.scores);
    Array edges;
    for (const auto &w : t.edges) {
      Dictionary edge;
      edge["i"] = w.i;
      edge["j"] = w.j;
      edge["w"] = w.w;
      edge["active"] = w.active;
      edges.push_back(edge);
    }
    d["edges"] = edges;
    Array steps;
    for (const auto &s : t.steps) {
      Dictionary v;
      v["before"] = values(s.before);
      v["after"] = values(s.after);
      v["transmitters"] = ids(s.transmitters);
      v["receivers"] = ids(s.receivers);
      v["connections"] = ids(s.connections);
      steps.push_back(v);
    }
    d["steps"] = steps;
    Array components;
    for (const auto &r : read.components) {
      Dictionary v;
      v["status"] = String::utf8(r.status.c_str());
      v["value"] = r.status == "válido" ? Variant(r.value) : Variant();
      v["members"] = ids(r.members);
      v["span"] = r.span;
      Array groups;
      for (const auto &g : r.groups)
        groups.push_back(ids(g));
      v["groups"] = groups;
      components.push_back(v);
    }
    d["reading"] = components;
    if (lab->reveal) {
      d["target"] = values(lab->inspectedTarget);
      d["coincident"] = lab->model.loss(t, lab->inspectedTarget).coincidentWells;
    }
    Array history;
    for (const auto &e : lab->history) {
      Dictionary v;
      v["e"] = e.e;
      v["training"] = e.training.loss;
      v["validation"] = e.validation.loss;
      v["distance2"] = values(e.validation.distanceSquared);
      v["read_sse"] = values(e.validation.readSquaredError);
      v["valid"] = ids(e.validation.validCount);
      v["total"] = e.validation.examples;
      history.push_back(v);
    }
    d["history"] = history;
    return d;
  }
};
void initialize(ModuleInitializationLevel level) {
  if (level == MODULE_INITIALIZATION_LEVEL_SCENE)
    ClassDB::register_class<DrannLaboratory>();
}
void terminate(ModuleInitializationLevel) {}
extern "C" GDExtensionBool GDE_EXPORT drann_library_init(GDExtensionInterfaceGetProcAddress p,
                                                         const GDExtensionClassLibraryPtr lib,
                                                         GDExtensionInitialization *init) {
  GDExtensionBinding::InitObject object(p, lib, init);
  object.register_initializer(initialize);
  object.register_terminator(terminate);
  object.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);
  return object.init();
}
