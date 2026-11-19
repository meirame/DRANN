#pragma once
#include <algorithm>  // fijar un mínimo en los intervalos de salida
#include <chrono>     // tiempos de entrenamiento y estimación de lo que queda
#include <cmath>      // comprobación de valores finitos
#include <cstddef>    // índices de los vectores
#include <cstdlib>    // TERM y NO_COLOR
#include <fstream>    // historial del entrenamiento
#include <functional> // notificar las evaluaciones sin depender de la interfaz
#include <iomanip>    // precisión de la salida
#include <iostream>   // salida por terminal
#include <limits>     // límite del contador de épocas
#include <optional>   // época desde la que se reanuda el entrenamiento
#include <random>     // inicialización de los pesos
#include <sstream>    // formar los logs sin perder precisión
#include <stdexcept>  // errores sin inventar una recuperación del modelo
#include <string>     // nombre del archivo de resultados
#include <vector>     // parámetros, estados y cachés

#if defined(__unix__) || defined(__APPLE__)
#include <sys/ioctl.h> // dimensiones de la terminal para fijar la barra abajo
#include <unistd.h>    // comprobar si la entrada y la salida usan una terminal
#elif defined(_WIN32)
#include <cstdio> // stdin
#include <io.h>   // detectar la entrada interactiva en Windows
#endif

struct drannConsole {
  using Clock = std::chrono::steady_clock;
  std::ostream &out;
  bool terminal = false, colors = false;

  explicit drannConsole(std::ostream &output = std::cout) : out(output) {
#if defined(__unix__) || defined(__APPLE__)
    terminal = (&out == &std::cout && isatty(STDOUT_FILENO)) ||
               (&out == &std::cerr && isatty(STDERR_FILENO));
#elif defined(_WIN32)
    terminal = (&out == &std::cout && _isatty(_fileno(stdout))) ||
               (&out == &std::cerr && _isatty(_fileno(stderr)));
#endif
    const char *term = std::getenv("TERM");
    terminal = terminal && (!term || std::string(term) != "dumb");
    colors = terminal && !std::getenv("NO_COLOR");
  }

  const char *color(int value) const {
    if (!colors)
      return "";
    switch (value) {
    case 4:
      return "\033[31m"; // Rojo: errores.
    case 6:
      return "\033[33m"; // Amarillo: esperas y avisos.
    case 8:
      return "\033[90m"; // Gris: detalles y tiempos.
    case 10:
      return "\033[32m"; // Verde: progreso y éxitos.
    case 15:
      return "\033[37m"; // Blanco: cabeceras y resultados.
    default:
      return "\033[0m";
    }
  }

  void line(int value, const std::string &message) const {
    out << color(value) << message << color(0) << '\n';
  }

  void prompt(int value, const std::string &message) const {
    out << color(value) << message << color(0) << std::flush;
  }

  static std::string duration(Clock::duration elapsed) {
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    std::ostringstream text;
    text << ms / 3600000 << "hrs " << (ms / 60000) % 60 << "min " << (ms / 1000) % 60 << "s "
         << ms % 1000 << "ms";
    return text.str();
  }

  std::string progress(int e, int goal, int first, Clock::duration elapsed,
                       bool stopped = false) const {
    const double ratio = static_cast<double>(e) / goal;
    const int width = 20;
    const int filled = static_cast<int>(ratio * width);
    std::ostringstream text;
    text << color(15) << "Epochs: [";
    for (int i = 0; i < width; i++)
      text << color(i < filled ? 10 : 6) << (i < filled ? '#' : '.');
    text << color(15) << "] " << color(10) << e << '/' << goal << " (" << std::fixed
         << std::setprecision(2) << ratio * 100.0 << "%)";
    if (stopped)
      text << color(6) << " Stopped.";
    else if (e == goal)
      text << " Completed.";
    else if (e > first) {
      // Al reanudar, estimar con las épocas ejecutadas en este tramo.
      const double seconds = std::chrono::duration<double>(elapsed).count();
      const double remaining = std::ceil(seconds * (goal - e) / (e - first));
      text << color(6) << " Remaining: " << std::setprecision(0) << std::floor(remaining / 3600.0)
           << "hrs, " << std::floor(std::fmod(remaining, 3600.0) / 60.0) << "min, "
           << std::fmod(remaining, 60.0) << "secs.";
    } else {
      text << color(6) << " Remaining: --";
    }
    text << color(0);
    return text.str();
  }
};

using utils = drannConsole;
