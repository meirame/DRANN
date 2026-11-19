# Ideario de DRANN

Registro de propuestas para explorar en el futuro. Una idea incluida aquí no modifica por sí sola la arquitectura vigente ni autoriza su implementación.

**Actualización del 19 de noviembre de 2026:** el alcance actual del primer laboratorio está en [Laboratorio inicial con Godot: C → (F, K)](docs/laboratorio-godot-fk.md). Incluye Godot desde el principio. La revisión actual usa potencial inmanente escalar y atracción trascendente afín. Para cada ejemplo, F y K definen dos pozos situados sobre sus respectivos ejes: $(F^*,0)$ y $(0,K^*)$.

Las secciones anteriores en el tiempo se conservan como registro histórico; sus propuestas de inicialización no sustituyen las decisiones pendientes del documento del laboratorio.

## 1. Entrenamiento desde el cognifenómeno ideal hacia la inmanencia

Fecha de registro: 18 de noviembre de 2026.

**Estado: propuesta exploratoria aparcada.** El usuario ha pedido conservarla y continuar ahora con el forward habitual. No hay implementación ni resultados que demuestren una mejora.

### Intuición propuesta por el usuario

En lugar de aprender únicamente haciendo avanzar la entrada por el rizoma hasta obtener el cognifenómeno primitivo, estudiar si sería útil comenzar por posiciones neuronales situadas en los núcleos del cognifenómeno ideal y reconstruir hacia atrás estados inmanentes capaces de producirlas.

La motivación es proporcionar a la red objetivos internos concretos que orienten el aprendizaje y comprobar si así resulta más sencillo organizar las congregaciones y estimar las relaciones entrada–salida.

La propuesta no se considera definitiva ni se presupone que vaya a ser más sencilla, rápida u óptima.

### Posible procedimiento de investigación

1. Presentar una entrada de entrenamiento y obtener el estado inmanente y el cognifenómeno primitivo actuales.
2. Establecer posiciones deseadas para las neuronas que cubran los núcleos ideales $\vec{\xi}_k(x)$.
3. Estudiar si puede reconstruirse un estado inmanente que produzca esas posiciones, compatible con la entrada y, cuando proceda, cercano al estado ya calculado.
4. Ajustar parámetros compartidos para que el forward desde la entrada genere por sí mismo ese estado y esas posiciones.
5. Repetir sobre distintos ejemplos.
6. Evaluar con un forward que solo recibe la entrada; el ideal se utiliza después para medir el resultado.

No crear una red o parámetros distintos para cada ejemplo. No proporcionar el cognifenómeno ideal para generar predicciones de validación.

### Conocer las reglas no garantiza poder invertirlas

Una operación puede perder información aunque su ecuación sea conocida. En una actualización escalar simplificada:

$$
v_j^{\mathrm{nuevo}}=(1-w)v_j^{\mathrm{anterior}}+wv_i,
$$

con $w\neq1$ y el resto de cantidades conocidas puede recuperarse el estado anterior. Con $w=1$, el resultado deja de depender de ese estado y la reconstrucción no es única.

Para una inversión completa habría que estudiar activaciones, dimensiones, mezclas de señales, decisiones de transmisión y disponibilidad del historial necesario. La retirada conserva el estado de la transmisora; no debe describirse por sí sola como un borrado de su potencial.

### Primer alcance posible: invertir solo la transcendencia

Antes de intentar invertir todo el rizoma, podría estudiarse una transcendencia explícitamente invertible. Como ejemplo matemático, no como arquitectura acordada:

$$
\vec{q}_i=\vec{b}_i^{(T)}+A_i\vec{V}_i,
$$

donde $A_i$ es cuadrada e invertible. Para una posición objetivo:

$$
\vec{V}_i^{\mathrm{obj}}
=
A_i^{-1}
\left(\vec{q}_i^{\mathrm{obj}}-\vec{b}_i^{(T)}\right).
$$

Esto permitiría obtener un objetivo inmanente y entrenar el rizoma para alcanzarlo. Las matrices de esta forma no están aprobadas para incorporarse al modelo activo.

### Preguntas pendientes

- ¿Qué transformación admite una inversa útil y qué información necesita?
- ¿La inversión es estable o amplifica errores pequeños?
- ¿Cómo se reparten las neuronas entre los núcleos sin fijar manualmente una solución para cada ejemplo?
- ¿Puede el rizoma generar los estados internos reconstruidos para las entradas correspondientes?
- ¿Cómo se mantienen las mismas reglas y parámetros al recorrer todo el dataset?
- ¿Qué coste adicional supone respecto al backward habitual?
- ¿Mejora aprendizaje, precisión, estabilidad o tamaño frente a una referencia comparable?

Backpropagar derivadas no equivale a deshacer los estados del forward. La propagación de objetivos utiliza estados deseados o inversas aproximadas; su adaptación a DRANN requeriría desarrollo propio.

### Referencias para retomarla

- [Lee et al., Difference Target Propagation](https://arxiv.org/abs/1412.7525).
- [Meulemans et al., A Theoretical Framework for Target Propagation](https://arxiv.org/abs/2006.14331).

Son antecedentes relacionados, no una validación de esta propuesta para DRANN.

## Camino activo al aparcar la idea

Continuar con el forward:

$$
\mathrm{entrada}
\longrightarrow
\mathrm{estado\ inmanente\ del\ rizoma}
\longrightarrow
\mathrm{transcendencia}
\longrightarrow
\mathrm{cognifenómeno\ primitivo}.
$$

El entrenamiento compara posteriormente el resultado con el cognifenómeno ideal. No implementar ahora la reconstrucción inversa ni exigir reversibilidad del modelo activo.

### Contexto conceptual que debe conservarse

- Usar $\vec{\ldots}$ para vectores, $\sigma$ para activación y $y^*$ para valores normalizados.
- El sesgo transcendente se escribe $\vec{b}_i^{(T)}$; el paréntesis identifica el plano y no una transpuesta.
- Los núcleos ideales se denominan $\vec{\xi}_k(x)$.
- El cognifenómeno primitivo ya pertenece al plano transcendente; hablamos de que transciende del estado inmanente.
- La salida buscada es una congregación de neuronas alrededor de varios núcleos, uno por componente del output, preservando su identidad y orden. No se pretende que cada neurona reproduzca simultáneamente todas las respuestas.
- La posición parte de un sesgo transcendente y cambia por un desplazamiento aprendido a partir del estado inmanente. Ese desplazamiento es vectorial y no se ha acordado congelar una coordenada.
- La preferencia de especialización puede ser estable sin restringir el movimiento a un único eje.
- No está acordado reconstruir la respuesta mediante una media por grupos; la transcendencia debe definirse como un cálculo aprendido, análogo pero no necesariamente idéntico al forward inmanente.
- Una neurona aislada internamente puede seguir recibiendo la entrada y aportar una posición útil. Conserva su potencial durante los pasos internos del ejemplo, pero puede aprender y responder de otro modo al siguiente ejemplo.
- Aislamiento interno, exclusión de la representación y eliminación completa son decisiones diferentes. Toda retirada debe conservar cobertura y suficiente precisión, y mantener o compensar las contribuciones necesarias.

Estas aclaraciones recogen el estado de la conversación teórica. No afirman que el código publicado ya las implemente. La transformación exacta de transcendencia, su pérdida y la lectura de la respuesta siguen por concretar en el camino activo.

## 2. Influencia colectiva transcendente sobre neuronas aisladas

Fecha de registro: 18 de noviembre de 2026.

**Estado: alternativa aparcada; fuera del modelo activo por decisión del usuario.**

### Idea

Permitir que una neurona sin conexiones internas lea un resumen dinámico del estado inmanente de otras neuronas para calcular su desplazamiento transcendente. Así podría conservar una influencia virtual del conjunto pese a su aislamiento inmanente.

Una posible forma de estudiarlo sería combinar el estado propio y una síntesis colectiva. No se ha elegido una fórmula, una topología transcendente ni una regla de agregación.

### Motivo para aparcarla

Una neurona aislada puede estimar un núcleo ideal eficazmente mediante su entrada y sus parámetros. Introducir dependencia del conjunto podría perturbar esa solución, acoplar errores o perjudicar el aislamiento que resultaba útil.

Además, eliminar una neurona aislada ya no garantizaría que las demás posiciones permanezcan iguales si participa en un resumen colectivo. Habría que revisar dependencias en ambos planos, el efecto de cambiar el tamaño de la población y las condiciones de reducción.

### Decisión activa

Por ahora, una neurona aislada de las otras neuronas debe permanecer aislada también de sus señales dinámicas durante la transcendencia. Recibe la entrada, calcula su estado propio y produce su desplazamiento con sus parámetros. No añadirle un resumen global ni conexiones virtuales nuevas.

Un origen geométrico fijo, calculado a partir del entrenamiento, es una referencia compartida y no un intercambio dinámico de estados entre neuronas.

### Posibles comprobaciones futuras

- Comparar cálculo exclusivamente propio con influencia colectiva, manteniendo datos, semillas y capacidad comparables.
- Comprobar si el mecanismo aprende a ignorar el contexto cuando perjudica.
- Distinguir lectura del conjunto de contribución al conjunto: son dos dependencias diferentes.
- Medir precisión por componente, cobertura, estabilidad y consecuencias de eliminar representantes.
- No interpretar proximidad geométrica como falta de utilidad ni exigir eliminar grupos que cooperan en estimar una propiedad.

## Aclaraciones del 18 de noviembre — registro histórico

- El origen transcendente propuesto es el centro de los núcleos ideales de todos los ejemplos de **entrenamiento**, calculado una vez con la normalización elegida y conservado al validar. No recalcularlo con el ideal de un ejemplo nuevo ni con las etiquetas de validación.
- Los sesgos transcendentes se inicializarían a cero respecto a ese origen y seguirían siendo aprendibles.
- Se mantiene la penalización por núcleos ideales sin representantes, además del acercamiento de las posiciones a núcleos.
- Las congregaciones pueden ser grupos cooperativos especializados. Ni compartir núcleo prueba inutilidad ni quedar fuera de un núcleo prueba ausencia de contribución interna.
- Está en discusión si prescindir de activaciones explícitas en ambos planos. Todavía no se ha tomado una decisión de implementación sobre ello. Seleccionar transmisoras por módulos ya introduce decisiones no lineales, pero no se ha demostrado que sean suficientes para las tareas deseadas.


## 3. Primer laboratorio con Godot

Fecha de registro: 19 de noviembre de 2026.

**Estado: alcance acordado para la próxima implementación; todavía no implementado por este documento.**

- Usaremos Godot desde el primer código de este nuevo diseño, una tarea sencilla C → (F, K) y pocas neuronas, como laboratorio para testear variantes.
- Cada componente tiene su eje y su pozo: $\vec{\xi}_F=(F^*,0)$ y $\vec{\xi}_K=(0,K^*)$. No se agrupan ambas magnitudes en un único pozo $(F^*,K^*)$.
- La normalización se recoge con un máximo compartido entre F y K, calculado en entrenamiento y mantenido al validar.
- La revisión actual usa potencial inmanente escalar, activación inmanente por concretar y atracción trascendente afín mediante un vector de pesos aprendibles. Los pozos gaussianos orientan el aprendizaje, sin un campo iterativo durante la predicción.
- La cobertura de los dos pozos, la especialización y la precisión se comprobarán antes de incorporar eyección, crecimiento o eliminación.
- El potencial inmanente de la referencia es escalar. La configuración de la pérdida, la inicialización, la lectura de la respuesta y la integración técnica con Godot siguen por concretar.
- La propuesta más reciente inicializa posiciones sobre los ejes, repartidas con el mismo criterio entre propiedades, sin especialización permanente. Sustituye a colocarlas todas en el mismo origen o sobre el segmento entre referencias; sus valores exactos siguen pendientes.

Véase [el documento del laboratorio](docs/laboratorio-godot-fk.md) para las ecuaciones, el caso de pozos coincidentes en cero y el alcance previsto.

## Terminología: donante y receptor

Fecha de registro: 19 de noviembre de 2026.

- **Pozo o congregación donante:** aquel del que se propone transferir una o varias neuronas mediante una eyección por sobrecongregación. La transferencia solo se acepta tras comprobar que el donante conserva suficiente precisión y cobertura.
- **Pozo o congregación receptor:** aquel subcongregado que necesita más neuronas y al que se dirige la transferencia. No basta con que tenga menos representantes que otro.

Son términos del mecanismo propuesto de eyección; no significan que ya esté implementado. No confundir el pozo receptor con una neurona receptora de señales en el forward inmanente.


## 4. Pozo constante como posible donante y exceso de neuronas

Fecha de registro: 19 de noviembre de 2026.

**Estado: idea desarrollada para el laboratorio; no implementada ni ejecutada.**

Estudiar $C^*\to(F^*,1)$ con dos pozos: $(F^*,0)$ y $(0,1)$. Transferir gradualmente candidatas del pozo constante hacia F, reajustando previamente sus parámetros transcendentes con varios ejemplos de entrenamiento. Comprobar precisión y cobertura del donante, mejora del receptor y recuperación.

Se aceptan como criterios: donante con precisión suficiente y estable y representantes aparentemente sustituibles; posible receptor con precisión insuficiente y estancamiento persistente; confirmación mediante simulacro que mantiene al donante y mejora al receptor.

Separar transferencias con N fijo de añadir neuronas. Investigar si el exceso de neuronas puede perjudicar aprendizaje, estabilidad o coste, como observó el usuario en DRANN_C_F.cpp, sin atribuir todavía una causa concreta. El ensayo debe poder encontrar un tamaño útil intermedio y conservar intentos fallidos.

La constante tiene una representación geométrica sencilla, pero sus representantes pueden contribuir al cálculo inmanente. No eliminarlas ni suponerlas donantes solo por ese motivo.

Véase [Experimento C* → (F*, 1)](docs/experimentos/celsius-fahrenheit-constante.md) para el protocolo, controles, medidas y límites.
