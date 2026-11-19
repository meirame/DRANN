#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.."
folder=$(mktemp -d "${TMPDIR:-/tmp}/drann2-godot-test.XXXXXXXX")
echo "Prueba Godot: $folder"
./build/drann --epochs 1000 --out "$folder/experiment" > "$folder/console.log"
python3 - "$folder" <<'PY'
import csv, json, pathlib, sys
p = pathlib.Path(sys.argv[1])
rows = list(csv.DictReader((p/'experiment/neurons.csv').open()))
last = max(int(r['epoch']) for r in rows)
rows = [r for r in rows if int(r['epoch']) == last and float(r['input_physical']) == 10]
n = max(int(r['neuron']) for r in rows) + 1
t = max(int(r['coordinate']) for r in rows) + 1
q = [[0.0]*t for _ in range(n)]
for r in rows:
    q[int(r['neuron'])][int(r['coordinate'])] = float(r['position_normalized'])
(p/'expected.json').write_text(json.dumps(q))
PY
./.deps/godot "$@" --path "$PWD/godot" --script res://smoke.gd -- "$folder" 2>&1 | tee "$folder/godot.log"
if ! rg -q '^GODOT_SMOKE_OK:' "$folder/godot.log"; then exit 1; fi
if rg -q 'SCRIPT ERROR|Assertion failed|ERROR:' "$folder/godot.log"; then exit 1; fi
