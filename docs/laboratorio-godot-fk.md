# Laboratorio inicial con Godot: C → (F, K)

Fecha: 19 de noviembre de 2026.

## Objetivo

La primera implementación de este nuevo diseño incluirá Godot desde el principio. Usaremos una tarea de estimación sencilla y pocas neuronas para tener un laboratorio donde observar la DRANN y testear ideas.

Este documento registra el alcance acordado. No significa que el laboratorio ya esté programado ni cierra las cuestiones que seguimos debatiendo.

## Dos ejes, dos pozos por ejemplo

La entrada será $x=C$ y la salida ordenada será:

$$
\vec{y}(x)=(F(x),K(x))=(1{,}8C+32,\ C+273{,}15).
$$

Cada componente tiene su propio eje transcendente. Con dos salidas, $T=2$, pero **no convertimos el vector completo en un único punto**:

$$
\boxed{
\vec{\xi}_F(x)=(F^*(x),0),
\qquad
\vec{\xi}_K(x)=(0,K^*(x)).
}
$$

Si $F^*=0{,}5$ y $K^*=1$, tendremos los pozos $(0{,}5,0)$ y $(0,1)$.

Buscamos que unas neuronas se especialicen en representar Fahrenheit y otras Kelvin. Las neuronas pueden moverse por todas las coordenadas; no se fija de antemano un eje de movimiento por neurona.

La generalización a un output de tamaño $T$ es:

$$
\vec{\xi}_k(x)=y_k^*(x)\vec{e}_k,
\qquad k=1,\ldots,T,
$$

donde $\vec{e}_k$ es el vector unitario del eje correspondiente. El orden de las propiedades se conserva mediante los ejes y sus etiquetas.

Los pozos cambian con cada entrada. Al procesar un ejemplo se comparan las posiciones con sus pozos, no con todas las respuestas del dataset simultáneamente.

**Caso pendiente:** si varias componentes valen cero, sus pozos coinciden en el origen. La distancia espacial por sí sola no permite distinguirlos. Mantener sus etiquetas y resolver su cobertura antes de afirmar que esta codificación funciona para cualquier vector; no introducir desplazamientos de los pozos sin debatirlo.

## Normalización compartida

Se recoge como interpretación de la propuesta un mismo divisor para F y K: el mayor valor entre ambas magnitudes en los ejemplos de entrenamiento.

$$
S=\max_{ej\in\mathrm{entrenamiento}}\{F_{ej},K_{ej}\},
\qquad
F^*=\frac{F}{S},
\qquad
K^*=\frac{K}{S}.
$$

La escala se calcula una vez con entrenamiento y se conserva al validar. Así el conjunto de validación queda reservado para evaluar generalización. No se normaliza cada magnitud por un máximo distinto en esta propuesta.

Para recuperar las unidades originales se multiplica por $S$. Un dato nuevo puede superar el máximo del entrenamiento; no recortarlo automáticamente a uno.

El rango de esta primera tarea debe dar una escala positiva y no nula. Otros casos de normalización quedan por estudiar. La normalización de la entrada también sigue por fijar.

## Potencial inmanente escalar y atracción trascendente

Actualización del 19 de noviembre de 2026: se sustituye la propuesta vectorial anterior por un potencial inmanente escalar y una atracción trascendente afín.

$$
\boxed{
\vec{q}_i(x)=
\left(\vec{o}+\vec{b}_i^{(T)}\right)
+
\underbrace{\vec{w}_i^{(T)}V_i^{(t)}(x)}_{\vec{d}_i(x)}.
}
$$

- $\vec{o}$: referencia geométrica fija calculada con entrenamiento.
- $\vec{b}_i^{(T)}$: sesgo transcendente aprendido, independiente de la entrada actual.
- $V_i^{(t)}$: potencial inmanente escalar conservado al finalizar los pasos internos del ejemplo.
- $\vec{w}_i^{(T)}$: vector de pesos aprendibles, con una componente por eje transcendente.
- $\|\vec{d}_i\|$: impulso; $\vec{d}_i/\|\vec{d}_i\|$: orientación cuando el impulso no es nulo.

Los pesos transcendentes pueden ser positivos, negativos o cero. No aplicarles la regla de anulación de conexiones inmanentes.

La no linealidad se estudia en el rizoma mediante la activación $\sigma$; no se añade una activación transcendente en esta referencia. Tanh es una candidata inmanente, no una decisión definitiva. Identidad puede servir de referencia para la tarea afín de temperaturas.

Cada neurona tiene una recta de movimiento que puede alinearse con un eje aprendiendo pesos y sesgo. Sus posiciones iniciales pueden situarse sobre los ejes sin bloquear los demás parámetros. Se pueden permitir cambios de especialización según la entrada, reconociendo las restricciones del desplazamiento afín escalar.

Orientación e impulso describen el mismo desplazamiento. Se realiza una única atracción directa, sin recorrer un campo iterativo en predicción. Su animación no añade pasos al cálculo.

Una neurona internamente aislada sigue recibiendo la entrada y puede aportar una representación útil. Su potencial queda congelado durante los pasos internos del ejemplo; no se excluye automáticamente de la transcendencia ni lee estados dinámicos del conjunto.

## Pozos gaussianos durante el aprendizaje

Conservamos como forma candidata para cada pozo:

$$
U_k(\vec{q},x)=
-A_k\exp\left(
-\frac{\|\vec{q}-\vec{\xi}_k(x)\|^2}{2r_k^2}
\right),
\qquad A_k>0,\quad r_k>0.
$$

Este potencial escalar del paisaje transcendente tiene una función distinta del potencial escalar inmanente de cada neurona.

Los pozos orientan el backward para corregir los parámetros que produjeron las posiciones. No colocar artificialmente una predicción sobre la respuesta y contarla como acierto.

El esquema base discutido combina atracción y cobertura, con alcance compartido r:

$
\ell_{ik}=1-\exp\left(-\frac{\|\vec{q}_i-\vec{\xi}_k\|^2}{2r^2}\right),
\qquad
L=\frac1N\sum_i\min_k\ell_{ik}
+\frac12\sum_{k\in\{F,K\}}\min_i\ell_{ik}.
$

Su configuración numérica y el tratamiento de empates aún deben concretarse. Se permite especialización variable según la entrada. No se ha aprobado una simple suma de atracciones de todos los pozos sobre cada neurona.

También deben concretarse profundidad, alcance y tratamiento de gradientes pequeños lejos de los pozos. La eyección y el impulso varado siguen siendo ideas para experimentar después.

En validación se ejecuta el forward con la entrada. Los pozos ideales se consultan después para medir el error y mostrarlos como referencia.

## Godot como laboratorio desde la primera implementación

La prioridad cambia respecto al plan anterior de incorporar Godot después de tener un ejecutable: ahora la primera implementación de este diseño ya incluirá Godot.

Como alcance visual propuesto:

- Rizoma inmanente con potenciales, conexiones, transmisiones simultáneas y estados de retirada o aislamiento.
- Plano transcendente con ejes $F^*$ y $K^*$, ambos pozos y posiciones neuronales.
- Sesgos transcendentes, desplazamientos y posiciones finales del tiro.
- Pérdidas de entrenamiento y validación, y registros para comparar ejecuciones.

La visualización no debe proporcionar respuestas ideales al forward de validación. El lenguaje, el mecanismo de integración y la organización del proyecto quedan por decidir.

Primero comprobaremos estimación y congregación con pocas neuronas; el número exacto aún no se ha fijado. Después podremos probar variantes y mecanismos de reducción sobre una referencia que ya sepamos interpretar.

## Antes de programar este laboratorio

Quedan por fijar:

1. Dataset, separación de entrenamiento y validación y normalización de la entrada.
2. Número de neuronas; el potencial inmanente de esta referencia es escalar.
3. Activación inmanente e inicialización de los parámetros y posiciones transcendentes.
4. Pérdida de atracción y cobertura, y parámetros de los pozos.
5. Lectura de la respuesta y métricas de especialización, sin asumir una media por grupos que no se ha acordado.
6. Integración técnica con Godot.

La coincidencia de varias neuronas en un pozo indica posibles candidatas a reducción, pero no demuestra que sean prescindibles: pueden cooperar inmanentemente.

La reconstrucción inversa y la influencia colectiva sobre neuronas aisladas permanecen aparcadas en [el ideario](../ideario.md).


## Variante propuesta: C* → (F*, 1)

Se incorpora como idea de experimento [la transferencia desde un pozo constante hacia Fahrenheit y el estudio del exceso de neuronas](experimentos/celsius-fahrenheit-constante.md).

El ensayo compara reasignación con N fijo, continuación sin intervención y distintos tamaños de red. Parte del antecedente observado por el usuario en DRANN_C_F.cpp: más neuronas podían empeorar error, estabilidad o velocidad de aprendizaje. El laboratorio debe registrar también ese posible perjuicio.

Es una propuesta documentada, todavía sin implementación ni resultados.
