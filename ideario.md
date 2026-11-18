# Ideario de DRANN

Registro de propuestas para explorar en el futuro. Una idea incluida aquí no modifica por sí sola la arquitectura vigente ni autoriza su implementación.

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

$
v_j^{\mathrm{nuevo}}=(1-w)v_j^{\mathrm{anterior}}+wv_i,
$

con $w\neq1$ y el resto de cantidades conocidas puede recuperarse el estado anterior. Con $w=1$, el resultado deja de depender de ese estado y la reconstrucción no es única.

Para una inversión completa habría que estudiar activaciones, dimensiones, mezclas de señales, decisiones de transmisión y disponibilidad del historial necesario. La retirada conserva el estado de la transmisora; no debe describirse por sí sola como un borrado de su potencial.

### Primer alcance posible: invertir solo la transcendencia

Antes de intentar invertir todo el rizoma, podría estudiarse una transcendencia explícitamente invertible. Como ejemplo matemático, no como arquitectura acordada:

$
\vec{q}_i=\vec{b}_i^{(T)}+A_i\vec{V}_i,
$

donde $A_i$ es cuadrada e invertible. Para una posición objetivo:

$
\vec{V}_i^{\mathrm{obj}}
=
A_i^{-1}
\left(\vec{q}_i^{\mathrm{obj}}-\vec{b}_i^{(T)}\right).
$

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

$
\mathrm{entrada}
\longrightarrow
\mathrm{estado\ inmanente\ del\ rizoma}
\longrightarrow
\mathrm{transcendencia}
\longrightarrow
\mathrm{cognifenómeno\ primitivo}.
$

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
