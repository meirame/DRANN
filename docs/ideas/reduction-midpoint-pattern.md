# Primer compensador: reducción de una neurona intermedia

Fecha: 18 de noviembre de 2026.

Estado: desarrollo matemático y especificación para implementar localmente. El usuario ha aceptado empezar por patrones algebraicos reconocidos, saltar los no resueltos y disponer de recuperación limitada con restauración. No se ha implementado este controlador en DRANN.cpp.

Contexto: [reducción por estancamiento](reduction-on-plateau.md) y [lectura vectorial alpha/beta](../experiment-celsius-fahrenheit-kelvin.md).

## 1. Selección de candidatas

Indicadores observados sobre entrenamiento, sin modificar el forward:

- Redundancia geométrica: cercanía de q_i a las posiciones de otras participantes. Puede resumirse como media por ejemplo de la menor distancia cuadrática a otra neurona, en coordenadas normalizadas.
- Fuerza total: S_i = suma de pesos internos habilitados incidentes.
- Conexión más fuerte: M_i = máximo de esos pesos. M_i pequeño indica que todas sus conexiones son débiles.
- Actividad efectiva: media por ejemplo de la suma de |w[j,i] * deltaV[j,i]| de las emisiones de i. Registrar también recepciones y frecuencia de transmisión como diagnóstico.

El usuario acepta combinar indicios sin exigirlos todos. Un peso bajo no garantiza poca influencia: intervienen la diferencia de potencial y la selección local de transmisoras.

Propuesta operativa inicial: formar la unión de las primeras candidatas de cada indicador y ordenar esa unión mediante rangos normalizados, con criterio determinista para empates. Número de candidatas y combinación exacta de rangos son configurables pendientes. No sumar directamente magnitudes de distintas escalas. El identificador solo desempata esta clasificación; no modifica los turnos del forward.

Una neurona con conexiones fuertes puede entrar por redundancia geométrica. La clasificación prioriza intentos; no demuestra que la eliminación sea válida.

## 2. Patrón reconocido

Considerar una componente estructural de exactamente tres participantes A, B y C. Puede haber otras componentes desconectadas; no debe haber conexiones habilitadas desde estas tres hacia otras participantes.

Condiciones matemáticas:

1. Activación interna identidad.
2. Sesgos escalares b_A = b_B = b_C = 0.
3. Pesos simétricos w_AB = w_BC = w, con 0 < w < 1.
4. Peso A–C igual a u, con 0 <= u <= 1; u=0 representa ausencia de arista en la derivación.
5. Para toda entrada, V_B^0 = (V_A^0 + V_C^0)/2.
6. Todas las neuronas comienzan el ejemplo disponibles; se usan las reglas locales simultáneas, agregación y retirada de v1.

La condición 5 debe comprobarse sobre la función de entrada, no únicamente sobre unos potenciales coincidentes en un ejemplo. Con entrada afín e identidad, requiere que cada peso de entrada de B sea la media de los correspondientes de A y C, además de los sesgos nulos exigidos aquí.

Las igualdades matemáticas son exactas. En punto flotante se utilizarán tolerancias absolutas y relativas de verificación y se registrarán los residuos. Una coincidencia aproximada no debe anunciarse como equivalencia algebraica exacta ni justificarse modificando silenciosamente los parámetros para que encajen.

Para la primera aplicación al modelo vigente, exigir además que A–C esté actualmente habilitada y u>0. No reabrir una arista anulada permanentemente. La derivación u=0 sirve de referencia matemática; crear una conexión en una transformación futura requiere definir expresamente esa extensión.

Si faltan condiciones, devolver un motivo de patrón no soportado; no eliminar B por su puntuación.

## 3. Fórmula de compensación

Definir:

~~~text
h = (1 + w) / 2
r = u + w * max(0, h - u)
~~~

Transformación:

- Conservar los pesos de entrada, sesgos y alpha/beta de A y C.
- Sustituir el único parámetro simétrico A–C por r.
- Deshabilitar las dos conexiones incidentes de B; B queda aislada y excluida.
- No cambiar las demás componentes ni renumerar innecesariamente las identidades.

r es positivo y no supera 1 bajo estas condiciones. Las dos supervivientes siguen conectadas.

### Demostración para V_A^0 > V_C^0

Escribir D = V_A^0 - V_C^0 > 0. B empieza entre ambos extremos, así que A transmite primero.

Tras la emisión de A:

~~~text
V_B^1 = V_C^0 + h * D
V_C^1 = V_C^0 + u * D
~~~

Hay tres posibilidades:

- u<h: B transmite a C. Su resultado es V_C^final = V_C^0 + [u + w*(h-u)]*D.
- u>h: C transmite a B y conserva su propio potencial V_C^0 + u*D.
- u=h: B y C empatan y no transmiten entre sí; C conserva V_C^0 + u*D.

Las tres se reúnen en:

~~~text
V_C^final = V_C^0 + r * (V_A^0 - V_C^0)
~~~

A conserva su potencial inicial. Esto coincide con el forward de las dos neuronas unidas por r.

Si V_C^0 > V_A^0, se intercambian los papeles de los extremos y se obtiene el mismo r, porque el patrón es simétrico. Si ambos son iguales, B también lo es y no hay transmisión en ninguna versión.

Por tanto se conservan exactamente los potenciales finales de A y C para todas las entradas que cumplan las condiciones, incluso al cambiar la dirección de transmisión. Cambian el número de pasos y la traza; no se afirma que ambas ejecuciones tengan el mismo recorrido.

## 4. Ejemplos

Potenciales iniciales A=10, B=6, C=2 y w=0.5:

| Peso previo u | Peso equivalente r | Final con B, orden A/B/C | Final sin B, orden A/C |
| --- | --- | --- | --- |
| 0 | 0.375 | (10,8,5) | (10,5) |
| 0.5 | 0.625 | (10,8,7) | (10,7) |
| 0.9 | 0.9 | (10,8.6,9.2) | (10,9.2) |

El primer caso ilustra la derivación sin arista directa; no habilita reintegración en v1. El tercero muestra que B puede dejar de influir sobre los extremos aunque reciba señales y su potencial cambie.

## 5. Alcance de la conservación

La garantía se refiere a las supervivientes, no a la igualdad de las nubes con multiplicidad.

Con la lectura local q_i = sigma_geom(alpha_i * V_i^final + beta_i), mantener alpha y beta conserva también q_A y q_C. La posición de B desaparece y el número de participantes cambia. La pérdida media y la dispersión pueden cambiar aunque las otras posiciones permanezcan idénticas.

Comprobar el criterio geométrico de la tarea y medir errores por coordenada antes de aceptar. No asumir que V_B intermedio implica q_B intermedio cuando alpha/beta difieren.

Un grafo completo de cinco o diez neuronas no cumple directamente esta estructura. Los sesgos aprendidos tampoco serán generalmente nulos y las relaciones entre pesos no aparecerán necesariamente. El primer compensador puede no encontrar ningún caso aplicable en una ejecución real. No forzar el patrón ni anunciar poda general; ampliar después la biblioteca de patrones.

## 6. Contratos propuestos para el código local

~~~text
collectCandidateStats(training_examples)
rankCandidates(stats)
matchReductionPattern(model, candidate)
buildReductionPlan(model, match)
applyReductionPlan(trial_model, plan)
verifySurvivorStates(reference_model, trial_model, training_examples)
recoverAndEvaluate(trial_model, budget)
acceptOrRestore(reference_model, trial_model, metrics)
~~~

El plan debe incluir identidades, parámetros afectados, residuos de condiciones, fórmula usada y motivos de rechazo. No mutar el modelo mientras se selecciona o se construye el plan.

El verificador usa el forward real de ambas estructuras. Contrastar los potenciales de supervivientes y, si existe lectura vectorial, sus posiciones. Las referencias para compensación se obtienen con entradas de entrenamiento; no usar objetivos de validación para backprop.

Posibles salidas explícitas: patrón no soportado, verificación fallida, fallo numérico, no recupera precisión, reducción aceptada. Saltar un patrón no soportado es una operación normal, no un error ni una eliminación parcial.

## 7. Recuperación y decisión

Ciclo aceptado por el usuario:

~~~text
entrenar normalmente
si hay estancamiento:
  guardar checkpoint anterior al intento
  recorrer candidatas, con presupuesto limitado:
    reconocer patrón y construir plan
    si no está soportado: registrar y continuar
    aplicar en copia y verificar
    si falla: descartar copia y continuar
    evaluar inmediatamente y guardar el estado válido de época cero
    entrenar la copia durante hasta recovery_epochs
    conservar su mejor checkpoint admisible, incluido el inicial
    aceptar si satisface los límites de precisión
    en otro caso restaurar el estado anterior
~~~

La recuperación es una oportunidad limitada, no una obligación de empeorar una compensación que ya era suficiente. Mantener el candidato de época cero evita perder una reducción válida si el entrenamiento posterior se vuelve ruidoso.

Para comparar, registrar pérdidas de entrenamiento/validación, errores por coordenada, peor error de punto y tamaño antes y después. Tolerancias absoluta y relativa, paciencia, mejora mínima y presupuesto de recuperación son configurables por fijar. Documentar los límites concretos antes de la ejecución.

El mejor modelo predictivo global según validación y el modelo anterior al intento tienen funciones distintas. Guardar parámetros, máscaras, identidades, configuración, estado del optimizador y aleatoriedad cuando corresponda. La restauración debe ser completa, incluso si la recuperación anuló más conexiones. No define reintegración dentro del entrenamiento ordinario.

Si aparece un cognifenómeno vacío o un valor no finito, descartar el intento y restaurar; no asignar pérdida cero ni inventar un tratamiento de aislamiento total.

Memorizar intentos fallidos con identidad de candidata y revisión del modelo. Después de restaurar, no repetir inmediatamente el mismo intento; volver a considerarlo solo tras una mejora significativa del entrenamiento o un cambio estructural relevante, según política configurada.

## 8. Comprobación numérica de la derivación

Se ejecutó en esta conversación un simulador independiente y pequeño, en JavaScript dentro de la herramienta de cálculo. No utiliza ni verifica el ejecutable C++ y no es un experimento de aprendizaje.

- 12.000 casos con extremos aleatorios en [-100,100], B como su media y w en [0.02,0.98).
- Para u se alternaron ausencia de arista, umbral h y valores aleatorios en [0,1).
- Generador reproducible: estado inicial 42, actualización de enteros sin signo s=(1664525*s+1013904223) modulo 2^32; muestra s/2^32.
- Nueve casos adicionales con A=B=C=7.
- Comparación de ambos potenciales supervivientes con umbral 1e-10.
- Casos totales: 12009; fallos: 0; máximo error absoluto observado: 5.684341886080802e-14.

La derivación proporciona la garantía bajo sus supuestos; el muestreo sirve para detectar errores de signo, selección y ramas, no para demostrarla por sí solo.

## 9. Comprobaciones pendientes al implementar en C++

- Reproducir los ejemplos y los casos de dirección invertida, empates, u<h, u=h y u>h.
- Rechazar conexiones externas, sesgos no soportados, pesos desiguales, incumplimiento de la relación de entrada y reapertura de conexiones anuladas.
- Probar conservación de alpha/beta y posiciones de supervivientes.
- Probar que una pérdida menor por cambiar la nube no se anuncia como igualdad de nubes.
- Verificar restauración completa, presupuesto de recuperación, candidato inicial de época cero, conservación del mejor modelo y bloqueo de reintentos inmediatos.
- Comprobar que las estadísticas y la validación no cambian el modelo.
- Mantener separados este controlador futuro y las pruebas del backward del modelo vigente.

No hay aquí parámetros de recuperación elegidos ni resultados de poda de la DRANN real.
