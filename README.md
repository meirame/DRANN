# DRANN

Diseño de una red neuronal artificial rizomática diferencial, implementada desde cero en C++.

El primer experimento será una regresión de Celsius a Fahrenheit. Primero se desarrollará un ejecutable de consola; la integración con Godot se abordará después.

El experimento Celsius a Fahrenheit está en `DRANN_C_F.cpp` y sigue la estructura y el estilo de `XOR.cpp`: `struct`, vectores, cachés, bucles y derivadas manuales. No utiliza frameworks neuronales ni autodiferenciación.

## Compilar y ejecutar

Se necesita un compilador con soporte para C++17. Desde la raíz del repositorio:

```bash
mkdir -p build
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic DRANN_C_F.cpp -o build/drann
./build/drann
```

La configuración actual ejecuta 100 000 épocas con 2 neuronas y `lr = 0.001`. Genera `training_history.csv` en el directorio de ejecución: una evaluación inicial y otra tras cada época, con las pérdidas geométricas de entrenamiento y validación, pasos medios, conexiones activas y neuronas participantes. Una nueva ejecución reemplaza ese archivo.

Al terminar muestra el cognifenómeno primitivo completo de los dos ejemplos de validación, desnormalizado a Fahrenheit. No sustituye la nube por una neurona de salida ni por un promedio para calcular la pérdida.

En una terminal interactiva, al terminar puedes introducir cuántas épocas más quieres entrenar. El programa continúa desde la siguiente época y agrega los resultados al mismo CSV. Pulsa Intro o escribe `0` para salir.

Para compilar y ejecutar directamente en la terminal, sin abrir la gráfica:

```bash
./run_drann.sh --console --epochs 1000
```

Este comando recompila la versión de consola antes de ejecutarla, de modo que muestra los logs actuales aunque existan ejecutables antiguos en `build/` u `output/`. Sin `--console`, el lanzador sigue abriendo la ventana gráfica.

## Logs de terminal

La salida de `DRANN_C_F.cpp` sigue el estilo de `satellite_CNN_HL5.cpp`: cabeceras de época, separadores, pérdidas de entrenamiento y validación, conexiones activas, neuronas participantes y pasos internos. Los colores ANSI son blanco para resultados, gris para detalles y tiempos, verde para progreso y éxitos, amarillo para esperas y paradas manuales, y rojo para errores.

Cada resumen muestra las dos pérdidas en una línea: `Training Loss: ... | Validation Loss: ...`. La línea siguiente usa `Active connections: ... | Participating neurons: ... | Steps: ...`. `Steps` es la media de pasos internos por ejemplo de validación de esa época.

La barra usa `#` para lo completado y `.` para lo pendiente, con contador, porcentaje y tiempo restante estimado. Se mantiene abajo en terminales Unix compatibles de al menos 120 columnas; en terminales más estrechas o al redirigir la salida se imprime junto a los resúmenes. Hay aproximadamente diez resúmenes por tramo de entrenamiento, más el estado inicial y el último estado, también cuando se detiene antes de la meta. Las duraciones se muestran al terminar el tramo de entrenamiento y la validación final, sin tiempos ni `lr` entre épocas. Al reanudar, la estimación utiliza solo las épocas ejecutadas en el nuevo tramo.

Los colores se desactivan al redirigir la salida, con `TERM=dumb` o con `NO_COLOR`. El CSV conserva todas las evaluaciones, sin colores ni cambios en su formato. La presentación no modifica los cálculos del modelo, la tasa de aprendizaje ni la continuación del entrenamiento.

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

Sin argumentos se utiliza la configuración de `main()`, con una meta inicial de 100 000 épocas. `--epochs` funciona también en el ejecutable de consola. El lanzador compila el código actual, sin reescribir tu configuración.

Si editaste `DRANN_C_F.cpp` localmente para cambiar las épocas, guarda ese cambio antes de actualizar la rama. Cuando `git status` muestre esa modificación, puedes usar:

```bash
git stash push -m "configuracion local de DRANN" -- DRANN_C_F.cpp
git pull --ff-only
git stash pop
```

Conserva cualquier conflicto de combinación para resolverlo, sin descartar tus cambios. En adelante, `--epochs 1000` permite elegir ese valor sin editar el archivo.

La ventana incluye:

- Pausa y continuación entre épocas: mientras está pausada no se actualiza el modelo.
- Botón «Finalizar»: detiene el entrenamiento, también desde pausa, y conserva la gráfica abierta y el CSV hasta la última época completada.
- «Continuar entrenamiento»: al completar la meta o pulsar «Finalizar», introduce las épocas adicionales que quieras ejecutar. Se conserva la red entrenada y se amplían la gráfica y el mismo CSV desde la siguiente época.
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

La [variante de 40 temperaturas](docs/DRANN.md#71-variante-local-con-40-temperaturas) se conserva en la especificación. Los resultados históricos corresponden al experimento inicial con 5 neuronas y `lr = 0.005`.

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
- [Retomar el trabajo local y publicar resultados](docs/LOCAL.md).
- [Historial verificado de 1000 épocas](results/celsius_fahrenheit_seed42_1000/README.md).
