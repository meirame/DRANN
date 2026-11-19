#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "${BASH_SOURCE[0]}")"
mode=${1:-godot}
if (( $# )); then shift; fi
case "$mode" in
  console) [[ -x build/drann ]] || ./build.sh core
           exec ./build/drann "$@" ;;
  godot|editor) [[ -x .deps/godot && -f godot/bin/libdrann_extension.so ]] || ./build.sh
           if [[ "$mode" == editor ]]; then
             exec ./.deps/godot --editor --path "$PWD/godot" "$@"
           fi
           exec ./.deps/godot --path "$PWD/godot" "$@" ;;
  *) echo 'Uso: ./run.sh [godot|editor|console] [argumentos]' >&2; exit 1 ;;
esac
