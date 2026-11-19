# Experimento: C* → (F*, 1), transferencia y exceso de neuronas

Fecha: 19 de noviembre de 2026.

**Estado: propuesta de experimento para el laboratorio de Godot. No implementado ni ejecutado por este documento.**

## Qué queremos averiguar

Proponemos una tarea con una propiedad variable y otra constante:

$$
C^*\longrightarrow\vec{y}^{\,*}(C)=(F^*(C),1).
$$

Queremos observar cómo cambia la estimación de Fahrenheit al transferirle neuronas inicialmente congregadas en el pozo constante. También queremos medir si llega un punto en que más neuronas no ayudan o empeoran la estimación.

La expresión del usuario «ir introduciendo neuronas de 1» se desarrolla principalmente como transferencia desde el pozo constante hacia Fahrenheit, manteniendo el tamaño total. El crecimiento de la red se estudiará por separado para no confundir reasignación con aumento de capacidad.

La constante puede ser una fuente de donantes, pero no se presupone que todas sus representantes sean prescindibles ni que Fahrenheit necesite más neuronas.

## Tarea y geometría

Se conserva la conversión $F(C)=1{,}8C+32$. Los dos pozos son:

$$
\boxed{
\vec{\xi}_F(C)=(F^*(C),0),
\qquad
\vec{\xi}_1(C)=(0,1).
}
$$

El primer pozo cambia con la entrada; el segundo permanece fijo. No representar el output como un único punto $(F^*,1)$.

$C^*$ y $F^*$ se obtienen con escalas calculadas únicamente en entrenamiento, fijadas antes de comparar variantes. Una configuración propuesta para este ensayo es:

$$
S_C=\max_{ej\in\mathrm{entrenamiento}}|C_{ej}|,
\quad
S_F=\max_{ej\in\mathrm{entrenamiento}}|1{,}8C_{ej}+32|,
\quad
C^*=\frac{C}{S_C},
\quad
F^*=\frac{1{,}8C+32}{S_F}.
$$

Seleccionar un dataset con ambas escalas no nulas. La segunda componente ya es el valor normalizado literal $1$: no volver a dividirla por $S_F$. Esta configuración es propia de la variante F–constante; no modifica por sí sola la normalización compartida del experimento F–K.

Conservar los mismos datos, escalas y separación para todas las variantes. No recortar automáticamente valores de evaluación que superen el rango de entrenamiento.

## Arquitectura de referencia

Usar el diseño actual: potencial inmanente escalar, activación inmanente configurable y atracción trascendente afín:

$$
\vec{q}_i(C)=
\left(\vec{o}+\vec{b}_i^{(T)}\right)
+\vec{w}_i^{(T)}V_i^{(t)}(C).
$$

Los pesos y sesgos transcendentes se aprenden; no imponer una especialización permanente. Para esta tarea afín, identidad inmanente puede servir como referencia y una activación no lineal como comparación separada. Mantener la activación elegida fija dentro de cada comparación.

Hay una solución constante representable mediante:

$$
\vec{w}_i^{(T)}=\vec{0},
\qquad
\vec{o}+\vec{b}_i^{(T)}=(0,1).
$$

Esto muestra que una representante puede cubrir geométricamente el pozo constante. No demuestra que una neurona que lo representa sea inútil en la inmanencia ni que el entrenamiento encuentre esa solución.

Usar la misma configuración de atracción y cobertura para ambos pozos al comenzar. No dar prioridad artificial a Fahrenheit mediante una ponderación distinta sin tratarlo como otro experimento.

## Prueba principal: redistribuir sin aumentar N

1. Entrenar una red pequeña y registrar qué neuronas representan realmente cada propiedad en distintos ejemplos. Su eje inicial no determina su pertenencia definitiva.
2. Guardar un estado de referencia. Si la constante está suficientemente bien representada y tiene candidatas sustituibles, ensayar una transferencia hacia F.
3. Ejecutar cada intento en una copia, mediante reajuste previo de los parámetros transcendentes.
4. Observar el resultado inmediato y después permitir un presupuesto de recuperación.
5. Compararlo con otra copia que continúa entrenando desde el mismo estado durante el mismo presupuesto, sin transferencia.
6. Aceptar la transferencia si el donante conserva suficiente precisión y cobertura y el receptor mejora de manera relevante. Si falla, restaurar.
7. Repetir gradualmente y registrar la respuesta al cambiar la distribución de representantes.

La política automática buscará un receptor con precisión insuficiente y estancamiento persistente. En modo de laboratorio también podremos probar transferencias deliberadas cuando F ya esté bien resuelto, para estudiar si recibe capacidad innecesaria. Esas intervenciones se etiquetarán como pruebas diagnósticas, no como decisiones justificadas de subcongregación.

No aplicar varias transferencias sin saber cuáles ya han perjudicado el resultado. En una primera exploración, una candidata por intento permite interpretar mejor los efectos.

## Reajuste previo de la candidata

Para una candidata $i$, recopilar su potencial final $v_{ej}=V_{i,ej}^{(t)}$ en ejemplos de entrenamiento y ajustar:

$$
\min_{a,c}\sum_{ej}
\left(a v_{ej}+c-F_{ej}^*\right)^2.
$$

Después establecer:

$$
\vec{w}_i^{(T)\prime}=a\vec{e}_F,
\qquad
\vec{b}_i^{(T)\prime}=c\vec{e}_F-\vec{o}.
$$

Así la candidata queda sobre el eje receptor con una estimación ajustada a varios ejemplos, en lugar de copiar su valor constante al eje de Fahrenheit.

El ajuste no añade parámetros a la arquitectura: asigna nuevos valores a sus pesos y sesgos transcendentes. No utilizar respuestas de validación para calcularlo. Si sus potenciales son casi constantes o no contienen información aprovechable, registrar el ajuste como insuficiente; no generar coeficientes enormes para forzar una solución.

Primero cambiar solo la transcendencia y conservar la inmanencia. Una candidata que represente la constante puede mantener potenciales dependientes de C aunque su lectura no los utilice. Si su estado inmanente tampoco distingue entradas, el reajuste afín no recuperará esa información.

Comparar después, como variante identificada, recuperación solo transcendente frente a recuperación que también modifica el rizoma. No mezclar sus resultados.

## Donantes, receptores y simulacro

Criterios aceptados para proponer un intento:

- **Posible donante:** precisión suficiente y estable, con representantes aparentemente sustituibles.
- **Posible receptor:** precisión insuficiente y estancamiento persistente.
- **Confirmación mediante simulacro:** el donante mantiene suficiente precisión y el receptor obtiene una mejora que justifica la transferencia.

Subcongregado significa que necesita más neuronas para resolver la tarea, no que tenga menos representantes que otro pozo. Sobrecongregado significa que puede ceder capacidad sin un perjuicio inaceptable. El conteo orienta la inspección, pero no determina esas condiciones.

Comprobar todas las propiedades y varios ejemplos, porque las especializaciones pueden cambiar con la entrada. Para el caso general de T componentes, la misma regla se aplica a cualquier par donante–receptor, con $\vec{\xi}_k=y_k^*\vec{e}_k$.

## Comparaciones para separar los efectos

| Variante | Qué cambia | Qué permite estudiar |
|---|---|---|
| Continuación sin intervención | Solo entrenamiento ordinario | Si la mejora se habría producido sin transferir. |
| Transferencia con N fijo | Parámetros transcendentes de una candidata del pozo constante | Efecto de reasignar representantes, conservando inicialmente el forward inmanente. |
| Redes entrenadas desde cero con distintos N | Número total de neuronas | Efecto del tamaño desde el inicio. |
| Crecimiento desde un estado guardado, como extensión posterior | Añadir una neurona, con conexiones explícitamente registradas | Efecto de aumentar población durante el aprendizaje. |

Para el barrido inicial pueden compararse, como propuesta, N=2, 3, 5 y 10, con varias semillas. Registrar el reparto inicial y el realmente aprendido. No garantizar que dos neuronas alcancen la mejor solución.

Las nuevas neuronas de una prueba de crecimiento pueden empezar aisladas o conectadas: son intervenciones distintas. Conectarlas puede alterar la selección de transmisoras, los potenciales y el número de pasos. Una transferencia solo transcendente no altera inmediatamente esos estados inmanentes.

Si la pérdida de atracción se promedia entre N representantes, aumentar N también modifica la escala de sus gradientes. Registrar ese factor y analizarlo como posible confusión; no atribuir automáticamente todo cambio al tamaño ni alterar silenciosamente la pérdida o lr para compensarlo.

## Hipótesis de exceso de neuronas

El usuario observó en DRANN_C_F.cpp que una red con más neuronas podía aprender más despacio, presentar más ruido y alcanzar un error mayor que una más pequeña. Se conserva como antecedente observado por el usuario, no como una causa diagnosticada ni como resultado de este nuevo ensayo.

El laboratorio debe permitir detectar un tamaño excesivo o un exceso de representantes en F, además de mejoras por capacidad adicional.

Posibles explicaciones a contrastar: cambios en los recorridos de transmisión, interferencia entre contribuciones, escala de gradientes, condicionamiento del entrenamiento, dependencia de la inicialización y mayor coste de cálculo. No llamar al resultado «sobreajuste» si también empeora entrenamiento sin evidencia adicional.

No suponer una relación monótona entre cantidad de neuronas y calidad. El tamaño más útil puede ser intermedio y depender de la tarea.

## Qué medir y mostrar en Godot

Por época y alrededor de cada intervención:

- Error geométrico por pozo en entrenamiento y validación; media y errores altos.
- Pérdida gaussiana junto con distancia sin saturación, para distinguir convergencia de gradientes débiles lejos de los pozos.
- Posiciones, ejes y cambios de especialización de cada neurona.
- Cobertura del donante y receptor antes, inmediatamente después y tras recuperación.
- Magnitud de la perturbación, duración de la recuperación y error final alcanzado.
- N total, conexiones, pasos internos y tiempo de cálculo del modelo.
- Normas de gradiente y actualizaciones cuando se investigue un posible bloqueo del aprendizaje.
- Semilla, configuración, estado de referencia e intervención aceptada o rechazada.

Una medida diagnóstica de cobertura es:

$$
E_{k,ej}=\min_i
\|\vec{q}_i(x_{ej})-\vec{\xi}_k(x_{ej})\|^2.
$$

Esta medida consulta el ideal después del forward; no es por sí sola una regla para extraer la respuesta durante predicción. Mantener pendiente la lectura explícita del output, sin adoptar automáticamente una media por grupos.

En la vista transcendente: pozo fijo $(0,1)$, pozo variable $(F^*,0)$, neuronas coloreadas por su representación observada y eventos de transferencia. Comparar gráficos de error F frente a número de representantes, error frente a épocas y error frente a tiempo. No contar el tiempo de animación como coste del modelo sin distinguirlo.

## Cómo interpretar los resultados

- Si una transferencia mejora F y mantiene la constante, apoya esa reasignación concreta.
- Si las siguientes transferencias dejan de ayudar o empeoran F, puede haber exceso de representantes o dificultades de optimización; investigar con las comparaciones anteriores.
- Si la constante empeora demasiado, no se ha demostrado que la candidata fuese prescindible como representante.
- Si solo se degrada tras entrenar la inmanencia, distinguirlo del efecto inmediato del cambio transcendente.
- Si una red grande funciona peor, no concluir que carece de capacidad representativa: puede fallar el aprendizaje o resultar ineficiente con esa configuración.

Guardar tanto intentos favorables como desfavorables y el mejor estado anterior. Comparar varias semillas y presupuestos antes de convertir una observación en una política automática.

La tarea afín no valida capacidad para cualquier relación no lineal. El procedimiento de transferencia sí se plantea sin reglas específicas de F–K y podrá estudiarse después con otras salidas vectoriales.

## Alcance y decisiones pendientes

Añadir esta variante al laboratorio, sin sustituir automáticamente C → (F, K). Quedan por fijar dataset concreto, presupuestos, tolerancias, lectura de salida y valores de configuración. Los números propuestos son puntos de partida de experimentación, no resultados.

Véanse [el laboratorio inicial](../laboratorio-godot-fk.md) y [el ideario](../../ideario.md).
