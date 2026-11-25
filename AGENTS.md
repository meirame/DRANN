# AGENTS.md — Filosofía de programación de DRANN

Instrucciones permanentes para Codex y cualquier agente que genere, revise o modifique código de este repositorio.

## 1. Principio fundamental

**DRANN debe parecer una continuación natural del código escrito por su autor, no un proyecto desarrollado con una filosofía de programación diferente.** Priorizar la transparencia matemática, la sencillez, el control directo y la facilidad de inspeccionar y modificar cada operación desde C++.

Referencia de estilo: [satellite_CNN_5HL.cpp](https://github.com/meirame/cnn-satellite1/blob/master/src/satellite_CNN_5HL.cpp), del repositorio `meirame/cnn-satellite1`. Estas instrucciones se han extraído del código C++ que el autor facilitó directamente el 25-11-2026. Si no hay permisos para consultar ese repositorio, aplicar igualmente esta guía. No es una dependencia de DRANN ni una referencia de arquitectura neuronal.

## 2. Filosofía de implementación

- C++ desde cero, inicialmente sobre CPU. Escribir explícitamente las operaciones matemáticas, el forward, el backward y Adam. No sustituirlos por frameworks de aprendizaje automático, autodiferenciación ni bibliotecas que oculten el núcleo matemático.
- Priorizar la biblioteca estándar, `std::vector`, bucles `for`, acumuladores y operaciones elementales. Utilizar vectores anidados cuando reflejen bien las dimensiones; evitar punteros, plantillas, macros y capas de abstracción innecesarias.
- La correspondencia entre ecuación y código debe ser reconocible a simple vista. Favorecer un bucle explícito y comprensible frente a una expresión compacta pero opaca.
- Separar claramente cálculo de estados, cálculo de gradientes y actualización de parámetros. Completar el backward antes de modificar los parámetros de los que depende.
- Utilizar `const` y referencias para datos de solo lectura y referencias mutables para resultados que deban escribirse. Inicializar explícitamente acumuladores, gradientes y momentos.
- No crear arquitecturas de software industriales, jerarquías de clases, patrones de diseño, dependencias o refactorizaciones generales por iniciativa propia. La simplicidad no impide separar responsabilidades cuando sea necesario.
- Mantener el código general respecto a las dimensiones del problema, sin imponer tamaños ni propiedades de la CNN de referencia a DRANN.

## 3. Organización y estética

La CNN de referencia agrupa responsabilidades en `struct modelSaving`, `struct utils`, `class sampling`, `struct CNNFunctions`, `struct CNN` y `main()`. Trasladar esa organización sencilla a DRANN cuando proceda, respetando la estructura existente del repositorio. Nombres equivalentes orientativos, no una API obligatoria: `modelSaving`, `utils`, `sampling`, `DRANNFunctions`, `DRANN`.

- Agrupar includes al comienzo del archivo. Usar `std::` explícito, no `using namespace std;`.
- Sangría de cuatro espacios. Llaves de estructuras, funciones y bloques extensos en línea independiente. Se admiten operaciones o bloques triviales en una línea, como en el código original.
- Separar generosamente funciones y bloques conceptuales con líneas en blanco. Mantener las firmas largas en varias líneas, agrupando argumentos relacionados.
- Usar encabezados de sección y subsección al estilo original: `//===UTILIDADES===`, `//===NEURAL FUNCTIONS===`, `//===PARAMETER UPDATE FUNCTIONS===`, `/*============Forward============*/`, `/*============Backward============*/`, `/*============Loss calculation============*/`.
- Nombres de funciones y variables descriptivos, normalmente en inglés y camelCase (por ejemplo, `initWeights`, `progressBar`, `train`, `validationProcedure`, `saveModelTxt`). Respetar la notación matemática cuando sea más expresiva: `V`, `q`, `c`, `eta`, `h`, `omega_h`, `lr`, `tau`, `i`, `j`; sufijos `Cache` para estados guardados y prefijos de gradiente/momentos como `dV`, `dQ`, `mW`, `vW`.
- Comentarios en español o inglés, naturales y útiles: explicar por qué se hace una operación, índices, dimensiones, convenciones matemáticas o advertencias. No llenar el código de comentarios redundantes, docstrings artificiales o documentación generada innecesaria.
- Mantener visibles las declaraciones de parámetros, momentos y cachés; usar constructores e inicializaciones comprensibles y agrupadas por significado. Evitar fragmentar el proyecto en multitud de archivos pequeños sin justificación.
- Hiperparámetros fácilmente localizables y modificables; configuración explícita y accesible. No esconder decisiones en constantes mágicas ni copiar valores de la CNN como si estuvieran aprobados para DRANN.

Ejemplo exclusivamente estilístico (no establece nuevas decisiones de arquitectura):

```cpp
//===UTILIDADES===
struct utils
{
    static inline double vecDot(const std::vector<double>& v,
        const std::vector<double>& u)
    {
        double dot = 0.0;
        for(int i = 0; i < v.size(); i++) {dot += v[i] * u[i];}

        return dot;
    }
};
```

## 4. Interacción, diagnósticos y persistencia

Cuando la tarea incluya estas funcionalidades, mantener la filosofía de la CNN:

- Terminal legible, mensajes directos con `std::cout` y errores con `std::cerr`. Colores ANSI sencillos mediante una utilidad equivalente a `setColor`: rojo para errores, amarillo para advertencias o espera, gris para información auxiliar, verde para éxito, blanco para mensajes generales; restablecer el color.
- Barras de progreso sencillas con `#` y `.`, porcentaje, tiempo restante y duración de procesos mediante `std::chrono`; evitar saturar la terminal.
- Comprobar dimensiones, apertura y lectura de archivos, valores inválidos y coherencia de estados antes de operar. No ocultar errores ni continuar con datos corruptos.
- Preferir persistencia inspeccionable en texto, mediante `std::ifstream` y `std::ofstream`, cabeceras o firmas de formato, dimensiones explícitas y `std::locale::classic()`. Mantener precisión suficiente para recuperar un `double` (p. ej., `max_digits10`), no copiar el redondeo de la CNN.
- Si se implementan checkpoints o recuperación, guardar los parámetros, estados del optimizador y demás información necesaria para reanudar coherentemente. Distinguir una pausa recuperable de la finalización definitiva y de la ablación. Validar la carga antes de sustituir el estado actual.
- Mantener resultados experimentales accesibles en archivos de texto tabulares y registros de configuración, si corresponde. No denominar `.ods` a un archivo de texto delimitado.
- Los detalles concretos de interfaz, formato de persistencia y recuperación todavía deben ajustarse a las decisiones aprobadas para DRANN. La referencia de la CNN no autoriza automáticamente nuevas funciones.

## 5. Respeto a la teoría y al alcance acordado

Antes de escribir código, leer [la teoría consolidada](docs/DRANN_TEORIA_CONSOLIDADA_2026-11-21.md). A fecha de esta guía, las secciones 23–30 describen el prototipo mínimo aprobado y prevalecen sobre las formulaciones históricas incompatibles. Consultar cualquier acuerdo posterior que figure en el repositorio.

- La estética del código anterior no sustituye la especificación matemática de DRANN. Usar `double` para sus cálculos, aunque la CNN utilice `float`.
- No trasladar convoluciones, activaciones, pérdidas de cuaterniones, data augmentation, weight decay, paciencia, modificación automática de tasas de aprendizaje, ni otras decisiones particulares de la CNN.
- No usar LogSumExp: el autor ha elegido la evaluación directa de KDE. Monitorizar los problemas numéricos sin introducir una corrección no aprobada.
- No implementar ideas pospuestas ni añadir regularizaciones, mecanismos de reactivación, nuevas ecuaciones o hiperparámetros de diseño por iniciativa propia.
- No confundir una preferencia estética con la obligación de reproducir errores, código muerto, comprobaciones defectuosas o duplicaciones accidentales del ejemplo. Corregir problemas de implementación sin alterar las ecuaciones aprobadas; si la corrección requiere una decisión matemática nueva, consultarla.
- Si surge una decisión pendiente o una ambigüedad que cambia el diseño, detener únicamente esa parte, explicar las alternativas y solicitar aprobación al autor antes de adoptarla. Continuar el trabajo independiente que sí esté definido. No hacer preguntas sobre decisiones ya aprobadas.
- No modificar la teoría para justificar cambios de código. No introducir mejoras no solicitadas ni ampliar el alcance. Distinguir las decisiones aprobadas de las propuestas.

## 6. Verificación y entrega

- Trabajar incrementalmente. Verificar el forward y el backward analítico (mediante diferencias finitas en topología fija), antes de interpretar resultados de entrenamiento o reorganización estructural.
- Comprobar los invariantes matemáticos y los casos límite que correspondan a la tarea. No presentar resultados de pruebas que no se hayan ejecutado.
- Al terminar, indicar qué archivos se modificaron, qué pruebas se ejecutaron, qué problemas se encontraron y qué decisiones siguen pendientes. Mantener las explicaciones concisas y directas.
- Antes de cualquier entrega, comprobar que el código sigue siendo legible para el autor sin necesitar conocer un framework o una arquitectura de software ajenos.

**Regla de revisión:** si el autor no puede seguir una ecuación de DRANN leyendo directamente su implementación en C++, simplificar el código antes de añadir más mecanismos.
