extends Control
# TÍTULOS EDITABLES DE LAS TRES REPRESENTACIONES
const TITLE_RHIZOME = "Rizoma inmanente"
const TITLE_TRANSCENDENT = "Plano transcendente"
const TITLE_LOSS = "Pérdida por época"
var configuration_locked = false
var updating_configuration = false
var node_positions: Array[Vector2] = []
var dragged_neuron = -1
var drag_offset = Vector2.ZERO
var lab: Node
var state: Dictionary = {}
var running = false
var widgets = {}
var tau: HSlider
var axis_x: OptionButton
var axis_y: OptionButton
var info: RichTextLabel
var status: Label
var progress: ProgressBar
var log_loss: CheckButton
var epoch_delay: HSlider
var epoch_waiting = false
var epoch_wait_elapsed = 0.0
var animate_time = 0.0
var last_epoch = -1
var inferred_t = -1
var font = ThemeDB.fallback_font

func number(row: HBoxContainer, text: String, key: String, value: float, step_value: float, max_value: float) -> void:
	var label = Label.new()
	label.text = text
	row.add_child(label)
	var spin = SpinBox.new()
	spin.min_value = 0
	spin.max_value = max_value
	spin.step = step_value
	spin.value = value
	row.add_child(spin)
	widgets[key] = spin

func button(row: HBoxContainer, text: String, action: Callable) -> void:
	var b = Button.new()
	b.text = text
	b.pressed.connect(action)
	row.add_child(b)

func _ready() -> void:
	if not ClassDB.class_exists("DrannLaboratory"):
		GDExtensionManager.load_extension("res://drann.gdextension")
	lab = ClassDB.instantiate("DrannLaboratory")
	add_child(lab)
	var defaults = lab.defaults()
	var box = VBoxContainer.new()
	box.position = Vector2(18, 12)
	add_child(box)
	var row = HBoxContainer.new()
	box.add_child(row)
	number(row, "Neuronas", "neurons", defaults.neurons, 1, 100)
	number(row, "lr", "lr", defaults.lr, 0.001, 1)
	number(row, "Semilla", "seed", defaults.seed, 1, 100000)
	number(row, "Meta épocas", "epochs", defaults.epochs, 1, 10000000)
	var activation = OptionButton.new()
	activation.add_item("Identidad")
	activation.add_item("Tanh")
	activation.select(defaults.activation)
	row.add_child(activation)
	widgets.activation = activation
	button(row, "Reiniciar", reset)
	var controls = HBoxContainer.new()
	box.add_child(controls)
	button(controls, "Entrenar / continuar", start_training)
	button(controls, "Pausar", func(): running = false)
	button(controls, "Un ejemplo", step_example)
	number(controls, "Entrada física", "input", 10, 1, 10000)
	widgets.input.min_value = -10000
	button(controls, "Predecir", func(): running = false; lab.inspect(widgets.input.value, false); refresh())
	button(controls, "Comparar ideal", func(): running = false; lab.inspect(widgets.input.value, true); refresh())
	var files = HBoxContainer.new()
	box.add_child(files)
	var path = LineEdit.new()
	path.custom_minimum_size.x = 420
	path.text = "user://laboratorio.model"
	files.add_child(path)
	button(files, "Guardar actual", func(): lab.set_goal(int(widgets.epochs.value)); lab.save_model(ProjectSettings.globalize_path(path.text), false); refresh())
	button(files, "Guardar mejor", func(): lab.set_goal(int(widgets.epochs.value)); lab.save_model(ProjectSettings.globalize_path(path.text), true); refresh())
	button(files, "Cargar", func(): load_configuration(ProjectSettings.globalize_path(path.text)))
	var inspect_row = HBoxContainer.new()
	box.add_child(inspect_row)
	var label = Label.new()
	label.text = "Paso interno τ (0 = inicial)"
	inspect_row.add_child(label)
	tau = HSlider.new()
	tau.custom_minimum_size.x = 320
	tau.step = 1
	tau.value_changed.connect(func(_v): queue_redraw())
	inspect_row.add_child(tau)
	axis_x = OptionButton.new()
	axis_y = OptionButton.new()
	inspect_row.add_child(axis_x)
	inspect_row.add_child(axis_y)
	axis_x.item_selected.connect(func(_i): queue_redraw())
	axis_y.item_selected.connect(func(_i): queue_redraw())
	var speed_row = HBoxContainer.new()
	box.add_child(speed_row)
	var speed_label = Label.new()
	speed_label.text = "Pausa entre épocas: 1.0 s"
	speed_label.custom_minimum_size.x = 240
	speed_row.add_child(speed_label)
	epoch_delay = HSlider.new()
	epoch_delay.min_value = 0
	epoch_delay.max_value = 5
	epoch_delay.step = 0.1
	epoch_delay.value = 1.0
	epoch_delay.custom_minimum_size.x = 360
	epoch_delay.tooltip_text = "0 s: máxima velocidad. La pausa deja visible la última instantánea de cada época."
	epoch_delay.value_changed.connect(func(value): speed_label.text = "Pausa entre épocas: %.1f s" % value)
	speed_row.add_child(epoch_delay)
	status = Label.new()
	box.add_child(status)
	info = RichTextLabel.new()
	info.position = Vector2(18, 690)
	info.size = Vector2(1760, 300)
	info.bbcode_enabled = false
	add_child(info)
	progress = ProgressBar.new()
	progress.position = Vector2(1370, 610)
	progress.size = Vector2(380, 25)
	add_child(progress)
	log_loss = CheckButton.new()
	log_loss.text = "Pérdida: escala log₁₀"
	log_loss.position = Vector2(1260, 250)
	log_loss.toggled.connect(func(_pressed): queue_redraw())
	add_child(log_loss)
	widgets.neurons.min_value = 1
	widgets.epochs.min_value = 1
	widgets.lr.min_value = 0.000001
	widgets.lr.step = 0.000001
	for key in ["neurons", "lr", "seed", "epochs"]:
		widgets[key].set_update_on_text_changed(true)
		widgets[key].value_changed.connect(func(_value): preview_configuration())
	widgets.activation.item_selected.connect(func(_index): preview_configuration())
	reset()

func preview_configuration() -> void:
	if configuration_locked or updating_configuration: return
	updating_configuration = true
	if lab.reset(int(widgets.neurons.value), widgets.lr.value, int(widgets.seed.value), widgets.activation.selected):
		lab.set_goal(int(widgets.epochs.value))
	refresh()
	updating_configuration = false

func _gui_input(event: InputEvent) -> void:
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT:
		if not event.pressed:
			if dragged_neuron >= 0:
				dragged_neuron = -1
				accept_event()
			return
		for i in range(node_positions.size() - 1, -1, -1):
			if event.position.distance_to(node_positions[i]) <= 22:
				dragged_neuron = i
				drag_offset = node_positions[i] - event.position
				accept_event()
				return
	elif event is InputEventMouseMotion and dragged_neuron >= 0:
		node_positions[dragged_neuron] = (event.position + drag_offset).clamp(Vector2(45, 305), Vector2(545, 555))
		queue_redraw()
		accept_event()

func set_configuration_locked(locked: bool) -> void:
	configuration_locked = locked
	for key in ["neurons", "lr", "seed", "epochs"]:
		if locked: widgets[key].get_line_edit().release_focus()
		widgets[key].editable = not locked
		widgets[key].tooltip_text = "Pulsa Reiniciar para editar la configuración." if locked else "Actualiza el modelo inicial en directo."
	widgets.activation.disabled = locked

func prepare_training() -> bool:
	if not configuration_locked:
		for key in ["neurons", "lr", "seed", "epochs"]:
			if widgets[key].get_line_edit().has_focus(): widgets[key].apply()
		if not lab.reset(int(widgets.neurons.value), widgets.lr.value, int(widgets.seed.value), widgets.activation.selected):
			refresh()
			return false
		if not lab.set_goal(int(widgets.epochs.value)): return false
		set_configuration_locked(true)
		refresh()
	return true

func start_training() -> void:
	if prepare_training(): running = true

func step_example() -> void:
	running = false
	if prepare_training():
		lab.advance()
		refresh()

func load_configuration(path: String) -> void:
	running = false
	if lab.load_model(path):
		updating_configuration = true
		var loaded = lab.snapshot()
		widgets.neurons.value = loaded.neurons
		widgets.lr.value = loaded.lr
		widgets.seed.value = loaded.seed
		widgets.epochs.value = loaded.goal
		widgets.activation.select(loaded.activation)
		epoch_waiting = false
		epoch_wait_elapsed = 0.0
		set_configuration_locked(true)
		updating_configuration = false
	refresh()

func reset() -> void:
	updating_configuration = true
	set_configuration_locked(false)
	for key in ["neurons", "lr", "seed", "epochs"]:
		if widgets[key].get_line_edit().has_focus(): widgets[key].apply()
	running = false
	epoch_waiting = false
	epoch_wait_elapsed = 0.0
	lab.reset(int(widgets.neurons.value), widgets.lr.value, int(widgets.seed.value), widgets.activation.selected)
	lab.set_goal(int(widgets.epochs.value))
	refresh()
	updating_configuration = false

func compact(values) -> String:
	var parts = PackedStringArray()
	for value in values: parts.append("%.6f" % value)
	return "[" + ", ".join(parts) + "]"

func refresh() -> void:
	state = lab.snapshot()
	if not state.has("q"):
		status.text = state.get("error", "Sin modelo")
		return
	if node_positions.size() != state.q.size():
		dragged_neuron = -1
		node_positions.clear()
		for i in range(state.q.size()):
			node_positions.append(Vector2(315, 425) + Vector2.from_angle(TAU * i / state.q.size()) * 115)
	if int(state.dimension) != inferred_t:
		inferred_t = int(state.dimension)
		axis_x.clear()
		axis_y.clear()
		for k in range(inferred_t):
			axis_x.add_item("Eje " + str(k))
			axis_y.add_item("Eje " + str(k))
		axis_y.select(mini(1, inferred_t - 1))
	tau.max_value = state.steps.size()
	status.text = "Época %d | modelo %d | mejor %d | ejemplo %d | entrada %.2f °C | radio %.5f | %s" % [state.epoch, state.model_epoch, state.best_epoch, state.cursor, state.x, state.radius, state.error]
	animate_time = 0.0
	progress.max_value = maxf(1, widgets.epochs.value)
	progress.value = state.epoch
	var text = "Lectura sin ideales — valor físico por eje; ausencia ≠ cero.\n"
	for k in range(state.reading.size()):
		var r = state.reading[k]
		text += "Eje %d: %s | valor %s | integrantes %s | grupos %s | dispersión %.6f\n" % [k, r.status, "%.6f" % (r.value * state.scaleY) if r.value != null else "—", str(r.members), str(r.groups), r.span]
	if not state.history.is_empty():
		var h = state.history.back()
		text += "Train Loss %s | Validation Loss %s | E_k distancia² %s | válidas %s / %d | SSE lectura normalizada %s\n" % [format_loss(h.training), format_loss(h.validation), str(h.distance2), str(h.valid), h.total, str(h.read_sse)]
	for i in range(state.q.size()):
		text += "Neurona %d: q=%s | eje=%d | puntuaciones=%s\n" % [i, compact(state.q[i]), state.axis[i], compact(state.scores[i])]
	if state.get("coincident", false):
		text += "AVISO: pozos coincidentes; identidad geométrica ambigua.\n"
	info.text = text
	queue_redraw()

func _process(delta: float) -> void:
	if running:
		if state.get("epoch", 0) >= widgets.epochs.value:
			running = false
		else:
			if epoch_waiting:
				epoch_wait_elapsed += delta
				if epoch_wait_elapsed >= epoch_delay.value:
					epoch_waiting = false
			if not epoch_waiting:
				var previous_epoch = state.get("epoch", 0)
				if not lab.advance(): running = false
				refresh()
				if state.get("epoch", 0) > previous_epoch:
					epoch_waiting = epoch_delay.value > 0
					epoch_wait_elapsed = 0.0
	animate_time = minf(1.0, animate_time + delta * 2)
	queue_redraw()

func label_at(p: Vector2, text: String, color: Color = Color.WHITE) -> void:
	draw_string(font, p, text, HORIZONTAL_ALIGNMENT_LEFT, -1, 15, color)

func _draw() -> void:
	draw_rect(Rect2(Vector2.ZERO, size), Color("101827"))
	if not state.has("q"): return
	for panel in [Rect2(15, 210, 565, 455), Rect2(620, 210, 565, 455), Rect2(1230, 210, 555, 455)]:
		draw_rect(panel, Color("142033"))
	label_at(Vector2(30, 240), TITLE_RHIZOME)
	label_at(Vector2(30, 270), "Arrastra las neuronas para reorganizar el rizoma")
	label_at(Vector2(640, 240), TITLE_TRANSCENDENT)
	label_at(Vector2(640, 270), "Atracción directa; proyección seleccionada")
	label_at(Vector2(1260, 240), TITLE_LOSS)
	var n = state.q.size()
	var nodes = node_positions
	var input_node = Vector2(60, 425)
	for destination in nodes:
		var direction = (destination - input_node).normalized()
		var tip = destination - direction * 26
		draw_line(input_node, tip, Color("356679"), 1.5)
		draw_line(tip, tip - direction.rotated(0.5) * 11, Color("69cdd4"), 2)
		draw_line(tip, tip - direction.rotated(-0.5) * 11, Color("69cdd4"), 2)
	draw_circle(input_node, 20, Color("69cdd4"))
	label_at(Vector2(25, 465), "Entrada")
	label_at(Vector2(15, 485), "%.2f °C" % state.x)
	label_at(Vector2(25, 580), "Entrada → todas las neuronas (también aisladas)", Color("69cdd4"))
	var potentials = state.initial
	var senders = []
	var receivers = []
	var retired = []
	var index = int(tau.value)
	for step_index in range(index):
		var step = state.steps[step_index]
		if step_index < index - 1: retired.append_array(step.transmitters)
		else:
			senders = step.transmitters
			receivers = step.receivers
		potentials = step.after
	var used = state.steps[index - 1].connections if index > 0 else []
	for edge_index in range(state.edges.size()):
		var edge = state.edges[edge_index]
		draw_line(nodes[edge.i], nodes[edge.j], Color("4f7094") if edge.active else Color("28303b"), 2 if edge.active else 1)
		if used.has(edge_index):
			var i = edge.i
			var j = edge.j
			if senders.has(j):
				i = edge.j
				j = edge.i
			if senders.has(i) and receivers.has(j):
				var a = nodes[i]
				var b = nodes[j]
				draw_line(a, b, Color.YELLOW, 4)
				var tip = a.lerp(b, 0.75)
				var direction = (b - a).normalized()
				draw_line(tip, tip - direction.rotated(0.5) * 15, Color.YELLOW, 3)
				draw_line(tip, tip - direction.rotated(-0.5) * 15, Color.YELLOW, 3)
	for i in range(n):
		var active_edges = 0
		for edge in state.edges:
			if edge.active and (edge.i == i or edge.j == i): active_edges += 1
		var color = Color("59baff")
		if senders.has(i): color = Color.ORANGE
		elif receivers.has(i): color = Color.GREEN
		elif retired.has(i): color = Color.GRAY
		draw_circle(nodes[i], 18, color)
		if active_edges == 0: draw_arc(nodes[i], 25, 0, TAU, 32, Color.MAGENTA, 3)
		label_at(nodes[i] + Vector2(-5, 38), str(i))
	label_at(Vector2(25, 615), "Naranja: transmite | verde: recibe | gris: retirada")
	label_at(Vector2(25, 640), "Aro magenta: aislada")
	var origin = Vector2(900, 460)
	var kx = axis_x.selected
	var ky = axis_y.selected
	var extent = 1.0
	for point in state.q:
		for coordinate in point: extent = maxf(extent, absf(coordinate) * 1.2)
	if state.has("target"):
		for coordinate in state.target: extent = maxf(extent, absf(coordinate) * 1.2)
	var scale_value = 170.0 / extent
	draw_line(origin + Vector2(-210, 0), origin + Vector2(210, 0), Color.WHITE)
	if inferred_t > 1: draw_line(origin + Vector2(0, 180), origin + Vector2(0, -180), Color.WHITE)
	label_at(origin + Vector2(215, 0), "eje %d" % kx)
	if inferred_t > 1: label_at(origin + Vector2(0, -175), "eje %d" % ky)
	for i in range(n):
		var p = state.q[i]
		var b = state.biasT[i]
		var start = origin + Vector2(b[kx], -b[ky] if inferred_t > 1 else 0) * scale_value
		var end = origin + Vector2(p[kx], -p[ky] if inferred_t > 1 else 0) * scale_value
		draw_line(start, end, Color("b7d0ee"), 1)
		draw_circle(start, 4, Color.WHITE)
		var color = Color.GRAY if state.axis[i] < 0 else Color.from_hsv(float(state.axis[i]) / inferred_t, 0.7, 1)
		draw_circle(start.lerp(end, animate_time), 8, color)
		var selected = state.axis[i] >= 0 and state.reading[state.axis[i]].members.has(i)
		if not selected:
			draw_line(end - Vector2(9, 9), end + Vector2(9, 9), Color.WHITE, 2)
			draw_line(end - Vector2(9, -9), end + Vector2(9, -9), Color.WHITE, 2)
		label_at(end + Vector2(8, -8), str(i), color)
	if state.has("target"):
		for k in range(inferred_t):
			var well = origin + Vector2(state.target[k] if k == kx else 0, -state.target[k] if k == ky and inferred_t > 1 else 0) * scale_value
			draw_arc(well, 13, 0, TAU, 32, Color.RED, 2)
			label_at(well + Vector2(10, 10), "ideal %d" % k, Color.RED)
	for k in range(state.reading.size()):
		var r = state.reading[k]
		if r.value != null and (k == kx or k == ky):
			var p = origin + Vector2(r.value if k == kx else 0, -r.value if k == ky and inferred_t > 1 else 0) * scale_value
			draw_rect(Rect2(p - Vector2(6, 6), Vector2(12, 12)), Color.GREEN, false, 2)
	label_at(Vector2(640, 640), "Blanco: sesgo | cruz: fuera de mediana | verde: mediana")
	var plot = Rect2(1370, 330, 380, 230)
	draw_rect(plot, Color("1f3048"))
	label_at(Vector2(1370, 295), "Entrenamiento", Color.CYAN)
	label_at(Vector2(1550, 295), "Validación", Color.ORANGE)
	var logarithmic = log_loss.button_pressed
	draw_set_transform(Vector2(1270, 500), -PI / 2)
	label_at(Vector2.ZERO, "Pérdida geométrica (log₁₀)" if logarithmic else "Pérdida geométrica")
	draw_set_transform(Vector2.ZERO)
	label_at(Vector2(1510, 600), "Época")
	if logarithmic: label_at(Vector2(1300, 655), "Valores ≤ 10^-12 se dibujan en 10^-12")
	var history = state.history
	if history.is_empty(): return
	var bottom = INF if logarithmic else 0.0
	var top = -INF if logarithmic else 0.001
	for h in history:
		for key in ["training", "validation"]:
			var value = loss_coordinate(h[key], logarithmic)
			bottom = minf(bottom, value)
			top = maxf(top, value)
	if logarithmic:
		bottom = floor(bottom)
		top = ceil(top)
	if top <= bottom: top = bottom + 1.0
	var first_epoch = float(history.front().e)
	var last_epoch = float(history.back().e)
	var epoch_span = maxf(1.0, last_epoch - first_epoch)
	for tick in range(5):
		var fraction = tick / 4.0
		var y = plot.end.y - fraction * plot.size.y
		draw_line(Vector2(plot.position.x, y), Vector2(plot.end.x, y), Color("35455c"))
		var value = lerpf(bottom, top, fraction)
		label_at(Vector2(1300, y + 5), "10^%.1f" % value if logarithmic else format_loss(value))
		var x = plot.position.x + fraction * plot.size.x
		label_at(Vector2(x - 10, plot.end.y + 20), "%.0f" % lerpf(first_epoch, last_epoch, fraction))
	for key in ["training", "validation"]:
		var previous = Vector2.ZERO
		for j in range(history.size()):
			var h = history[j]
			var point = plot.position + Vector2((float(h.e) - first_epoch) / epoch_span * plot.size.x, (1 - (loss_coordinate(h[key], logarithmic) - bottom) / (top - bottom)) * plot.size.y)
			var color = Color.CYAN if key == "training" else Color.ORANGE
			if j > 0: draw_line(previous, point, color, 2)
			else: draw_circle(point, 2, color)
			previous = point

func loss_coordinate(value: float, logarithmic: bool) -> float:
	return log(maxf(value, 1e-12)) / log(10.0) if logarithmic else value

func format_loss(value: float) -> String:
	if absf(value) >= 1e-3: return "%.8f" % value
	if value == 0: return "0.0000e+00"
	if absf(value) < 1e-300: return String.num_scientific(value)
	var exponent = int(floor(log(absf(value)) / log(10.0)))
	var mantissa = value / pow(10.0, exponent)
	if round(absf(mantissa) * 10000) >= 100000:
		mantissa /= 10.0
		exponent += 1
	return "%.4fe%s%02d" % [mantissa, "+" if exponent >= 0 else "-", absi(exponent)]
