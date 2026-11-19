# Celsius → (Fahrenheit, Kelvin): primera ejecución vectorial local

Ejecución real local del 18 de noviembre de 2026. Código basado en `db66a822e5d87fcecc458d510aeb568676a06014`, con la extensión vectorial todavía sin commit al ejecutar. `metadata.json` registra los SHA-256 exactos del núcleo, las pruebas y el adaptador gráfico; no se atribuye la ejecución al código escalar de ese commit base.

Se conservaron los ajustes locales: **3 neuronas**, `lr=0.001`, semilla 42 y orden fijo. Presupuesto de verificación explícito: **1000 épocas**, en vez de la meta por defecto de 100000. Dataset: entrenamiento `[-40,-20,0,20,40,60,80,100]`, validación `[10,70]`. Normalización por 100 de entrada y de cada coordenada del objetivo. Ideal ordenado `(F,K)` con `F=1.8*C+32`, `K=C+273.15`; las fórmulas generan solo etiquetas.

Cada neurona aprende `q=alpha*V_last+beta`, con identidad geométrica, alpha inicial 1 y beta inicial 0. La pérdida promedia las neuronas participantes y suma las dos coordenadas. Actualización SGD después de cada ejemplo. No hay reducción automática de lr, selección de neuronas ni parada temprana.

Compilación local: GCC 16.2.1, GNU libstdc++, Linux, C++17, Release y advertencias tratadas como errores. Se ejecutó el binario de consola con `--epochs 1000` en un directorio temporal para conservar los historiales anteriores; aquí se copian exactamente su CSV como `history.csv` y su salida como `console.log`.

| Métrica normalizada | Época 0 | Época 1000 |
| --- | ---: | ---: |
| Entrenamiento total | 8.4946026806313064 | 0.18212777077839271 |
| Entrenamiento F | 0.66861718594320563 | 0.093714890431582945 |
| Entrenamiento K | 7.8259854946881005 | 0.088412880346809747 |
| Validación total | 8.7715840468961943 | 0.073669405968024723 |
| Validación F | 0.67396609497694027 | 0.038049368190383896 |
| Validación K | 8.0976179519192542 | 0.035620037777640827 |
| Conexiones internas | 3 | 3 |
| Neuronas participantes | 3 | 3 |

RMSE físico sobre **todos los puntos** y ejemplos de validación: **19.50625 °F** y **18.87327 K**. En entrenamiento: 30.61289 °F y 29.73430 K. Se obtiene con `100*sqrt(loss_coordinate)`; no es el error de una media usada como predictor. Los puntos físicos individuales de ambos ejemplos están en `console.log`.

El error baja en ambas coordenadas, pero esta ejecución no produce una conversión exacta. No se ajustaron hiperparámetros con las etiquetas de validación. Dos ejemplos de validación y objetivos situados en una recta no demuestran generalización amplia ni una ventaja de arquitectura.

Comprobaciones: 1001 filas consecutivas, todas las métricas finitas, totales iguales a la suma de coordenadas y conexiones no crecientes. Pasaron las pruebas anteriores, 101 derivadas vectoriales por diferencias finitas, las comprobaciones manuales y AddressSanitizer/UndefinedBehaviorSanitizer. El callback y la continuación generan el mismo CSV que la consola sin callback. La interfaz se compiló; no se realizó una comprobación visual de la ventana en esta entrega.

Para repetir con los ajustes actuales del archivo:

```bash
./run_drann.sh --console --epochs 1000
```

La consola escribe `training_history_fk.csv`. Para guardar otra ejecución, crear una carpeta diferente en `results/`; no reemplazar esta ni la referencia escalar de v1.
