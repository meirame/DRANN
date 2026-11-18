# Comparación local del 18 de noviembre de 2026

Las ejecuciones usaron la misma arquitectura, tres neuronas, SGD con lr 0,001,
presupuesto de 4000 épocas y semillas 42 y 43. Se cambió la mejora mínima a
`0.001` en todos los casos para que hubiera intentos dentro del presupuesto.
La sensibilidad a lambda se midió con la semilla 42. Los tiempos incluyen los
diagnósticos y el CSV en este portátil. Las pérdidas y los peores puntos están normalizados.

| Datos | Semilla | λ | Trabajo registrado | Tiempo (s) | N | Validación total | Coord. 0 | Coord. 1 | Peor punto | Decisión |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| [celsius](celsius_seed42_control/history.csv) | 42 | 0 | 4000 | 0.435 | 3 | 0.000533317 | 0.000437188 | 9.61292e-05 | 0.0390157 | Control |
| [celsius](celsius_seed42_pressure/history.csv) | 42 | 0.001 | 4000 | 0.441 | 3 | 0.00253076 | 0.0020049 | 0.000525859 | 0.0774966 | Restauración sin reducción |
| [celsius](celsius_seed42_lambda0.01/history.csv) | 42 | 0.01 | 4000 | 0.431 | 3 | 0.00253076 | 0.0020049 | 0.000525859 | 0.0774966 | Restauración sin reducción |
| [celsius](celsius_seed42_lambda0.1/history.csv) | 42 | 0.1 | 4000 | 0.439 | 3 | 0.00253076 | 0.0020049 | 0.000525859 | 0.0774966 | Restauración sin reducción |
| [celsius](celsius_seed43_control/history.csv) | 43 | 0 | 4000 | 0.443 | 3 | 0.000272228 | 0.000222197 | 5.00306e-05 | 0.0290346 | Control |
| [celsius](celsius_seed43_pressure/history.csv) | 43 | 0.001 | 4000 | 0.434 | 3 | 0.00187027 | 0.00145568 | 0.000414592 | 0.0630967 | Restauración sin reducción |
| [sincos](sincos_seed42_control/history.csv) | 42 | 0 | 4000 | 3.075 | 3 | 0.847032 | 0.34765 | 0.499382 | 1.36722 | Control |
| [sincos](sincos_seed42_pressure/history.csv) | 42 | 0.001 | 4000 | 3.325 | 3 | 0.848797 | 0.349019 | 0.499778 | 1.36489 | Restauración sin reducción |
| [sincos](sincos_seed42_lambda0.01/history.csv) | 42 | 0.01 | 4000 | 2.589 | 2 | 0.845416 | 0.345395 | 0.500021 | 1.3596 | Reducción aceptada |
| [sincos](sincos_seed42_lambda0.1/history.csv) | 42 | 0.1 | 3455 | 1.956 | 2 | 0.844894 | 0.345857 | 0.499037 | 1.35936 | Abortó en 3456 |
| [sincos](sincos_seed43_control/history.csv) | 43 | 0 | 4000 | 3.015 | 3 | 0.871361 | 0.37134 | 0.500021 | 1.35085 | Control |
| [sincos](sincos_seed43_pressure/history.csv) | 43 | 0.001 | 4000 | 3.266 | 3 | 0.886159 | 0.38615 | 0.500009 | 1.35234 | Restauración sin reducción |

Lambda 0,001 no aisló una neurona en estas ejecuciones. Con 0,01 el caso
seno/coseno de semilla 42 sí aceptó una reducción de tres a dos participantes.
Con 0,1 también aceptó una reducción, pero más adelante SGD anuló la última
arista fuera de un intento. El proceso registró el fallo, conservó el mejor
snapshot predictivo y salió con error. La fila de la tabla corresponde al
último estado evaluado antes del fallo.

La aceptación compara contra la referencia justo antes del intento, no contra
el control tras 4000 épocas. El ajuste de seno/coseno sigue teniendo un error
considerable. Estas ejecuciones ilustran la sensibilidad a lambda y al presupuesto
en un modelo pequeño.

Ejemplo para repetir el caso que conservó la reducción desde la raíz de la copia.

```sh
./bin/drann --batch --dataset sincos --seed 42 --epochs 4000 --lambda 0.01 --min-improvement 0.001
```
