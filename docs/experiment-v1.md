# Primer experimento ejecutable

## Configuración

- Núcleo: `DRANN.cpp`, C++17, sin frameworks ni autodiferenciación.
- Compilador utilizado: GCC 14.2.0, GNU libstdc++, Linux.
- Neuronas: 5, con 10 parejas internas inicialmente habilitadas.
- Inicialización: `std::mt19937`, semilla `42`, uniforme en `(0, 1)`; sesgos cero.
- Activación: identidad.
- Entrenamiento: 100 épocas, actualización tras cada ejemplo, `lr = 0.005` fijo.
- Entrenamiento Celsius: `{-40, -20, 0, 20, 40, 60, 80, 100}`.
- Validación Celsius: `{10, 70}`.
- Normalización: entrada y objetivo divididos entre 100.
- Orden de los ejemplos fijo; sin barajado, parada temprana ni ajuste de tasa.

La semilla permite repetir la inicialización con la misma implementación de biblioteca estándar; no se presupone que la transformación de la distribución sea idéntica entre todas las bibliotecas C++.

## Comprobaciones ejecutadas

- Compilación del programa y las pruebas con `-std=c++17 -O2 -Wall -Wextra -Wpedantic -Werror`, sin avisos.
- Recorridos manuales con transmisión secuencial, simultánea entre máximos locales de distinto potencial y simultánea entre máximos empatados.
- Receptora compartida: agregación desde el mismo estado anterior y sesgo una sola vez por paso.
- Exclusividad de papeles, cambio de papel tras recibir, conservación de transmisoras y reinicio de disponibilidad por ejemplo.
- Inclusión de retiradas en la pérdida, exclusión de aisladas y ausencia de transmisión con potenciales iguales.
- Anulación de pesos internos cero y negativos sin recuperación posterior; parámetros simétricos únicos y ausencia de anulación aplicada a pesos de entrada.
- 49 derivadas comparadas con diferencias finitas centradas, comprobando que las perturbaciones preservan participantes, transmisoras, receptoras y transmisiones. Incluyen entradas negativas, emisoras distintas, recepción compartida, recepciones sucesivas, sesgos reutilizados y parámetros excluidos.
- Evaluación sin cambios de pesos, sesgos o máscaras de anulación.
- Pruebas ejecutadas también con AddressSanitizer y UndefinedBehaviorSanitizer, sin incidencias.
- Ejecución completa: 101 filas de evaluación (épocas 0 a 100), métricas finitas y número de conexiones no creciente.

## Resultados observados

| Medida | Antes del entrenamiento | Tras 100 épocas |
| --- | ---: | ---: |
| Error geométrico normalizado de entrenamiento | 0.6230380312461784 | 0.031869825340849603 |
| Error geométrico normalizado de validación | 0.6280624832034999 | 0.011807684001846962 |
| Conexiones internas habilitadas | 10 | 9 |
| Neuronas participantes | 5 | 5 |
| Pasos medios de entrenamiento | 3.5 | 4 |
| Pasos medios de validación | 4 | 4 |

La menor pérdida de validación de esta ejecución aparece en la época 100. Esto no demuestra ausencia de sobreajuste en general: hay solo dos ejemplos de validación y la relación objetivo es lineal.

Los puntos de los cognifenómenos primitivos finales, desnormalizados a Fahrenheit y conservando el orden de las neuronas, fueron:

| Entrada Celsius | Ideal Fahrenheit | Cognifenómeno primitivo Fahrenheit |
| --- | ---: | --- |
| 10 | 50 | `[50.9610384, 38.80684083, 38.09041502, 46.48357677, 52.19675924]` |
| 70 | 158 | `[147.9466911, 149.6212802, 133.8265984, 152.0474837, 168.2211073]` |

La pérdida utiliza todos los puntos con la misma participación, no el error de su media. Todavía queda un error apreciable: no se presenta el experimento como una conversión exacta ni como evidencia de consciencia o de superioridad frente a otra arquitectura.

Para regenerar todas las filas, compilar y ejecutar siguiendo el README. El programa escribe `training_history.csv` en el directorio actual; el archivo generado no se versiona automáticamente.

## Alcance

La geometría vacía provoca un error explícito: no se ha añadido una recuperación para el aislamiento total. El backward conserva las decisiones discretas del recorrido y no evalúa alternativas. No hay gradientes sustitutos, reintegración ni Godot en este ejecutable.
