# Próximo experimento: Celsius → (Fahrenheit, Kelvin)

Fecha: 18 de noviembre de 2026.

## Estado y propósito

El usuario ha elegido este experimento para la futura implementación local. Este documento conserva las decisiones sobre la lectura vectorial y separa las propuestas pendientes. No se ha implementado ni ejecutado aquí; no contiene resultados nuevos.

La versión escalar actual sigue descrita en [DRANN.md](DRANN.md). El contexto de reducción compensada está en [ideas/reduction-on-plateau.md](ideas/reduction-on-plateau.md).

## Acuerdos de la conversación

- El output del dataset se interpreta como un punto cuyas coordenadas conservan su orden, no como un segmento definido por dos valores.
- Una entrada Celsius debe producir un punto bidimensional `(Fahrenheit, Kelvin)`. No intercambiar las coordenadas.
- Cada neurona participante tendrá una posición geométrica dependiente del estado final obtenido para esa entrada.
- Se investigará la lectura por neurona con vectores aprendibles llamados `alpha` y `beta`.
- La activación geométrica se aplica después de transformar cada coordenada con alpha y beta; no se ha fijado definitivamente su función.
- Los potenciales internos siguen siendo escalares. La lectura vectorial no cambia por sí sola la selección de transmisoras.
- Coincidir en posición geométrica identifica candidatas a reducción; antes de retirarlas hay que compensar su contribución al cálculo.
- Esta tarea no requiere asignar una neurona portavoz ni una neurona diferente a cada coordenada.
- El código se implementará después en el entorno local, desde cero en C++, manteniendo el estilo de la referencia XOR.cpp.

## Tarea y transformación del dataset

Entrada física: Celsius `C`.

Objetivo físico ordenado:

```text
y_physical = (F, K)
F = 1.8 * C + 32
K = C + 273.15
```

Ejemplo: `C = 20` tiene como objetivo `(68, 293.15)`.

Estas fórmulas generan las etiquetas; no se incorporan como predictor ni como información del objetivo dentro del forward.

Para una primera comparación se propone reutilizar los diez Celsius de v1 y la misma partición. Es una propuesta de configuración reproducible, no un dataset nuevo confirmado por separado:

| Celsius | Fahrenheit | Kelvin | Uso propuesto |
| --- | --- | --- | --- |
| -40 | -40.00 | 233.15 | Entrenamiento |
| -20 | -4.00 | 253.15 | Entrenamiento |
| 0 | 32.00 | 273.15 | Entrenamiento |
| 10 | 50.00 | 283.15 | Validación |
| 20 | 68.00 | 293.15 | Entrenamiento |
| 40 | 104.00 | 313.15 | Entrenamiento |
| 60 | 140.00 | 333.15 | Entrenamiento |
| 70 | 158.00 | 343.15 | Validación |
| 80 | 176.00 | 353.15 | Entrenamiento |
| 100 | 212.00 | 373.15 | Entrenamiento |

La extensión más directa de la normalización previa sería:

```text
x = C / 100
y = (F / 100, K / 100)
F_display = 100 * q[0]
K_display = 100 * q[1]
```

Entonces los objetivos normalizados son `(1.8*x + 0.32, x + 2.7315)`. Son dos coordenadas diferentes pero dependientes de una única magnitud; los puntos ideales forman una recta. El éxito en esta tarea comprueba la salida vectorial, no dos grados de libertad independientes ni una capacidad general para cualquier geometría.

La normalización final del experimento vectorial debe documentarse explícitamente, junto con su inversa y orden de coordenadas. No calcularla usando etiquetas de validación ni ajustarla durante la predicción.

## Posición geométrica por neurona

Sean `i` una neurona participante, `k` una coordenada, `V_last[i](x)` su potencial final, y `D = 2`.

```text
z[i,k] = alpha[i,k] * V_last[i](x) + beta[i,k]
q[i,k](x) = sigma_geom(z[i,k])
```

Por tanto:

```text
q_i(x) = (q[i,0](x), q[i,1](x))
coordinate 0 = Fahrenheit normalizado
coordinate 1 = Kelvin normalizado
```

Alpha y beta son vectores propios de cada neurona, con D componentes cada uno. Son parámetros aprendibles; no posiciones fijas que se optimicen por separado para cada ejemplo. No proporcionan acceso al objetivo durante validación.

Los beta geométricos son distintos del sesgo escalar `b[i]` reutilizado en la inicialización y recepciones del rizoma. Alpha y beta pueden tener cualquier signo y no reciben la regla de anulación de conexiones internas.

Con N neuronas, esta lectura añade `2*N*D` parámetros: veinte con N=5 y D=2. La dimensión del output determina D; no determina cuántas neuronas son necesarias.

El cognifenómeno primitivo pasa a ser la nube de vectores `q_i(x)` de las participantes, incluidas las retiradas por transmitir. El ideal de este experimento es un único punto bidimensional. Varias posiciones próximas entre sí no se deduplican automáticamente ni justifican por sí solas una eliminación.

## Activación: propuesta y límites

La identidad geométrica permite analizar primero `q_i = alpha_i * V_last[i] + beta_i`. Para cada neurona, sus posiciones posibles están sobre una recta; esto es compatible con la geometría ideal de Celsius → (Fahrenheit, Kelvin).

Se propuso, sin confirmación final, Leaky ReLU:

```text
sigma_geom(z) = z             si z >= 0
sigma_geom(z) = 0.01 * z      si z < 0
```

El 0.01 sería una elección inicial configurable, no lr ni un factor de propagación interna. La activación del forward interno continúa siendo identidad.

Aplicar una no linealidad a cada `alpha[i,k]*V_last[i]+beta[i,k]` puede doblar la trayectoria geométrica. Aplicarla solamente a V y multiplicar después por alpha conserva una recta y es una construcción distinta.

Una activación monótona mantiene cada coordenada monótona respecto al único V escalar. Añadir neuronas no elimina esa restricción individual si cada una debe reproducir el vector completo. La lectura no es una solución general para salidas de dos componentes independientes ni para curvas como una circunferencia.

Además, con la normalización directa propuesta hay objetivos Fahrenheit negativos y positivos, mientras Kelvin permanece positivo. Con Leaky ReLU de pendiente negativa 0.01, un mismo potencial escalar no puede representar exactamente toda esta recta al cruzar la coordenada Fahrenheit el cambio de pendiente mientras Kelvin queda en la rama positiva. Esto es una limitación de la lectura, no necesariamente del entrenamiento.

Opciones para estudiar antes de cerrar esa elección:

- Usar identidad geométrica como control para esta tarea lineal.
- Si se ensaya Leaky ReLU, adoptar una transformación fija compatible con sus ramas y documentar su inversa. Por ejemplo, para el intervalo propuesto [-40,100] Celsius, `((F+50)/100, (K+50)/100)` deja ambas coordenadas positivas. Este desplazamiento es solo una opción, no un acuerdo ni una garantía de entrenamiento; fuera de ese intervalo hay que reconsiderar las ramas.
- Mantener una comparación explícita entre activaciones sin confundir una restricción representacional con falta de convergencia.

La lectura colectiva desde todos los potenciales se discutió como alternativa más expresiva; no es la opción seleccionada para este primer estudio de alpha y beta.

## Pérdida propuesta y orden

Sea A el conjunto de participantes y N su tamaño:

```text
L_ej = sum(i in A, sum(k=0..D-1, (q[i,k] - y[k])^2)) / N
```

La suma de coordenadas no se divide por D en esta propuesta. Registrar esa convención para interpretar escalas y lr; si se cambia, actualizar también sus derivadas y las comparaciones.

Comparar coordenada con coordenada. No ordenar componentes ni emparejarlos buscando la menor distancia. Por ejemplo, contra el ideal (2,8), el punto (8,2) tiene error cuadrático 72, mientras (2,8) tiene cero. La identidad de las neuronas no fija el orden de las coordenadas.

La media de posiciones puede mostrarse como estadística auxiliar, pero no sustituye esta pérdida sobre todas las participantes.

## Backward manual de la lectura

Manteniendo fijos recorrido, máscaras y participación:

```text
dL_dq[i,k] = 2 * (q[i,k] - y[k]) / N
delta_geom[i,k] = dL_dq[i,k] * sigma_geom_derivative(z[i,k])

dAlpha[i,k] = delta_geom[i,k] * V_last[i]
dBeta[i,k] = delta_geom[i,k]
g_last[i] = sum(k=0..D-1, delta_geom[i,k] * alpha[i,k])
```

Usar g_last como señal inicial del backward interno existente. La retirada temporal no impide acumular gradientes desde las transmisiones y desde la posición geométrica.

Guardar potenciales finales y preactivaciones geométricas necesarios. Calcular todos los gradientes con los parámetros originales del ejemplo antes de actualizarlos. Después actualizar también alpha y beta y aplicar la anulación irreversible únicamente a conexiones internas, siguiendo las reglas de v1.

## Optimizador y reducción: trabajos separados

Adam fue propuesto por el usuario para ajustar los parámetros. Adapta las actualizaciones usando momentos de los gradientes; no determina la representación, las correspondencias de coordenadas ni el número de neuronas necesario.

Se propone verificar primero esta lectura con el optimizador actual y comparar después con Adam manteniendo el mismo objetivo y métricas. No se ha aprobado ni implementado Adam mediante este documento. Si se incorpora, checkpoints y restauraciones deben incluir sus momentos y contador.

La reducción por estancamiento sigue siendo una propuesta aparte. Una neurona puede tener una posición redundante y ser necesaria para los potenciales de las demás. Su compensación debe considerar también alpha, beta y el cambio en la ponderación de la pérdida al variar N. No introducir poda ni reintegración al implementar únicamente este experimento.

## Plan para la implementación local

1. Revisar AGENTS.md, DRANN.md, LOCAL.md y este documento, conservando los cambios locales del usuario.
2. Mantener un caso de compatibilidad escalar: D=1, identidad geométrica, alpha=1 y beta=0 fijados deben recuperar exactamente la lectura, pérdida y gradientes previos.
3. Incorporar objetivos vectoriales, alpha/beta, cachés geométricas, pérdida y backward manual.
4. Ejecutar primero un control con identidad geométrica y después los ensayos de activación definidos explícitamente.
5. Mantener inicialmente la dinámica, simetría, selección local, retirada, parada y reglas de anulación de v1.
6. Evaluar sin actualizar parámetros después de cada época. Mostrar puntos (F,K) desnormalizados, pérdidas por coordenada y pérdida total; conservar la gráfica de entrenamiento/validación.
7. Publicar configuración y resultados propios cuando se ejecuten. No presentar esta nota como una ejecución.

N=5, lr=0.005, 100 épocas, semilla 42 y orden fijo pueden servir como referencia de configuración heredada de v1, no como una selección ajustada al nuevo problema. Se conservan dos ejemplos de validación como primera comparación, con las limitaciones ya documentadas.

Este experimento tiene una sola entrada escalar. Admitir entradas vectoriales se discutió como extensión: `V[i,0]=sigma(sum(r, w_input[i,r]*x[r])+b[i])`; no es necesario ampliar ahora la entrada para realizarlo.

## Comprobaciones y registro

- Caso calculado a mano para la lectura y el error por coordenada.
- Comprobación del orden Fahrenheit/Kelvin; evitar que una permutación pase inadvertida.
- Derivadas de alpha, beta, pesos internos, pesos de entrada y sesgos contrastadas por diferencias finitas, conservando recorrido y rama de activación y evitando umbrales y empates.
- Inclusión de retiradas y exclusión de aisladas; no inventar tratamiento de aislamiento total.
- Evaluación sin cambios en parámetros, máscaras ni estado del optimizador.
- Registro de normalización/inversa, activación y derivada en el origen si procede, inicialización de alpha/beta, convención de pérdida, semilla, dataset, configuración y métricas por coordenada.
- No interpretar que coincidir geométricamente permite eliminar automáticamente una neurona ni que esta tarea prueba salidas independientes.

Antes de ejecutar quedan por fijar los valores iniciales de alpha/beta y la activación/normalización concreta del ensayo. Son decisiones de implementación que deben quedar visibles y justificadas, no cambios implícitos a los acuerdos.
