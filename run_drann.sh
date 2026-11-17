#!/usr/bin/env sh
set -eu

# Ejecutar desde la raíz, también si el lanzador se invoca desde otra carpeta.
drann_directory=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$drann_directory"
drann_cmake=${DRANN_CMAKE:-cmake}
"$drann_cmake" -S . -B build/native -DDRANN_BUILD_GUI=ON -DCMAKE_BUILD_TYPE=Release
"$drann_cmake" --build build/native --target drann_gui --parallel 2
exec ./build/native/drann_gui "$@"
