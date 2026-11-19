# DRANN 2 — laboratorio C++ / Godot

Implementación del encargo `docs/ENCARGO.md`. El cálculo usa C++17 y `double`, derivadas manuales y SGD por ejemplo. Consola y GDExtension enlazan el mismo núcleo. La tarea inicial es Celsius → dos pozos distintos, Fahrenheit y Kelvin. Los parámetros provisionales se encuentran en `laboratoryConfig()`, al principio de **main.cpp**; el reparto de datos está inmediatamente después, en `laboratoryDataset()`.

## Ejecutar aquí

Desde `/home/luis/personal/DRANN/DRANN_2`:

```bash
./run.sh                        # laboratorio gráfico
./run.sh editor                 # abrir proyecto en el editor Godot
./run.sh console --epochs 1000  # consola; carpeta nueva results/run_...
```

Se entregan compilados locales y las dependencias descargadas en directorios ignorados por Git. Tras editar C++, ejecuta `./build.sh` antes de iniciar Godot o consola. La interfaz usa el nuevo código cuando se vuelve a abrir. No se requieren credenciales de GitHub para descargar las dependencias públicas.

## Construir

Requisitos: Linux x86_64, compilador C++17, CMake >=3.20, Python 3, curl, tar y sha256sum. El modo gráfico necesita las bibliotecas de sistema de Godot/OpenGL.

```bash
./build.sh                 # núcleo, consola, pruebas y GDExtension
./build.sh core            # solo C++; no descarga ni necesita Godot
ctest --test-dir build --output-on-failure
./tests/godot_smoke.sh --headless
./tests/godot_smoke.sh --display-driver x11 --rendering-method gl_compatibility
```

`build.sh` fija **Godot 4.5.1 stable** y **godot-cpp godot-4.5-stable**, commit `e83fd0904c13356ed1d4c3d09f8bb9132bdc6b77`. Verifica ambos archivos descargados con SHA256. CMake también fija el hash del SDK. Se compila sin `-march=native`, para evitar exigir la ISA de otra CPU. `DRANN_JOBS=2 ./build.sh` limita el paralelismo.

Alternativa CMake sin Godot:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DDRANN_BUILD_GODOT=OFF
cmake --build build --parallel 6
```

## Usar el laboratorio

- Neuronas, lr, semilla, activación y meta de épocas se editan arriba. Los selectores actualizan inmediatamente el modelo inicial y su representación, sin pulsar Entrenar. Neuronas, lr, semilla, meta de épocas y activación quedan bloqueados desde el primer paso, incluso al pausar. **Reiniciar** crea un modelo nuevo y desbloquea los controles para la siguiente ejecución. Al cargar un modelo se muestran y bloquean sus parámetros efectivos. La meta es el contador de trabajo que detiene la ejecución gráfica.
- **Entrenar / continuar** procesa un ejemplo por frame. **Pausar** congela el entrenamiento. El deslizador **Pausa entre épocas** ajusta en directo una espera de 0 a 5 segundos (inicialmente 1 s), manteniendo visible la última instantánea mientras termina la animación. Con 0 s se entrena a la máxima velocidad; la espera no modifica SGD ni el orden de los ejemplos y se congela al pausar. **Un ejemplo** ejecuta un único forward/backward/SGD.
- El deslizador **τ** recorre la cinta del último ejemplo: potenciales, transmisoras naranjas, receptoras verdes, retiradas grises y aisladas con aro magenta. `τ=0` es la inicialización. Las flechas usan exactamente las conexiones grabadas en ese paso.
- La distribución circular inmanente es una disposición visual. La vista transcendente representa sesgo → posición final con animación directa; la animación no calcula ni modifica parámetros. Cruces blancas indican posiciones fuera de la mediana; permanecen visibles y siguen aprendiendo. Las tablas conservan posiciones y puntuaciones de todas las dimensiones.
- **Predecir** toma la entrada física y calcula con parámetros congelados, sin etiquetas. **Comparar ideal** revela después las etiquetas de temperaturas. Tras cargar un dataset genérico, esta comparación solo está disponible si existe un generador de etiquetas compatible.
- Cambia los ejes de proyección cuando T>2. La pertenencia y la mediana siempre utilizan T completo. T=1 se representa sobre una recta.
- El nodo de entrada muestra sus conexiones dirigidas a todas las neuronas, incluidas las aisladas del grafo interno. La gráfica distingue entrenamiento (cian) y validación (naranja), etiqueta los ejes Pérdida geométrica / Época y permite alternar a escala logarítmica de base 10. Para representar ceros en esa escala se utiliza un suelo visual de 1e-12, sin modificar las pérdidas. La barra de progreso es un control nativo de Godot. Las métricas indican cuántas lecturas válidas hay: una ausencia nunca se convierte en cero.
- En la caja de ruta puedes escribir, por ejemplo, la ruta absoluta de `results/celsius_fahrenheit_kelvin_seed42_1000/final.model` o `best.model` y pulsar **Cargar**. **Guardar actual/mejor** usa esa caja; cambia la ruta para conservar copias. `user://laboratorio.model` apunta al directorio de datos de Godot, no al repositorio.
- La cabecera distingue época ejecutada, época del modelo cargado y época del mejor modelo. Tras cargar el mejor modelo, el trabajo acumulado conserva su contador.

Reanudar por consola (el número de épocas es adicional):

```bash
./run.sh console --load results/celsius_fahrenheit_kelvin_seed42_1000/final.model --epochs 200
```

## Núcleo y registros

`DRANN.cpp` separa inicialización, forward, pérdida, backward, actualización, lectura, validación y persistencia. Cada conexión simétrica tiene un único peso; si deja de ser positivo se anula permanentemente. Las neuronas aisladas conservan sus salidas y gradientes. El estado inmanente es escalar. La transcendencia es `q_i = biasT_i + weightsT_i * V_i`. No hay origen añadido, regularización ni lambda.

El forward compara potenciales firmados, selecciona máximos locales simultáneos, suma las recepciones antes de aplicar el sesgo y la activación una sola vez, y retira las transmisoras conservando su potencial. El backward deriva la ruta ejecutada. La pérdida es atracción media por neurona más cobertura media por pozo, con reparto de empates exactos. El mínimo se selecciona mediante distancia cuadrada, que preserva el orden matemático incluso al saturarse la evaluación numérica de la gaussiana.

Las escalas `100` y `373.15` se calculan exclusivamente con entrenamiento. El radio inicial es la mitad de la mediana de las distancias entre pozos distintos de cada ejemplo de entrenamiento: aproximadamente `0.42269`. Si no hay distancias positivas se usa y registra el fallback `0.5`.

Cada ejecución de consola exige una carpeta nueva. Exporta:

- `history.csv`: pérdidas geométricas de entrenamiento/validación por época; atracción, cobertura, media de pasos; distancia cuadrada media al pozo por eje; cobertura de lectura y suma de errores cuadrados normalizados (SSE). RMSE físico = `scaleY * sqrt(SSE / lecturas_validas)`; debe interpretarse junto con válidas/total.
- `examples.csv`: lectura de cada ejemplo de validación y eje en cada época, valor físico, ideal físico, integrantes y diámetro normalizado del grupo elegido. Un valor ausente queda vacío.
- `neurons.csv`: por época, entrada de validación, neurona y coordenada; posición normalizada, eje asignado (`-1` ambiguo), puntuación, cambios de especialización entre épocas para esa entrada, desacuerdo entre los ejes dominantes de posición/sesgo/peso, impulso y orientación. La orientación nula queda vacía. Estos diagnósticos no intervienen en el entrenamiento.
- `metadata.txt`: configuración efectiva, versiones, estado RNG/parámetros y contadores.
- `final.model` y `best.model`: todos los parámetros, máscaras, escalas, tolerancias, dataset, RNG, orden de entrenamiento y cursor; permiten continuación exacta incluso a mitad de época. El mejor se selecciona por pérdida geométrica de validación. Al cargar se inicia un historial de visualización nuevo con las métricas del estado cargado; no se finge haber recuperado las filas anteriores.

El núcleo admite T arbitrario y también menos neuronas que ejes, por lo que puede haber ejes sin representantes. Para cambiar T en la tarea de consola/interfaz hay que adaptar las etiquetas y el generador de comparación en `main.cpp`; no basta con cambiar T manteniendo dos etiquetas. El soporte de entradas de esta entrega es escalar, como pide el experimento.

## Comprobaciones y experimento entregado

Se han comprobado: recepción simultánea y sesgo único, retirada, invariancia al renombrar neuronas, comparación firmada, conexiones simétricas/anulación irreversible, aprendizaje aislado, 42 derivadas por diferencias finitas con identidad/tanh y ruta estable, sesgo compartido entre recepciones, reparto de empates, geometría y lectura T=1/2/3, signos negativos, direcciones nulas, grupos sin encadenamiento, ambigüedad/ausencias, independencia de etiquetas, validación inmutable y reanudación exacta del modelo actual y del mejor. También se ejecutan ASan/UBSan y una prueba automatizada real de los controles de Godot, incluyendo paridad con las posiciones exportadas por consola.

`results/celsius_fahrenheit_kelvin_seed42_1000/` contiene la ejecución de referencia con semilla 42, cinco neuronas, ocho ejemplos de entrenamiento y dos de validación. Se guarda también una captura de la prueba gráfica. El informe de esa carpeta recoge las mediciones finales y sus límites. El conjunto es pequeño: esos errores no acreditan generalización fuera del intervalo ni resuelven pozos coincidentes en cero.

La especificación original permanece en `docs/ENCARGO.md`, incluidos los experimentos posteriores todavía pendientes. Los archivos de DRANN_1 y la reorganización previa del repositorio se han conservado.

Los títulos de las tres representaciones se editan al principio de `godot/laboratory.gd`, en `TITLE_RHIZOME`, `TITLE_TRANSCENDENT` y `TITLE_LOSS`.

Puedes arrastrar las neuronas del rizoma con el botón izquierdo, incluso entrenando o en pausa. Se recolocan sus conexiones junto con ellas. Esta disposición es visual, independiente de los pesos y de las posiciones matemáticas del plano transcendente. Se conserva al refrescar o cambiar parámetros que no alteren el número de neuronas; al cambiar ese número se regenera la disposición.
