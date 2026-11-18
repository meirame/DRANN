#!/usr/bin/env sh
set -eu

# Ejecutar desde la raíz, también si el lanzador se invoca desde otra carpeta.
drann_directory=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$drann_directory"
drann_cmake=${DRANN_CMAKE:-cmake}
# --console ejecuta el entrenamiento y sus logs sin abrir la ventana gráfica.
drann_build=build/native
drann_target=drann_gui
drann_gui=ON
if [ "${1:-}" = "--console" ]; then
  drann_build=build/headless
  drann_target=drann
  drann_gui=OFF
  shift
fi
"$drann_cmake" -S . -B "$drann_build" -DDRANN_BUILD_GUI="$drann_gui" -DCMAKE_BUILD_TYPE=Release
"$drann_cmake" --build "$drann_build" --target "$drann_target" --parallel 2
exec "./$drann_build/$drann_target" "$@"
