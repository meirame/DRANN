#ifndef DRANN_PLOT_H
#define DRANN_PLOT_H

#include <memory>

struct drannEpoch;

// El núcleo solo entrega métricas. GLFW, ImGui e ImPlot quedan en el adaptador.
struct DRANNPlot {
  struct Impl;
  std::unique_ptr<Impl> impl;

  explicit DRANNPlot(int epochs);
  ~DRANNPlot();
  bool update(const drannEpoch &epoch);
  void finish();
};

#endif
