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

## Potencial inmanente vectorial y tiro directo

Se mantiene un potencial vectorial por neurona. Su posición transcendente será:

$$
\boxed{
\vec{q}_i(x)=
\left(\vec{o}+\vec{b}_i^{(T)}\right)
+
\underbrace{W_i^{(T)}\vec{V}_i^{(t)}(x)}_{\vec{d}_i(x)}.
}
$$

- $\vec{o}$: referencia geométrica fija calculada con entrenamiento.
- $\vec{b}_i^{(T)}$: sesgo transcendente aprendido, independiente de la entrada actual.
- $\vec{V}_i^{(t)}$: potencial inmanente vectorial conservado al finalizar los pasos internos del ejemplo.
- $W_i^{(T)}$: pesos aprendibles que convierten ese estado en desplazamiento.
- $\|\vec{d}_i\|$: impulso; $\vec{d}_i/\|\vec{d}_i\|$: orientación cuando el impulso no es nulo.

Los pesos transcendentes pueden ser positivos, negativos o cero. No aplicarles la regla de anulación de conexiones inmanentes.

Orientación e impulso describen el mismo desplazamiento, sin dos mecanismos de aprendizaje separados. Se realiza un único tiro; no se simula un campo iterativo durante la predicción. Una animación de ese tiro en Godot no añade pasos al modelo.

La cantidad de componentes del potencial inmanente aún debe concretarse. No viene impuesta por el número de entradas, de neuronas ni de coordenadas transcendentes.

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

Este potencial escalar transcendente es distinto del potencial vectorial inmanente.

Los pozos orientan el backward para corregir los parámetros que produjeron las posiciones. No colocar artificialmente una predicción sobre la respuesta y contarla como acierto.

La pérdida completa sigue pendiente: debe promover acercamiento a algún pozo y cobertura de ambos, evitando que todas las neuronas representen solo una propiedad. No se ha aprobado una simple suma de atracciones que pueda compensarse entre pozos.

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
2. Número de neuronas y componentes de cada potencial inmanente.
3. Activación inmanente e inicialización de los parámetros y posiciones transcendentes.
4. Pérdida de atracción y cobertura, y parámetros de los pozos.
5. Lectura de la respuesta y métricas de especialización, sin asumir una media por grupos que no se ha acordado.
6. Integración técnica con Godot.

La coincidencia de varias neuronas en un pozo indica posibles candidatas a reducción, pero no demuestra que sean prescindibles: pueden cooperar inmanentemente.

La reconstrucción inversa y la influencia colectiva sobre neuronas aisladas permanecen aparcadas en [el ideario](../ideario.md).
