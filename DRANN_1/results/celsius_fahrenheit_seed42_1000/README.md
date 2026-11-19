# Celsius–Fahrenheit: semilla 42, 1000 épocas

Procedencia: ejecución verificada en el entorno remoto antes de pasar el desarrollo a local. No es una lectura de los archivos del ordenador del usuario. Los cognifenómenos finales coinciden con los resultados de 1000 épocas que el usuario compartió en la conversación.

Código ejecutado: commit `7e68d849f51b67339de30325cc8f1cbef722a68a`, rama `feature/drann-v1`. Consola e interfaz nativa generan exactamente el mismo [history.csv](history.csv), comprobado byte a byte.

## Configuración

| Parámetro | Valor |
| --- | --- |
| Neuronas | 5 |
| Semilla | 42 |
| `lr` | 0.005 |
| Épocas | 1000 |
| Entrenamiento Celsius | -40, -20, 0, 20, 40, 60, 80, 100 |
| Validación Celsius | 10, 70 |
| Etiquetas | `F = 1.8*C + 32` |
| Normalización | `x = C/100`, `y = F/100` |
| Orden | Fijo, sin barajado |
| Compilador | GCC 14.2.0, Linux |

## Historial y resultados

El CSV contiene 1001 filas de datos: evaluación inicial en época 0 y evaluación después de cada una de las 1000 épocas. Las pérdidas se calculan sobre todos los puntos participantes del cognifenómeno primitivo, no sobre un promedio usado como predicción.

| Medida | Época 0 | Época 1000 |
| --- | ---: | ---: |
| Pérdida de entrenamiento | 0.6230380312461784 | 0.010053725427410436 |
| Pérdida de validación | 0.6280624832034999 | 0.004309649949651236 |
| Conexiones internas habilitadas | 10 | 9 |
| Neuronas participantes | 5 | 5 |

Con solo dos ejemplos de validación, estos resultados no establecen por sí solos la capacidad de generalización. El historial se conserva sin suavizado ni selección de las mejores épocas.

## Reproducción

Con las dependencias del README instaladas:

```bash
./run_drann.sh --epochs 1000
```

Para guardar una repetición local, elegir una carpeta de resultados diferente y anotar su procedencia, en lugar de reemplazar esta referencia.
