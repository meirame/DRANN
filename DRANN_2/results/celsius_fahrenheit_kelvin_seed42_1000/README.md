# Celsius → Fahrenheit / Kelvin — semilla 42

Fecha: 19 de noviembre de 2026. Modelo final: época 1000; mejor modelo geométrico: época 998. Cinco neuronas, lr 0.005, identidad, reparto 8/2. Escalas de entrenamiento: Sx=100 y Sy=373.15. Ninguna fórmula de temperaturas interviene en el predictor.

| Medida | Inicial | Final |
|---|---:|---:|
| Training Loss | 0.7021203950 | 0.0006370550 |
| Validation Loss | 0.7192433929 | 0.0002605799 |

Mejor Validation Loss: **0.000251098005**, época 998. Se conservaron nueve conexiones activas de diez; las cinco neuronas participan en la geometría. Media final: cuatro pasos internos por ejemplo.

| Entrada °C | Eje | Lectura física | Ideal físico | Estado |
|---:|---|---:|---:|---|
| 10 | Fahrenheit | 44.596590 | 50.000000 | válido |
| 10 | Kelvin | 280.838264 | 283.150000 | válido |
| 70 | Fahrenheit | 158.242801 | 158.000000 | válido |
| 70 | Kelvin | 343.667948 | 343.150000 | válido |

RMSE de lectura: **3.824643 °F** y **1.675171 K**. Cobertura: 2/2 ejemplos válidos por componente. La distancia al pozo más próximo y el error de la mediana son métricas distintas; no se han sustituido entre sí.

## Archivos

`history.csv`: 1001 estados, incluida inicialización. `examples.csv`: 4004 lecturas. `neurons.csv`: 20020 coordenadas/diagnósticos. `final.model` y `best.model` contienen configuración, parámetros, máscaras, RNG, dataset y orden de ejemplos. Las cabeceras y unidades se describen en el README principal. `console.log` conserva los registros terminales; en terminal interactiva llevan colores ANSI.

`godot-screenshot.png` corresponde a una prueba de controles: cargó el modelo final y continuó dos épocas y cuatro ejemplos antes de pausar. No es la imagen del estado exacto de la época 1000. La gráfica de esa captura muestra el historial posterior a la carga; el CSV conserva las 1000 épocas originales.

## Verificación

- CMake Release: consola, núcleo independiente y GDExtension compilados con GCC 16.2.1 y Godot 4.5.1 stable, SDK fijado por commit/hash.
- CTest: prueba numérica completa aprobada. Incluye 42 diferencias finitas para ambos tipos de activación, más tres para el sesgo reutilizado, y los casos de comportamiento descritos en el README principal.
- AddressSanitizer y UndefinedBehaviorSanitizer: ejecutable de pruebas aprobado sin diagnósticos. Compilación usada: `g++ -std=c++17 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer DRANN.cpp main.cpp tests/DRANN_tests.cpp -I. -o /tmp/drann2-final-sanitized`.
- Godot real con X11/OpenGL: entrenamiento, pausa, avance por ejemplo, predicción sin ideal, comparación posterior, guardado/carga, continuidad e inspección τ aprobados. Diferencia máxima admitida frente a posiciones C++: 1e-12.
- Apertura sin caché de importación, en copia limpia: aprobada en modo headless. La escena carga explícitamente la GDExtension cuando no está registrada; no necesita un autoimportado previo del editor.

## Límites observados

El modelo mejora pero no predice exactamente las temperaturas. Dos ejemplos de validación son insuficientes para concluir que generaliza. Los umbrales de lectura y el radio son defaults provisionales. Pozos coincidentes en cero se señalan, sin resolver su identidad geométrica. No se activaron mecanismos de transferencia, crecimiento/reducción, regularización ni reintegración de conexiones.

La semilla y el estado RNG permiten reproducir y continuar con la misma implementación de la biblioteca estándar. No se promete identidad bit a bit entre compiladores o bibliotecas estándar diferentes. No se modificaron archivos existentes fuera de DRANN_2.
