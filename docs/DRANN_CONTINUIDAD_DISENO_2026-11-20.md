# DRANN — Continuidad del diseño conceptual (20-11-2026)

**Estado:** decisiones aprobadas, propuestas en exploración y asuntos pendientes. Este documento conserva el diseño antes de abrir la investigación del lector KDE. **No implica que estas propuestas estén implementadas ni autoriza cambios automáticos al código.**

## 1. Terminología y notación

- **Rizoma:** dinámica inmanente de neuronas con potenciales escalares y conexiones simétricas.
- **Espacio cognifenomenológico:** espacio transcendente donde se expresan las posiciones neuronales.
- **Cognifenómeno / intuición:** sinónimos para la representación obtenida; preferir «cognifenómeno».
- **Fenómeno:** entrada en validación/inferencia, sin utilizar etiquetas ideales en la predicción.
- **Eje ideal:** eje asociado a una componente de salida; ejes normalizados F* y K* llevan un solo asterisco.
- **Ejemplo ideal:** antigua referencia individual denominada «pozo».
- **Congregación:** conjunto de neuronas reunidas alrededor de un ejemplo ideal concreto. **No** equivale a especialización respecto a un eje.
- **Especialización:** afinidad estructural con un eje ideal, con independencia de estar congregada.
- **Congruencia ideal:** \(\chi_{i,\mathrm{ej}}\), afinidad o especialización geométrica estructural de la neurona i respecto al eje ideal \(\mathrm{ej}\), distinta de su participación efectiva.
- **Sesgo transcendente:** \(\vec b_i^{(tr)}\). **Impulso transcendente:** \(\vec I_i^{(tr)}\), antiguo vector \(\vec w_i^{(T)}\); el superíndice (tr) va entre paréntesis.
- Índices de neuronas i, j; índice de componente ideal \(\mathrm{ej}\); dimensión escrita explícitamente como \(\dim(\vec\xi)\), evitando introducir T o D.
- Épocas e=1,...,E; intervalo de análisis e+=1,...,E+, exactamente E+ observaciones (los arrays C++ podrán usar índices desde 0).

## 2. Representación transcendente y congruencia ideal

\[
\boxed{\vec q_i(x)=\vec b_i^{(tr)}+\vec I_i^{(tr)} V_i^{(t)}(x).}
\]

El sesgo marca la posición de referencia; el impulso determina dirección y amplitud de desplazamiento por unidad de potencial inmanente, **no velocidad cronológica**. Se distinguen especialización **dinámica** (impulso orientado) y **estática** (impulso nulo, sesgo sobre un eje). Un impulso pequeño puede ser útil si la tarea exige pocos desplazamientos. Si impulso y sesgo son ambos cero, no existe preferencia estructural por un eje.

Formulación conceptual aceptada:

\[
A_{i,\mathrm{ej}}=
\frac{(I_{i,\mathrm{ej}}^{(tr)})^2+(b_{i,\mathrm{ej}}^{(tr)})^2+\varepsilon}
{\|\vec I_i^{(tr)}\|^2+\|\vec b_i^{(tr)}\|^2+\varepsilon\dim(\vec\xi)}
\]

\[
P^{\mathrm{prox}}_{i,\mathrm{ej}}=
\exp\left(
-\frac{\sum_{j\ne\mathrm{ej}}(b_{i,j}^{(tr)})^2}
{2\sigma_{g,\mathrm{ej}}^2}
\right),
\qquad
\boxed{\chi_{i,\mathrm{ej}}=A_{i,\mathrm{ej}} P^{\mathrm{prox}}_{i,\mathrm{ej}}.}
\]

- El sesgo y el impulso intervienen en la congruencia de forma continua, sin necesidad de definir un ángulo para el impulso nulo.
- \(\sigma_{g,\mathrm{ej}}\) es **aprendible por eje ideal** (parametrización positiva propuesta: softplus de \(\rho_{\mathrm{ej}}\)); inicialización elegida: **calibración por distancias geométricas iniciales**, p. ej. una mediana. Valores y protección numérica finales pendientes.
- La **incongruencia cognifenomenológica** es una discrepancia entre sesgo e impulso de magnitudes comparables orientados hacia ejes diferentes. Es un **diagnóstico**, no razón automática para sancionar/hibernar: puede representar un uso mixto funcional. No obligar al impulso a seguir el eje del sesgo.

## 3. Lector anterior aprobado (preservar mientras se explora KDE)

- Parámetros aprendibles \(a_{i,\mathrm{ej}}\), \(c_{i,\mathrm{ej}}\) **inicializados a cero**.
- Participación estructuralmente modulada:

\[
s_{i,\mathrm{ej}}(x)=a_{i,\mathrm{ej}}V_i^{(t)}(x)+c_{i,\mathrm{ej}},
\qquad
p_{i,\mathrm{ej}}(x)=
\frac{\chi_{i,\mathrm{ej}}e^{s_{i,\mathrm{ej}}(x)}}
{\sum_j\chi_{j,\mathrm{ej}}e^{s_{j,\mathrm{ej}}(x)}}.
\]

- Lector ponderado (sustituye mediana/mayoría histórica):

\[
\widehat y_{\mathrm{ej}}(x)=\sum_i p_{i,\mathrm{ej}}(x)q_{i,\mathrm{ej}}(x).
\]

- Pérdida individual geométrica ponderada:

\[
L(x)=\frac1{\dim(\vec\xi)}
\sum_{\mathrm{ej}}\sum_i
p_{i,\mathrm{ej}}(x)\|\vec q_i(x)-\vec\xi_{\mathrm{ej}}(x)\|^2.
\]

**Pendientes:** filtros/candidaturas, casos degenerados, gradientes exactos del backward manual, integración con código previo y pruebas. **No consta implementación de estas piezas.**

## 4. Diagnóstico y simplificación estructural

- Número efectivo de participantes por componente:
\[
M_{\mathrm{ej}}^{\mathrm{eff}}(x)=\frac1{\sum_i p_{i,\mathrm{ej}}(x)^2}.
\]
Sirve de **diagnóstico** de concentración/posible redundancia, no demuestra por sí solo que una neurona sea prescindible.
- Elegido **Group Lasso**, no L0, aplicado a **grupos por neurona que incluyen parámetros propios y conexiones inmanentes simétricas compartidas** (opción C: grupos solapados y posible compensación por conexiones duplicadas). Procedimiento proximal/gradiente aún no cerrado.
- Activar Group Lasso en estancamiento oscilante de pérdida inaceptable para la tarea **o** cuando la participación sugiera redundancia. No elimina automáticamente.
- Los pesos internos permanecen **simétricos**; eliminación de una neurona **completa** de inmanencia y transcendencia, con conexiones, **sin bypass**.
- Antes de eliminar, **simulacro reversible**: guardar estado; retirar provisionalmente; continuar entrenamiento; medir pérdidas individuales; aceptar/restaurar. No crear una rama original entrenándose en paralelo obligatoriamente si ya existe estancamiento conocido.
- Incremento relativo de pérdida por eje y neurona, comparando promedios de un intervalo de E+ épocas:
\[
\widehat L_{i,\mathrm{ej}}=
\frac{\overline L_{\mathrm{ej},E^+}^{\mathrm{sim}}-\overline L_{\mathrm{ej},E^+}^{\mathrm{pre}}}
{\max(\overline L_{\mathrm{ej},E^+}^{\mathrm{pre}},\varepsilon)}.
\]
Hay que evaluar posibles perjuicios a otros ejes por dinámica compartida; no aceptar por una mejora local que empeore otro eje más de lo tolerado.
- Duración del simulacro adaptativa, con límite de seguridad en épocas; puede aceptarse antes de reconvergencia si vuelve una **tendencia negativa sostenida**.
- Detectar estancamiento mediante **regresión de pendiente y varianza por eje ideal**. Umbral \(\tau_m\) relacionado con variabilidad/incertidumbre de pendiente; para detectar regreso del aprendizaje: **regresión + paciencia**, con propuesta provisional \(\mathrm P_{\mathrm{pac}}=\max(1,\lceil\alpha E^+\rceil)\). La elección de alpha y mínimo puede revisarse. E+ se vincula a fase de estancamiento, no constante universal. **Confirmar persistencia y estimar retrospectivamente su inicio**. La correlación entre ventanas importa.

## 5. Ideas futuras registradas en GitHub

- [#6](https://github.com/meirame/DRANN/issues/6) Ablaciones/simulacros reversibles en lotes con muchas neuronas.
- [#7](https://github.com/meirame/DRANN/issues/7) Godot: mostrar en pantalla detección de convergencia/estancamiento por eje, con comienzo estimado.
- [#8](https://github.com/meirame/DRANN/issues/8) Distribución inicial de neuronas según media y varianza de ejemplos por componente; volver tras la discusión actual.
- [#9](https://github.com/meirame/DRANN/issues/9) Nuevo criterio de swap: conectividad inmanente con neuronas de tareas subcongregadas.

## 6. Hibernación y reintegración — exploratorio, NO implementación aprobada

- Diagnóstico de incongruencia cognifenomenológica **no implica hibernación automática**.
- Hibernación propuesta: retirar contribución al forward/estimaciones y conservar posibilidad de **aprendizaje auxiliar**. La pérdida activa por sí sola no da gradientes a parámetros retirados del cómputo.
- Reintegración rizomática: usar preferentemente **actividad inmanente de vecinas directas**, no obligar a copiar sus pesos/sesgos. Estudiar señal unilateral durante el ensayo, congelando efecto sobre red activa, sin cambiar simetría de pesos almacenados.
- Notación candidata para potencial de prueba: \(\mathring V_i^{(t)}\), circunferencia sobre V (LaTeX: comando mathring).
- Si mejora el entrenamiento y persiste incongruencia: posible ablación; si sale de la incongruencia: intentar reintroducción y comprobar; si hibernar empeora y no se corrige: restaurar provisionalmente en vez de asumir inutilidad.
- Nueva sugerencia para estudiar: llamar «hibernación» retrospectivamente a una ausencia seguida de retorno y «ablación» a una ausencia que resulta definitiva. Explorar si una neurona retirada puede regresar utilizando infraestructura compartida; la retención de sus parámetros/estado debe diseñarse.

## 7. Nueva exploración tras este checkpoint: KDE

**No se sustituye todavía la lectura aprobada del apartado 3.** Propuesta: permitir posiciones de neuronas en todo el espacio cognifenomenológico; respuesta como el **punto de máxima densidad espacial de neuronas**:

\[
\rho(\vec z\mid x)=\sum_i K_h(\vec z-\vec q_i(x)),
\qquad
\widehat{\vec\xi}(x)=\operatorname*{argmax}_{\vec z}\rho(\vec z\mid x).
\]

No añadir ponderación explícita por actividad sin consentimiento; los potenciales inmanentes ya cambian las posiciones q. Pendientes: elección de kernel y ancho h; múltiples máximos; cálculo de máximo y retropropagación (argmax no es directamente diferenciable); pérdida compatible con **referencias vectoriales conjuntas** en lugar de puntos ideales por eje; comparativa experimental con lectura ponderada.

## 8. Regla de control

La discusión es teórica. La implementación C++ con backward manual y el laboratorio Godot se preservan; no introducir rediseños o defaults no aprobados. Este documento guarda decisiones y distingue expresamente hipótesis de mecanismos ya decididos.
