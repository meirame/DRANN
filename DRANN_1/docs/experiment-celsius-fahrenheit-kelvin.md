# Experimento vectorial: Celsius → (Fahrenheit, Kelvin)

Esta extensión sustituye la lectura escalar para el ejecutable actual, conservando la dinámica interna y los controles de consola y GUI. El código permanece en `DRANN_C_F.cpp`. La configuración local conservada es de 3 neuronas, lr 0.001, semilla 42 y meta inicial de 100000 épocas; usar `--epochs` para elegir un presupuesto distinto. No se añade reducción de neuronas, reintegración, Adam, entrada vectorial general ni Godot.

## Datos y representación

Entrenamiento Celsius: `{-40,-20,0,20,40,60,80,100}`. Validación: `{10,70}`. Los ejemplos mantienen su orden, sin barajar. `x=C/100`; el ideal es un único punto bidimensional ordenado `y=((1.8*C+32)/100,(C+273.15)/100)`. El índice 0 representa Fahrenheit y el 1 Kelvin. Las fórmulas generan etiquetas; no aparecen en la lectura geométrica ni se usan como predictor.

Cada neurona conserva su potencial escalar `V_last[i]` y aprende dos vectores independientes de su sesgo interno: `alpha[i,k]` y `beta[i,k]`. La lectura tiene identidad geométrica:

```text
q[i,k] = alpha[i,k] * V_last[i] + beta[i,k]
alpha inicial = 1; beta inicial = 0
```

La inicialización geométrica no consume números aleatorios. No hay restricción de signo ni anulación permanente para estos parámetros. Los potenciales y trazas internos siguen separados de las posiciones. Las transmisoras retiradas aportan su punto final; las aisladas se excluyen. La nube no se promedia, deduplica ni se ordena para compararla con el ideal. El aislamiento total continúa siendo un error explícito.

## Pérdida y backward manual

Con A participantes y N su número, se promedian neuronas y se suman coordenadas, **sin dividir por D**:

```text
L_coordinate[k] = sum(i in A, (q[i,k]-y[k])²) / N
L = sum(k, L_coordinate[k])
delta_geom[i,k] = 2*(q[i,k]-y[k])/N
dAlpha[i,k] = delta_geom[i,k]*V_last[i]
dBeta[i,k] = delta_geom[i,k]
g_last[i] = sum(k, delta_geom[i,k]*alpha[i,k])
```

`g_last` sustituye la antigua señal de pérdida escalar y se propaga por el mismo backward interno. No se añade otra derivada del error sobre V. Se acumulan todos los gradientes con los mismos parámetros previos; después se aplica SGD a pesos, sesgos, alpha y beta. Solo los pesos internos mantienen su regla de anulación irreversible.

Los métodos `checkTarget`, `checkGeometry` y `checkDataset` verifican dimensiones y valores finitos. El constructor `DRANN(n, lr, D, trainGeometry)` permite el control escalar `D=1, trainGeometry=false`; sus valores por defecto preservan las llamadas antiguas de las pruebas. El ejecutable usa explícitamente `DRANN(n, lr, 2, true)`. `initWeights` reinicia también alpha y beta. La entrada sigue siendo escalar.

Las funciones `loss` y `backpropRhizome` aceptan el vector objetivo. `evaluateVector` y `trainVector` reciben una fila de objetivos por ejemplo; los adaptadores escalares solo convierten targets y reutilizan el núcleo. La evaluación puede cambiar cachés, pero no parámetros ni máscaras.

## Salida e historial

La consola muestra cada entrada, el ideal físico `(F,K)`, todos los puntos participantes desnormalizados mediante `100*q`, los pasos y las pérdidas total y por coordenada. Los callbacks llevan `drannMetrics.coordinateLoss`. La GUI conserva sus curvas de pérdida total, muestra las dos componentes y mantiene pausa, finalización y continuación.

El nuevo ejecutable utiliza **`training_history_fk.csv`** para conservar los historiales escalares existentes. Se mantienen las siete columnas previas y se añaden:

- `training_loss_fahrenheit`, `training_loss_kelvin`.
- `validation_loss_fahrenheit`, `validation_loss_kelvin`.

Los totales coinciden con la suma de componentes salvo redondeo. La época 0 es la referencia inicial. La continuación agrega las épocas siguientes sin repetir cabecera ni referencia. El caso escalar D=1 conserva el formato original del CSV. Para dimensiones generales se utilizan sufijos `coordinate_k`.

## Compilación y comprobaciones

```bash
cmake -S . -B build/headless -DDRANN_BUILD_GUI=OFF
cmake --build build/headless --parallel 2
ctest --test-dir build/headless --output-on-failure
./run_drann.sh --console --epochs 1000
# Con las dependencias gráficas instaladas:
./run_drann.sh --epochs 1000
```

Se comprobaron lectura y gradientes manuales, error 72 para `(8,2)` frente a `(2,8)`, 101 derivadas vectoriales por diferencias finitas con recorrido y máscaras conservados, aisladas/retiradas, dimensiones inválidas, evaluación inmutable y actualización geométrica. Siguen pasando las 49 derivadas escalares y las pruebas previas. Una comparación con el archivo escalar anterior produjo pérdidas, gradientes, actualizaciones y CSV idénticos byte a byte con D=1 y geometría congelada. Pasaron los sanitizadores de memoria y comportamiento indefinido.

El mismo entrenamiento produce idénticos parámetros y CSV con y sin callback, al detener en una época y al continuar. Se compiló la GUI; esto no equivale a una comprobación visual de la ventana.

## Resultado registrado y límites

La [primera ejecución local de 1000 épocas](../results/celsius_fahrenheit_kelvin_local_2026-11-18_1000/README.md) conserva CSV, log, configuración, procedencia y huellas del código ejecutado. La pérdida de validación pasó de 8.77158405 a 0.073669406; RMSE físico por punto de 19.50625 °F y 18.87327 K. Queda error apreciable y no se afirma convergencia exacta.

Dos ejemplos de validación son una primera comparación; ambas coordenadas dependen de una misma magnitud y sus ideales forman una recta. No se comparó capacidad de generalización frente a otras arquitecturas ni se ajustó con etiquetas de validación. Los resultados escalares publicados anteriormente siguen siendo referencias históricas, con otra definición de salida y pérdida.
