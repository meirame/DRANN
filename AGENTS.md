# Instrucciones para trabajar en DRANN

## Contexto y alcance

- Leer `docs/DRANN.md` antes de implementar o modificar el modelo. Distingue acuerdos, propuestas y detalles pendientes.
- La primera implementación está en `DRANN.cpp`; `tests/DRANN_tests.cpp` incluye ese archivo con `DRANN_NO_MAIN` para comprobar el mismo núcleo sin duplicarlo. Leer también el README y `docs/experiment-v1.md` para compilar y conocer los resultados observados. Las solicitudes claras de implementación autorizan avanzar dentro del alcance acordado, sin añadir una aprobación adicional por este archivo.
- El núcleo se implementará desde cero en C++, incluidos forward, backward y actualización de parámetros. No usar PyTorch, LibTorch ni otro framework de redes neuronales o autodiferenciación.
- Primero obtener un ejecutable de consola. Godot es secundario y se integrará después; no añadir ahora GDExtension ni dependencias de Godot.
- `run_drann.sh` compila y ejecuta el núcleo, y después abre `plot_loss.py` con Matplotlib. Python se usa solo para gráficas, no para el modelo ni el entrenamiento. Preservar los cambios locales de configuración en `DRANN.cpp`; el lanzador no modifica ese archivo.
- No introducir cambios matemáticos de forma implícita. Documentar las decisiones nuevas y distinguirlas de lo que ya pidió el usuario. Resolver las elecciones rutinarias de implementación con criterio, sin convertirlas en permisos innecesarios.

## Estilo solicitado

La referencia del usuario es `docs/reference/XOR.cpp`, conservada sin modificaciones. Es una referencia de estilo, no una implementación de la DRANN ni una especificación de sus matemáticas.

- Empezar con un `.cpp` sencillo, `struct`, métodos explícitos, `std::vector`, `double` y bucles legibles. Evitar una infraestructura general de tensores/autodiferenciación para este experimento escalar.
- Indentación de dos espacios, llaves de apertura en la misma línea, `std::` explícito, comentarios explicativos en español y nombres principalmente en inglés.
- Conservar separadores como `/*=======FORWARD=======*/` y una organización reconocible: funciones matemáticas, parámetros y cachés, inicialización, forward, pérdida, backward, actualización, entrenamiento, validación y `main()`.
- Tomar como guía nombres como `initWeights`, `loss`, `lossDerivative`, `parameterUpdate`, `train` y `validation`, adaptándolos cuando corresponda. No conservar nombres de capas o de una red densa si ya no representan la DRANN.
- Mantener un `main()` legible con dataset y configuración, y mensajes por terminal para seguir el experimento.
- No copiar limitaciones o elementos específicos del XOR: activación ReLU, inicialización normal de He, targets enteros, umbral de clasificación, tamaños de capas, argumentos sin uso o código comentado obsoleto.

## Reglas del modelo que no deben perderse

- Notación: época `e`, ejemplo `ej`, paso interno `tau`, paso terminal `t`, entrada `x`, peso `w`, sesgo `b`, activación `sigma` y tasa de aprendizaje `lr`.
- Pesos internos simétricos: un único parámetro por pareja, sin autoconexiones. Inicialización positiva en `(0, 1)` y sesgos inicialmente cero. Activación identidad para la primera versión.
- Selección simultánea por máximos locales sobre las neuronas disponibles y conexiones internas habilitadas. No seleccionar por máximo global ni desempatar mediante pesos o identificadores.
- Una neurona no puede transmitir y recibir en el mismo paso. Sumar las contribuciones recibidas y aplicar sesgo/activación una sola vez por receptora y paso.
- Diferencia de potencial: potencial superior menos inferior. No dividir la señal como un caudal, no descontar automáticamente de la transmisora lo recibido y no introducir `eta` en el forward.
- Una transmisora conserva su potencial y se retira hasta terminar ese ejemplo. Sus operaciones siguen participando en el backward y su potencial en el cognifenómeno primitivo.
- Una conexión interna cuyo peso actualizado sea nulo o negativo queda anulada permanentemente durante ese entrenamiento, en ambas direcciones, y deja de actualizarse. No reintegrar conexiones ni introducir gradientes sustitutos en esta primera versión.
- El aislamiento estructural excluye una neurona del cognifenómeno primitivo; no confundirlo con quedarse sin vecinas disponibles por retirada temporal. El tratamiento del aislamiento total está fuera del alcance de v1: no inventar una recuperación, un acierto o una pérdida cero para ese caso.
- No aprender una condición de parada: terminar cuando no haya transmisoras locales válidas. El cognifenómeno primitivo es un conjunto de puntos con multiplicidad, no una neurona de salida designada.
- Actualizar después de cada ejemplo. Validar después de cada época sin modificar parámetros ni conexiones.

## Comprobaciones al implementar

- Comprobar el forward con ejemplos pequeños calculados a mano: transmisoras locales simultáneas, receptora compartida, sesgo una sola vez por paso, retirada y terminación.
- Verificar la simetría y la anulación irreversible de conexiones, así como la exclusión de neuronas aisladas y la inclusión de las retiradas en la pérdida.
- Verificar el backward manual mediante diferencias finitas en casos alejados de umbrales y empates; las perturbaciones deben conservar el recorrido, las máscaras y la participación. No exigir derivadas ordinarias en discontinuidades.
- Acumular todos los gradientes antes de modificar parámetros. Recordar que el mismo sesgo aparece en la inicialización y en cada recepción.
- Comprobar que la evaluación no cambia pesos, sesgos ni máscaras de anulación. Informar de las pruebas realmente ejecutadas y de las limitaciones observadas.

## Continuidad y GitHub

- Actualizar la especificación cuando el usuario cambie el diseño. No reintroducir propuestas descartadas como si siguieran aprobadas.
- El repositorio privado es `meirame/DRANN`. Trabajar mediante ramas y pull requests según el flujo acordado; no fusionar sin una instrucción que lo autorice.
- La carpeta local del usuario es `/home/luis/personal/DRANN`. Un chat remoto puede ejecutarse en `/workspace`; no fingir acceso al disco local ni crear esa ruta aquí como sustituto.
- Git sincroniza archivos, no el historial literal de los chats. Estos documentos sirven como contexto portable.
