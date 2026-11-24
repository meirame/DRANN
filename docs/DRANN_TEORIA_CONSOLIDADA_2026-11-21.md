# DRANN — Teoría consolidada y frontera de investigación
**Fecha del estado:** 21 de noviembre de 2026.
**Carácter:** documento de continuidad conceptual. Distingue acuerdos, formulaciones provisionales y cuestiones abiertas. **No presupone implementación** de las propuestas. No sustituye automáticamente al código existente ni autoriza cambios de arquitectura.

**Antecedente:** [checkpoint previo (20-11-2026)](DRANN_CONTINUIDAD_DISENO_2026-11-20.md). Esta síntesis conserva la arquitectura anterior para poder compararla con la nueva línea de lectura KDE.

## 1. Vocabulario y convenciones

- **Rizoma / inmanencia:** estructura de neuronas con potenciales inmanentes escalares y conexiones simétricas.
- **Espacio cognifenomenológico:** espacio transcendente donde cada neurona manifiesta una posición y donde aparece el cognifenómeno o intuición.
- **Fenómeno:** entrada \(x\) en validación/inferencia.
- **Ejemplo ideal:** objetivo \(\vec\xi(x)\) de un fenómeno del conjunto de entrenamiento. **Eje ideal:** eje de una componente \(\mathrm{ej}\) del vector objetivo (ejemplos \(F^*,K^*\)).
- **Congregación:** conjunto de neuronas agrupadas espacialmente ante un fenómeno. No es lo mismo que **comunidad inmanente** (grupo definido por sus relaciones estructurales).
- **Congregación ideal:** la vinculada al ejemplo ideal del fenómeno considerado. Las otras pueden ser **competidoras**; **interferentes** solamente cuando su densidad compromete la intuición correcta.
- **Congruencia ideal:** \(\chi_{i,\mathrm{ej}}\), especialización estructural respecto a un **eje**; distinta de la **afinidad congregacional relativa**, que siempre se refiere a una congregación particular.
- **Plasticidad selectiva:** nombre elegido para la posible reorganización individual mediante experiencia propia y transmitida, con proyección hacia la congregación ideal. La eyección no implica copiar parámetros ajenos.
- **Sesgo transcendente** \(\vec b_i^{(tr)}\) e **impulso transcendente** \(\vec I_i^{(tr)}\). El impulso denota desplazamiento por unidad de potencial, no velocidad cronológica. **Superíndice (tr) entre paréntesis**, nunca tr sin paréntesis.
- Índices \(i,j\) para neuronas; \(\mathrm{ej}\) para coordenadas ideales; no introducir una letra redundante para \(\dim(\vec\xi)\).
- **Liderazgo** \(L_i\) (mayúscula); **función de pérdida** \(\mathcal L\) caligráfica para evitar colisión.
- **Potencial inmanente de ensayo de neurona hibernada:** \(\mathring V_i^{(t)}\). Media inmanente, cuando haga falta, \(\overline V\), **no** \(\mu\).

## 2. Arquitectura histórica de manifestación transcendente

\[
\boxed{\vec q_i(x)=\vec b_i^{(tr)}+\vec I_i^{(tr)}V_i^{(t)}(x)}
\]

La inmanencia es escalar, mientras \(\vec q_i\), \(\vec b_i^{(tr)}\), \(\vec I_i^{(tr)}\) son vectores transcendentes.

**Limitación ahora reconocida:** para una neurona fija, el conjunto de posiciones que permite esta expresión al cambiar \(V_i^{(t)}\) está contenido en **una recta afín**. Por ello no puede representar exactamente, por sí sola, objetivos arbitrarios no colineales en una tarea multidimensional. **Se ha acordado estudiar una manifestación transcendente más expresiva**, que integre información individual, observable local e inferida. **Todavía no existe ecuación ampliada aprobada**. No introducir términos nuevos de forma silenciosa.

### Congruencia ideal por eje — formulación histórica acordada

\[
A_{i,\mathrm{ej}}=
\frac{(I_{i,\mathrm{ej}}^{(tr)})^2+(b_{i,\mathrm{ej}}^{(tr)})^2+\varepsilon}
{\|\vec I_i^{(tr)}\|^2+\|\vec b_i^{(tr)}\|^2+\varepsilon\dim(\vec\xi)}
\]

\[
P^{\mathrm{prox}}_{i,\mathrm{ej}}=
\exp\left(-\frac{\sum_{j\ne\mathrm{ej}}(b_{i,j}^{(tr)})^2}
{2\sigma_{g,\mathrm{ej}}^2}\right),
\qquad
\chi_{i,\mathrm{ej}}=A_{i,\mathrm{ej}}P^{\mathrm{prox}}_{i,\mathrm{ej}}.
\]

Interpretación: especialización **estática** cuando el impulso es nulo y el sesgo está situado sobre un eje; **dinámica** cuando el impulso está orientado. Cuando sesgo e impulso son cero, no hay eje preferente. \(\sigma_{g,\mathrm{ej}}\) se concibió **aprendible por eje**, positivo e inicializado a partir de distancias geométricas iniciales. Detalles numéricos por fijar. **Incongruencia cognifenomenológica**: sesgo e impulso de magnitudes comparables favoreciendo ejes diferentes; es un **diagnóstico**, no condena ni hibernación automática. Pueden existir representaciones mixtas útiles.

## 3. Lector anterior preservado como alternativa, NO borrado

Antes de la exploración KDE, se había aprobado estudiar participación neuronal dependiente del fenómeno y congruencia ideal:

\[
s_{i,\mathrm{ej}}(x)=a_{i,\mathrm{ej}}V_i^{(t)}(x)+c_{i,\mathrm{ej}},
\quad
p_{i,\mathrm{ej}}(x)=
\frac{\chi_{i,\mathrm{ej}}\exp(s_{i,\mathrm{ej}}(x))}
{\sum_j\chi_{j,\mathrm{ej}}\exp(s_{j,\mathrm{ej}}(x))}.
\]

\(a\) y \(c\): aprendibles, inicialmente cero.

\[
\widehat y_{\mathrm{ej}}(x)=\sum_i p_{i,\mathrm{ej}}(x)q_{i,\mathrm{ej}}(x).
\]

Pérdida geométrica individual de referencia (formulación histórica, pendiente de validación de detalles de referencias por eje):

\[
\mathcal L_{\mathrm{geom}}(x)=
\frac{1}{\dim(\vec\xi)}
\sum_{\mathrm{ej}}\sum_i p_{i,\mathrm{ej}}(x)
\|\vec q_i(x)-\vec\xi_{\mathrm{ej}}(x)\|^2.
\]

**Estado:** preservado para comparación. No confundir con la nueva propuesta de lectura por densidad.

## 4. Nueva orientación elegida: convergencia cognifenomenológica común

**Objetivo conceptual confirmado:** tras el aprendizaje y las ablaciones pertinentes, **todas las neuronas supervivientes** deben tender, ante **cada fenómeno**, al **mismo** punto \(\vec\xi(x)\). La convergencia no implica igualar los estados inmanentes, parámetros, historias o modos de aprender.

\[
\boxed{\vec q_i(x)\longrightarrow\vec\xi(x),
\qquad \forall i\in\mathcal N_{\mathrm{supervivientes}}.}
\]

La meta cambia con el fenómeno: no hay un único punto fijo de toda la tarea. **No se desea imponer las dinámicas o parámetros de unas neuronas sobre las demás**: cada una debe asimilar lo que recibe y encontrar su propia forma de converger. Por tanto, la congregación única ideal al final es **objetivo emergente**, no instrucción de copiar estados.

**Precisión matemática:** esto es un ideal de entrenamiento, no garantía de convergencia exacta en toda tarea con recursos finitos. Si todas \(\vec q_i\) coinciden, las **posiciones neuronales** se concentran en un punto, pero con anchura gaussiana \(h>0\) la KDE no se convierte en una delta.

## 5. Lectura KDE y aprendizaje directo de densidad — línea seleccionada, detalles provisionales

Permitir acceso de \(\vec q_i(x)\) a todo el espacio cognifenomenológico. Construir función de densidad espacial:

\[
\boxed{\rho(\vec z\mid x)=
\sum_{i\in\mathcal N_{\mathrm{act}}}
\exp\left(-\frac{\|\vec z-\vec q_i(x)\|^2}{2h^2}\right)}
\]

y localizar la intuición por el **máximo global** de \(\rho\):

\[
\widehat{\vec\xi}(x)\in\operatorname*{argmax}_{\vec z}\rho(\vec z\mid x).
\]

No se añade automáticamente peso adicional por actividad: los potenciales ya afectan a las posiciones. \(h>0\), su inicialización y posible aprendizaje permanecen abiertos. Se omitió la constante de normalización de probabilidad de la KDE porque no cambia la localización del máximo para parámetros fijos; sí puede importar al entrenar \(h\) o usar directamente alturas de densidad.

**Se prefiere la cuarta vía de aprendizaje discutida: aprendizaje directo de densidad**, en vez de derivar necesariamente todas las operaciones del localizador de máximos durante el backward.

Propuestas de pérdida **exploratorias, no cerradas**:

\[
\mathcal L_{\mathrm{atr}}(x)=
-\log(\rho(\vec\xi(x)\mid x)+\varepsilon).
\]

La mera atracción no garantiza que el máximo ideal venza a todos los competidores. Se estudió penalizar **solo congregaciones interferentes**, no toda congregación lejana: pueden resultar útiles a la dinámica inmanente. Para un conjunto de posiciones rivales detectadas \(\mathcal Z_-(x)\), un margen \(\Delta\) y peso \(\lambda\), una **candidata** de pérdida contrastiva es:

\[
\mathcal L_{\mathrm{comp}}(x)=
\frac{\lambda}{|\mathcal Z_-(x)|}
\sum_{\vec z\in\mathcal Z_-(x)}
\big[\max(0,\rho(\vec z\mid x)-\rho(\vec\xi(x)\mid x)+\Delta)\big]^2.
\]

**No está elegido el margen**: fijo absoluto vs aprendible; también se planteó relativo, sin cerrar. No definir \(\mathcal L_{\mathrm{comp}}\) para \(\mathcal Z_-=\varnothing\) por esa expresión sin especificar un caso separado. Si una posición competidora no fue examinada puede escapar a la penalización. **Durante inferencia** ya no conocemos \(\vec\xi\), por lo que todavía hace falta un localizador del máximo de densidad.

### Detección de máximos KDE — procedimiento provisional elegido

Usar el gradiente espacial para encontrar puntos estacionarios:

\[
\nabla_{\vec z}\rho=
\frac1{h^2}\sum_i
\kappa_i(\vec z,x)(\vec q_i(x)-\vec z),\qquad
\kappa_i=\exp(-\|\vec z-\vec q_i(x)\|^2/(2h^2)).
\]

Un punto estacionario verifica

\[
\vec z=\frac{\sum_i\kappa_i\vec q_i}{\sum_i\kappa_i}.
\]

Y la **hessiana** (matriz de derivadas segundas) permite confirmar máximos estrictos si es definida negativa:

\[
H_\rho(\vec z)=
\sum_i\kappa_i
\left[
\frac{(\vec q_i-\vec z)(\vec q_i-\vec z)^\top}{h^4}
-\frac{\mathbf 1_{\mathrm{mat}}}{h^2}
\right].
\]

Aquí \(\mathbf 1_{\mathrm{mat}}\) representa la identidad matricial (no el impulso \(\vec I_i^{(tr)}\)). La implementación numérica exacta sigue abierta; se acordó intentar **búsqueda con gradientes y comprobación de hessiana**, y revisarla si resulta demasiado cara. Evaluación por punto: densidad y gradiente \(\mathcal O(N\dim(\vec\xi))\); hessiana \(\mathcal O(N\dim(\vec\xi)^2)\). El coste principal puede proceder de múltiples puntos iniciales/iteraciones. **No se garantiza hallar todos los máximos** ni el global sin verificaciones adicionales. No se prefirió Mean Shift como método base.

## 6. Congregaciones, identidad persistente y firma

Para un fenómeno hay **una congregación asociada al ejemplo ideal**; pueden aparecer otras congregaciones competidoras, algunas interferentes y otras inofensivas o beneficiosas. **El objetivo final** es que las neuronas supervivientes converjan a la congregación del ejemplo ideal, sin eliminar prematuramente organizaciones útiles.

**Identidad persistente de la congregación ideal** a través de (a) continuidad de sus integrantes y (b) asociación al ejemplo ideal. Se debe distinguir \(\vec z_{c^*}(x)\), centro/máximo de la congregación ideal, de \(\vec\xi(x)\), objetivo; **solo idealmente coinciden**. Una congregación puede cambiar de composición y posición; su identificador **no debe reasignarse** a otra. No asumir automáticamente que las congregaciones de fenómenos diferentes son literalmente la misma entidad.

Para comparar composición en etapas sucesivas, propuesta de similitud de Jaccard:

\[
J(c_a,c_b)=\frac{|\mathcal C_{c_a}^{(e)}\cap\mathcal C_{c_b}^{(e+1)}|}
{|\mathcal C_{c_a}^{(e)}\cup\mathcal C_{c_b}^{(e+1)}|}.
\]

Umbrales, escisiones/fusiones, desapariciones y reidentificación quedan abiertos. La representación de afinidades será **compacta**, con correspondencia externa entre posición de vector e **ID congregacional estable**; al desaparecer una congregación se puede compactar el vector sin renombrar la sobreviviente. Gestión de memoria de congregaciones desaparecidas aún abierta.

**Firma congregacional**: nombre aceptado. Combina **composición** (identidad) con propiedades mensurables para reconocer miembros y congregaciones sin contacto directo. Propiedades geométricas propuestas: \(\vec z_c\) y dispersión

\[
\delta_c^2(x)=\frac1{|\mathcal C_c(x)|}
\sum_{i\in\mathcal C_c(x)}\|\vec q_i(x)-\vec z_c(x)\|^2.
\]

Propiedad inmanente apreciada: **cohesión estructural interna**, con \(w_{ij}^{+}=\max(0,w_{ij})\) y conexiones nulas/negativas **anuladas**:

\[
W_c^{\mathrm{int}}=\sum_{\substack{i<j\\i,j\in\mathcal C_c}} w_{ij}^{+},
\quad
\kappa_c=
\frac{2W_c^{\mathrm{int}}}
{\sum_{i\in\mathcal C_c}\sum_{j\ne i} w_{ij}^{+}},
\]

para denominador positivo. Mide proporción de fuerza de conexiones internas frente a la conectividad de sus miembros. **No** asumir que congregación transcendente equivale a comunidad inmanente.

**Descartado como descriptor principal de firma:** media/varianza de potenciales de los miembros. Potenciales iguales no garantizan movimientos transcendentes iguales: sesgos e impulsos pueden diferir. Cuando haga falta notación para una media de potenciales, usar \(\overline V_c^{(t)}\), pero no devolverla automáticamente a la firma. La firma final aún no está aprobada.

## 7. Estados de cada neurona: observable e inferido

**Estado observable**: su propio estado y el de las **vecinas directas conectadas**, incluyendo sesgo(s) propios, potencial, parámetros particulares, pesos activos y actividades de su localidad. **No** hay decisión aprobada de dar mayor peso al estado individual en \(\vec q_i\).

**Estado inferido** (nombre aprobado, sustituye «extrapolable»): la **interpretación / estimación** que la neurona construye de la organización del rizoma en su conjunto, más allá de sus contactos directos. No es conocer directamente conectividad global exacta. Puede diferir entre neuronas; debe proceder de experiencia propia y/o mensajes propagados, no de acceso inexplicado al estado global.

Conexiones simétricas \(w_{ij}=w_{ji}\); **solo son efectivas si \(w_{ij}>0\)**. Vecindad \(\mathcal N(i)=\{j\ne i:w_{ij}>0\}\). Eliminar/anular conexiones de pesos nulos o negativos de la localidad efectiva.

Variables observables locales **candidatas**:

\[
R_i(x)=\sum_{j\in\mathcal N(i)}w_{ij}V_j^{(t)}(x)
\]

\[
E_i(x)=V_i^{(t)}(x)\sum_{j\in\mathcal N(i)}w_{ij}.
\]

Recibida vs señal emitida **no tienen por qué coincidir** aunque \(w\) sea simétrica; \(E_i\) no mide el efecto final sobre receptores.

Para distinguir muchas relaciones moderadas de una sola dominante:

\[
u_{ij}(x)=w_{ij}|V_j^{(t)}(x)|,\quad
\pi_{ij}(x)=\frac{u_{ij}}{\sum_{k\in\mathcal N(i)}u_{ik}},\quad
n_i^{\mathrm{eff}}(x)=\frac1{\sum_j\pi_{ij}^2}.
\]

Estas expresiones requieren suma positiva; si todas las contribuciones son cero, se estudió definir el número efectivo como cero. También se discutió varianza ponderada de actividad local como descriptor de heterogeneidad, pero **no** identificarla con similitud de manifestaciones transcendentes. No fijar todavía qué descriptores entrarán a la nueva \(\vec q_i\).

**Conectividad global como magnitud del rizoma**, que una neurona **solo puede inferir** salvo realimentación explícita:

\[
A_{ij}=\mathbf 1[w_{ij}>0,\ i\ne j],\qquad
M=\frac12\sum_i\sum_{j\ne i}A_{ij},\qquad
C_{\mathrm{riz}}=\frac{2M}{N(N-1)}
=\frac{\overline k}{N-1}.
\]

Esto describe densidad de conexiones, **no** la organización en comunidades. El estado inferido deberá abarcar **cómo está conectado y cómo se organiza el rizoma en comunidades**, mientras lo observable registra contactos reales de la neurona.

## 8. Afinidad congregacional relativa: percepción y experiencia

Se decidió el nombre **afinidad congregacional relativa**: grado de vínculo de una neurona respecto a una congregación de **ID estable**. Se distingue **cuánto** vínculo hay y **con quién**. Conjunto/vector dinámico por neurona, inicialmente todo cero:

\[
\boldsymbol\alpha_i=(\alpha_{i,c_1},\alpha_{i,c_2},\ldots);
\quad
\boldsymbol\alpha_i^{\mathrm{act}}(x),
\quad
\boldsymbol\alpha_i^{\mathrm{mem}}.
\]

- **Percepción actual**: pertenencia/sintonía ante el fenómeno presente.
- **Memoria**: familiaridad adquirida por experiencia a través del entrenamiento.
- **Ninguna tiene por qué ser un parámetro directamente entrenable por backward.**
- Se permite **olvido gradual** y memoria comprimida (sin guardar necesariamente toda la historia); candidata no aprobada:

\[
\alpha_{i,c}^{\mathrm{mem},(n+1)}
=(1-\beta)\alpha_{i,c}^{\mathrm{mem},(n)}
+\beta\alpha_{i,c}^{\mathrm{act}}(x_n).
\]

- Para la percepción, explorar **reconocimiento geométrico cognifenomenológico** y **reconocimiento observable/inmanente**: una neurona puede reconocer a integrantes por características compartidas aun sin ser vecina directa. Ese reconocimiento más allá de lo observable necesita una **firma aprendida/inferida o comunicación**.
- La firma puede ser realimentada durante entrenamiento por la congregación ideal y propagarse «boca a boca» por el rizoma. Preferencia por **propagación local**; realimentación global no está descartada si es objetivamente más simple y efectiva. Evitar fugas de etiquetas ideales durante inferencia.

## 9. Liderazgo distribuido y asimilación de conocimiento

**Grado de liderazgo \(L_i\)** (notación preferida), emergente desde la **experiencia, no por backprop directo**. No reducir liderazgo a una sola neurona: **múltiples integrantes** de la congregación ideal que han alcanzado reiteradamente el ejemplo ideal pueden compartir conocimientos.

La influencia será mayor entre congéneres, pero **puede propagarse hacia otras congregaciones y neuronas sin congregación** a través de enlaces activos. Simetría de conexiones **no** implica simetría de transmisión funcional. Se quiere combinar:

- **Competencia / conocimiento demostrado** de cada neurona.
- **Reconocimiento / receptividad** que sus vecinas han desarrollado con la experiencia.

Una neurona acertada debe poder ser escuchada aunque aún no tenga prestigio; reconocimiento sin acierto tampoco debe sostener influencia arbitraria. **Escuchar no es copiar parámetros**: receptoras deben **interpretar y asimilar** según su estado observable, inferido y experiencia; mensajes iguales pueden producir adaptaciones distintas.

Línea de estudio sobre qué transmitir: experiencia inmanente (circunstancias de actividad propia y vecinal en los aciertos), experiencia transcendente (respuesta individual) y experiencia congregacional (evolución de afinidad). **No se ha cerrado formato de mensajes, reputación, receptividad ni regla de aprendizaje**.

## 10. Pulso cognifenomenológico: dispersión + eyección como una sola dinámica

La idea del usuario es **un único pulso emitido desde el centro de una congregación interferente**: repele/dispersa a sus integrantes y **sesga la eyección de algunos hacia la congregación ideal** (analogía de polos). No son dos operaciones independientes. Seleccionar neuronas por **afinidad inmanente/ideológica y relación funcional con la congregación ideal**, no meramente por proximidad actual. Las conexiones por sí solas no demuestran utilidad funcional.

Campo conceptual provisional para una neurona candidata, con centro interferente \(\vec z_c\):

\[
\vec F_i=
\kappa_i(\vec z_c,x)
\left[
\lambda(\vec q_i-\vec z_c)
+\nu\,\gamma_{i,c^*}(x)(\vec\xi-\vec q_i)
\right].
\]

**No aprobado**: intensidad radial, factor direccional \(\gamma\), activación del pulso y actualización de parámetros. La parte centrípeta evita que neuronas exactamente coincidentes en el centro queden inmóviles por gradiente radial nulo. Evitar que trasladar todas igual se confunda con dispersar. La intervención ante un ejemplo puede afectar a otros: comprobar comportamiento interfenoménico.

La afinidad congregacional relativa, liderazgo y asimilación podrían ayudar a definir qué neuronas son candidatas. El **propósito último** de la plasticidad selectiva es aprender individualmente a converger ante cada fenómeno, no imponerles parámetros ajenos.

## 11. Hibernación, ablación y control de aprendizaje — diseño anterior a revisar con nueva arquitectura

- **Group Lasso** (en lugar de L0) sobre grupos de neurona completa, incluidos pesos inmanentes simétricos compartidos (problema de solapamiento todavía abierto).
- Candidaturas por estancamiento en pérdidas inaceptables o concentración/redundancia de participación (antiguo número efectivo \(M_{\mathrm{ej}}^{\mathrm{eff}}=1/\sum_i p_{i,\mathrm{ej}}^2\)). Estas métricas proceden del **lector anterior** y requieren reinterpretación si KDE lo sustituye.
- Ablación integral de la neurona, parámetros transcendentes e inmanentes y conexiones, sin bypass. Anteriormente se acordaron **simulacros reversibles** antes de eliminar, entrenamiento adicional y pérdidas relativas por componente:

\[
\widehat L_{i,\mathrm{ej}}=
\frac{\overline L_{\mathrm{ej},E^+}^{\mathrm{sim}}-\overline L_{\mathrm{ej},E^+}^{\mathrm{pre}}}
{\max(\overline L_{\mathrm{ej},E^+}^{\mathrm{pre}},\varepsilon)}.
\]

- Estancamiento/convergencia por componente mediante **regresión de pendiente y variabilidad**, inicio retrospectivo tras persistencia, \(E^+\) como duración observada, paciencia provisional \(\max(1,\lceil\alpha E^+\rceil)\), límite máximo de época, posibilidad de aceptar una mejora sostenida antes de reconverger, comprobar otros ejes.
- **Revisión futura:** el usuario expresó posteriormente reservas sobre realizar ensayos reversibles sistemáticos en el paradigma de aprendizaje social; **no se ha decidido todavía eliminar del diseño previo los simulacros de ablación**. Mantener explícita esta tensión hasta nueva decisión.
- Hibernación exploratoria: retirar temporalmente de estimaciones, pero permitir potencial de prueba \(\mathring V_i^{(t)}\) y aprendizaje auxiliar por actividad inmanente de vecinas. Sin dependencia de la pérdida activa, **no hay gradiente automáticamente** en parámetros aislados: falta objetivo auxiliar.
- Se propuso distinguir retrospectivamente **hibernación** si la neurona regresa y **ablación** si no regresa. Si se desea retorno tras retiro, hay que conservar suficiente estado; las reglas exactas no están cerradas.
- No hibernar automáticamente por incongruencia geométrica.

## 12. Próxima frontera acordada: ampliar la manifestación transcendente

**Punto exacto desde el que se reanudará el diseño:** la expresión

\[
\vec q_i(x)=\vec b_i^{(tr)}+\vec I_i^{(tr)}V_i^{(t)}(x)
\]

es **demasiado simple** para conseguir trayectorias multidimensionales complejas y que todas las neuronas supervivientes converjan al ejemplo ideal ante diferentes fenómenos.

Investigar **antes de proponer ecuación** qué elementos entran a la manifestación de una neurona:

1. **Estado observable individual:** potencial, sesgo(s) propios, impulso y demás parámetros propios; **no se ha aprobado** darle peso numérico necesariamente mayor.
2. **Estado observable local:** vecinas con \(w_{ij}>0\), intensidad y diversidad de señales, influencia recibida/emitida, dispersión y número efectivo de vecinas influyentes.
3. **Estado inferido:** representación aprendida que esa neurona construye del rizoma completo, de sus comunidades y de firmas/afinidades congregacionales; puede ser incompleta e idiosincrásica.
4. **Experiencia y comunicación:** memoria de afinidad, liderazgo distribuido y mensajes asimilados; aprendizaje de cada neurona, **no imitación mecánica** de otra.

Diseñar cómo estas fuentes pueden producir **movimiento transcendente genuinamente multidimensional**, preservando singularidad neuronal y objetivo común, sin crear un lector artificialmente dependiente de la etiqueta durante validación. **Ningún término, vector o coeficiente adicional ha sido aprobado todavía.**

## 13. Pendientes e issues

- Determinar algoritmo robusto para identificar congregaciones y miembros a partir de modos KDE; elección de \(h\), múltiples máximos y coste de búsqueda.
- Precisar aprendizaje directo de densidad, elección de margen (fijo/aprendible/relativo **pospuesta**) y penalización solo de interferentes; no imponer eliminar toda diversidad transitoria.
- Definir firma, reglas de asociación persistente y propagación de información, memoria y liderazgo, evitando que «reconocer» implique acceso al estado global oculto.
- Formalizar pulso y plasticidad selectiva en relación con la **nueva** manifestación transcendente.
- Revisar compatibilidad con criterios de Group Lasso, simulacros y monitoreo heredados.

Asuntos de implementación histórica:
- [#6](https://github.com/meirame/DRANN/issues/6): simulacros de ablación de grupos.
- [#7](https://github.com/meirame/DRANN/issues/7): observabilidad de convergencia/estancamiento en Godot.
- [#8](https://github.com/meirame/DRANN/issues/8): inicialización según media/varianza de ejemplos ideales por componente.
- [#9](https://github.com/meirame/DRANN/issues/9): posible swap por conexiones con neuronas de una tarea subcongregada.

**Regla de continuidad:** registrar qué es acuerdo conceptual y qué es solo candidato; no implementar sin decisión expresa. El siguiente trabajo es **diseñar la nueva ecuación transcendente**.


## 14. Ideario experimental posterior — simetría frente a asimetría de la confianza (22-11-2026)

**Decisión para la fase actual:** mantener **un único peso compartido por conexión**, con \(w_{ij}=w_{ji}\). El peso se interpreta como grado de **confianza mutua** entre las neuronas conectadas y también interviene en el intercambio diferencial de potencial. Solo \(w_{ij}>0\) define una conexión activa. Los pesos pueden evolucionar con la experiencia; se contempla aparición, debilitamiento, desaparición de conexiones y ablación de neuronas.

**Experimento futuro expresamente solicitado:** contrastar este modelo de confianza simétrica con un modelo en el que \(w_{i\leftarrow j}\) y \(w_{j\leftarrow i}\) puedan ser diferentes. Motivo: una receptora puede perder confianza en una emisora que transmite conclusiones erróneas sin que la emisora pierda necesariamente confianza en la receptora. No se aprueba todavía la asimetría; preocupa duplicar los pesos de conexiones en modelos grandes.

**Preguntas para la comparación experimental:**
- ¿Mejora la precisión y generalización al aprender confianza dirigida?
- ¿Facilita la detección, reducción de influencia y rehabilitación de falsos líderes?
- ¿Qué coste adicional de parámetros y memoria ocasiona, especialmente en rizomas grandes?
- ¿Cómo cambia la dinámica de intercambio diferencial si la confianza deja de ser simétrica?
- ¿Compensa explorar una representación híbrida con peso compartido y asimetrías almacenadas solo cuando aparezcan?

**No introducir un segundo peso por dirección ni una segunda matriz de confianza en la versión actual.** Conservar esta posibilidad exclusivamente en el ideario experimental.

## 15. Ideario experimental posterior — meritocracia estricta y diferencial como incentivo comunicativo (22-11-2026)

**Decisión actual (no experimental):** mantener la amplificación por liderazgo del tipo **\(1+L_i\)** para que incluso neuronas sin liderazgo reconocido puedan comunicar. El liderazgo \(\,L_i\in[0,1]\,\) es gradual, adquirido por aciertos, revisable y puede influir **tanto en la intensidad de emisión como en la persistencia/alcance** del conocimiento al propagarse. Ni el liderazgo ni la comunicación justifican copiar parámetros o trayectorias ajenas.

**Alternativa solicitada para estudio futuro — meritocracia estricta:** comparar la multiplicación directa de la intensidad de emisión por **\(L_i\)**, en lugar de **\(1+L_i\)**. En una expresión ilustrativa previa con potencial no negativo se consideraba \(\Gamma_{i\to j}=c_{ij}V_iL_i\), frente a \(c_{ij}V_i(1+L_i)\); **ninguna de esas expresiones se adopta como definitiva**, ya que ahora se admiten potenciales negativos y la comunicación se reinterpreta como impulsada por el diferencial. La idea interesante es privilegiar la difusión de enseñanzas de neuronas con mayor mérito.

**Pros a investigar:** selección de fuentes con competencia demostrada, reducción de interferencias de mensajes erróneos, debilitamiento funcional de falsos líderes sin ablación, mayor eficiencia comunicativa y revalorización progresiva mediante nuevos aciertos.

**Contras a investigar:** silenciamiento de neuronas sin historial (\(L_i=0\)), dificultad para descubrir conocimiento nuevo, concentración de poder comunicativo, cámaras de eco, falso liderazgo, y especialización contextual (liderazgo global que no equivale a competencia para cada fenómeno). Notar que \(L_i\in[0,1]\) **atenúa** la intensidad base, no la amplifica por encima de ella.

**Decisión conceptual adicional:** el **diferencial de potencial** \(\Delta V_{ij}^{(\tau)}=V_i^{(\tau)}-V_j^{(\tau)}\) es una **fuerza que impulsa la interacción y comunicación**, no solamente una corriente inmanente paralela a un deseo de comunicarse independiente. Se permite \(V_i^{(\tau)}<0\); el signo del diferencial da sentido y su magnitud favorece una corriente mayor, \(J_{i\to j}^{(\tau)}=c_{ij}\Delta V_{ij}^{(\tau)}\), con \(c_{ij}=c_{ji}>0\) en enlaces activos. La intensidad de corriente crece con \(|\Delta V|\) a confianza fija.

**Compatibilidad pendiente de formalizar:** ya se acordó que **dos neuronas con igual potencial deben poder comunicarse y aprender entre sí**, aunque \(J_{i\to j}=0\). Por tanto, la nueva interpretación del diferencial como incentivo o fuerza comunicativa **no debe convertirlo en requisito exclusivo** de la comunicación. Diseñar una componente comunicativa de base o condición equivalente sin introducir parámetros injustificados; separar el contenido \(\mathcal C_i=(\mathcal E_i,\mathcal K_i,\vec q_i)\), la confianza \(c_{ij}\), el liderazgo \(L_i\), la corriente y la intensidad comunicativa. Se acordó eliminar la función de modulación \(\mathcal M\) y no introducir \(\nu\) sin justificación.

**Estado:** reflexión matemática/conceptual, no implementación aprobada.

## 16. Experiencia individual: memoria temporal aprobada y dos ideas pospuestas (23-11-2026)

### Decisión vigente: experiencia actual e histórica

Se mantiene la separación conceptual

\[
\mathcal E_i^{(\tau)}=
\bigl(\mathcal E_{i,\mathrm{actual}}^{(\tau)},
\mathcal E_{i,\mathrm{hist}}^{(\tau)}\bigr).
\]

**Aprobado como punto de partida:** utilizar una media móvil para conservar la experiencia histórica con olvido progresivo,

\[
\boxed{
\mathcal E_{i,\mathrm{hist}}^{(\tau)}
=(1-\mu_E)\mathcal E_{i,\mathrm{hist}}^{(\tau-1)}
+\mu_E\mathcal E_{i,\mathrm{actual}}^{(\tau)},
\quad 0<\mu_E\le 1.
}
\]

**Decisión posterior de 23-11-2026, que sustituye el listado provisional:** la experiencia actual queda definida por **tres magnitudes** y no almacena variaciones temporales:

\[
\boxed{
\vec{\mathcal E}_{i,\mathrm{actual}}^{(\tau)}
=
\begin{pmatrix}
V_i^{(\tau)}\\
A_i^{(\tau)}\\
G_i^{(\tau)}
\end{pmatrix}
\in\mathbb R^3
}
\]

Con la vecindad activa \(\mathcal N(i)\), usar **explícitamente** \(|\mathcal N(i)|\) y no una variable abreviada:

\[
A_i^{(\tau)}
=
\frac{1}{|\mathcal N(i)|}
\sum_{j\in\mathcal N(i)}|J_{j\to i}^{(\tau)}|,
\qquad
G_i^{(\tau)}
=
\frac{1}{|\mathcal N(i)|}
\sum_{j\in\mathcal N(i)}\Gamma_{j\to i}^{(\tau)}.
\]

Para \(|\mathcal N(i)|=0\), adoptar \(A_i=G_i=0\). La experiencia histórica aplica la media móvil ya aprobada sobre el mismo vector de tres componentes; no se fija aún \(\mu_E\). La media, no la suma, evita que el registro de actividad dependa mecánicamente del número de vecinos.

**Prioridad semántica:** no interesa transmitir anécdotas ni sucesos sin valor. El objetivo de \(\mathcal C_i=(\mathcal E_i,\mathcal K_i,\vec q_i)\) es que las demás neuronas puedan conocer **qué circunstancias vivió \(i\)** (\(\mathcal E_i\)), **qué aprendió sobre cómo responder a ellas para estimar** (\(\mathcal K_i\)) y **qué respuesta manifestó** (\(\vec q_i\)). Evitar duplicar \(\vec q_i\) dentro de la experiencia, ya que se transmite por separado. Aún queda por demostrar cómo \(\mathcal K_i\) codifica relaciones entre circunstancias, métodos y resultados: las tres magnitudes agregadas no contienen por sí solas el contenido ni la causalidad de cada mensaje recibido. No convertir diferencias entre posiciones de neuronas en errores automáticos.

### Ideario futuro 1 — memoria matricial de asociaciones entre experiencias (pospuesto)

Explorar si, además de la media móvil de observaciones, merece la pena registrar asociaciones temporales entre componentes de la experiencia mediante una matriz de productos cruzados:

\[
\mathcal R_i^{(\tau)}
=(1-\mu_R)\mathcal R_i^{(\tau-1)}
+\mu_R\,\vec{\mathcal E}_{i,\mathrm{actual}}^{(\tau)}
\vec{\mathcal E}_{i,\mathrm{actual}}^{(\tau)\top}.
\]

**No incorporar por ahora.** Comprobar si ofrece valor adicional frente a \(\mathcal K_i\), si genera redundancia o costes cuadráticos, y recordar que un producto cruzado medio no constituye automáticamente una correlación ni una relación causal.

### Ideario futuro 2 — medias móviles de error cognifenomenológico (pospuesto)

**Idea válida para investigar más adelante, pero expresamente aparcada por ahora**: durante entrenamiento supervisado, medir el error individual respecto al ideal

\[
\vec e_i^{(\tau)}(x)=\vec\xi(x)-\vec q_i^{(\tau)}(x)
\]

y mantener dos memorias para no perder información cuando errores de signos opuestos se cancelan:

\[
\overline{\vec e}_i^{(\tau)}
=(1-\mu_e)\overline{\vec e}_i^{(\tau-1)}
+\mu_e\vec e_i^{(\tau)}(x),
\]

\[
\overline{\varepsilon}_i^{(\tau)}
=(1-\mu_e)\overline{\varepsilon}_i^{(\tau-1)}
+\mu_e\|\vec e_i^{(\tau)}(x)\|.
\]

La primera representa sesgo/dirección típica; la segunda, magnitud habitual. No almacenar directamente cada error de ejemplo en \(\mathcal K_i\); eventualmente estudiar las medias como memoria auxiliar asociada al aprendizaje. Idea de aplicación futura: comparar errores sobre ejemplos equivalentes para detectar neuronas que progresan peor que sus congéneres y **acelerar su aprendizaje**, no desplazarlas automáticamente hacia las posiciones de las otras neuronas. **La etiqueta ideal solo está disponible durante entrenamiento/evaluación**: validación sin backward ni actualización de memorias aprendidas; inferencia sin error supervisado disponible ni dependencia de este en el forward. **No activar estas medias móviles ahora.**

**Otras decisiones recientes de diseño:** no introducir RNG por ahora; descartar ANN individuales dentro de cada neurona; se sigue explorando \(\mathcal K_i\) como tensor/matriz estructurado con alguna dimensión vinculada a \(d=\dim(\vec\xi)\), sin adoptar aún una forma final. La intensidad comunicativa con escala \(\lambda\) y la variante de saturación se han considerado candidatas, pero no se aprueba aún sustituir la primera por la segunda. Para asimilación, mantener provisionalmente suma de contribuciones \(\sum_{j\in\mathcal N(i)}\Delta\mathcal K_{i\leftarrow j}\), sin coeficiente de aceptación \(\alpha\) separado.

**Siguiente tarea actualizada:** con la experiencia mínima fijada, investigar cómo \(\mathcal K_i\) registra y utiliza relaciones entre circunstancias, respuesta de la neurona y manifestación transcendente sin crear ANN individuales. No implementar.


## 17. Ideario experimental — variaciones temporales eliminadas de la experiencia vigente (23-11-2026)

**Petición expresa:** retirar **todos los deltas temporales** de la experiencia actual, sin descartarlos del ideario futuro. No confundir con el potencial **entre neuronas** \(\Delta V_{ij}=V_i-V_j\) que continúa siendo esencial en la corriente y comunicación.

- Variación temporal del potencial: \(\delta_\tau V_i^{(\tau)}=V_i^{(\tau)}-V_i^{(\tau-1)}\). Hipótesis futura: informa de perturbaciones o tendencias de actividad; hoy no se ha justificado que el receptor necesite conocerla o que mejore la asimilación frente al estado actual y su media móvil.
- Desplazamiento transcendente: \(\delta_\tau\vec q_i^{(\tau)}=\vec q_i^{(\tau)}-\vec q_i^{(\tau-1)}\). Hipótesis futura: permite representar dirección de movimiento ideológico; hoy se transmite directamente \(\vec q_i\) y no se ha establecido una regla de aprendizaje que necesite su variación.
- Variación del conocimiento: \(\delta_\tau\mathcal K_i^{(\tau)}=\mathcal K_i^{(\tau)}-\mathcal K_i^{(\tau-1)}\), o su norma. Hipótesis futura: indica repercusión de los mensajes y experiencias sobre el conocimiento, pero **un cambio grande no demuestra una mejora**. No incorporarla hasta definir la estructura de \(\mathcal K_i\).
- Como alternativa eventual a los incrementos entre dos ciclos, comparar una magnitud actual con su media histórica para medir desviaciones de tendencia; **no es equivalente a una diferencia temporal** y tampoco se activa por ahora.

**Regla actual:** experiencia actual de solo \((V_i,A_i,G_i)\), memoria histórica por media móvil, sin deltas temporales. La manifestación \(\vec q_i\) forma parte del contenido del mensaje por separado, y la pregunta central pendiente es cómo comunicar y asimilar **las relaciones útiles** entre condiciones vividas, la forma en que la neurona respondió, y la estimación que obtuvo.


## 18. Repliegue deliberado al núcleo experimental mínimo (23-11-2026)

**Nueva directriz aprobada por el usuario:** detener la acumulación de mecanismos anticipatorios. Primero construir una dinámica básica **rizomática y recurrente**, observar fallos reales y solo entonces recuperar herramientas que resuelvan dificultades verificadas. Los acuerdos conceptuales no desaparecen: **quedan aparcados, no revocados**. Evitar presentar propuestas pendientes como si fueran decisiones definitivas.

### Núcleo que sí se desea explorar ahora

- Neuronas con potenciales escalares \(V_i^{(\tau)}\), rizoma de conexiones de confianza \(c_{ij}=c_{ji}>0\) en aristas activas, corriente firmada \(J_{i\to j}^{(\tau)}=c_{ij}(V_i^{(\tau)}-V_j^{(\tau)})\).
- Recurrencia local en ciclos \(\tau\): primero evolución/información rizomática, después nueva manifestación transcendente, que estará disponible en el **siguiente** ciclo de comunicación. No introducir dependencia simultánea implícita.
- Posición transcendente individual \(\vec q_i^{(\tau)}\in\mathbb R^{\dim(\vec\xi)}\), inicialmente con \(\vec b_i^{(tr)}\) y \(\vec I_i^{(tr)}\), **pero no limitarla para siempre a una recta afín**. Debe incorporarse información del rizoma, sin red ANN privada.
- Conservar como meta la convergencia por fenómeno al ejemplo ideal, sin imponer identidad de potenciales ni de parámetros neuronales. Lector KDE continúa siendo orientación conceptual elegida, aunque puede posponerse al comprobar primero el forward mínimo.

### Aprobado previamente, pero fuera del primer experimento

1. **Experiencia individual** \(\mathcal E_i=(\mathcal E_{i,\mathrm{actual}},\mathcal E_{i,\mathrm{hist}})\): vector actual \((V_i,A_i,G_i)^\top\), con \(A_i=\sum_{j\in\mathcal N(i)}|J_{j\to i}|/|\mathcal N(i)|\), \(G_i=\sum_{j\in\mathcal N(i)}\Gamma_{j\to i}/|\mathcal N(i)|\), ambos cero si no hay vecinos; memoria histórica por media móvil exponencial de coeficiente \(\mu_E\). **Conservar como acuerdo, no exigir su implementación inicial.**
2. **Contenido informativo** \(\mathcal C_i=(\mathcal E_i,\mathcal K_i,\vec q_i)\), como aspiración de comunicación de circunstancias relevantes, modo de responder y estimación obtenida. En el prototipo básico basta provisionalmente con transmitir \(\vec q_i\); transmisión íntegra de \(\mathcal C\) pendiente.
3. **Conocimiento individual distribuido** \(\mathcal K_i\): refleja cómo alcanzar las distintas componentes del ideal y por ello tendrá estructura ligada al número \(\dim(\vec\xi)\). Las neuronas deben poder aprender e interpretar sin copia directa. **Su estructura, regla de actualización y decisión parámetro/estado NO están aprobadas**. La matriz de dimensión \(\dim(\vec\xi)\times6\) y su producto por experiencia actual/histórica fueron una **candidata no adoptada**, no un acuerdo.
4. **Liderazgo gradual** \(L_i\in[0,1]\), adquirido por acierto repetido en supervisión, reversible y con potencial efecto en emisión y persistencia de enseñanzas. Se prefirió factor \(1+L_i\) frente a \(L_i\) para mantener voz en principiantes. **Aparcar su activación** hasta saber si el prototipo la necesita.
5. **Intensidad comunicativa**: se eligió provisionalmente \(\Gamma_{i\to j}=(1+L_i)(c_{ij}+\lambda|J_{i\to j}|)\) con factor de escala \(\lambda\), sujeto a estudio; la expresión con saturación \(\Gamma_{i\to j}=c_{ij}(1+L_i)\bigl(1+\lambda|\Delta V_{ij}|/(1+\lambda|\Delta V_{ij}|)\bigr)\) es **candidata interesante, no adoptada**. Ambas pueden quedar fuera del prototipo, que solo usa \(c_{ij}\) para transmitir estados.
6. **Asimilación individual de enseñanzas**: idea provisional de suma de modificaciones \(\mathcal K_j^{(\tau+1)}=\mathcal K_j^{(\tau)}+\sum_i\Delta\mathcal K_{j\leftarrow i}\); sin nuevo coeficiente \(\alpha\) de aceptación. No implementar antes de descubrir qué necesita la arquitectura.
7. **Ideas ya expresamente pospuestas**: matriz de asociaciones de experiencias \(\mathcal R_i\), medias móviles de error cognifenomenológico supervisado, variaciones temporales del potencial, posición y conocimiento, confianza dirigida, meritocracia estricta, reglas de influencia y congregación, pulso, hibernación y ablación; ver secciones anteriores. **Sin RNG ni ANN individuales**, por decisión actual.

### Primeras ecuaciones candidatas para una dinámica mínima (NO aprobadas todavía)

Con entradas \(x\) que determinan estados iniciales \(V_i^{(0)}(x)\) (mecanismo de entrada pendiente), la difusión inmanente firmada más sencilla sería:

\[
V_i^{(\tau+1)}
=
V_i^{(\tau)}
+
\eta\sum_{j\in\mathcal N(i)}
c_{ij}(V_j^{(\tau)}-V_i^{(\tau)}),
\]

con \(\eta>0\) como tamaño de paso, sujeto a estabilidad. Para que la nueva manifestación dependa realmente del rizoma y no solo de un potencial escalar individual, estudiar:

\[
\boxed{
\vec q_i^{(\tau+1)}
=
\vec b_i^{(tr)}
+
\vec I_i^{(tr)}V_i^{(\tau+1)}
+
\frac{
\sum_{j\in\mathcal N(i)}c_{ij}\vec q_j^{(\tau)}
}{
1+\sum_{j\in\mathcal N(i)}c_{ij}
}.
}
\]

Para neuronas sin vecinos, la fracción vale cero; inicializar \(\vec q_i^{(0)}=\vec b_i^{(tr)}+\vec I_i^{(tr)}V_i^{(0)}\). Se reutilizan los mismos \(c_{ij}\), sin una ANN privada, sin coeficientes nuevos en el acoplamiento y sin necesidad de conocimiento ni experiencia explícitos. El denominador conserva la magnitud **absoluta** de la confianza (una arista de confianza casi cero implica influencia casi cero incluso cuando es la única), y acota la suma de influencias vecinales frente a repetir sin límite. Las \(\vec q_j^{(\tau)}\) son mensajes del ciclo anterior, evitando un lazo algebraico instantáneo.

**Limitaciones que deben medirse, no esconderse:** (i) la evolución inmanente puramente difusiva tiende a igualar potenciales y puede borrar información de entrada; (ii) añadir posiciones de vecinas puede fomentar imitación o convergencia a un punto erróneo, no aprendizaje independiente; (iii) esta regla sigue siendo lineal en los estados para parámetros fijos y no resuelve por sí sola tareas arbitrariamente no lineales; (iv) en la candidata, \(\vec q\) influye en manifestaciones posteriores a través de la comunicación, pero **no retroalimenta aún la actualización escalar de \(V\)**. Si se exige retorno directo al potencial, habrá que añadirlo justificadamente tras probar el modelo mínimo. **No confundir candidata con acuerdo aprobado ni implementarla todavía sin permiso.**


## 19. Núcleo recurrente: acuerdos posteriores y tamaño de paso adaptativo en el ideario (24-11-2026)

**Decisiones vigentes que prevalecen sobre las formulaciones históricas:**

- Se adopta provisionalmente la expresión transcendente de la sección 18:
\[
\vec q_i^{(\tau+1)}
=\vec b_i^{(tr)}+\vec I_i^{(tr)}V_i^{(\tau+1)}
+\frac{\sum_{j\in\mathcal N(i)}c_{ij}\vec q_j^{(\tau)}}
{1+\sum_{j\in\mathcal N(i)}c_{ij}}.
\]
- Para la dinámica inmanente básica, mantener \(V_i^{(\tau+1)}=V_i^{(\tau)}+\eta\sum_{j\in\mathcal N(i)}J_{j\to i}^{(\tau)}\), con \(J_{j\to i}^{(\tau)}=c_{ij}(V_j^{(\tau)}-V_i^{(\tau)})\). **\(\eta>0\) será un hiperparámetro fijo, no entrenable, por decisión expresa.** Su valor numérico no está fijado.
- Emplear inicialmente un número finito y fijo de ciclos \(\tau_{\max}\) durante el entrenamiento; no exigir equilibrio inmanente.
- Nueva nomenclatura: \(\mathring V^{(\tau)}=N^{-1}\sum_iV_i^{(\tau)}\) para potencial medio del rizoma; \(V_i^{(h)}\) para potencial de neurona hibernada. Sustituye las convenciones históricas incompatibles. Usar \(\tau\) como índice de ciclo.
- Con topología y confianzas fijas y simétricas durante el forward, la difusión conserva el potencial medio. Una condición suficiente para acotación y consenso por componente conexa es \(0<\eta<1/\max_i\sum_{j\in\mathcal N(i)}c_{ij}\) cuando existen conexiones activas. Si las confianzas crecen durante el entrenamiento, comprobar de nuevo la condición; un hiperparámetro fijo no garantiza por sí solo estabilidad para todos los pesos posibles.
- Diagnóstico propuesto, no parte del forward: varianza de potenciales \(\sigma_V^{2(\tau)}=N^{-1}\sum_i(V_i^{(\tau)}-\mathring V^{(\tau)})^2\) y proporción final/inicial cuando la varianza inicial no es cero. Observar pérdida de diversidad sin introducir mecanismos correctivos preventivos.

### Ideario experimental — tamaño de paso inmanente dependiente de las confianzas

**Pospuesto expresamente por el usuario.** Comparar en el futuro el hiperparámetro fijo con la alternativa
\[
\eta=\frac{1}{2\max_i\sum_{j\in\mathcal N(i)}c_{ij}}.
\]
Si no existen conexiones activas, no hay intercambio y no es necesario evaluar esa fracción. Con confianzas simétricas no negativas y fijas durante el forward, esta elección garantiza que el operador de difusión tiene autovalores en \([0,1]\), evitando modos de signo alternante y asegurando acotación. No confundir el tamaño de paso inmanente con la tasa de aprendizaje del optimizador. Estudiar coste de reducción global, sensibilidad a conexiones extremas, efecto de reescalar todas las confianzas (se cancela en la dinámica inmanente) y tratamiento de la dependencia de \(\eta\) respecto a parámetros entrenables en el backward. **No activar esta alternativa ahora.**

No se modifica código. Continuar con el análisis de estabilidad transcendente y la utilidad experimental de la recurrencia, sin agregar mecanismos innecesarios.


## 20. Percepción distribuida inicial, recorte de conexiones y excitación continuada pospuesta (24-11-2026)

**Decisión expresa:** empezar con percepción distribuida, con todas las neuronas inicialmente conectadas a todas las componentes del fenómeno y entre sí (sin autoconexiones). La arquitectura debe poder aprender a recortar tanto las conexiones internas del rizoma como las conexiones de entrada. No imponer de antemano grupos receptores o percepciones parciales.

La expresión de percepción distribuida propuesta y elegida como punto de partida es
\[
V_i^{(0)}(x)=b_i^{(in)}+
\sum_{k=1}^{\dim(\vec x)}w_{ik}^{(in)}x_k.
\]
La parametrización, inicialización numérica y regla de recorte se concretarán posteriormente. Distinguir los pesos de entrada \(w_{ik}^{(in)}\) de las confianzas internas \(c_{ij}=c_{ji}>0\): un peso de entrada negativo puede aportar información útil y no debe considerarse por ello una conexión eliminada. El criterio de actividad de las conexiones de entrada aún no está cerrado. El fenómeno no se convierte en una neurona del rizoma.

**Simetría:** el usuario observa que, con conectividad interna completa y neuronas inicialmente equivalentes, no importa cuál recibe una componente si solo se cambia su etiqueta. Esta equivalencia es por permutación de neuronas y sus parámetros; conectividad completa por sí sola no garantiza equivalencia si los sesgos, impulsos o confianzas ya son distintos. La percepción distribuida evita imponer de entrada una división receptora/no receptora. Objetivo experimental: permitir que neuronas que pierdan acceso directo a parte o toda la entrada puedan seguir estimando gracias al rizoma, sin imponer todavía una regla concreta de poda ni un objetivo de eliminación total.

**Nomenclatura vigente:** llamar dispersión inmanente a \(\sigma_V^{2(\tau)}\) y dispersión cognifenomenológica a \(\sigma_q^{2(\tau)}\). Son diagnósticos; no forman parte del forward ni de la pérdida por defecto.

### Ideario experimental — excitación continuada del rizoma

**Pospuesta expresamente.** Frente a excitar el rizoma únicamente en \(\tau=0\) y permitir su evolución libre durante un número finito de ciclos, explorar más adelante si conviene mantener una aportación externa dependiente de \(x\) durante la dinámica inmanente. Posible motivación: contrarrestar la pérdida de información inicial por difusión. Estudiar si la excitación repetida impide conservar el potencial medio, aumenta la amplificación o modifica la estabilidad; comparar con el prototipo sin excitación continuada. No se ha definido ni aprobado su ley matemática, y no debe introducirse en el prototipo actual.

**Siguiente cuestión:** definir la naturaleza y el recorte de las conexiones de entrada, sin confundirlas con la confianza interna. No se modifica código.


## 21. Reactivación de conexiones y distinción entre hibernación y ablación (24-11-2026)

**Decisión vigente:** una conexión inactiva debe poder reactivarse conceptualmente; una neurona hibernada debe poder reincorporarse al rizoma. Se mantiene la distinción ya establecida entre **neurona activa**, **neurona hibernada** (retirada reversible, conserva los parámetros necesarios) y **neurona ablada** (eliminación definitiva). La ablación no es sinónimo de hibernación. Los criterios de retirada, recuperación y eliminación definitiva todavía deben concretarse. No se modifica código.

### Ideario experimental — reactivación por derivada lateral (prioridad alta)

**Alternativa preferida expresamente por el usuario, pospuesta para después del prototipo mínimo.** Para una conexión rizomática inactiva, estudiar el gradiente lateral desde el lado activo:

\[
\left.\frac{\partial\mathcal L}{\partial c_{ij}}\right|_{c_{ij}=0^+}.
\]

Evaluarlo con los estados y adjuntos del forward del rizoma activo, incorporando las dos contribuciones inmanentes y las dos transcendentes de la conexión compartida a lo largo de todos los ciclos. La derivada mide el efecto local de introducir una confianza positiva infinitesimal, aun cuando el gradiente ordinario de una conexión excluida sea cero. Un valor negativo indica mejora a primer orden; uno positivo, empeoramiento; uno nulo no descarta mejoras de orden superior. La evaluación debe corresponder a la frontera cero, no al valor negativo de un parámetro enmascarado. No es garantía de mejora para una reactivación de magnitud finita ni para todo el conjunto de entrenamiento.

**Orden de prioridad propuesto:** alta, primera opción para investigar reactivación de conexiones tras disponer del forward/backward básico y de un criterio de inactivación; por delante de mecanismos más complejos de experiencia, conocimiento y liderazgo cuando el objetivo sea recuperar conectividad. No ejecutar en el prototipo actual. Estudiar evaluación periódica o selectiva para conservar las ventajas de la dispersidad estructural, sin fijar todavía frecuencia, umbral ni magnitud de reactivación. Conservar la alternativa de simulacros de reactivación como secundaria si la derivada lateral resulta insuficiente.

**Siguiente cuestión de diseño:** concretar el mecanismo de hibernación y ablación neuronal, respetando los acuerdos anteriores de la sección 11 y revisándolos a la luz del núcleo recurrente y la pérdida KDE actuales.
