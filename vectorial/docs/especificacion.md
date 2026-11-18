# Encargo para Codex local: potencial vectorial y aislamiento estructural

Fecha: 18 de noviembre de 2026.

## Petición y alcance

Implementa una variante experimental de DRANN en el proyecto local `/home/luis/personal/DRANN`, conservando las versiones actuales de Celsius–Fahrenheit–Kelvin y seno/coseno. Implementa, compila, comprueba y entrega código ejecutable; no termines únicamente con otro plan.

El usuario ha elegido probar potencial vectorial, selección por módulo y transmisión por diferencia vectorial. También quiere un mecanismo que favorezca aislar neuronas. La regularización por grupos especificada aquí es la propuesta operativa para este segundo objetivo; sus parámetros iniciales son elecciones experimentales, no resultados ya validados.

Conserva el trabajo local, ramas, configuraciones e historiales. Lee las instrucciones aplicables, `AGENTS.md`, README y documentos de diseño disponibles. Inspecciona el código real: puede haber evolucionado desde el chat. No sustituyas cambios locales por versiones remotas antiguas. Trabaja en una variante identificable sin romper las anteriores. No fusionar PR sin autorización.

Mantén C++ desde cero, `struct`, `std::vector`, `double`, bucles y derivadas manuales, comentarios en español e indentación de dos espacios. No usar frameworks neuronales, autodiferenciación ni bibliotecas de optimización para sustituir el núcleo. Conserva la consola y la GUI C++ existentes. No añadir Godot ni Adam.

## 1. Arquitectura seleccionada

Cada participante i tiene un único estado vectorial `V[i]` de dimensión D, igual a la dimensión del objetivo. Para los dos experimentos iniciales, D=2.

No añadir el vector auxiliar h, la matriz de mensajes M ni una lectura externa compleja. En esta variante se prescinde de alpha/beta de salida: `q[i] = V_last[i]`. La implementación previa de alpha/beta queda conservada como referencia independiente.

### Inicialización del estado

```text
z_initial[i,k] = sum(r, w_input[i,k,r] * x[r]) + b[i,k]
V_initial[i,k] = sigma(z_initial[i,k])
```

Las tareas iniciales usan una sola entrada; admite su representación actual sin crear un sistema general de tensores. Cada componente dispone de sus propios pesos de entrada y sesgo. Inicialización experimental: pesos de entrada uniformes en (-1,1), sesgos cero y conexiones internas simétricas uniformes en (0,1). Registrar semilla y configuración.

Activación componente a componente:

```text
sigma(z) = z        si z >= 0
sigma(z) = 0.01*z   si z < 0
```

Usar Leaky ReLU en la inicialización y en las recepciones. Usar siempre el nombre sigma. Declarar su derivada y la convención elegida en cero; las diferencias finitas evitarán ese punto.

### Selección de transmisoras

Calcular `rho[i] = ||V[i]||_2` con los estados previos al paso. Una participante disponible transmite si no tiene vecinas disponibles habilitadas de mayor módulo y tiene alguna de menor módulo. Seleccionar todos los máximos locales simultáneamente.

Módulos iguales no transmiten entre sí. No desempatar por pesos o identificadores ni convertir la selección en máximo global. Detectar empates o casi empates para diagnóstico, sin introducir silenciosamente una tolerancia que cambie el forward.

### Recepción y retirada

```text
deltaV[j,i,k] = V_prev[i,k] - V_prev[j,k]
z_next[j,k] = V_prev[j,k]
              + sum(i in T_j, w[j,i] * deltaV[j,i,k])
              + b[j,k]
V_next[j,k] = sigma(z_next[j,k])
```

La diferencia ahora es vectorial: sus componentes pueden ser negativas aunque la emisora tenga mayor módulo. No sustituirla por la diferencia de módulos.

Todas las receptoras usan los estados previos; sumar primero y aplicar sesgo/activación una sola vez por receptora y paso. La emisora conserva su vector y se retira hasta terminar el ejemplo. Los demás estados sin recepción se conservan sin volver a aplicar sigma ni sesgo. Terminar cuando no haya transmisoras válidas. No introducir eta, reparto como caudal, conservación de la suma, emisores repetidos ni parada aprendida.

Un único parámetro por conexión simétrica y sin autoconexiones. Mantener las anulaciones permanentes por pesos internos actualizados nulos o negativos. No extender esa anulación a pesos de entrada ni sesgos.

Una neurona con todas sus conexiones internas anuladas está aislada y se excluye de la salida; una retirada temporal sigue incluida. Mantener las identidades aunque se aíslen neuronas. No reactivar conexiones durante entrenamiento; restaurar un checkpoint completo es una operación distinta.

## 2. Salida, pérdida y backward

Ideal: un punto ordenado y de dimensión D. Cognifenómeno primitivo: nube de los vectores finales de todas las participantes A, incluidas las retiradas. N es su número.

```text
L_data = sum(i in A, sum(k, (V_last[i,k] - y[k])^2)) / N
L_coordinate[k] = sum(i in A, (V_last[i,k] - y[k])^2) / N
g_last[i,k] = 2 * (V_last[i,k] - y[k]) / N
```

Sumar coordenadas, sin dividir adicionalmente por D. No ordenar componentes ni sustituir la nube por su media. No aplicar además la pérdida de alpha/beta de otro modelo.

Guardar las preactivaciones y vectores de cada paso. El backward mantiene fijos recorrido, máscaras, pertenencia y selección por módulos; no deriva las comparaciones. No necesita calcular el gradiente del módulo utilizado exclusivamente para ordenar.

Para una receptora j, con gradiente posterior `g_next[j]`:

```text
delta[j,k] = g_next[j,k] * sigma_derivative(z_next[j,k])
dW[j,i] += sum(k, delta[j,k] * (V_prev[i,k] - V_prev[j,k]))
db[j,k] += delta[j,k]                 una vez por recepción agregada

g_prev[j,k] += (1 - sum(i in T_j, w[j,i])) * delta[j,k]
g_prev[i,k] += w[j,i] * delta[j,k]    para cada emisora i
```

Los vectores no actualizados tienen paso identidad y acumulan gradientes indirectos. Usar buffers separados para no contar dos veces una receptora. Acumular sobre el único peso simétrico.

En la entrada aplicar la derivada de sigma, multiplicar por x para los pesos de entrada y sumar al sesgo su uso inicial. El mismo sesgo vectorial se usa al inicio y en las recepciones. Calcular todos los gradientes antes de actualizar.

Importante: D=1 con selección por valor absoluto no es, en general, equivalente al modelo escalar antiguo que ordenaba por potencial con signo. No exigir esa equivalencia en pruebas que incluyan negativos ni cambiar el modelo antiguo para simularla.

## 3. Mecanismo propuesto: regularización por grupos de conexiones

Sea E el conjunto de conexiones habilitadas y w el vector que contiene cada parámetro simétrico una sola vez. Para cada neurona i, G_i contiene los índices de sus aristas incidentes.

```text
R(w) = sum(i, ||w[G_i]||_2)
J_training = L_data + lambda * R(w)
```

La intención es favorecer que algunos grupos incidentes queden completamente a cero, aislando neuronas. No es una penalización directa del número de neuronas ni garantiza el tamaño mínimo. Una arista pertenece a dos grupos porque tiene dos extremos; esto forma parte de R y no crea dos parámetros.

No basta con añadir epsilon bajo la raíz y esperar ceros exactos por SGD. Implementar el paso proximal de la penalización, después del paso de gradiente de L_data. No añadir también su gradiente: contaría la penalización dos veces.

1. Actualizar pesos de entrada, sesgos y conexiones mediante SGD de L_data.
2. Anular conexiones internas que ya resulten <=0 por esa actualización ordinaria, como antes.
3. Sobre las restantes, tomar v como sus pesos positivos y resolver:

```text
tau_prox = lr * lambda
z = argmin_z  0.5 * ||z-v||_2^2 + tau_prox * sum(i, ||z[G_i]||_2)
```

4. Solo al terminar el solver, trasladar el resultado al modelo y anular los ceros finales. No modificar máscaras durante sus iteraciones internas.

Con v positivo, el proximal de estas normas de grupo preserva no negatividad. Comprobarlo; no convertir pesos negativos inesperados en positivos tomando valores absolutos.

### Grupos solapados y solver

No aplicar una única contracción por neurona y presentarla como el proximal exacto de la suma: los grupos se solapan. Implementar un solver de proximal de suma, por ejemplo Dykstra cíclico con correcciones, y comprobar su convergencia. Una formulación operativa para este problema es:

```text
z = v
p_i = 0 para cada grupo G_i
repetir barridos completos:
  para cada grupo i:
    a = z[G_i] + p_i
    s = max(0, 1 - tau_prox / ||a||_2)   si ||a||>0; si no, s=0
    z_new_group = s * a
    p_i = a - z_new_group
    z[G_i] = z_new_group
```

Reiniciar las correcciones en cada nuevo problema proximal. Mantener un orden determinista de grupos. Tratar los ceros y grupos vacíos sin dividir por cero.

Para verificar convergencia, comprobar duales soportados en sus grupos y `||p_i||<=tau_prox`, el invariante `z + sum_i embed(p_i) = v` y la brecha primal-dual:

```text
P = 0.5*||z-v||^2 + tau_prox*R(z)
s = sum_i embed(p_i)
D_dual = dot(v,s) - 0.5*||s||^2
gap = P - D_dual
```

Usar tolerancias absolutas/relativas y un máximo de barridos. Una brecha significativamente negativa, valores no finitos o falta de convergencia invalidan el intento; no anunciar éxito por agotar iteraciones. Propuesta inicial: tolerancia 1e-10 y hasta 2000 barridos, configurables.

`lambda=0` debe omitir enteramente este paso y cualquier umbral adicional. Así proporciona un control limpio.

Con lambda positiva puede utilizarse un umbral numérico explícito `sparsity_zero_tol=1e-12` para fijar residuos positivos minúsculos a cero. Es una decisión adicional de poda que puede cambiar los turnos; registrarla y verificar el forward real. No confundir tolerancia del solver, umbral de arista y tolerancia de aceptación predictiva.

La pérdida no es globalmente suave por los cambios de recorrido y de participantes. El proximal resuelve el subproblema de R; no garantiza convergencia global del aprendizaje ni que una anulación conserve automáticamente el comportamiento.

## 4. Activación gradual y recuperación

Implementar el mecanismo como opción configurable y compararlo con lambda=0 en la misma arquitectura. Primero verificar el potencial vectorial sin presión y después activar la presión.

Propuesta de ciclo:

1. Entrenamiento inicial sin regularización.
2. Tras calentamiento y estancamiento, guardar una copia completa anterior al intento.
3. Aumentar lambda gradualmente desde cero hasta lambda_target durante un número configurable de épocas.
4. Limitar el tiempo de presión. Cuando aparezca una nueva neurona aislada, desactivar lambda para el resto del intento y pasar a recuperación. Si una actualización aísla varias, registrarlas sin inventar un orden arbitrario de eliminación.
5. Evaluar el modelo reducido antes de recuperar y conservar ese estado como candidato de época cero.
6. Entrenar con lambda=0 durante un presupuesto limitado, conservando el mejor estado admisible, incluido el inicial.
7. Aceptar solo si disminuye el número de participantes y cumple los límites de precisión. En caso contrario, restaurar completamente la copia anterior.

Si no se aísla ninguna neurona dentro del presupuesto, registrar que el intento no obtuvo la reducción buscada y restaurar. No declarar éxito únicamente porque bajó R o disminuyeron algunas conexiones.

La selección de checkpoint usa pérdidas puras de datos, sin incluir lambda*R en validación. Comparar errores por coordenada en entrenamiento y validación con la copia anterior:

```text
error_trial[k] <= error_reference[k]
                  + absolute_tolerance[k]
                  + relative_tolerance * error_reference[k]
```

La tolerancia absoluta es necesaria cuando el error anterior ya es prácticamente cero. Mostrar además peor error de punto, número de conexiones y neuronas; una media menor no demuestra igualdad entre nubes de tamaños distintos.

Valores de partida configurables, sin promesa de buen ajuste: calentamiento 1000 épocas, paciencia 200, mejora mínima absoluta 1e-8, lambda_target=1e-3, rampa 200 épocas, presión máxima 2000 épocas, recuperación máxima 500, tolerancia absoluta por coordenada normalizada 1e-6 y relativa 0.05. Documentar cualquier ajuste. No imponerlos sobre la configuración local del usuario.

Respetar un presupuesto total explícito. Puede ocurrir que una ejecución corta no llegue a activar la presión. No prolongarla indefinidamente ni calibrar el modelo usando gradientes de validación. Las tolerancias y lambda son hiperparámetros; reportar su sensibilidad.

Guardar por separado el mejor modelo predictivo según validación y la copia anterior al intento. Los snapshots incluyen parámetros, máscaras, identidades, configuración, semilla/estado aleatorio y estado del entrenamiento necesario para continuar. Si luego se añade otro optimizador, incluir también su estado.

Si quedan menos de dos participantes, aparece aislamiento total o un valor no finito, invalidar el intento y restaurar. No fabricar pérdida cero, preservar a la fuerza una arista negativa ni inventar reintegración durante el entrenamiento.

Evitar reintentos inmediatos desde el mismo estado: registrar modelo/candidatas y esperar una mejora significativa, un cambio estructural o una configuración explícitamente diferente. Mantener también un límite de intentos. La GUI debe distinguir entrenamiento, presión, recuperación y restauración.

## 5. Datos y comparaciones

Mismo mecanismo para ambos problemas; solo cambian datos, normalización e inversa.

### Celsius → (Fahrenheit, Kelvin)

Reutilizar, como referencia si no hay otra configuración local acordada:

```text
training_C = {-40,-20,0,20,40,60,80,100}
validation_C = {10,70}
x = C/100
y = ((1.8*C+32)/100, (C+273.15)/100)
```

Desnormalizar cada coordenada multiplicando por 100. No usar las fórmulas de etiquetas en el predictor. Los historiales antiguos de 3 y 10 neuronas alcanzaron precisiones prácticamente exactas, pero ninguna anuló conexiones; compararlos por precisión útil, tiempo y tamaño, no por diferencias entre errores de 1e-22 y 1e-27.

### x → (sin(x), cos(x))

Usar radianes, período completo y orden de componentes seno/coseno. Conservar la partición local si ya existe y está documentada. Si no existe, propuesta reproducible: 64 entradas de entrenamiento uniformes en [-pi,pi), y 64 de validación en los puntos medios de esos intervalos. Entrada del modelo x/pi; etiquetas calculadas con el ángulo original, sin escalar sus valores en [-1,1].

La matriz/pesos de entrada y sesgos se aprenden; no introducir seno, coseno, ángulos objetivo ni una parametrización circular hecha a mano en el forward.

Para cada dataset, comparar primero lambda=0 y lambda>0 con iguales datos, tamaño inicial, semilla, lr y presupuesto. Mostrar épocas y tiempo real, además de pérdidas por coordenada. Usar varias semillas para conclusiones; una demostración pequeña no establece una ventaja general.

Mantener SGD y los argumentos existentes como --epochs. La intensidad de una misma lambda puede tener efectos diferentes al cambiar N o la escala de los datos: no asumir equivalencia entre configuraciones.

## 6. Diagnóstico e historial

Por época registrar pérdidas puras totales y por coordenada de entrenamiento/validación, R, lambda efectiva, objetivo penalizado de entrenamiento, conexiones, participantes, pasos medios, fase, intentos y decisiones de restauración/aceptación. No reiniciar el contador del trabajo realizado en el CSV al restaurar pesos; distinguirlo de la época asociada al checkpoint.

Por neurona registrar como diagnóstico suma y máximo de pesos, magnitud de mensajes vectoriales, frecuencia de transmisión/recepción y distancia a las posiciones de otras neuronas. Estos indicios no autorizan eliminarla sin evaluación.

Registrar también terminaciones por módulos iguales con direcciones diferentes. No afirmar que eso impide aprender todos los problemas ni ocultarlo modificando la selección.

Conservar los historiales anteriores y guardar cada ejecución nueva en una carpeta propia de results con sus metadatos. Adaptar métricas/callbacks de la GUI sin crear un segundo entrenamiento. Las curvas principales deben comparar pérdida pura de datos.

## 7. Comprobaciones obligatorias

- Forward vectorial calculado a mano, módulos, máximos locales simultáneos, empate de módulo con distinta dirección, receptora compartida, sesgo una sola vez, retirada y parada.
- Derivadas por diferencias finitas de entradas, sesgos y conexiones compartidas, lejos de cambios de recorrido, ceros de sigma y anulaciones. No exigir gradientes ordinarios de las decisiones discretas.
- Evaluación sin cambios en parámetros ni máscaras, inclusión de retiradas y exclusión de aisladas.
- lambda=0: mismo resultado que la variante vectorial sin solver ni umbral extra, hasta errores de redondeo de implementación.
- Proximal con dos neuronas y una arista: R(w)=2*abs(w), por lo que z=max(v-2*tau_prox,0) para v>=0.
- Proximal en triángulo, orden de aristas AB/AC/BC: v=(1,0.01,0.01), tau_prox=0.05 debe producir aproximadamente (0.9,0,0), aislando C. Comprobar simetría, ceros, brecha y máscaras. Estos valores son un caso de prueba, no hiperparámetros para los datasets.
- No anular máscaras durante iteraciones del solver; ninguna arista anulada antes del solver puede reabrirse.
- Restauración completa cuando no se conserva precisión, no hay reducción o el intento resulta inválido. Conservar el candidato de recuperación de época cero.
- Prueba de integración que registra un aislamiento aceptado y otra que registra rechazo/restauración. Usar casos pequeños controlados; no forzar una reducción en el dataset real si no la encuentra.
- Compilar consola y ejecutar pruebas existentes y nuevas. Compilar la GUI si están disponibles sus dependencias; distinguir compilación de comprobación visual.

No anunciar éxito por reducir pesos sin aislar neuronas, por una pérdida penalizada menor o por anular todas las conexiones y omitir sus salidas.

## 8. Decisiones y límites importantes

La regularización por grupos es un mecanismo nuevo de presión estructural; no es la reducción algebraica compensada de tres neuronas discutida antes. La recuperación intenta compensar por aprendizaje, sin garantía de equivalencia exacta.

No reutilizar automáticamente aquella fórmula de reducción: se derivó para estados escalares, identidad, sesgos cero y una topología concreta. Esta variante tiene selección por módulo y Leaky ReLU vectorial y necesita su propia demostración para futuros compensadores.

El potencial vectorial y la regularización son cambios separados. Implementarlos en pasos verificables y mantener el control lambda=0. Un regularizador de grupos solapados es un sustituto de complejidad, no un contador exacto de neuronas; comprobar si funciona en esta arquitectura.

Entregar código, comandos, resultados reales y limitaciones observadas. Resolver decisiones rutinarias de implementación y documentarlas; no detenerse con otro plan ni afirmar que estos mecanismos ya fueron probados en la DRANN real.

## Referencias técnicas

- [Wen et al., Learning Structured Sparsity in Deep Neural Networks](https://arxiv.org/abs/1608.03665): regularización de grupos para estructuras de redes neuronales. La adaptación a aristas incidentes simétricas de DRANN es una propuesta propia que se debe evaluar.
- [Barbero y Sra, Modular Proximal Optimization for Multidimensional Total-Variation Regularization](https://www.jmlr.org/papers/volume19/13-538/13-538.pdf), sección 4: combinación de operadores proximales. Sirve como referencia matemática, no como dependencia del programa.
