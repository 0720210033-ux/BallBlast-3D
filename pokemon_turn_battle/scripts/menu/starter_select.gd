## Tela de escolha do inicial: lista as regiões cadastradas em
## PokemonDatabase e monta os cartões dos 3 iniciais sob demanda.
## Os cartões e botões de região são gerados em código para não precisar
## duplicar nós na cena para cada região/Pokémon.
extends Control

@onready var region_list: VBoxContainer = $Content/RegionPanel/RegionScroll/RegionList
@onready var starter_grid: GridContainer = $Content/StarterPanel/StarterScroll/StarterGrid

@onready var detail_panel: Panel = $DetailPanel
@onready var detail_sprite: TextureRect = $DetailPanel/Margin/HBox/DetailSprite
@onready var detail_name_label: Label = $DetailPanel/Margin/HBox/DetailInfo/DetailNameLabel
@onready var detail_stats_label: Label = $DetailPanel/Margin/HBox/DetailInfo/DetailStatsLabel
@onready var confirm_button: Button = $DetailPanel/Margin/HBox/Actions/ConfirmButton
@onready var cancel_button: Button = $DetailPanel/Margin/HBox/Actions/CancelButton

var pending_species: PokemonSpecies = null

func _ready() -> void:
	for region in PokemonDatabase.regions_order:
		var btn := Button.new()
		btn.text = region
		btn.custom_minimum_size = Vector2(0, 40)
		btn.pressed.connect(_on_region_selected.bind(region))
		region_list.add_child(btn)

	confirm_button.pressed.connect(_on_confirm_pressed)
	cancel_button.pressed.connect(_on_cancel_pressed)
	detail_panel.hide()

	if not PokemonDatabase.regions_order.is_empty():
		_on_region_selected(PokemonDatabase.regions_order[0])

func _on_region_selected(region: String) -> void:
	for child in starter_grid.get_children():
		child.queue_free()

	for species in PokemonDatabase.get_starters(region):
		starter_grid.add_child(_build_starter_card(species))

func _build_starter_card(species: PokemonSpecies) -> Button:
	var card := Button.new()
	card.custom_minimum_size = Vector2(130, 130)
	card.pressed.connect(_on_starter_tapped.bind(species))

	var vbox := VBoxContainer.new()
	vbox.mouse_filter = Control.MOUSE_FILTER_IGNORE
	vbox.set_anchors_preset(Control.PRESET_FULL_RECT)
	vbox.alignment = BoxContainer.ALIGNMENT_CENTER

	var sprite := TextureRect.new()
	sprite.texture = PixelSpriteGenerator.generate(species.id, species.sprite_color)
	sprite.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
	sprite.custom_minimum_size = Vector2(72, 72)
	sprite.stretch_mode = TextureRect.STRETCH_KEEP_ASPECT_CENTERED
	sprite.expand_mode = TextureRect.EXPAND_IGNORE_SIZE
	sprite.mouse_filter = Control.MOUSE_FILTER_IGNORE

	var label := Label.new()
	label.text = species.species_name
	label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	label.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	label.mouse_filter = Control.MOUSE_FILTER_IGNORE

	vbox.add_child(sprite)
	vbox.add_child(label)
	card.add_child(vbox)
	return card

func _on_starter_tapped(species: PokemonSpecies) -> void:
	pending_species = species
	detail_sprite.texture = PixelSpriteGenerator.generate(species.id, species.sprite_color)
	detail_sprite.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
	detail_name_label.text = "%s (%s)" % [species.species_name, species.region]
	detail_stats_label.text = "Tipo: %s\nHP %d   ATK %d   DEF %d   VEL %d" % [
		species.get_type_label(), species.base_hp, species.base_attack,
		species.base_defense, species.base_speed,
	]
	detail_panel.show()

func _on_confirm_pressed() -> void:
	if pending_species == null:
		return
	GameState.selected_species = pending_species
	GameState.player_level = 5
	get_tree().change_scene_to_file("res://scenes/Battle.tscn")

func _on_cancel_pressed() -> void:
	pending_species = null
	detail_panel.hide()
