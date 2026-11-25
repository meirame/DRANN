# DRANN — Teoría consolidada y frontera de investigación
**Fecha del estado:** 25 de noviembre de 2026 (checkpoint del prototipo mínimo, secciones 23–30).
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


## 22. Hibernación durante el entrenamiento y reactivación neuronal lateral pospuesta (24-11-2026)

**Decisiones expresas que prevalecen sobre las formulaciones anteriores:** la distinción entre hibernación y ablación viene dada por el momento del entrenamiento: toda neurona hibernada que no se haya reincorporado al finalizar el entrenamiento pasa a estar ablada definitivamente. Durante el entrenamiento, la retirada es conceptualmente reversible. Para el prototipo actual se empleará únicamente Group Lasso como mecanismo de regularización estructural. No implementar ahora reactivación mediante derivadas laterales, aprendizaje auxiliar de hibernadas ni simulacros adicionales. La forma exacta de aplicar Group Lasso, declarar hibernación y tratar los grupos nulos queda por decidir.

### Ideario experimental — reactivación neuronal por derivada lateral (prioridad alta, posterior a la de conexiones)

**Pospuesto expresamente por el usuario.** Generalizar la idea de la sección 21 mediante un coeficiente auxiliar de participación neuronal \(a_i\in[0,1]\), no necesariamente entrenable: cero representa ausencia, uno participación completa. Estudiar
\[
\left.\frac{\partial\mathcal L}{\partial a_i}\right|_{a_i=0^+}.
\]
Para que sea una prueba de reincorporación integral, la participación debe modular tanto la influencia rizomática de la neurona sobre las demás como su contribución a la KDE normalizada:
\[
\rho(\vec z)=
\frac{\sum_i a_i\exp[-\|\vec z-\vec q_i\|^2/(2h^2)]}
{(2\pi h^2)^{\dim(\vec\xi)/2}\sum_i a_i}.
\]
Mantener \(h\) fijo al evaluar el gradiente de la pérdida neuronal \(\mathcal L=-\log\rho(\vec\xi)\). Exigir al menos una neurona activa. Un gradiente negativo indica beneficio infinitesimal, no garantiza beneficio de la reincorporación completa. Estudiar cómo calcular la posición de prueba de la hibernada y cómo modular su comunicación sin perturbar el rizoma activo cuando \(a_i=0\). La derivada lateral no proporciona automáticamente gradientes de aprendizaje a sus parámetros aislados.

**Prioridad propuesta:** alta dentro de la futura reorganización reversible, después de comprobar el mecanismo lateral de conexiones, cuya derivación es más directa, y el Group Lasso del núcleo mínimo. No incorporar \(a_i\), evaluaciones laterales ni dinámica auxiliar al prototipo actual. No se modifica código.


# Prototipo mínimo de DRANN — especificación consolidada (25-11-2026)

**Estado:** diseño teórico aceptado para iniciar la implementación en C++ sobre CPU. Las secciones 23–30 reúnen los acuerdos vigentes y prevalecen sobre las formulaciones históricas incompatibles de las secciones anteriores. No se presupone que el código existente implemente este diseño. Se conservan el ideario y los antecedentes, pero no se incorporan sus mecanismos al prototipo. Los detalles señalados como pendientes no son decisiones aprobadas.

## 23. Datos, parámetros, inicialización y forward

### 23.1. Convenciones y datos

- Fenómeno \(\vec x\), ejemplo ideal vectorial \(\vec\xi(x)\), dimensión cognifenomenológica \(\dim(\vec\xi)\). Índice de ciclo rizomático \(\tau\), de época \(e\), de neurona \(i,j\), de componente ideal \(\mathrm{ej}\).
- \(\mathcal D_{\mathrm{train}}\) es el conjunto de entrenamiento. Estandarizar por componente las entradas y salidas mediante \(z'=(z-\mu)/\sigma\), con estadísticas calculadas exclusivamente en entrenamiento; reutilizar las mismas en validación e inferencia. Las ecuaciones siguientes se entienden en el espacio estandarizado. Invertir la transformación de salida para expresar el cognifenómeno en unidades originales. Tratamiento de desviaciones exactamente nulas pendiente de concretar.
- Cálculos en double (64 bits). CPU y descenso de gradiente clásico sobre todo el entrenamiento, sin Adam ni minilotes. No ANN internas en cada neurona.
- \(\mathcal A\) denota el conjunto activo y \(N=|\mathcal A|\) su tamaño. Inicialmente \(N=N_0>1\); se permite acabar con \(N=1\). Las sumas neuronales siguientes se restringen a activas salvo indicación contraria.
- Confianzas internas simétricas, un solo parámetro por par no ordenado: \(c_{ij}=c_{ji}\). Sin autoconexiones. \(\mathcal N(i)\) son las vecinas activas con confianza positiva. Los pesos de entrada son distintos de las confianzas y pueden ser negativos.

### 23.2. Inicialización aprobada

Conectividad inicial completa entre neuronas y percepción de todas las componentes por todas ellas. Para \(i\ne j\):
\[
c_{ij}=\frac1{N_0-1},\qquad
b_i^{(in)}=0,\qquad
w_{ik}^{(in)}\sim\mathcal N\left(0,\frac1{\dim(\vec x)}\right).
\]
El segundo argumento de \(\mathcal N\) es la varianza. No imponer durante el aprendizaje que la suma de confianzas siga siendo uno. Para cada componente ideal:
\[
b_{i,\mathrm{ej}}^{(tr)}\sim\mathcal N(\mu_{\mathrm{ej}},\sigma_{\mathrm{ej}}^2),
\qquad
I_{i,\mathrm{ej}}^{(tr)}\sim\mathcal N(0,\sigma_{\mathrm{ej}}^2).
\]
Las estadísticas corresponden al espacio de salida utilizado por el modelo; al estandarizar, son cero y uno para componentes no constantes. La inicialización del ancho de banda se realiza después de inicializar el resto (sección 24). Semilla y valores de hiperparámetros aún sin fijar.

### 23.3. Percepción e intercambio inmanente

\[
V_i^{(0)}=b_i^{(in)}+\sum_{k=1}^{\dim(\vec x)}w_{ik}^{(in)}x_k.
\]
Excitar únicamente en \(\tau=0\). No hay excitación continuada. Potencial entre neuronas e intercambio:
\[
\Delta V_{ij}^{(\tau)}=V_i^{(\tau)}-V_j^{(\tau)},\qquad
J_{i\to j}^{(\tau)}=c_{ij}\Delta V_{ij}^{(\tau)}=-J_{j\to i}^{(\tau)}.
\]
\[
V_i^{(\tau+1)}
=V_i^{(\tau)}+\eta\sum_{j\in\mathcal N(i)}J_{j\to i}^{(\tau)}.
\]
\(\eta>0\) es hiperparámetro fijo, no entrenable. Si no tiene vecinas, el potencial no cambia. Actualizaciones sincrónicas, leyendo estados del ciclo anterior, para no depender del orden de recorrido.

### 23.4. Manifestación transcendente

\[
\vec q_i^{(0)}=\vec b_i^{(tr)}+\vec I_i^{(tr)}V_i^{(0)},
\]
\[
\boxed{
\vec q_i^{(\tau+1)}=
\vec b_i^{(tr)}+\vec I_i^{(tr)}V_i^{(\tau+1)}
+\frac{\sum_{j\in\mathcal N(i)}c_{ij}\vec q_j^{(\tau)}}
{1+\sum_{j\in\mathcal N(i)}c_{ij}}
}.
\]
Los vectores \(\vec q_i,\vec b_i^{(tr)},\vec I_i^{(tr)}\) tienen dimensión \(\dim(\vec\xi)\). Parámetros compartidos entre ciclos. La comunicación transcendente usa las mismas confianzas, con retraso de un ciclo; no hay retroalimentación directa de \(\vec q\) hacia \(V\). Ejecutar un número fijo y finito de ciclos \(\tau_{\max}\). No calcular el modo KDE en cada ciclo ni introducir parada por fenómeno.

### 23.5. Diagnósticos y limitaciones

\[
\mathring V^{(\tau)}=\frac1N\sum_iV_i^{(\tau)},\quad
\sigma_V^{2(\tau)}=\frac1N\sum_i(V_i^{(\tau)}-\mathring V^{(\tau)})^2,
\]
\[
\mathring{\vec q}^{(\tau)}=\frac1N\sum_i\vec q_i^{(\tau)},\quad
\sigma_q^{2(\tau)}=\frac1N\sum_i\|\vec q_i^{(\tau)}-\mathring{\vec q}^{(\tau)}\|^2.
\]
Nombres vigentes: dispersión inmanente y dispersión cognifenomenológica. No forman parte del forward ni de la pérdida neuronal. Con topología y parámetros fijos, las posiciones individuales son funciones afines de la entrada; el lector de modos puede ser no lineal, pero no resuelve por sí solo la limitación de capacidad individual. Observar experimentalmente antes de ampliar la arquitectura. La difusión simétrica conserva la media inmanente con topología fija.

## 24. KDE, lectura y aprendizaje diferenciado del ancho de banda

### 24.1. KDE normalizada y pérdida neuronal

En lo sucesivo \(\vec q_i\) sin índice de ciclo significa \(\vec q_i^{(\tau_{\max})}\).
\[
\rho(\vec z\mid x)=
\frac{1}{N(2\pi h^2)^{\dim(\vec\xi)/2}}
\sum_{i\in\mathcal A}\exp\left(-\frac{\|\vec z-\vec q_i\|^2}{2h^2}\right).
\]
Kernel gaussiano isotrópico, igual peso para cada neurona activa, ancho escalar positivo compartido entre neuronas y fenómenos. No usar lectores ponderados históricos.
\[
\boxed{\mathcal L(x)=-\log\rho(\vec\xi(x)\mid x).}
\]
Actualizar parámetros neuronales y confianzas usando esta pérdida y manteniendo \(h\) constante durante su diferenciación. La NLL de una densidad continua puede ser negativa. No retropropagar a través del cognifenómeno leído, Mean Shift ni argmax.

**Decisión explícita:** evaluación directa de KDE, sin LogSumExp. Monitorizar subdesbordamiento, suma nula, logaritmo de cero y resultados no finitos; no sustituir por LogSumExp sin nueva autorización. La precisión double no elimina el riesgo.

### 24.2. Error colectivo y pérdida específica de h

\[
\varepsilon_{\mathrm{ind}}(x)=\frac1N\sum_i\|\vec q_i-\vec\xi(x)\|^2.
\]
\[
\boxed{\mathcal L_h(x)=\dim(\vec\xi)\log h+
\frac{\varepsilon_{\mathrm{ind}}(x)}{2h^2}.}
\]
Es la media de NLL gaussianas individuales salvo constante independiente de \(h\). Actualizar \(h\) exclusivamente mediante \(\mathcal L_h\), manteniendo las posiciones neuronales constantes en esa derivación. No transmitir su gradiente a los parámetros neuronales. Así el ancho depende del acierto colectivo y no solo de las neuronas más próximas al ideal.

\[
\omega_h=\log h,\qquad h=\exp(\omega_h),\qquad
\frac{\partial\mathcal L_h}{\partial\omega_h}
=\dim(\vec\xi)-\frac{\varepsilon_{\mathrm{ind}}(x)}{h^2}.
\]
Inicializar tras un forward completo del conjunto de entrenamiento con los parámetros iniciales y sin backward:
\[
h_{\mathrm{init}}=
\sqrt{
\frac{\sum_{x\in\mathcal D_{\mathrm{train}}}\sum_i
\|\vec q_i^{(\tau_{\max})}(x)-\vec\xi(x)\|^2}
{N_0|\mathcal D_{\mathrm{train}}|\dim(\vec\xi)}
},
\qquad
\omega_{h,\mathrm{init}}=\log h_{\mathrm{init}}.
\]
No localizar modos para inicializar. Si las posiciones se mantienen fijas y el error medio es positivo, el óptimo de la pérdida de ancho es la raíz cuadrada del error individual medio dividido por la dimensión. No se impone suelo conceptual a \(h\), ni penalización adicional. Protección exclusivamente numérica aprobada; falta concretar su magnitud/procedimiento y el caso degenerado de error inicial exactamente cero.

### 24.3. Lectura final: Mean Shift y argmax

Para el fenómeno considerado, inicializar una búsqueda por cada posición final \(\vec z_i^{(0)}=\vec q_i\). El índice \(k\) siguiente corresponde a iteraciones del lector, no a ciclos rizomáticos:
\[
\vec z^{(k+1)}
=
\frac{\sum_i\vec q_i\exp(-\|\vec z^{(k)}-\vec q_i\|^2/(2h^2))}
{\sum_i\exp(-\|\vec z^{(k)}-\vec q_i\|^2/(2h^2))}.
\]
Reunir los puntos de convergencia candidatos en \(\mathcal M\), eliminando duplicados para la búsqueda:
\[
\boxed{\widehat{\vec\xi}(x)=
\operatorname*{argmax}_{\vec z\in\mathcal M}\rho(\vec z\mid x).}
\]
Selección del mayor valor encontrado, no certificación del máximo global en todo el espacio. El argmax no es un algoritmo de búsqueda ni es diferenciable en general. La hessiana no es obligatoria; su uso se mencionó como verificación opcional, no como requisito aprobado. Definir tolerancia de convergencia, duplicados, máximo de iteraciones y empates al implementar. No utilizar etiquetas ideales para la lectura en inferencia. El lector no es necesario para obtener los gradientes ni para calcular el error de control del entrenamiento.

## 25. Backward analítico a través de todos los ciclos

Para un ejemplo, definir abreviaturas auxiliares (no nuevos parámetros ni estados neuronales):
\[
p_i(x)=\frac{\exp(-\|\vec q_i-\vec\xi\|^2/(2h^2))}
{\sum_j\exp(-\|\vec q_j-\vec\xi\|^2/(2h^2))},
\quad
d_i=1+\sum_{j\in\mathcal N(i)}c_{ij},
\quad
\vec R_i^{(\tau)}=\frac{\sum_j c_{ij}\vec q_j^{(\tau)}}{d_i},
\]
donde la suma de \(\vec R_i\) es sobre vecinas activas. Los adjuntos \(\vec g_{q,i}^{(\tau)}=\partial\mathcal L/\partial\vec q_i^{(\tau)}\) y \(g_{V,i}^{(\tau)}=\partial\mathcal L/\partial V_i^{(\tau)}\) son gradientes totales de la pérdida respecto a los estados correspondientes.

### 25.1. Condiciones finales y recurrencia hacia atrás

\[
\vec g_{q,i}^{(\tau_{\max})}
=p_i(x)\frac{\vec q_i-\vec\xi}{h^2},\qquad
g_{V,i}^{(\tau_{\max})}=\vec I_i^{(tr)}\cdot\vec g_{q,i}^{(\tau_{\max})}.
\]
Para \(\tau=\tau_{\max}-1,\ldots,0\):
\[
\vec g_{q,i}^{(\tau)}=\sum_{j\in\mathcal N(i)}
\frac{c_{ij}}{d_j}\vec g_{q,j}^{(\tau+1)},
\]
\[
g_{V,i}^{(\tau)}
=\vec I_i^{(tr)}\cdot\vec g_{q,i}^{(\tau)}
+\left(1-\eta\sum_{j\in\mathcal N(i)}c_{ij}\right)g_{V,i}^{(\tau+1)}
+\eta\sum_{j\in\mathcal N(i)}c_{ij}g_{V,j}^{(\tau+1)}.
\]
Acumular todos los usos de cada parámetro compartido en el tiempo:
\[
\frac{\partial\mathcal L}{\partial\vec b_i^{(tr)}}=
\sum_{\tau=0}^{\tau_{\max}}\vec g_{q,i}^{(\tau)},
\qquad
\frac{\partial\mathcal L}{\partial\vec I_i^{(tr)}}=
\sum_{\tau=0}^{\tau_{\max}}V_i^{(\tau)}\vec g_{q,i}^{(\tau)},
\]
\[
\frac{\partial\mathcal L}{\partial w_{ik}^{(in)}}=
g_{V,i}^{(0)}x_k,\qquad
\frac{\partial\mathcal L}{\partial b_i^{(in)}}=g_{V,i}^{(0)}.
\]

### 25.2. Gradiente de una confianza compartida activa

\[
\boxed{
\begin{aligned}
\frac{\partial\mathcal L}{\partial c_{ij}}
=\sum_{\tau=0}^{\tau_{\max}-1}\Bigg[
&\eta\Delta V_{ij}^{(\tau)}
(g_{V,j}^{(\tau+1)}-g_{V,i}^{(\tau+1)})\\
&+\frac{\vec g_{q,i}^{(\tau+1)}\cdot
(\vec q_j^{(\tau)}-\vec R_i^{(\tau)})}{d_i}\\
&+\frac{\vec g_{q,j}^{(\tau+1)}\cdot
(\vec q_i^{(\tau)}-\vec R_j^{(\tau)})}{d_j}
\Bigg].
\end{aligned}}
\]
Una sola derivada y una sola actualización por arista no ordenada. Las dos contribuciones inmanentes y las dos transcendentes están incluidas; no duplicar indirectamente la contribución de \(V\) ya incluida en su adjunto. Las neuronas o conexiones excluidas no reciben gradiente ordinario. No implementar gradientes laterales ahora. Verificar estos gradientes manuales mediante diferencias finitas en puntos de topología fija y fuera de fronteras no diferenciables antes de entrenar.

## 26. Descenso de gradiente clásico y Group Lasso

**Revisión expresa del autor, 25-11-2026:** retirar Adam y los minilotes del prototipo. Ambos quedan en el ideario con prioridad baja (§30). Esta revisión sustituye la optimización anterior; no modifica las ecuaciones del forward ni del backward analítico (§§23–25).

### 26.1. Optimización ordinaria

Descenso de gradiente clásico sobre el conjunto completo de entrenamiento. Una única actualización por época, después de acumular y promediar los gradientes de todos los ejemplos con parámetros y topología congelados:
\[
g_\theta=\frac1{|\mathcal D_{\rm train}|}\sum_{x\in\mathcal D_{\rm train}}
\frac{\partial\mathcal L(x)}{\partial\theta},
\qquad
g_{\omega_h}=\frac1{|\mathcal D_{\rm train}|}\sum_{x\in\mathcal D_{\rm train}}
\left(\dim(\vec\xi)-\frac{\varepsilon_{\mathrm{ind}}(x)}{h^2}\right).
\]
\[
\boxed{\theta^*=\theta-lr\,g_\theta,\qquad
\omega_h^{\,\mathrm{nuevo}}=\omega_h-lr\,g_{\omega_h}.}
\]
Usar los estados del mismo forward para ambos gradientes antes de modificar parámetros. Se conserva la separación de pérdidas y \(h=\exp(\omega_h)\); no regularizar el ancho. No utilizar momentos, corrección de sesgo, precondicionamiento, momentum ni actualizaciones por ejemplo. Se pueden recorrer los ejemplos secuencialmente y reutilizar las cachés; no se requiere guardar todos los forward a la vez. La tasa \(lr\) del entrenamiento ordinario sigue pendiente de fijar para este optimizador. Los ensayos con tasas separadas y dependientes de \(h\) son experimentales, no leyes aprobadas para el entrenamiento ordinario.

### 26.2. Grupo neuronal completo y activación selectiva

\[
G_i=\|\vec\theta_i\|_2=
\sqrt{(b_i^{(in)})^2+\|\vec w_i^{(in)}\|^2
+\|\vec b_i^{(tr)}\|^2+\|\vec I_i^{(tr)}\|^2
+\sum_{j\ne i}c_{ij}^2}.
\]
Incluye parámetros propios y las confianzas incidentes; las inactivas se mantienen en cero. Grupos naturalmente solapados, **sin factor de compensación 1/2**: una arista es compartida, no dos parámetros. Regularizar una sola neurona candidata a la vez en el prototipo. Cuando se activa:
\[
\mathcal L_{\mathrm{reg}}=\mathcal L+\lambda_{\mathrm{GL}}G_i,\qquad
\lambda_{\mathrm{GL}}>0\ \text{constante}.
\]
No sumar el gradiente de la regularización al gradiente ordinario: aplicar el proximal después del único paso de descenso de gradiente de la época, cuando GL esté activo. No usar regularización L1 adicional ni L0. La intensidad no es adaptativa ni calibrada por neurona. El único desencadenante es estancamiento inválido (sección 27); no activar por redundancia cognifenomenológica, que es un resultado deseado.

### 26.3. Operador proximal uniforme de Group Lasso

Al abandonar Adam se recupera el operador radial de métrica euclídea previamente aceptado:
\[
\vec\theta_i^{\,\mathrm{nuevo}}
=\max(0,1-lr\lambda_{\mathrm{GL}}/\|\vec\theta_i^*\|_2)\vec\theta_i^*.
\]
Si \(\|\vec\theta_i^*\|_2\le lr\lambda_{\mathrm{GL}}\), incluido el grupo ya nulo, el resultado es exactamente cero. Solo se aplica al grupo seleccionado y una vez por época, después del descenso de gradiente ordinario. Una arista compartida se actualiza una sola vez. Se mantiene el orden descenso de gradiente → proximal → proyección/recorte. No se necesita resolver una raíz por bisección. La anulación que aparezca únicamente después de la proyección sigue siendo una cuestión a aclarar (§29.3).

## 27. Control del entrenamiento: regresión segmentada, TOST y validez

### 27.1. Magnitud de control

Al finalizar cada época, evaluar con parámetros fijos sobre todo el conjunto de entrenamiento, después de la actualización de la época, sin calcular modos:
\[
\varepsilon_{\mathrm{ind}}^{(e)}
=\frac1{N|\mathcal D_{\mathrm{train}}|}
\sum_{x\in\mathcal D_{\mathrm{train}}}\sum_{i\in\mathcal A}
\|\vec q_i(x)-\vec\xi(x)\|^2.
\]
No usar NLL KDE para detectar estancamiento ni determinar validez, porque depende de \(h\) y puede favorecer desigualmente a neuronas próximas al ideal. Para cada fenómeno:
\[
\varepsilon_{\mathrm{ind}}(x)=\sigma_q^{2(\tau_{\max})}
+\|\mathring{\vec q}^{(\tau_{\max})}-\vec\xi(x)\|^2.
\]
El indicador mide conjuntamente dispersión y error del centro. Es media, no garantiza tolerancia individual de cada neurona, componente o fenómeno. El forward de diagnóstico puede requerir un recorrido adicional de entrenamiento.

### 27.2. Segmentación y duración emergente

Comparar una recta con una regresión de dos segmentos independientes:
\[
\varepsilon_{\mathrm{ind}}^{(e)}\approx
\begin{cases}m_1e+b_1&e<e_c,\\m_2e+b_2&e\ge e_c.\end{cases}
\]
\[
e_c=\operatorname*{argmin}_{e_c}(S_1+S_2),\qquad
\mathrm{BIC}=n\log(S/n)+k\log n.
\]
\(S\) es suma de cuadrados residuales, \(n\) observaciones, \(k=2\) para una recta y \(k=5\) para dos rectas contando el punto de cambio (convención operativa aprobada; el parámetro de varianza común se omite en ambos). Seleccionar menor BIC. Si no se justifica segmentación, usar toda la recta única. Si se seleccionan dos segmentos, usar el último para evaluar el régimen actual. En este último caso \(E^+=e_{\mathrm{actual}}-e_c+1\); para una recta, \(E^+\) es la longitud del intervalo analizado. No imponer una duración fija al régimen. Falta concretar mínimo de observaciones, candidatos admisibles y empates, historial exacto que se examina y tratamiento de \(S=0\). Para estimar la varianza residual de cada pendiente hacen falta al menos tres observaciones en ese segmento.

### 27.3. Pendiente, incertidumbre y criterio TOST operativo

Sobre las \(E^+\) épocas del régimen actual:
\[
\overline e=\frac1{E^+}\sum_e e,\quad
\overline{\varepsilon}_{\mathrm{ind}}=\frac1{E^+}\sum_e\varepsilon_{\mathrm{ind}}^{(e)},
\]
\[
m=\frac{\sum_e(e-\overline e)(\varepsilon_{\mathrm{ind}}^{(e)}-\overline{\varepsilon}_{\mathrm{ind}})}
{\sum_e(e-\overline e)^2},\quad b=\overline{\varepsilon}_{\mathrm{ind}}-m\overline e,
\]
\[
s^2=\frac{\sum_e[\varepsilon_{\mathrm{ind}}^{(e)}-(me+b)]^2}{E^+-2},
\qquad s_m=\sqrt{\frac{s^2}{\sum_e(e-\overline e)^2}}.
\]
Tolerancia relativa con suelo dado por la precisión exigida:
\[
m_{\mathrm{tol}}=r_m\max(\overline{\varepsilon}_{\mathrm{ind}},\varepsilon_{\mathrm{tol}}).
\]
Estancamiento confirmado según criterio TOST:
\[
\boxed{|m|+t_{1-\alpha_{\mathrm{TOST}},E^+-2}s_m<m_{\mathrm{tol}}.}
\]
Recuperación significativa del aprendizaje:
\[
\boxed{m+t_{1-\alpha_{\mathrm{TOST}},E^+-2}s_m<-m_{\mathrm{tol}}.}
\]
Una pendiente no significativa no basta para demostrar equivalencia. Usar TOST como criterio operativo: la selección del segmento a partir de los mismos datos, autocorrelación entre épocas, márgenes dependientes de datos y contrastes repetidos impiden atribuir una tasa de error estadística exacta al procedimiento. No incorporar correcciones estadísticas adicionales ahora.

### 27.4. Validez y elección de candidata

\[
\begin{cases}
\overline{\varepsilon}_{\mathrm{ind}}\le\varepsilon_{\mathrm{tol}}&
\text{estancamiento válido},\\
\overline{\varepsilon}_{\mathrm{ind}}>\varepsilon_{\mathrm{tol}}&
\text{estancamiento inválido}.
\end{cases}
\]
Solo clasificar así después de confirmar estancamiento. Usar la nomenclatura **inválido**, no inaceptable. La tolerancia depende de la tarea y es hiperparámetro. Interpretación RMS: \(\delta_{\mathrm{tol}}=\sqrt{\varepsilon_{\mathrm{tol}}}\); si se expresa por componente normalizada, \(\varepsilon_{\mathrm{tol}}=\dim(\vec\xi)\delta_{\mathrm{ej,tol}}^2\). Son parametrizaciones equivalentes, no nuevas exigencias.

Criterio de candidatura aprobado:
\[
\overline p_i=\frac1{|\mathcal D_{\mathrm{train}}|}\sum_x p_i(x),
\qquad i_{\mathrm{GL}}\in\operatorname*{argmin}_{i\in\mathcal A}\overline p_i.
\]
La menor participación KDE es solo criterio de candidatura, nunca de hibernación directa: siempre hay un mínimo y el valor equitativo \(1/N\) cambia al reducir el rizoma. Una neurona poco participativa puede ayudar indirectamente a otras. No activar GL únicamente porque exista candidata. El criterio anterior de redundancia por eje se considera obsoleto: congregación no equivale a redundancia funcional. Los empates de candidatura quedan pendientes.

### 27.5. Máquina de control

- Entrenamiento ordinario: si se confirma estancamiento inválido y \(N>1\), seleccionar una candidata y activar GL con intensidad constante.
- Durante GL: mantenerlo sobre la misma candidata hasta detectar recuperación del aprendizaje o anulación exacta del grupo. No establecer duración máxima propia de GL, por decisión expresa de sencillez. Puede persistir un equilibrio entre aprendizaje y regularización; se conserva el límite global de épocas.
- Al recuperar aprendizaje, desactivar GL; la neurona no nula sigue activa. Al anular el grupo, hibernar inmediatamente y desactivar GL.
- Reiniciar el historial de regresión al activar/desactivar GL o hibernar una neurona. No mezclar métricas de poblaciones activas distintas en el mismo régimen. Si hay hibernación durante una época, usar los parámetros y la población finales en el diagnóstico.
- Al alcanzar estancamiento válido o el máximo global \(E_{\max}\), finalizar entrenamiento. Si \(N=1\), deshabilitar GL y continuar aprendizaje ordinario hasta uno de los criterios de fin. Nunca eliminar la última neurona.
- La finalización por límite de épocas no implica precisión válida. Registrar por qué se ha detenido el entrenamiento.

## 28. Recorte, hibernación y ablación

- Confianzas: tras descenso de gradiente y proximal cuando proceda, imponer \(c_{ij}\leftarrow\max(0,c_{ij})\); una confianza exactamente nula deja de participar en ambos sentidos, se excluye de forward/backward. No reactivarla en el prototipo. Conservar simetría y no duplicar parámetros.
- Entrada: recortar solo si \(w_{ik}^{(in)}=0\) exactamente; no usar umbral de magnitud ni L1. Pesos negativos permanecen válidos. Se acepta que haya escaso recorte espontáneo con descenso de gradiente.
- Hibernación neuronal: cuando GL produce grupo exactamente nulo, retirar inmediatamente la neurona y todas sus conexiones del forward, backward, KDE, pérdida de ancho y diagnósticos, actualizando \(N=|\mathcal A|\). No dejar una gaussiana espuria en el origen. Conservar su registro como hibernada durante el entrenamiento y dejar de actualizar sus parámetros. No hay mecanismo de recuperación implementado en esta fase.
- Ablación: al finalizar el entrenamiento, las neuronas todavía hibernadas se eliminan definitivamente con sus parámetros y conexiones, sin bypass. La distinción hibernación/ablación es temporal; no realizar ablaciones irreversibles intermedias.
- No hacer simulacros de ablación o reactivación, aprendizaje auxiliar de hibernadas, ni derivadas laterales en el prototipo. Su posibilidad futura está preservada en el ideario.

## 29. Estabilidad, hiperparámetros y cuestiones de implementación abiertas

### 29.1. Estabilidad aprobada

Monitorizar la condición suficiente:
\[
\eta\max_i\sum_{j\in\mathcal N(i)}c_{ij}<1.
\]
Si deja de cumplirse, advertir; no limitar las confianzas ni adaptar \(\eta\) automáticamente. Incumplir una condición suficiente no demuestra necesariamente divergencia. Si no hay aristas activas, no hay intercambio. Observar las dispersiones y resultados finitos. Double para cálculos, protección numérica de \(h\), KDE directa expresamente sin LogSumExp. No introducir mecanismos correctivos conceptuales adicionales sin autorización.

### 29.2. Hiperparámetros identificados (valores sin fijar)

| Magnitud | Función |
|---|---|
| \(N_0\) | Número inicial de neuronas, mayor que uno. |
| \(\tau_{\max}\) | Número fijo de ciclos rizomáticos. |
| \(\eta\) | Tamaño de paso inmanente fijo. |
| \(lr\) | Tasa de aprendizaje del descenso de gradiente. |
| \(\lambda_{\mathrm{GL}}\) | Intensidad constante de Group Lasso. |
| \(E^+_{\min}\) | Mínimo de observaciones para evaluar un régimen. |
| \(\alpha_{\mathrm{TOST}}\) | Nivel de significación de referencia operativo. |
| \(r_m\) | Variación relativa por época considerada despreciable. |
| \(\varepsilon_{\mathrm{tol}}\) | Precisión cognifenomenológica exigida. |
| \(E_{\max}\) | Límite global de épocas. |

\(h\) es parámetro aprendible, no hiperparámetro fijo; \(E^+\) emerge de la segmentación. \(N\) evoluciona por hibernación. No hay duración máxima propia de GL, umbral de poda de entrada ni regularización adicional. Las tolerancias de solvers y protecciones numéricas todavía no tienen valores aprobados.

### 29.3. Detalles no cerrados, a resolver explícitamente al preparar código

- Inicialización degenerada de \(h\), límites representables para \(\exp(\omega_h)\), comportamiento ante densidad cero y divisiones no finitas; no sustituir la evaluación directa de KDE por LogSumExp.
- Componentes de datos de varianza cero, formato de datos y división de conjuntos; semilla, valores concretos de hiperparámetros.
- Mean Shift: tolerancias, máximo de iteraciones, deduplicación y empates. BIC: caso de residuo cero, mínimo de observaciones y rango de búsqueda; TOST: calendario operativo de reevaluación y empates.
- Proyección de confianzas después del proximal uniforme: distinguir el subproblema proximal sin restricciones de su composición con la proyección; sigue pendiente aclarar la anulación del grupo que solo aparece tras proyectar. Mantener las entradas recortadas fuera de futuras actualizaciones. No hacen falta momentos ni tolerancia de bisección.
- Especificación de pruebas: contrastar backward manual con diferencias finitas, simetría, conservación de media inmanente, caso de neurona única, anulación proximal, normalización de KDE tras hibernación y diagnóstico de inestabilidad. Estos son controles de verificación propuestos, no resultados experimentales ni cambios implementados.

## 30. Ideario y frontera de implementación

Conservar, sin implementar ahora, los mecanismos de las secciones históricas: reactivación lateral de conexiones (prioridad alta, primera opción), reactivación lateral neuronal (alta, posterior a conexiones), aprendizaje auxiliar de hibernadas, experiencia actual/histórica y sus asociaciones, conocimiento, influencia comunicativa, liderazgo y meritocracia, confianzas asimétricas, congregaciones persistentes y pulso, excitación continuada, tamaño de paso inmanente adaptativo, simulacros como alternativa futura y lector histórico. No introducir ANN individuales ni mecanismos adicionales para corregir por anticipado las limitaciones observadas.

### Ideario futuro — Adam y minilotes (prioridad baja, 25-11-2026)

- **Adam: prioridad baja.** Pospuesto expresamente por el autor. No mantenerlo como optimizador activo ni desarrollar variantes adicionales en esta versión.
- **Minilotes: prioridad baja.** Pospuestos expresamente por el autor. El prototipo empleará el gradiente medio del conjunto completo de entrenamiento, con una actualización por época.

La formulación histórica de Adam y del proximal diagonal asociado se conserva en la [revisión anterior de §26](https://github.com/meirame/DRANN/blob/091ee090345e87d7d0eb98f2910bfa4535c615f2/docs/DRANN_TEORIA_CONSOLIDADA_2026-11-21.md#26-adam-minilotes-y-group-lasso). Solo reconsiderarlos si el estudio del descenso de gradiente clásico lo justifica y el autor lo aprueba. No confundir esta retirada con la eliminación del backward analítico.

### Ideario futuro — MORFEO (25-11-2026)

**Pospuesto expresamente por el autor: fuera del prototipo actual.** En una versión futura, DRANN se entrenará y se utilizará a través de un programa llamado MORFEO (Morfeo). No implementar MORFEO ni su integración en esta primera versión, ni condicionar el diseño del prototipo a una interfaz futura. Su arquitectura, tecnología, interfaz y mecanismo de integración quedan por definir.

**Siguiente paso:** preparar implementación C++ en CPU del núcleo anterior y contrastarlo experimentalmente, empezando por forward y backward verificados antes de evaluar la reorganización por Group Lasso. Revisar primero los detalles operativos pendientes de la sección 29.3 sin tratarlos como decisiones ya aprobadas. Este checkpoint modifica documentación, no código.
