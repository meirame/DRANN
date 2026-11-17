#!/usr/bin/env sh
set -eu

# Ejecutar desde la raíz, también si el lanzador se invoca desde otra carpeta.
drann_directory=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$drann_directory"
drann_python=${DRANN_PYTHON:-python3}
drann_compiler=${DRANN_CXX:-g++}

if ! "$drann_python" -c 'import matplotlib.pyplot' >/dev/null 2>&1; then
  printf '%s\n' \
    'Se necesita Python con Matplotlib para abrir la gráfica.' \
    'Consulta la instalación del entorno de gráficas en README.md.' >&2
  exit 1
fi

mkdir -p build
"$drann_compiler" -std=c++17 -O2 -Wall -Wextra -Wpedantic DRANN.cpp -o build/drann
./build/drann
exec "$drann_python" plot_loss.py training_history.csv "$@"
