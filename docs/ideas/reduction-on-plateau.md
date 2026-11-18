# Propuesta: reducción compensada al estancarse el entrenamiento

Fecha: 18 de noviembre de 2026.

Estado: idea aceptada para investigación y posible incorporación futura. Este documento registra la propuesta; no modifica el modelo ni implementa poda, checkpoints o cambios en el entrenamiento. La especificación vigente de v1 sigue en [DRANN.md](../DRANN.md).

## Motivación

El usuario observa que, en sus ejecuciones de Celsius–Fahrenheit, una red de diez neuronas puede converger más despacio, con más oscilaciones y peor pérdida que una de dos. La anulación de conexiones internas por pesos nulos o negativos no parece suficiente para encontrar una estructura pequeña. Son observaciones comunicadas por el usuario; este documento no añade una ejecución ni un resultado reproducido.

Una neurona puede ser redundante en la descripción geométrica y seguir siendo útil para calcular los estados de otras. En el ejemplo A–B–C, B puede quedar dentro del segmento definido por A y C, pero haber contribuido a situar sus extremos. Por ello, se propone estudiar su eliminación junto con una compensación de su función mediante las neuronas conservadas.

La pertenencia al interior de una geometría no basta por sí sola para justificar una eliminación.

## Ciclo propuesto

1. **Entrenamiento normal.** Actualizar pesos y sesgos con el procedimiento vigente. Evaluar sin modificar parámetros y guardar el mejor modelo según validación.
2. **Detección de estancamiento.** Si la pérdida de entrenamiento no mejora significativamente durante una paciencia configurable, iniciar un intento de simplificación. El estancamiento es un disparador, no una prueba de que sobren neuronas.
3. **Reducción de una neurona.** Guardar el estado exacto anterior al intento. Seleccionar una candidata cuya función pueda absorberse y calcular una compensación: reajustar parámetros de las supervivientes y, cuando proceda, proponer conexiones que sustituyan su intermediación.
4. **Recuperación limitada.** Entrenar la red reducida durante un presupuesto de épocas para ajustar una compensación aproximada.
5. **Aceptación o restauración.** Conservar la reducción si alcanza una precisión aceptable con menos neuronas. Si no recupera suficiente precisión, restaurar el estado anterior.

Empezar por una sola candidata en cada intento permite atribuir el resultado a una transformación concreta. La paciencia, el umbral de mejora, el presupuesto de recuperación y la tolerancia de aceptación siguen pendientes; no se han fijado valores.

## Dos checkpoints con funciones diferentes

| Copia | Función |
| --- | --- |
| Mejor modelo según validación | Conservar el mejor resultado predictivo encontrado, aunque una reducción posterior sacrifique algo de precisión. |
| Modelo anterior al intento | Deshacer la última transformación y continuar desde el estado exacto previo. |

Guardar los parámetros de entrada, sesgos, parámetros internos simétricos, máscaras de conexiones anuladas y la identidad de las neuronas conservadas. Para una continuación reproducible, incluir también configuración, época, estado aleatorio si se utiliza y estado del optimizador cuando lo tenga. Si se ensaya Adam en el futuro, esto incluye sus momentos y contador de actualizaciones; Adam no está aprobado ni implementado mediante esta nota.

La retirada temporal de una transmisora se reinicia en cada ejemplo; no debe confundirse con la eliminación estructural.

Restaurar un checkpoint revierte un intento completo. No establece una regla de reintegración de conexiones durante el entrenamiento ordinario de v1.

## Línea de trabajo para calcular la compensación

1. Expresar algebraicamente la contribución de la candidata en los recorridos de transmisión observados.
2. Determinar qué pesos de entrada, sesgos y conexiones de las supervivientes podrían absorberla.
3. Buscar una reparametrización común para varios ejemplos y recorridos, en lugar de resolver cada ejemplo con parámetros distintos.
4. Medir qué parte de la función no puede conservarse y estudiar una aproximación de segundo orden, inspirada en Optimal Brain Surgeon, para valorar ese perjuicio.

Con activación identidad y un recorrido fijado, las operaciones son afines respecto a los estados de entrada. Esto permite componerlas y estudiar sustituciones locales. No garantiza que toda eliminación sea expresable mediante la misma arquitectura ni que el recorrido supuesto siga siendo válido después.

Una conexión positiva, incluso pequeña, puede cambiar quién es máximo local. Eliminarla puede modificar los papeles y el orden de transmisión. Una aproximación basada en el recorrido original debe contrastarse con el forward real de la estructura reducida.

Las nuevas conexiones propuestas durante la transformación y su relación con la anulación irreversible de v1 deben definirse expresamente antes de implementar esta extensión.

Referencias para investigar, no algoritmos incorporados:

- [Data-free Parameter Pruning for Deep Neural Networks, Srinivas y Babu](https://arxiv.org/abs/1507.06149): fusión de neuronas y transferencia de sus contribuciones.
- [Optimal Brain Surgeon, Hassibi y Stork](https://papers.neurips.cc/paper_files/paper/1992/file/303ed4c69846ab36c2904d3ba8573050-Paper.pdf): eliminación de pesos acompañada de compensación de los restantes mediante información de segundo orden.

## Evaluación y decisiones pendientes

- Definir la representación del cognifenómeno y una medida de calidad comparable entre redes de tamaños distintos. El ejemplo del segmento es una hipótesis geométrica; v1 compara todos los potenciales con un único punto ideal.
- Eliminar una neurona cambia la pérdida media actual y su ponderación aunque no cambien los potenciales restantes. Una caída de esa pérdida no demuestra por sí sola que se haya conservado la geometría.
- Seleccionar y ajustar la compensación con datos de entrenamiento. Reservar validación para seguimiento y selección, sin backpropagar sus etiquetas.
- Una tolerancia de precisión permitiría aceptar una red algo menos precisa y más pequeña; su valor y el modelo usado como referencia aún no están definidos.
- Con dos ejemplos de validación, la selección es sensible a esos casos. Para evaluar el procedimiento habrá que ampliar la evaluación y conservar un conjunto de prueba independiente.
- Definir cómo evitar repetir el mismo intento fallido desde el mismo estado sin nueva información.
- La política de checkpoints no resuelve el aislamiento total ni modifica su exclusión del alcance de v1.
- No se ha supuesto un número ideal de neuronas conocido ni proporcionalidad fija entre dimensión de entrada, dimensión de salida y tamaño necesario del rizoma.

## Registro mínimo de un intento futuro

Registrar época del disparador, candidata, tamaño anterior y posterior, parámetros modificados o conexiones propuestas, pérdidas antes de reducir, después de compensar y después de recuperar, épocas de recuperación, decisión final y checkpoints asociados. Distinguir resultados medidos de estimaciones algebraicas.

Este registro debe permitir seguir las ideas entre el chat teórico y el trabajo local sin presentar propuestas como funcionalidades existentes.
