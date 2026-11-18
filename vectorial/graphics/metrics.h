#ifndef DRANN_PLOT_METRICS_H
#define DRANN_PLOT_METRICS_H

#include "../entrenamiento.h"

struct drannEpoch {
  int e = 0, modelEpoch = 0;
  vectorial::Metrics training, validation;
  std::size_t connections = 0, participants = 0;
  double lambda = 0;
  vectorial::Phase phase = vectorial::Phase::training;
  std::string coordinates;
};

#endif
