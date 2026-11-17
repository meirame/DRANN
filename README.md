# DRANN

Diseño de una red neuronal artificial rizomática diferencial, implementada desde cero en C++.

El primer experimento será una regresión de Celsius a Fahrenheit. Primero se desarrollará un ejecutable de consola; la integración con Godot se abordará después.

La primera implementación está en `DRANN.cpp`, siguiendo la estructura y el estilo de `XOR.cpp`: `struct`, vectores, cachés, bucles y derivadas manuales. No utiliza frameworks neuronales ni autodiferenciación.

## Compilar y ejecutar

Se necesita un compilador con soporte para C++17. Desde la raíz del repositorio:

```bash
mkdir -p build
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic DRANN.cpp -o build/drann
./build/drann
```

El programa ejecuta 100 épocas con 5 neuronas y `lr = 0.005`. Genera `training_history.csv` en el directorio de ejecución: una evaluación inicial y otra tras cada época, con las pérdidas geométricas de entrenamiento y validación, pasos medios, conexiones activas y neuronas participantes. Una nueva ejecución reemplaza ese archivo.

Al terminar muestra el cognifenómeno primitivo completo de los dos ejemplos de validación, desnormalizado a Fahrenheit. No sustituye la nube por una neurona de salida ni por un promedio para calcular la pérdida.

## Gráfica nativa en tiempo real (C++)

`run_drann.sh` compila y abre `drann_gui`, con una ventana de ImPlot + Dear ImGui + GLFW/OpenGL. Al completar cada época se añade un punto a las curvas de entrenamiento y validación. No necesita Python ni espera al final del entrenamiento para dibujar.

En Debian/Ubuntu instala las dependencias de compilación:

```bash
sudo apt install build-essential cmake libglfw3-dev libgl1-mesa-dev
```

Se necesita CMake 3.20 o posterior. La primera configuración descarga las versiones fijadas de Dear ImGui (`v1.91.9b`) e ImPlot (`v0.17`) desde sus repositorios oficiales y verifica sus hashes SHA-256. Las siguientes compilaciones reutilizan la descarga.

Para abrir la ventana y entrenar durante 1000 épocas:

```bash
./run_drann.sh --epochs 1000
```

Sin argumentos se utiliza la configuración de `main()`, inicialmente 100 épocas. `--epochs` funciona también en el ejecutable de consola. El lanzador compila el código actual, sin reescribir tu configuración.

Si editaste `DRANN.cpp` localmente para cambiar las épocas, guarda ese cambio antes de actualizar la rama. Cuando `git status` muestre esa modificación, puedes usar:

```bash
git stash push -m "configuracion local de DRANN" -- DRANN.cpp
git pull --ff-only
git stash pop
```

Conserva cualquier conflicto de combinación para resolverlo, sin descartar tus cambios. En adelante, `--epochs 1000` permite elegir ese valor sin editar el archivo.

La ventana incluye:

- Pausa y continuación entre épocas: mientras está pausada no se actualiza el modelo.
- Escala lineal o logarítmica.
- Ajuste automático de los ejes; desactívalo para explorar con zoom y desplazamiento.
- Épocas por actualización, para acelerar el entrenamiento agrupando puntos por fotograma. Todos los puntos se conservan.
- Pérdidas actuales, progreso y número de conexiones y neuronas participantes.

La ventana permanece abierta al completar el entrenamiento. Cerrarla antes detiene el proceso después de la época actual y conserva en el CSV las evaluaciones completadas. El límite de refresco visual no es un parámetro de la dinámica neuronal ni cambia el orden de las actualizaciones.

También puedes compilar manualmente:

```bash
cmake -S . -B build/native -DCMAKE_BUILD_TYPE=Release
cmake --build build/native --parallel 2
./build/native/drann_gui --epochs 1000
ctest --test-dir build/native --output-on-failure
```

Se necesita una sesión gráfica y OpenGL 3.3 (3.2 en macOS). El ejecutable de consola conserva su compilación directa independiente de GLFW, ImGui e ImPlot. Para usar CMake sin bibliotecas gráficas ni descargar esas dependencias:

```bash
cmake -S . -B build/headless -DDRANN_BUILD_GUI=OFF
cmake --build build/headless --parallel 2
./build/headless/drann --epochs 1000
ctest --test-dir build/headless --output-on-failure
```

La versión anterior que dibujaba con Python al terminar se ha sustituido por esta interfaz. El CSV sigue escribiéndose y se actualiza después de cada evaluación.

## Configuración del experimento

La configuración y el dataset están en `main()` para mantenerlos visibles y fáciles de modificar:

- Entrenamiento en Celsius: `{-40, -20, 0, 20, 40, 60, 80, 100}`.
- Validación en Celsius: `{10, 70}`.
- Normalización: `x = C / 100`, `y = F / 100`, con `F = 1.8*C + 32` para generar las etiquetas.
- Semilla inicial: `42`; ejemplos en orden fijo, sin barajado.
- Pesos internos compartidos entre ambas direcciones; su anulación es irreversible durante el entrenamiento. Los pesos de entrada son parámetros ordinarios y no reciben esa anulación.

Los valores concretos del dataset, la semilla y el orden son elecciones iniciales de implementación. Los parámetros acordados de tamaño, normalización, tasa y épocas se mantienen.

## Comprobar el modelo

```bash
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic tests/DRANN_tests.cpp -o build/drann_tests
./build/drann_tests
```

Las pruebas incluyen recorridos calculados a mano, máximos locales simultáneos y empatados, receptora compartida, sesgos, cambios de papel, aislamiento parcial, anulación irreversible, evaluación sin cambios de parámetros y 49 derivadas contrastadas por diferencias finitas sin cambiar las decisiones discretas.

Para comprobar además memoria y comportamiento indefinido con un compilador que soporte estos sanitizadores:

```bash
g++ -std=c++17 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer tests/DRANN_tests.cpp -o build/drann_tests_sanitized
./build/drann_tests_sanitized
```

## Resultado inicial y límites

Con GCC 14.2.0 y la semilla 42, el error geométrico normalizado de validación pasó de `0.6280624832` a `0.0118076840`; el de entrenamiento pasó de `0.6230380312` a `0.0318698253`. Quedaron 9 de las 10 conexiones internas y las 5 neuronas participantes. La pérdida es sobre todos los puntos de la nube, no el error de un único escalar predicho.

La conversión todavía no es exacta. Dos ejemplos de validación y una tarea lineal no bastan para establecer la capacidad de generalización ni una ventaja de esta arquitectura. El backward deriva el recorrido ejecutado y no los cambios discretos de papeles. No hay reintegración, parada aprendida ni integración con Godot. El aislamiento total sigue fuera de alcance: si sucede, el programa termina con un mensaje de error, sin inventar una pérdida o una recuperación.

## Contexto del proyecto

- [Especificación y decisiones de diseño](docs/DRANN.md).
- [Instrucciones de trabajo y estilo](AGENTS.md).
- [Referencia de estilo original: XOR.cpp](docs/reference/XOR.cpp). Este archivo es una referencia del usuario, no la implementación de la DRANN.
- [Registro del primer experimento](docs/experiment-v1.md).
