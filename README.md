# DRANN

Diseño de una red neuronal artificial rizomática diferencial, implementada desde cero en C++.

El primer experimento será una regresión de Celsius a Fahrenheit. Primero se desarrollará un ejecutable de consola; la integración con Godot se abordará después.

La primera implementación está en `DRANN.cpp`, siguiendo la estructura y el estilo de `XOR.cpp`: `struct`, vectores, cachés, bucles y derivadas manuales. No utiliza frameworks neuronales ni autodiferenciación.

## Compilar y ejecutar

Se necesita un compilador con soporte para C++17. Desde la raíz del repositorio:

```bash
mkdir -p build
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic DRANN.cpp -o build/drann
./build/drann
```

El programa ejecuta 100 épocas con 5 neuronas y `lr = 0.005`. Genera `training_history.csv` en el directorio de ejecución: una evaluación inicial y otra tras cada época, con las pérdidas geométricas de entrenamiento y validación, pasos medios, conexiones activas y neuronas participantes. Una nueva ejecución reemplaza ese archivo.

Al terminar muestra el cognifenómeno primitivo completo de los dos ejemplos de validación, desnormalizado a Fahrenheit. No sustituye la nube por una neurona de salida ni por un promedio para calcular la pérdida.

## Configuración del experimento

La configuración y el dataset están en `main()` para mantenerlos visibles y fáciles de modificar:

- Entrenamiento en Celsius: `{-40, -20, 0, 20, 40, 60, 80, 100}`.
- Validación en Celsius: `{10, 70}`.
- Normalización: `x = C / 100`, `y = F / 100`, con `F = 1.8*C + 32` para generar las etiquetas.
- Semilla inicial: `42`; ejemplos en orden fijo, sin barajado.
- Pesos internos compartidos entre ambas direcciones; su anulación es irreversible durante el entrenamiento. Los pesos de entrada son parámetros ordinarios y no reciben esa anulación.

Los valores concretos del dataset, la semilla y el orden son elecciones iniciales de implementación. Los parámetros acordados de tamaño, normalización, tasa y épocas se mantienen.

## Comprobar el modelo

```bash
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic tests/DRANN_tests.cpp -o build/drann_tests
./build/drann_tests
```

Las pruebas incluyen recorridos calculados a mano, máximos locales simultáneos y empatados, receptora compartida, sesgos, cambios de papel, aislamiento parcial, anulación irreversible, evaluación sin cambios de parámetros y 49 derivadas contrastadas por diferencias finitas sin cambiar las decisiones discretas.

Para comprobar además memoria y comportamiento indefinido con un compilador que soporte estos sanitizadores:

```bash
g++ -std=c++17 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer tests/DRANN_tests.cpp -o build/drann_tests_sanitized
./build/drann_tests_sanitized
```

## Resultado inicial y límites

Con GCC 14.2.0 y la semilla 42, el error geométrico normalizado de validación pasó de `0.6280624832` a `0.0118076840`; el de entrenamiento pasó de `0.6230380312` a `0.0318698253`. Quedaron 9 de las 10 conexiones internas y las 5 neuronas participantes. La pérdida es sobre todos los puntos de la nube, no el error de un único escalar predicho.

La conversión todavía no es exacta. Dos ejemplos de validación y una tarea lineal no bastan para establecer la capacidad de generalización ni una ventaja de esta arquitectura. El backward deriva el recorrido ejecutado y no los cambios discretos de papeles. No hay reintegración, parada aprendida ni integración con Godot. El aislamiento total sigue fuera de alcance: si sucede, el programa termina con un mensaje de error, sin inventar una pérdida o una recuperación.

## Contexto del proyecto

- [Especificación y decisiones de diseño](docs/DRANN.md).
- [Instrucciones de trabajo y estilo](AGENTS.md).
- [Referencia de estilo original: XOR.cpp](docs/reference/XOR.cpp). Este archivo es una referencia del usuario, no la implementación de la DRANN.
- [Registro del primer experimento](docs/experiment-v1.md).
