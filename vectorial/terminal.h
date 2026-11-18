#ifndef DRANN_TERMINAL_H
#define DRANN_TERMINAL_H
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#if defined(__unix__) || defined(__APPLE__)
#include <sys/ioctl.h>
#include <unistd.h>
#elif defined(_WIN32)
#include <cstdio>
#include <io.h>
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
      case 4: return "\033[31m";  // Rojo: errores.
      case 6: return "\033[33m";  // Amarillo: esperas y avisos.
      case 8: return "\033[90m";  // Gris: detalles y tiempos.
      case 10: return "\033[32m"; // Verde: progreso y éxitos.
      case 15: return "\033[37m"; // Blanco: cabeceras y resultados.
      default: return "\033[0m";
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
    text << ms / 3600000 << "hrs " << (ms / 60000) % 60 << "min "
         << (ms / 1000) % 60 << "s " << ms % 1000 << "ms";
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
    text << color(15) << "] " << color(10) << e << '/' << goal << " ("
         << std::fixed << std::setprecision(2) << ratio * 100.0 << "%)";
    if (stopped)
      text << color(6) << " Stopped.";
    else if (e == goal)
      text << " Completed.";
    else if (e > first) {
      // Al reanudar, estimar con las épocas ejecutadas en este tramo.
      const double seconds = std::chrono::duration<double>(elapsed).count();
      const double remaining = std::ceil(seconds * (goal - e) / (e - first));
      text << color(6) << " Remaining: " << std::setprecision(0)
           << std::floor(remaining / 3600.0) << "hrs, "
           << std::floor(std::fmod(remaining, 3600.0) / 60.0) << "min, "
           << std::fmod(remaining, 60.0) << "secs.";
    } else {
      text << color(6) << " Remaining: --";
    }
    text << color(0);
    return text.str();
  }
};

// Acota la región de scroll durante cada tramo y la restaura antes de pedir entrada.
struct ConsoleProgress {
  const drannConsole &console;
  drannConsole::Clock::time_point last;
  int rows = 0;

  explicit ConsoleProgress(const drannConsole &output) : console(output) {}
  ~ConsoleProgress() { close(); }

  void open() {
#if defined(__unix__) || defined(__APPLE__)
    struct winsize size {};
    if (console.terminal && ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0 &&
        size.ws_row > 2 && size.ws_col >= 120) {
      rows = size.ws_row;
      console.out << "\033[1;" << rows - 1 << "r\033[" << rows - 1 << ";1H" << std::flush;
    }
#endif
    last = {};
  }

  void close() {
    if (rows) {
      console.out << "\033[r\033[" << rows << ";1H\r\n" << std::flush;
      rows = 0;
    }
  }

  void line(int color, const std::string &message) const {
    if (rows)
      console.out << "\033[" << rows - 1 << ";1H\r\033[2K";
    console.line(color, message);
  }

  void draw(int e, int goal, int first, drannConsole::Clock::time_point started,
            bool summary, bool stopped = false) {
    const auto now = drannConsole::Clock::now();
    if (!rows) {
      if (summary)
        console.line(15, console.progress(e, goal, first, now - started, stopped));
      return;
    }
    if (!summary && now - last < std::chrono::milliseconds(100))
      return;
    last = now;
    console.out << "\033[" << rows << ";1H\r"
                << console.progress(e, goal, first, now - started, stopped)
                << "\033[K\033[" << rows - 1 << ";1H" << std::flush;
  }
};

inline int waitForConsoleContinuation(int completed) {
  bool interactive = false;
#if defined(__unix__) || defined(__APPLE__)
  interactive = isatty(STDIN_FILENO) != 0;
#elif defined(_WIN32)
  interactive = _isatty(_fileno(stdin)) != 0;
#endif
  if (!interactive)
    return 0;

  drannConsole console;
  const int maxAdditional = std::numeric_limits<int>::max() - 1 - completed;
  if (maxAdditional == 0) {
    console.line(6, "Se alcanzó el límite de épocas.");
    return 0;
  }
  while (true) {
    console.prompt(6, "¿Cuántas épocas más quieres entrenar? [0 o Intro para salir] ");
    std::string line;
    if (!std::getline(std::cin, line))
      return 0;
    std::istringstream input(line);
    input >> std::ws;
    if (input.eof())
      return 0;

    int additional = 0;
    if (input >> additional && (input >> std::ws).eof() &&
        additional >= 0 && additional <= maxAdditional)
      return additional == 0 ? 0 : completed + additional;

    console.line(4, "Introduce un número entero entre 0 y " + std::to_string(maxAdditional) + '.');
  }
}
#endif
