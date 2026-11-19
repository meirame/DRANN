#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "${BASH_SOURCE[0]}")"
mode=${1:-godot}
if [[ "$mode" == core ]]; then
  cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DDRANN_BUILD_GODOT=OFF
else
  mkdir -p .deps
  if [[ ! -d .deps/godot-cpp ]]; then
    curl --fail --location --retry 3 'https://codeload.github.com/godotengine/godot-cpp/tar.gz/e83fd0904c13356ed1d4c3d09f8bb9132bdc6b77' -o .deps/godot-cpp.tar.gz
    echo 'd4c03daf46c8bef4544614182b0762c3e32789151c7f8f44e8eb2053d5cddeba  .deps/godot-cpp.tar.gz' | sha256sum -c -
    mkdir -p .deps/godot-cpp
    tar -xzf .deps/godot-cpp.tar.gz --strip-components=1 -C .deps/godot-cpp
  fi
  if [[ ! -x .deps/godot ]]; then
    curl --fail --location --retry 3 'https://github.com/godotengine/godot-builds/releases/download/4.5.1-stable/Godot_v4.5.1-stable_linux.x86_64.zip' -o .deps/godot.zip
    echo '02ec53d1cc7dbb9cc6355393c61b9ab43d1244751a124f10248a4802830788cd  .deps/godot.zip' | sha256sum -c -
    python3 -c 'import zipfile; zipfile.ZipFile(".deps/godot.zip").extractall(".deps")'
    mv .deps/Godot_v4.5.1-stable_linux.x86_64 .deps/godot
    chmod +x .deps/godot
  fi
  cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DDRANN_BUILD_GODOT=ON \
    -DFETCHCONTENT_SOURCE_DIR_GODOT_CPP="$PWD/.deps/godot-cpp"
fi
cmake --build build --parallel "${DRANN_JOBS:-6}"
ctest --test-dir build --output-on-failure
