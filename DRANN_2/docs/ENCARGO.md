# DRANN: laboratorio con Godot — encargo para Codex local

Fecha: 19 de noviembre de 2026.

## 1. Qué debes hacer

Implementa un laboratorio ejecutable de DRANN en C++ desde cero, con Godot 4 incluido desde esta primera entrega. Inspecciona el proyecto local, crea el código, compílalo y realiza las comprobaciones necesarias. El resultado debe poder entrenarse, visualizarse y guardar/cargar un modelo.

La primera tarea será Celsius → (Fahrenheit, Kelvin), con pocas neuronas. El núcleo debe admitir salidas vectoriales de dimensión arbitraria; el experimento inicial tendrá dos componentes. No incorporar reglas matemáticas especiales de temperaturas a la predicción: las fórmulas C→F,K solo generan el dataset y sirven para evaluar después.

Este documento consolida la versión actual de la idea. Sustituye las especificaciones anteriores que describan potenciales inmanentes vectoriales, un único pozo para todo el vector de salida, un origen transcendente desplazado o exclusión automática de neuronas aisladas. No mezclar esas variantes con esta implementación.

## 2. Referencias de código y estilo: requisito importante

Se entregan junto a este documento:

1. `satellite_CNN_HL6.cpp`: referencia principal de prácticas, organización y presentación del código.
2. `DRANN_C_F.cpp`: referencia de la DRANN anterior, su organización, registros, barra de progreso y ejecución.

Lee ambos antes de implementar. El usuario quiere que el nuevo código se parezca **lo máximo posible** a su forma de programar. No basta con implementar una red equivalente con una arquitectura de software ajena a esas referencias.

Conservar, en lo compatible con Godot:

- Organización reconocible mediante `struct`, clases y funciones explícitas; bucles y cálculo numérico comprensibles.
- Estilo de nombres como `samplesTraining`, `targetsValidation`, `trainLoss`, `validationLoss`, `lr`, `e`, y auxiliares como `utils`, `progressBar` y `timeCast`, adaptando los nombres a la DRANN.
- Indentación, comentarios, separadores de secciones y disposición de las funciones según los archivos entregados.
- Un bloque de parámetros fácil de localizar en `main.cpp`, junto a la función `main`: cantidad de neuronas, dimensiones, dataset, semilla, `lr`, épocas, activación, opciones de visualización y tolerancias.
- Barra de progreso similar a la referencia: nombre del proceso, barra con `#` y relleno, porcentaje, contador, tiempo restante y finalización. Colores ANSI cuando la terminal los soporte.
- Registros de entrenamiento y validación, medición de duración y guardado/carga explícitos.
- Separación reconocible de inicialización, forward, pérdida, backward, actualización, entrenamiento y validación.

La estructura de los registros de consola debe seguir `DRANN_C_F.cpp`; toma del archivo de la CNN las prácticas de presentación y utilidades. Añade la información de la nueva geometría de forma compatible y ordenada. No reemplazarlo por un formato de logs completamente distinto sin necesidad.

Las referencias son código para estudiar y reutilizar, no instrucciones para copiar su arquitectura matemática. No trasladar convoluciones, Adam, regularizadores ni condiciones de parada de la CNN si este documento no los especifica. La matemática de este encargo tiene prioridad sobre las versiones antiguas.

Usa C++17 o superior, preferiblemente `double` para el cálculo del modelo. No utilizar PyTorch, LibTorch, autodiferenciación ni bibliotecas de redes neuronales. Las derivadas serán manuales. Godot y `godot-cpp` sí son dependencias previstas.

## 3. Integración y organización del proyecto

Revisa `git status` e instrucciones locales antes de editar. Conserva código, configuraciones y resultados existentes. Implementa esta variante en una carpeta identificable, por ejemplo `DRANN_Godot/`, salvo que ya exista una estructura apropiada que puedas completar.

Organización mínima propuesta, adaptándola al estilo de los `.cpp`:

```text
DRANN_Godot/
  CMakeLists.txt
  main.cpp                    # configuración central + ejecución por consola
  DRANN.h / DRANN.cpp          # modelo, entrenamiento y backward manual
  utils.h / utils.cpp          # progreso, tiempos, registros y auxiliares
  godot/                      # project.godot, escenas y presentación
  extension/                  # GDExtension y registro de clases
  tests/                      # comprobaciones numéricas y de comportamiento
  README.md                   # compilación y ejecución local
```

No es obligatorio crear un archivo por cada concepto pequeño. Evita una infraestructura genérica de tensores o una jerarquía extensa de clases.

El núcleo C++ será independiente de Godot, compartido por el ejecutable de consola y la GDExtension. Godot no tiene un `main()` de usuario como un ejecutable convencional: adapta esa entrada sin duplicar los parámetros.

Una solución válida es definir una función de configuración compartida en `main.cpp`, junto al bloque editable, y compilar el `main()` de consola solo en ese destino. La extensión llama a la misma función. Documenta dónde cambiar los valores; no mantener dos listas de constantes que puedan divergir. La interfaz puede permitir cambios para una ejecución, registrando sus valores efectivos.

Usa una versión estable concreta de Godot 4 y una versión compatible de `godot-cpp`; documenta ambas. CMake es la propuesta de construcción. Si el proyecto ya tiene un sistema compatible, consérvalo. No dejar la dependencia en una rama flotante sin indicar qué versión se ha probado.

## 4. Terminología y configuración

- Época: `e`, una pasada por todos los ejemplos de entrenamiento.
- Ejemplo: `ej`, un par entrada–salida.
- Paso interno: `tau`; `t` es el último índice interno del ejemplo.
- Tasa de aprendizaje: `lr`.
- Activación: `sigma` o \(\sigma\).
- Valores normalizados: \(x^*\), \(y_k^*\).
- Dimensión transcendente: `T` o `outputDimension`, no `D`.
- Vectores matemáticos: \(\vec{\ldots}\).
- Sesgo transcendente: \(\vec b_i^{(T)}\); `(T)` identifica el plano, no una transpuesta.
- Pozos ideales: \(\vec\xi_k\).
- El cognifenómeno primitivo **transciende** del estado inmanente.
- El desplazamiento se denomina **atracción trascendente**, no tiro.

Valores iniciales aceptados por el usuario:

| Parámetro | Valor |
|---|---:|
| Neuronas | 5 |
| Ejemplos totales | 10 |
| Entrenamiento / validación | 8 / 2 |
| Semilla | 42 |
| `lr` | 0.005 |
| Épocas | 1000 |

Los valores numéricos adicionales de este documento son **defaults provisionales de implementación**, configurables y registrables. No son resultados de un ajuste experimental ni deben describirse como elecciones óptimas.

## 5. Dataset inicial y normalización

Para concretar los diez ejemplos, usa este reparto inicial:

```text
samplesTraining_C   = {-40, -20, 0, 20, 40, 60, 80, 100}
samplesValidation_C = {10, 70}

F(C) = 1.8 * C + 32
K(C) = C + 273.15
```

Normalización propuesta para este laboratorio:

\[
S_x=\max_{ej\in\mathrm{training}}|C_{ej}|=100,
\qquad x^*=C/S_x.
\]

Para las dos salidas se utiliza **un mismo divisor**, según la propuesta del usuario:

\[
S_y=\max_{ej\in\mathrm{training}}\{F_{ej},K_{ej}\}=373.15,
\qquad F^*=F/S_y,\quad K^*=K/S_y.
\]

Calcula y guarda las escalas usando solo entrenamiento. No recalcularlas con validación ni por ejemplo, ni recortar valores nuevos a [0,1]. Fahrenheit puede ser negativo.

La generación de etiquetas y la evaluación deben estar separadas del forward. Al validar, el predictor solo recibe la entrada normalizada; después se consultan las etiquetas para medir el error.

En el núcleo, cada objetivo es un vector de tamaño `outputDimension`. Para una entrada de varias componentes, puede inicializarse el potencial con un producto escalar de pesos de entrada y \(\vec x^*\); este experimento solo necesita una entrada escalar. No introducir un estado vectorial por neurona para soportar varias entradas o salidas.

## 6. Geometría: un pozo sobre cada eje

**Dos componentes de salida implican dos ejes y dos pozos, no un único punto (F*, K*).**

\[
\vec\xi_F(x)=(F^*(x),0),
\qquad
\vec\xi_K(x)=(0,K^*(x)).
\]

Para cualquier dimensión \(T\):

\[
\boxed{\vec\xi_k(x)=y_k^*(x)\vec e_k,\quad k=1,\ldots,T.}
\]

El eje conserva la identidad y el orden de la magnitud. No ordenar ni permutar las componentes de los objetivos.

Al procesar un ejemplo, utilizar sus pozos, no todos los pozos de todos los ejemplos del dataset simultáneamente.

El caso en que varias magnitudes tienen valor cero y sus pozos coinciden se deja para una versión posterior por decisión del usuario. No afirmar que una regla puramente geométrica ya distingue esas etiquetas. Conserva sus índices y permite señalar ambigüedad. El experimento inicial no presenta esa coincidencia.

## 7. Forward inmanente: potencial escalar

Cada neurona tiene **un potencial escalar** \(V_i\), un sesgo inmanente escalar \(b_i\) y pesos de entrada. Las conexiones internas son escalares y simétricas: un parámetro por pareja, sin autoconexiones.

Para la entrada inicial:

\[
V_i^{(0)}=\sigma(w_i^{\mathrm{input}}x^*+b_i).
\]

Activaciones: implementar identidad y tanh como opciones. **Default para C→F,K: identidad.** Tanh queda disponible para experimentos no lineales; no introducir seno o coseno conocidos como atajo de una tarea futura.

### Selección simultánea por máximos locales

En cada `tau`, considerar solo neuronas que todavía no se hayan retirado y conexiones internas habilitadas.

Una neurona es transmisora si:

1. No tiene ninguna vecina disponible conectada de potencial estrictamente mayor.
2. Tiene al menos una vecina disponible conectada de potencial estrictamente menor.

Todas las transmisoras válidas actúan simultáneamente. Emiten a todas sus vecinas disponibles de menor potencial. No usar máximo global, prioridad por peso ni identificadores para dar turnos. Los potenciales iguales no producen transmisión entre sí.

**Comparar el valor del potencial escalar, no su valor absoluto ni su módulo.** La selección por módulo correspondía a la variante vectorial descartada.

Para la receptora \(j\), con transmisoras \(\mathcal T_j\):

\[
\Delta V_{j,i}^{(\tau)}=V_i^{(\tau)}-V_j^{(\tau)}>0,
\]

\[
\boxed{
V_j^{(\tau+1)}=
\sigma\left(
V_j^{(\tau)}+
\sum_{i\in\mathcal T_j}w_{j,i}\Delta V_{j,i}^{(\tau)}+b_j
\right).
}
\]

- Calcular todo con los estados previos al paso.
- Sumar contribuciones y aplicar sesgo y activación **una vez por receptora y paso**.
- No actualizar ni volver a aplicar el sesgo a neuronas que no reciben.
- Una neurona no recibe y transmite en el mismo paso.
- La señal no se reparte como un caudal. No dividir entre destinatarias, no restar a la transmisora lo recibido por otras y no añadir `eta`.
- Tras transmitir, la transmisora conserva su potencial y se retira durante el resto del ejemplo.
- Terminar cuando no haya transmisoras válidas. Puede haber neuronas que no hayan transmitido. No aprender una condición de parada ni añadir iteraciones recurrentes tras retirarse.
- Cada paso retira al menos una transmisora: el proceso es finito. Incluir una comprobación de consistencia del límite de pasos, no una nueva regla de parada matemática.

### Aislamiento

Una neurona sin conexiones internas habilitadas recibe la entrada, calcula \(V_i^{(0)}\) y lo conserva. No transmite de vuelta a la entrada ni participa en las transmisiones internas, pero **sí puede contribuir al cognifenómeno y aprender**.

No excluirla de la pérdida ni del backward. Aislamiento interno, exclusión de la representación y eliminación completa son conceptos diferentes. No añadir resúmenes del resto del rizoma a su transcendencia.

## 8. Atracción trascendente afín y origen cero

Cada neurona tiene dos vectores aprendibles de tamaño \(T\): \(\vec b_i^{(T)}\) y \(\vec w_i^{(T)}\).

\[
\boxed{
\vec q_i(x)=\vec b_i^{(T)}+
\underbrace{\vec w_i^{(T)}V_i^{(t)}(x)}_{\vec d_i(x)}.
}
\]

- **No hay un origen adicional desplazado** \(\vec o\). El origen es cero y el sesgo transcendente es directamente la posición inicial.
- No usar una matriz que multiplique un potencial vectorial: el estado es escalar y los pesos transcendentes son un vector.
- No añadir activación transcendente, espiral, circunferencia, campo virtual aprendido ni simulación orbital.
- Se calcula un desplazamiento directo. La animación es solo visual.
- Los coeficientes transcendentes pueden ser positivos, negativos o cero, y los ceros siguen siendo aprendibles.
- Impulso: \(\|\vec d_i\|\). Orientación: \(\vec d_i/\|\vec d_i\|\) si no es nulo. No crear dos predictores separados para ambas cantidades.
- Una neurona puede aprender a alinear sesgo y pesos con un eje. No introducir una asignación permanente manual.
- Permitir que la lectura detecte cambios de especialización según la entrada, aunque una transformación afín escalar limita qué cambios son representables.

## 9. Inicialización reproducible

- Pesos de entrada: uniformes en (0,1), excluyendo cero; sesgos inmanentes a cero.
- Grafo interno inicialmente completo para cinco neuronas, con pesos independientes por pareja uniformes en (0,1), compartidos en ambas direcciones.
- Repartir ejes iniciales de la forma más equilibrada posible; asignar aleatoriamente las neuronas a esos ejes con la semilla indicada. El reparto no determina cuántas necesitará cada propiedad.
- Posiciones iniciales sobre los ejes, cerca del origen, usando la misma distribución de distancias para todos. Default provisional: distancia uniforme entre 0.01 y 0.05 en unidades normalizadas.
- En la tarea de temperaturas, iniciar hacia el semieje positivo. En un dataset genérico puede elegirse el signo según una referencia de entrenamiento; no utilizar etiquetas de validación.
- \(\vec b_i^{(T)}=s_i\vec e_{k_i}\).
- Inicializar \(\vec w_i^{(T)}\) con una pequeña componente sobre ese eje, uniforme entre -0.05 y 0.05; las otras comienzan en cero, pero **no se congelan**.

No es necesario un sesgo exactamente uno para reconocer un eje. Los valores del sesgo son posiciones aprendibles, no etiquetas one-hot.

Para entradas/dimensiones distintas, registrar la configuración efectiva. Una población menor que el número de pozos distintos no puede cubrirlos todos simultáneamente; detectar esa configuración en lugar de prometer cobertura completa.

## 10. Pérdida de atracción y cobertura

Los pozos gaussianos orientan el aprendizaje, no el forward de validación:

\[
U_k(\vec q,x)=-A\exp\left(-\frac{\|\vec q-\vec\xi_k(x)\|^2}{2r^2}\right).
\]

Para la referencia, fijar \(A=1\) y un alcance compartido \(r>0\). Usar la forma normalizada del error:

\[
\ell_{ik}=1-\exp\left(-\frac{\|\vec q_i-\vec\xi_k\|^2}{2r^2}\right).
\]

\[
\boxed{
L_{\mathrm{atracción}}=\frac1N\sum_i\min_k\ell_{ik},
\quad
L_{\mathrm{cobertura}}=\frac1T\sum_k\min_i\ell_{ik},
\quad
L=L_{\mathrm{atracción}}+L_{\mathrm{cobertura}}.
}
\]

`N` incluye todas las neuronas existentes, también las internamente aisladas. La interpretación geométrica de una salida es un proceso aparte; una neurona no se excluye de la pérdida porque el lector la marque ambigua.

Default provisional para `r`: la mitad de la mediana de las distancias entre pozos distintos de cada ejemplo de entrenamiento. Calcular una vez y guardar. Si no hay parejas o el valor no es positivo, usar un fallback configurable de 0.5 y registrarlo. Puede proporcionarse explícitamente un `r` positivo.

En los mínimos exactos, repartir el gradiente por igual entre los minimizadores empatados. No usar el ideal para forzar una asignación distinta de la definida por la pérdida. Las pruebas numéricas de gradiente evitarán empates y cambios de rama.

La gaussiana puede saturar lejos del pozo. Registrar también distancia cuadrática sin saturación. No sustituir silenciosamente la pérdida ni activar un rescate que todavía no forma parte de la entrega.

## 11. Backward manual y actualización

Guardar una cinta del recorrido ejecutado: estados previos, receptoras, transmisoras, conexiones utilizadas, argumentos de las activaciones y estados conservados.

No derivar las comparaciones discretas que seleccionan transmisoras. Derivar las operaciones del recorrido realizado. No introducir gradientes sustitutos ni evaluar órdenes alternativos.

### Desde la geometría

Para una pareja seleccionada por un mínimo:

\[
\frac{\partial\ell_{ik}}{\partial\vec q_i}
=\exp\left(-\frac{\|\vec q_i-\vec\xi_k\|^2}{2r^2}\right)
\frac{\vec q_i-\vec\xi_k}{r^2}.
\]

Acumular las contribuciones de atracción y cobertura con sus factores \(1/N\), \(1/T\) y el reparto de empates. Una misma pareja puede contribuir por ambos términos.

Si \(\vec g_i=\partial L/\partial\vec q_i\):

\[
\frac{\partial L}{\partial\vec b_i^{(T)}}=\vec g_i,
\quad
\frac{\partial L}{\partial\vec w_i^{(T)}}=V_i^{(t)}\vec g_i,
\quad
\frac{\partial L}{\partial V_i^{(t)}}=\vec w_i^{(T)}\cdot\vec g_i.
\]

### Por una recepción interna

Con \(z_j\) igual al argumento de la activación y \(\delta_j=(\partial L/\partial V_j^{(\tau+1)})\sigma'(z_j)\):

\[
\frac{\partial L}{\partial w_{j,i}}\mathrel{+}=
\delta_j(V_i^{(\tau)}-V_j^{(\tau)}),
\qquad
\frac{\partial L}{\partial b_j}\mathrel{+}=\delta_j,
\]

\[
\frac{\partial L}{\partial V_i^{(\tau)}}\mathrel{+}=w_{j,i}\delta_j,
\qquad
\frac{\partial L}{\partial V_j^{(\tau)}}\mathrel{+}=
\left(1-\sum_{i\in\mathcal T_j}w_{j,i}\right)\delta_j.
\]

Las neuronas no actualizadas transmiten su gradiente al estado anterior mediante identidad. Retirarse del forward no elimina la contribución al backward. Acumular antes de escribir en los estados anteriores; no retropropagar secuencialmente como si las recepciones no hubieran sido simultáneas.

Derivar finalmente la inicialización de entrada. El sesgo inmanente utilizado al inicializar y al recibir es el mismo parámetro: acumula todas sus contribuciones.

### Actualización

Después de cada ejemplo: forward → pérdida → backward → SGD → anulación de conexiones internas.

\[
\theta\leftarrow\theta-lr\frac{\partial L}{\partial\theta}.
\]

Actualizar cada peso interno simétrico una sola vez. Si queda nulo o negativo, anular esa conexión permanentemente para los ejemplos y épocas posteriores. No reintegrarla en esta entrega. No imponer una cota superior nueva a los pesos.

La anulación no afecta a pesos de entrada, pesos transcendentes ni sesgos. No borrar neuronas por quedarse sin conexiones. No añadir Adam, group-lasso, presión de reducción, regularización o clipping sin identificarlos como cambios de experimento fuera de esta referencia.

## 12. Lectura sin ideales: pertenencia conjunta y mediana

La lectura no consulta \(\vec\xi_k\) ni las etiquetas. Es una heurística de interpretación; no modifica la pérdida ni decide qué neuronas participan en el backward.

### Pertenencia conjunta

Para un vector no despreciable:

\[
A_k(\vec z)=\frac{z_k^2}{\|\vec z\|^2}.
\]

Combinar **a la vez** las evidencias de posición, sesgo y dirección:

\[
\boxed{
S_{i,k}=\operatorname{media}_{\vec z\in\mathcal Z_i}A_k(\vec z),
\quad
\mathcal Z_i\subseteq\{\vec q_i,\vec b_i^{(T)},\vec w_i^{(T)}\}.
}
\]

Omitir de \(\mathcal Z_i\) los vectores con norma menor o igual a una tolerancia. No asignarles una dirección inventada. `S` es una puntuación heurística, no una probabilidad de evidencias independientes.

Defaults provisionales para cerrar el algoritmo:

- `zeroNormTolerance = 1e-12`.
- `membershipMinScore = 0.75`.
- `membershipMinMargin = 0.20`, entre la primera y segunda puntuación.

Asignar como máximo un eje por neurona si supera ambos criterios. Si no hay evidencias, hay empate o no se alcanza confianza suficiente, marcarla ambigua para la lectura. Para T=1 no existe segundo eje y el margen no aplica.

No congelar etiquetas entre entradas. Registrar cambios de especialización y desacuerdos, sin penalizarlos por sí mismos. Se puede registrar historial como diagnóstico, pero no añadir otro clasificador aprendido.

### Congregaciones y salida

Para cada eje:

1. Reunir las coordenadas de las neuronas asignadas a él.
2. Ordenarlas conservando identificadores para los registros.
3. Formar grupos de diámetro limitado. Default `clusterMaxSpan = 0.05` en coordenadas normalizadas: un valor entra en el grupo actual solo si la diferencia respecto al menor valor de ese grupo no supera el límite. Si lo supera, iniciar otro. Evitar el encadenamiento que une grupos por vecinos intermedios.
4. Una sola representante puede constituir un grupo.
5. Si solo hay un grupo, seleccionarlo. Si hay varios, seleccionar uno únicamente si contiene más de la mitad de las candidatas de ese eje. En otro caso, declarar respuesta ambigua.
6. Devolver la mediana del grupo seleccionado. Para cantidad par, promedio de sus dos valores centrales.

Devolver por componente: valor, estado válido/ambiguo/sin representantes, integrantes, puntuaciones y dispersión. Una ausencia no se representa como cero. El consenso no garantiza acierto: se evaluará después contra el ideal.

No añadir la propuesta posterior de una esfera de influencia: el usuario pidió descartarla. Los umbrales anteriores son controles de lectura, no radios de un campo físico ni criterios de eliminación.

## 13. Entrenamiento, validación, modelo y registros

- Usar RNG reproducible con semilla 42. Barajar solo entrenamiento por época, de manera reproducible.
- Validar después de cada época sin actualizar parámetros ni máscaras.
- Registrar pérdidas geométricas por separado de los errores del vector explícito leído mediante medianas.
- Para cada pozo, incluir el diagnóstico \(E_{k,ej}=\min_i\|\vec q_i-\vec\xi_k\|^2\), que no se satura como la gaussiana. Es una medida supervisada posterior, no el predictor.
- Si faltan componentes válidas, reportar cobertura de lectura y estados. No ocultar errores calculando una media solo sobre casos favorables sin decir cuántos faltan.
- Guardar el mejor modelo por pérdida geométrica media de validación, además del estado final si se desea. Identificar cuál se muestra.
- Guardar parámetros, máscaras de conexiones, escalas, activación, tolerancias de lectura, configuración y semilla. Para reanudar exactamente, conservar contador de entrenamiento y estado del RNG.
- Separar `bestEpoch` del contador de épocas efectivamente ejecutadas; cargar un mejor modelo no debe falsificar el progreso ni duplicar filas del historial.
- Guardar registros en una carpeta nueva por ejecución, sin sobrescribir resultados previos.

Conservar formatos existentes aprovechables de `DRANN_C_F.cpp`. Como mínimo, exportar historial por época, resultados por ejemplo y posiciones/membresías neuronales con identificadores. Puede utilizarse CSV con cabeceras claras y unidades indicadas. Si cambian columnas, documentarlo; no reinterpretar silenciosamente registros antiguos.

No hace falta lambda en esta entrega porque no hay presión de reducción. Si se mantiene una columna antigua por compatibilidad, su valor es cero y su significado debe quedar claro.

## 14. Godot: laboratorio y visualización del modelo entrenado

Primera entrega con interfaz funcional, no solo una carpeta vacía de Godot ni un visor estático de archivos finales.

### Vista inmanente

- Nodos y conexiones; disposición visual reproducible que no se confunda con la geometría transcendente.
- Potenciales escalares, transmisoras y receptoras por `tau`, retiradas y aisladas.
- Transmisiones simultáneas visualizadas desde la misma instantánea.
- Distinción clara entre retirada temporal del ejemplo y conexión anulada permanentemente.

### Vista transcendente

- Ejes etiquetados, sesgos/posiciones iniciales y posiciones finales.
- Atracción directa animable sin alterar el cálculo.
- Congregaciones, asignaciones, representantes ambiguas y medianas.
- Pozos ideales durante entrenamiento. En validación, mostrar primero la predicción; el ideal solo aparece como comparación posterior cuando está disponible.
- Mostrar explícitamente las posiciones excluidas de la mediana, sin eliminarlas del modelo.

El núcleo admite T arbitrario. En pantalla, permitir seleccionar dos ejes para una proyección; conservar todas las dimensiones en tablas y cálculo. Para T=1, representar una recta. No usar una proyección 2D para decidir pertenencia en un modelo de más dimensiones.

### Controles e historial

- Entrenar, pausar, continuar, avanzar un ejemplo y reiniciar con una semilla/configuración.
- Inspeccionar los pasos internos grabados de un ejemplo; no actualizar parámetros hasta completarlo.
- Gráfica de pérdidas de entrenamiento y validación y métricas por componente.
- Cargar/guardar modelo y ejecutar nuevas entradas con parámetros congelados.
- Barra de progreso nativa en Godot; barra de consola con el estilo de las referencias en el ejecutable.

Para esta población pequeña, puede ejecutarse trabajo incremental con presupuesto por frame. No bloquear la interfaz con mil épocas dentro de una sola llamada. El núcleo no debe depender de las animaciones ni de la frecuencia de refresco.

El modo de visualización del modelo entrenado no realiza backward. Las consultas de inspección no deben modificar parámetros ni consumir aleatoriedad de entrenamiento inadvertidamente.

## 15. Comprobaciones necesarias

No exigir una pérdida final específica sin haber ejecutado el modelo. Si no aprende, registrar e investigar el fallo; no sustituir la tarea por las fórmulas conocidas en el forward.

Comprobar al menos:

1. Transmisiones simultáneas con dos emisoras y una receptora; suma de contribuciones y sesgo una sola vez.
2. Retirada y conservación de potenciales; terminación sin depender del orden del vector de neuronas.
3. Simetría, ausencia de autoconexiones y anulación permanente de un peso interno no positivo.
4. Una neurona internamente aislada sigue produciendo posición y recibiendo gradiente de entrada y transcendencia.
5. Gradientes manuales mediante diferencias finitas en casos pequeños sin empates, sin pesos próximos a anulación y con el mismo recorrido discreto al perturbar. Incluir sesgo compartido en varias recepciones y las dos partes de la pérdida. No comparar derivadas atravesando cambios de ruta como si fuera una función suave.
6. Geometría y lectura con T=1, T=2 y T=3; ejes negativos, vectores de dirección nulos, respuesta ambigua y ausencia de representantes.
7. Dos representantes cercanas espacialmente pero pertenecientes a ejes distintos no deben mezclarse por una distancia circular.
8. Validación no modifica el modelo. Cambiar las etiquetas sin cambiar la entrada no cambia la predicción previa a evaluación.
9. Guardar/cargar reproduce posiciones y estados de lectura para las mismas entradas.
10. Consola y Godot utilizan el mismo núcleo y producen la misma predicción con el mismo modelo.

Si hay entorno gráfico disponible, abrir Godot y comprobar entrenamiento, pausa, inspección y carga de modelo. Si alguna comprobación gráfica no puede ejecutarse, distinguirla de lo probado realmente y entregar instrucciones concretas de ejecución local.

## 16. Fuera de la primera entrega

No implementar todavía:

- Eyección y transferencia entre ejes, impulso varado, reducción o crecimiento.
- Reconstrucción inversa/target propagation.
- Influencia colectiva nueva sobre neuronas aisladas.
- Activaciones transcendentes espirales, circulares, splines o módulos adicionales.
- Campos iterativos conocidos o aprendidos durante predicción.
- Reintegración de conexiones anuladas.
- Solución general de etiquetas de pozos coincidentes en cero.

Estos mecanismos no deben bloquear el primer laboratorio. Conservar puntos de ampliación sencillos, sin implementar sistemas vacíos extensos.

### Experimento posterior ya registrado: C* → (F*, 1)

Después se estudiará el pozo constante como posible donante, con dos pozos `(F*, 0)` y `(0, 1)`. Transferir candidatas hacia F con reajuste previo, simulacro y recuperación; comparar con seguir entrenando sin transferencia y con distintos tamaños de red.

- Donante: precisión suficiente y estable, con representantes aparentemente sustituibles.
- Posible receptor: precisión insuficiente y estancamiento persistente.
- Confirmar mediante un simulacro que mantiene al donante y mejora al receptor.
- Subcongregado significa que necesita más neuronas, no simplemente que tiene menos.
- Una representante constante puede seguir siendo útil inmanentemente.
- Más neuronas pueden empeorar precisión, estabilidad o velocidad, como observó el usuario en `DRANN_C_F.cpp`. No asumir mejora monótona ni una causa ya diagnosticada.

Este ensayo permanece documentado como trabajo posterior, no activado en la primera implementación.

## 17. Entrega esperada de Codex local

Entrega código compilable y ejecutable con Godot, no únicamente otro documento o pseudocódigo. Incluye:

- Instrucciones exactas para construir el núcleo, la extensión y abrir el proyecto.
- Ubicación del bloque de configuración en `main.cpp`.
- Cómo entrenar, inspeccionar y cargar el modelo entrenado.
- Comprobaciones ejecutadas, resultados obtenidos y límites observados.
- Resumen de los archivos creados/modificados y ubicación de logs y modelos.

Resuelve los detalles rutinarios utilizando los defaults provisionales y las referencias. Identifica cualquier desviación matemática o de alcance; no presentar una alternativa diferente como si fuera la acordada. No hace falta otra ronda de diseño para elegir nombres de archivos o formatos auxiliares.

El objetivo es una primera DRANN observable y comprobable, con el estilo de programación del usuario y una base clara para experimentar después.
