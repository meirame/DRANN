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
