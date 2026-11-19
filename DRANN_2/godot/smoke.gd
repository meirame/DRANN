extends SceneTree
func press(scene: Node, title: String) -> void:
	for node in scene.find_children("*", "Button", true, false):
		if node.text == title:
			node.pressed.emit()
			return
	assert(false, "Botón no encontrado: " + title)
func _initialize() -> void:
	call_deferred("run")
func run() -> void:
	var folder = OS.get_cmdline_user_args()[0]
	var scene = load("res://laboratory.tscn").instantiate()
	root.add_child(scene)
	await process_frame
	assert(scene.state.error == "")
	press(scene, "Entrenar / continuar")
	for i in range(20): await process_frame
	press(scene, "Pausar")
	var work = scene.state.epoch * 8 + scene.state.cursor
	for i in range(4): await process_frame
	assert(scene.state.epoch * 8 + scene.state.cursor == work)
	press(scene, "Un ejemplo")
	assert(scene.state.epoch * 8 + scene.state.cursor == work + 1)
	press(scene, "Predecir")
	assert(not scene.state.has("target"))
	var predicted = scene.state.q.duplicate(true)
	press(scene, "Comparar ideal")
	assert(scene.state.has("target") and scene.state.q == predicted)
	assert(scene.lab.save_model(folder + "/ui-current.model", false))
	press(scene, "Reiniciar")
	assert(scene.lab.load_model(folder + "/ui-current.model"))
	scene.refresh()
	assert(scene.state.q == predicted)
	assert(scene.lab.load_model(folder + "/experiment/final.model"))
	assert(scene.lab.inspect(10, false))
	scene.refresh()
	var file = FileAccess.open(folder + "/expected.json", FileAccess.READ)
	var expected = JSON.parse_string(file.get_as_text())
	for i in range(expected.size()):
		for k in range(expected[i].size()):
			assert(absf(expected[i][k] - scene.state.q[i][k]) < 1e-12)
	press(scene, "Comparar ideal")
	scene.widgets.epochs.value = scene.state.epoch + 5
	press(scene, "Entrenar / continuar")
	for i in range(20): await process_frame
	press(scene, "Pausar")
	scene.tau.value = minf(1, scene.tau.max_value)
	await process_frame
	if DisplayServer.get_name() != "headless":
		await RenderingServer.frame_post_draw
		root.get_texture().get_image().save_png(folder + "/screenshot.png")
	print("GODOT_SMOKE_OK: entrenar, pausar, ejemplo, inspección sin ideales, carga y paridad C++.")
	quit()
