# DRANN con potencial vectorial y aislamiento experimental

Esta carpeta es un proyecto independiente. [referencia/](referencia/) contiene una copia del código, la gráfica, las pruebas y la documentación que estaban en el proyecto original al iniciar el trabajo. El experimento nuevo comparte el estilo y los controles con esa versión.

## Ejecutar

Desde la raíz de esta carpeta, en el portátil donde se compiló:

```sh
./bin/drann --dataset celsius --epochs 4000
./bin/drann --dataset sincos --epochs 4000 --batch
./bin/drann_gui --dataset celsius --epochs 4000
```

La consola interactiva y la ventana permiten añadir épocas explícitamente. `--batch` termina tras consumir el presupuesto indicado. `--help` lista todas las opciones. Los binarios se pueden recompilar con el lanzador, usando C++17 y CMake:

```sh
./run_drann.sh --console --dataset celsius --epochs 4000
./run_drann.sh --dataset sincos --epochs 4000
```

La ventana usa ImGui, ImPlot, GLFW y OpenGL, con las versiones y hashes fijados en [CMakeLists.txt](CMakeLists.txt). El lanzador fija la raíz de esta copia como directorio de ejecución. El binario directo escribe desde el directorio actual o en la ruta que se le dé por `--out`.

## Editar el experimento en main()

Los valores de F6 están en `main()` de `DRANN_vectorial.cpp`, bajo `CONFIGURATION`: `neuronNumber`, `lr`, `epochs`, `seed`, `outputDimension`, `dataset`, `output` y `batch`. Inmediatamente después aparece la configuración de presión, recuperación y tolerancias, en `trainingConfig`.

El bloque `DATASET: INPUTS AND OUTPUTS` del mismo `main()` contiene entradas, objetivos y normalización. Para Celsius, editar `trainingCelsius`, `validationCelsius` y el generador `example`, que transforma cada entrada escalar en el punto ordenado (Fahrenheit, Kelvin). Para seno/coseno, editar `sampleNumber` y su generador. El registro y la interfaz de esta entrega están preparados para dos coordenadas.

Los argumentos de terminal sobrescriben esos valores solo para la ejecución solicitada; por ejemplo, `--neurons 10 --lr 0.005 --epochs 1000`. La reorganización conserva el núcleo y las matemáticas de entrenamiento de los archivos `.h`.

## Logs de terminal

La consola conserva la estructura y los colores de `DRANN_C_F.cpp`: separador, cabecera `Epoch -> e / meta.`, las pérdidas en una línea y `Active connections: ... | Participating neurons: ... | Steps: ... | lambda: ...` en otra. La línea de pérdidas por coordenada usa las etiquetas del dataset elegido. `Steps` es la media de pasos de validación. La única métrica añadida al resumen original es lambda; las decisiones de reducción se conservan como avisos separados.

Se mantienen los mensajes de inicio, continuación y finalización, el progreso con estimación temporal y el bloque final de ideales y puntos físicos. No se imprimen tiempos ni lr entre épocas. La salida redirigida no incluye secuencias ANSI. El cambio de formato no altera historiales, snapshots ni decisiones de entrenamiento.

## Modelo y control

- Cada neurona mantiene un vector. Las transmisoras se eligen por máximos locales del módulo L2 y pasan la diferencia **vectorial**. Se agregan los mensajes antes del sesgo y Leaky ReLU (`sigma(0) = 0`, derivada en cero igual a 1).
- La nube de salida contiene los vectores finales de las participantes, incluidas las retiradas. La pérdida promedia neuronas y suma las coordenadas. `referencia/` conserva la lectura anterior con alpha y beta.
- Se conservó la partición de Celsius. Para seno/coseno se usa la partición propuesta en el encargo, con 64 ángulos de entrenamiento y 64 puntos medios de validación. La entrada es `x/pi` y las etiquetas son `(sin(x), cos(x))`. El cálculo de estas funciones solo se usa para generar el dataset.
- El control por defecto es `--lambda 0` y omite el solver y el umbral del proximal. Para habilitar presión, da un valor positivo a `--lambda`. La configuración base de esta copia conserva 3 neuronas, `lr=0.001`, semilla 42 y meta inicial de 100 000 épocas. Conviene indicar un presupuesto explícito para cada comparación.

## Presión, recuperación y restauración

```sh
./bin/drann --batch --dataset celsius --seed 42 --epochs 4000 \
  --lambda 0.001 --min-improvement 0.001
```

La presión empieza después del calentamiento y el estancamiento de la **pérdida de entrenamiento**. Una arista simétrica tiene un solo peso y pertenece a los grupos de sus dos extremos. Se aplica SGD a la pérdida de datos, se anulan los pesos internos no positivos y se resuelve el proximal de los grupos solapados por Dykstra. Se comprueban factibilidad dual, invariante y brecha. El umbral de poda se aplica una vez terminado el solver.

Si una actualización aísla neuronas, se evalúa inmediatamente ese estado y se guarda como recuperación de época cero. La recuperación usa lambda cero. Se acepta el mejor candidato por pérdida de validación entre los que reducen el número de participantes y cumplen las tolerancias **por coordenada**, tanto en entrenamiento como en validación. Si no hay reducción, falla el cálculo o ningún candidato cumple, se restaura la copia previa. Con una misma estructura, un nuevo intento exige mejora significativa respecto a la referencia anterior. El historial distingue las épocas gastadas de la época del modelo restaurado.

Los valores iniciales son calentamiento 1000, paciencia 200, mejora mínima `1e-8`, rampa 200, hasta 2000 épocas de presión, 500 de recuperación, tres intentos y tolerancias predictivas absoluta `1e-6` y relativa 0,05. El solver usa tolerancias absoluta y relativa `1e-10`, hasta 2000 barridos y umbral de arista `1e-12`. `--absolute-tolerance X,Y` permite un umbral distinto por coordenada. La [especificación](docs/especificacion.md) contiene el backward y el problema proximal completos.

## Historiales

Cada ejecución crea una carpeta propia con historial, diagnóstico por neurona, eventos, configuración y snapshots. `--out` exige una carpeta nueva. El CSV principal registra pérdida pura, pérdida por coordenada, peor punto, regularizador, objetivo, lambda, brecha, pasos y fase. `neurons.csv` reúne el diagnóstico calculado sobre validación, con distancias medias ordenadas por identificador y `-` para parejas no evaluables. Los empates terminales cuentan pares conectados con módulos iguales y vectores distintos.

`pre_attempt_N.txt` y `recovery_0_N.txt` dejan constancia de cada referencia y del candidato inicial. `best_predictive.txt` se selecciona independientemente por pérdida pura de validación observada y puede corresponder a un estado exploratorio. `final_model.txt` contiene el modelo resultante de las decisiones de aceptación o restauración. Los archivos incluyen pesos, sesgos, máscaras, identidades, configuración, tiempos de entrenamiento en épocas y estado del generador. La continuación desde la interfaz opera dentro del proceso; esta versión exporta los snapshots para inspección y conserva las copias completas en memoria durante el entrenamiento.

## Comprobaciones

```sh
cmake -S . -B build/headless -DDRANN_BUILD_GUI=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build/headless --parallel 2
ctest --test-dir build/headless --output-on-failure
```

Las pruebas incluyen los casos analíticos de una arista y de un triángulo, 52 derivadas por diferencias finitas, empates, recepción compartida, máscaras, evaluación inmutable y aceptación y restauración con presupuestos acotados. La misma compilación ejecuta la batería guardada en `referencia/`.

Consulta [la comparación guardada](results/README.md) para las pérdidas, tiempos y reducciones observadas.
