# Especificación de DRANN, primera versión

Estado: primera implementación en `DRANN.cpp`, con pruebas en `tests/DRANN_tests.cpp`. Esta especificación recoge las decisiones de la conversación hasta el 17 de noviembre de 2026 y distingue acuerdos de elecciones de implementación. Los resultados iniciales están en [experiment-v1.md](experiment-v1.md).

## 1. Propósito y alcance

DRANN es el nombre de trabajo de una red neuronal artificial rizomática diferencial. Los nodos se interpretan como neuronas. Una posible interpretación social es una exploración futura, no el objetivo actual.

La predicción debe describir el estado colectivo final del rizoma. No se designa una neurona portavoz ni se fuerza una distribución mínima de la actividad. El modelo puede concentrar su solución si las reglas lo permiten; con la definición actual de participación, una neurona sin conexiones internas habilitadas queda excluida.

Primero se desarrollará desde cero un ejecutable de consola en C++, incluido su entrenamiento. Godot es secundario y se abordará cuando el código ejecutable funcione. GDExtension fue recomendada como integración futura, no como dependencia del primer programa.

La representación inicial no requiere topología avanzada ni coordenadas físicas tridimensionales. Distintas organizaciones internas pueden producir el mismo cognifenómeno primitivo. Las posiciones gráficas futuras no modifican por sí solas las matemáticas.

## 2. Notación y parámetros

| Símbolo | Significado |
| --- | --- |
| `e` | Época: recorrido completo del dataset de entrenamiento |
| `ej` | Índice de un ejemplo entrada–salida |
| `tau` / `τ` | Paso interno de un forward |
| `t` | Último paso alcanzado para ese ejemplo; `τ = 0, ..., t` |
| `x` | Entrada del ejemplo |
| `y` | Resultado correcto, transformado al espacio del cognifenómeno ideal |
| `w` | Peso; el primer índice interno indica el destino |
| `b` | Sesgo de una neurona |
| `sigma` / `σ` | Activación, identidad en v1: `σ(z) = z` |
| `lr` | Tasa de aprendizaje, ajena a la dinámica de transmisión |

Todos los inputs de un fenómeno se conectan inicialmente a todas las neuronas. El experimento inicial solo tiene una entrada escalar.

Los pesos internos son simétricos: `w[j,i] = w[i,j]`, con un único parámetro por pareja de neuronas distintas. No hay autoconexiones. El rizoma empieza completamente interconectado. Con cinco neuronas hay diez parámetros internos, cinco pesos de entrada y cinco sesgos: veinte parámetros iniciales.

Los pesos se inicializan aleatoriamente en `(0, 1)`, con media teórica `0.5`; no es necesario que la media de una muestra concreta sea exactamente esa. Los sesgos se inicializan a cero y son aprendibles. El intervalo de pesos es una inicialización, no un límite superior durante el entrenamiento.

## 3. Estados y participación

Hay dos distinciones independientes:

- **Anulación estructural:** una conexión interna se habilita mientras no haya sido anulada y tenga peso positivo. Si su peso actualizado queda nulo o negativo, se anula permanentemente durante ese entrenamiento. No se reintegra ni se actualiza después. La anulación afecta a ambas direcciones del parámetro compartido.
- **Disponibilidad temporal:** durante un ejemplo, una neurona puede estar disponible o retirada por haber transmitido. Al comenzar otro ejemplo se reinicia la disponibilidad temporal.

Una neurona está aislada si no conserva ninguna conexión interna estructuralmente habilitada. No contribuye al cognifenómeno primitivo. Tener únicamente vecinas ya retiradas no equivale a estar estructuralmente aislada.

El conjunto `A_ej` de neuronas participantes se determina con las conexiones estructuralmente habilitadas para ese ejemplo. Incluye las neuronas que se retiren durante el forward. Las anulaciones se aplican tras la actualización de parámetros, no en medio de un forward o de su backward.

El tratamiento del aislamiento total fue excluido expresamente de v1. Una pérdida sobre un conjunto vacío no está definida por esta especificación: no presentarla como acierto ni introducir por iniciativa propia un mecanismo de recuperación.

## 4. Forward

### 4.1 Entrada

Para cada neurona participante del experimento escalar:

```text
V[i,0] = sigma(w_input[i] * x + b[i])
```

La entrada fija el estado inicial; no se ha añadido una fuente externa persistente durante los pasos internos. El mismo sesgo `b[i]` se reutiliza en la entrada y en las recepciones internas.

### 4.2 Selección local simultánea

En cada paso se usa exclusivamente el estado anterior a la actualización. Sea `U_tau` el conjunto de neuronas participantes todavía disponibles. Las vecinas de `i` son las neuronas de `U_tau` conectadas a `i` mediante un peso interno habilitado y positivo.

Una neurona de `U_tau` es transmisora si:

1. No tiene ninguna vecina disponible de potencial superior al suyo.
2. Tiene al menos una vecina disponible de potencial inferior al suyo.

Se seleccionan todas las transmisoras locales simultáneamente, aunque sus potenciales sean distintos. Las neuronas sin conexión activa directa no se bloquean entre sí mediante una comparación global. Los empates de potencial no generan transmisión entre las empatadas; varias empatadas sí pueden transmitir a vecinas inferiores.

No hay prioridad global, clasificación por pesos ni desempates por identificador. Los pesos deciden habilitación e intensidad de influencia, no turnos.

Cada transmisora envía a todas sus vecinas disponibles de menor potencial. Una neurona no puede ser transmisora y receptora en el mismo paso: una receptora tiene una vecina de mayor potencial y, por tanto, no cumple la condición de transmisora.

### 4.3 Transmisión y actualización

Para una transmisora `i` y su receptora `j`:

```text
deltaV[j,i,tau] = V[i,tau] - V[j,tau] > 0
```

Es la diferencia entre el potencial superior y el inferior, sin incluir el peso. Si `T_j` reúne las transmisoras que envían a `j` en ese paso:

```text
V[j,tau+1] = sigma(V[j,tau]
                  + sum(i in T_j, w[j,i] * deltaV[j,i,tau])
                  + b[j])
```

Solo se aplica a receptoras con al menos una señal. Todas las contribuciones se calculan con los potenciales previos; el sesgo y la activación se aplican una sola vez por receptora y paso, no por conexión recibida.

Las transmisoras conservan su potencial y se retiran: no vuelven a emitir ni a recibir en ese ejemplo. Las demás neuronas sin recepción conservan su potencial y siguen disponibles. Se recalculan los papeles en el siguiente paso; una receptora puede convertirse entonces en transmisora.

La señal no se reparte como un caudal entre destinatarias, no se descuenta de la emisora lo recibido y no se introduce `eta`. Esta dinámica no exige conservar una suma de potenciales ni es un circuito pasivo biológicamente fiel. La activación y el sesgo pueden permitir que una receptora supere a otras neuronas.

### 4.4 Terminación

El forward termina cuando no queda ninguna transmisora local válida. No es necesario que todas las neuronas hayan transmitido. No hay condición de parada aprendida. Cada paso productivo retira al menos una neurona, por lo que el número de pasos productivos está acotado por el número inicial de participantes.

## 5. Cognifenómenos y pérdida

El **cognifenómeno primitivo** del caso escalar es el conjunto de puntos con multiplicidad formado por los últimos potenciales de las neuronas de `A_ej`. Cuenta cada neurona participante por igual, incluidas las retiradas. Si varios potenciales coinciden, sus aportaciones no se deduplican.

Depende del estado final, no de una lectura de toda la historia. La historia se conserva como caché para entrenar y, más adelante, visualizar.

El **cognifenómeno ideal** es un punto en `y`, obtenido del output del dataset mediante una transformación fija. La pérdida geométrica acordada es:

```text
L_ej = sum(j in A_ej, (V_last[j] - y_ej)^2) / size(A_ej)
```

Penaliza tanto la posición incorrecta como la dispersión de los puntos respecto al ideal. No equivale a calcular únicamente el error de su promedio. Una media para presentar una predicción numérica fue propuesta como lectura auxiliar, pero no sustituye esta pérdida ni se ha confirmado como interfaz de salida.

## 6. Backward y actualización

La primera versión usa derivadas del recorrido ejecutado, manteniendo fijas las decisiones de papeles, conexiones habilitadas, retirada y pertenencia al cognifenómeno. No explora forwards alternativos ni calcula derivadas de cambios de recorrido. No emplea gradientes sustitutos ni intenta reintegrar conexiones.

Inicializar los gradientes finales:

```text
g_last[j] = 2 * (V_last[j] - y_ej) / size(A_ej)   para j participante
```

Los demás gradientes comienzan en cero. Recorrer la traza en orden inverso, conservando los valores previos a cada actualización. Para una receptora `j`, sea `delta_j` el gradiente respecto a su potencial después de ese paso:

```text
dW[j,i] += delta_j * deltaV[j,i,tau]     para cada i que transmitió a j
db[j]   += delta_j                     una sola vez por recepción agregada
```

Las derivadas respecto a los estados previos son:

```text
d V_next[j] / d V_prev[j] = 1 - sum(i in T_j, w[j,i])
d V_next[j] / d V_prev[i] = w[j,i]
```

Los estados no actualizados tienen paso identidad. Una transmisora puede acumular tanto el gradiente de su estado conservado como las contribuciones que le llegan desde las receptoras. Calcular estas sumas en buffers separados; no modificar gradientes que todavía deban leerse para otras contribuciones del mismo paso.

Al llegar a la entrada:

```text
dW_input[i] = g_initial[i] * x_ej
db[i]      += g_initial[i]
```

El gradiente del sesgo suma su uso inicial y sus usos posteriores. Cada peso interno acumula sus aportaciones sobre un único parámetro simétrico. La retirada temporal no corta la cadena de gradientes.

Después de calcular todos los gradientes del ejemplo:

```text
theta_new = theta_old - lr * dL_ej/dtheta
```

Aplicar a los parámetros aprendibles que sigan habilitados y después anular permanentemente las conexiones internas cuyos pesos queden nulos o negativos. No actualizar parámetros durante el recorrido inverso.

La secuencia por ejemplo es: **forward → pérdida → backward → actualización → anulación**. No se espera a completar la época para actualizar.

## 7. Primer experimento

Configuración acordada:

| Elemento | Valor |
| --- | --- |
| Neuronas | 5 |
| Relación del dataset | `F = 1.8 * C + 32` |
| Ejemplos totales | 10 |
| Entrenamiento | 8 ejemplos |
| Validación | 2 ejemplos |
| `lr` | `0.005`, fijo |
| Épocas | 100 |
| Normalización fija | `x = C / 100`, `y = F / 100` |
| Desnormalización para mostrar Fahrenheit | multiplicar por `100` |

La relación normalizada es `y = 1.8*x + 0.32`. Esta fórmula genera las etiquetas; no se introduce como predictor en la DRANN.

Después de cada época, volver a evaluar los ocho ejemplos de entrenamiento y los dos de validación con los mismos parámetros finales de esa época. La evaluación no modifica pesos, sesgos ni anulaciones. Así se comparan errores del mismo modelo, en vez de confundirlos con el promedio de pérdidas obtenidas mientras el modelo iba cambiando durante el entrenamiento.

Registrar las curvas para observar posibles indicios de sobreajuste. Dos ejemplos de validación no permiten determinar con precisión cuándo deja de generalizar. Mantener las cien épocas; no se ha acordado parada temprana ni reducción automática de `lr`.

Temperaturas utilizadas como elección inicial de implementación, a partir de la propuesta de la conversación:

- Entrenamiento Celsius: `{-40, -20, 0, 20, 40, 60, 80, 100}`.
- Validación Celsius: `{10, 70}`.

El usuario confirmó la normalización, pero no seleccionó explícitamente esas diez temperaturas. Se utilizan para concretar el experimento autorizado. La implementación emplea semilla `42` y orden fijo, sin barajado. Son elecciones reproducibles, visibles en `main()`, no requisitos matemáticos del modelo.

## 8. Referencia de implementación y estado del trabajo

La referencia estética y estructural es [XOR.cpp](reference/XOR.cpp), proporcionada por el usuario y conservada sin modificaciones. No ejecutar su `main()` como si fuera la DRANN ni trasladar sus capas, activaciones, distribución de inicialización o clasificación binaria al nuevo modelo.

Se busca una implementación manual legible: `struct`, vectores, cachés, bucles, funciones de pérdida y derivadas explícitas, métodos separados y configuración sencilla en `main()`. Las instrucciones de estilo y comprobación están en [AGENTS.md](../AGENTS.md).

La implementación comienza con un único `.cpp` de producción y utiliza C++17 y la biblioteca estándar. Se compila directamente con `g++`, sin un sistema de construcción adicional. El historial se guarda en `training_history.csv`: evaluación inicial (época 0) y cien evaluaciones posteriores. La salida por terminal muestra los puntos individuales del cognifenómeno primitivo de validación en Fahrenheit; no introduce un promedio como nueva definición de predicción.

El usuario aclaró que desea gráficas nativas C++ durante el entrenamiento. `run_drann.sh` compila mediante CMake y abre `drann_gui`, con ImPlot, Dear ImGui y GLFW/OpenGL. La función de entrenamiento notifica la evaluación inicial y cada época completada; la ventana representa ambos errores, permite pausa, escala logarítmica y agrupación de épocas por fotograma. El modelo sigue siendo C++ manual y la consola no necesita dependencias gráficas. El callback no modifica el cálculo y puede solicitar la cancelación entre épocas; cerrar la ventana conserva el CSV hasta la última evaluación. La visualización Python previa fue sustituida. El número de épocas puede elegirse con `--epochs 1000` sin modificar la configuración del archivo.

La regla irreversible se confirmó explícitamente para las conexiones internas simétricas. En esta implementación no se extiende a los pesos de entrada: son parámetros ordinarios del gradiente y pueden cambiar de signo. Esta decisión de alcance está documentada para que no se confunda con un recorte permanente de todos los parámetros.

El programa no define una recuperación para el aislamiento total. Si encuentra un cognifenómeno vacío, termina con un error explícito en lugar de dividir entre cero, devolver un acierto o inventar una pérdida. Se comprueba esta frontera sin añadir un tratamiento matemático al modelo.

## 9. Propuestas reemplazadas o descartadas para v1

- Neuronas que transmiten repetidamente: reemplazadas por retirada tras transmitir.
- Parada aprendida y penalización por pasos para entrenarla: reemplazadas por terminación local.
- Selección de una única transmisora global, prioridad por el mayor peso y desempates por identificador: reemplazadas por máximos locales simultáneos.
- Pesos internos dirigidos independientes: reemplazados por parámetros simétricos.
- Transferencia conservativa con disminución de la emisora y factor `eta`: no forman parte de la actualización acordada.
- Sesgo por cada señal individual: reemplazado por una aplicación tras sumar las señales del paso.
- Reintegración, presión de apertura, gradientes sustitutos y forwards contrafactuales: fuera de la primera versión tras decidir la anulación irreversible.
- Topología avanzada, simulación social y Godot: posibles trabajos posteriores, no requisitos del primer ejecutable.
- Frameworks neuronales: el usuario pidió desarrollar la DRANN desde cero.

## 10. Continuidad entre ordenadores

Repositorio privado: `https://github.com/meirame/DRANN`. Carpeta preparada por el usuario: `/home/luis/personal/DRANN`.

Este chat remoto no tiene acceso directo a esa carpeta. El flujo acordado es trabajar aquí, publicar cambios en GitHub mediante ramas y pull requests, y descargar la rama correspondiente en el ordenador local. Un `pull` en `main` no incorpora cambios de una rama todavía no fusionada.

Estos archivos preservan el contexto del proyecto, no una copia literal ni una sincronización del historial de conversación. Al retomar desde otro chat, leer esta especificación y `AGENTS.md`, revisar el estado real del repositorio y continuar desde lo ya implementado.
