# Trabajo local y publicación en GitHub

La preferencia actual es desarrollar, compilar y ejecutar la DRANN en el ordenador del usuario, y publicar los cambios y resultados en el repositorio privado `meirame/DRANN`.

## Retomar en Codex local

1. Abrir `/home/luis/personal/DRANN` como proyecto local en Codex. Esa es la carpeta preparada por el usuario; no sustituirla por otro proyecto de nombre parecido sin comprobar la ruta.
2. Usar la ejecución local de ese proyecto. El chat remoto anterior no cambia automáticamente de equipo ni tiene acceso a los archivos de ese ordenador.
3. Comprobar la rama y los cambios existentes antes de actualizar:

```bash
cd /home/luis/personal/DRANN
git status
git branch --show-current
git fetch origin
```

El código actual está en `feature/drann-v1`, asociado a la PR #2. La PR #1 contiene la documentación inicial y es su base. Mientras esas ramas no estén incorporadas a `main`, actualizar únicamente `main` no descarga la implementación.

Si no hay cambios locales que guardar:

```bash
git switch feature/drann-v1
git pull --ff-only
```

Si se editó `DRANN_C_F.cpp` para ajustar las épocas, conservar ese cambio antes de actualizar. El README explica el uso de `git stash` y su recuperación. Ahora puede elegirse `--epochs 1000` sin editar el código.

## Mensaje para el chat local

Se puede comenzar con este texto:

> Continúa el proyecto DRANN en esta carpeta local. Primero revisa el estado de Git y actualiza feature/drann-v1 desde origin conservando mis cambios locales. Lee AGENTS.md, docs/DRANN.md, docs/LOCAL.md y docs/experiment-v1.md. El núcleo C++ y la interfaz ImPlot/ImGui/GLFW ya están implementados. Comprueba la compilación y las pruebas; trabajaremos localmente y subiremos a GitHub los cambios y resultados de los experimentos mediante commits y push en una rama. Godot sigue para después. No cambies las matemáticas acordadas de forma implícita.

Estos documentos conservan el contexto técnico; no transfieren el historial literal del chat.

## Compilar y ejecutar

Las dependencias y los comandos completos están en el README. Con ellas instaladas:

```bash
./run_drann.sh --epochs 1000
```

Para comprobar el núcleo sin ventana:

```bash
cmake -S . -B build/headless -DDRANN_BUILD_GUI=OFF
cmake --build build/headless --parallel 2
ctest --test-dir build/headless --output-on-failure
./build/headless/drann --epochs 1000
```

El historial se escribe en `training_history_fk.csv`. Cada nueva ejecución lo reemplaza, por lo que hay que copiar los resultados que se quieran conservar antes de repetir el experimento.

## Publicar un experimento local

Crear una carpeta nueva por ejecución dentro de `results/`. Guardar allí:

- `history.csv`: copia exacta del historial generado.
- `README.md`: procedencia local, commit del código ejecutado, número de neuronas, dataset y partición, normalización, semilla, `lr`, épocas, orden de los ejemplos, compilador y resultados relevantes.

Por ejemplo, para una carpeta nueva llamada `ejecucion_local_01`:

```bash
mkdir results/ejecucion_local_01
cp training_history_fk.csv results/ejecucion_local_01/history.csv
```

Después de escribir sus metadatos:

```bash
git add results/ejecucion_local_01/history.csv results/ejecucion_local_01/README.md
git commit -m "results: registrar experimento local"
git push origin feature/drann-v1
```

Si el trabajo se hace en otra rama, publicar esa rama. Inspeccionar y añadir expresamente los cambios de código que correspondan; no incluir archivos ajenos al experimento. No fusionar una pull request sin una instrucción que lo autorice.

## Referencia ya publicada

[celsius_fahrenheit_seed42_1000](../results/celsius_fahrenheit_seed42_1000/README.md) conserva las 1001 evaluaciones de la ejecución verificada antes del traslado. Su CSV gráfico coincide byte a byte con el de consola. Está identificado como referencia del entorno remoto, no como una captura del disco local del usuario.

La lectura vectorial vigente aprende (Fahrenheit, Kelvin). Ver [su especificación y resultados](experiment-celsius-fahrenheit-kelvin.md). Anotar en cada nuevo resultado alpha/beta iniciales, la dimensión y la convención de suma de coordenadas, además de la configuración habitual.
