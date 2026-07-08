## Camada de apresentação da batalha: liga os sinais do BattleManager (lógica
## pura) aos elementos visuais estilo Game Boy. Não faz nenhum cálculo de
## jogo — apenas mostra o que o BattleManager já decidiu.
extends Control

@onready var battle_manager: BattleManager = $BattleManager

@onready var enemy_sprite: TextureRect = $EnemySprite
@onready var enemy_name_label: Label = $EnemyPanel/Margin/VBox/NameRow/EnemyNameLabel
@onready var enemy_level_label: Label = $EnemyPanel/Margin/VBox/NameRow/EnemyLevelLabel
@onready var enemy_hp_bar: ProgressBar = $EnemyPanel/Margin/VBox/EnemyHPBar

@onready var player_sprite: TextureRect = $PlayerSprite
@onready var player_name_label: Label = $PlayerPanel/Margin/VBox/NameRow/PlayerNameLabel
@onready var player_level_label: Label = $PlayerPanel/Margin/VBox/NameRow/PlayerLevelLabel
@onready var player_hp_bar: ProgressBar = $PlayerPanel/Margin/VBox/PlayerHPBar
@onready var player_hp_label: Label = $PlayerPanel/Margin/VBox/PlayerHPLabel

@onready var message_label: Label = $BottomArea/MessageBox/Margin/MessageLabel
@onready var advance_overlay: Button = $BottomArea/AdvanceOverlay

@onready var action_menu: GridContainer = $BottomArea/ActionMenu
@onready var fight_button: Button = $BottomArea/ActionMenu/FightButton
@onready var run_button: Button = $BottomArea/ActionMenu/RunButton

@onready var move_menu: VBoxContainer = $BottomArea/MoveMenu
@onready var move_grid: GridContainer = $BottomArea/MoveMenu/MoveGrid
@onready var move_back_button: Button = $BottomArea/MoveMenu/BackButton

@onready var result_overlay: Panel = $ResultOverlay
@onready var result_label: Label = $ResultOverlay/Margin/VBox/ResultLabel
@onready var back_to_menu_button: Button = $ResultOverlay/Margin/VBox/BackToMenuButton

func _ready() -> void:
	var player_species: PokemonSpecies = GameState.selected_species
	if player_species == null:
		# Acesso direto à cena de batalha sem passar pela seleção: usa um
		# inicial padrão para não travar o teste rápido no editor.
		player_species = PokemonDatabase.get_starters("Kanto")[0]

	var player_bp := BattlePokemon.new(player_species, GameState.player_level)
	var enemy_species := PokemonDatabase.get_random_species(player_species.id)
	var enemy_level := max(2, GameState.player_level + randi_range(-1, 2))
	var enemy_bp := BattlePokemon.new(enemy_species, enemy_level)

	_setup_static_ui(player_bp, enemy_bp)

	battle_manager.message_shown.connect(_on_message_shown)
	battle_manager.hp_changed.connect(_on_hp_changed)
	battle_manager.player_menu_ready.connect(_on_player_menu_ready)
	battle_manager.battle_ended.connect(_on_battle_ended)

	advance_overlay.pressed.connect(battle_manager.acknowledge_message)
	fight_button.pressed.connect(_on_fight_pressed)
	run_button.pressed.connect(battle_manager.player_run)
	move_back_button.pressed.connect(_show_action_menu)
	back_to_menu_button.pressed.connect(_on_back_to_menu_pressed)

	action_menu.hide()
	move_menu.hide()
	result_overlay.hide()

	battle_manager.start_battle(player_bp, enemy_bp)

func _setup_static_ui(player_bp: BattlePokemon, enemy_bp: BattlePokemon) -> void:
	enemy_sprite.texture = PixelSpriteGenerator.generate(enemy_bp.species.id, enemy_bp.species.sprite_color)
	enemy_sprite.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
	enemy_name_label.text = enemy_bp.species.species_name
	enemy_level_label.text = "Nv%d" % enemy_bp.level
	enemy_hp_bar.max_value = enemy_bp.max_hp
	enemy_hp_bar.value = enemy_bp.current_hp
	enemy_hp_bar.modulate = GBPalette.hp_color(enemy_bp.hp_ratio())

	player_sprite.texture = PixelSpriteGenerator.generate(player_bp.species.id, player_bp.species.sprite_color)
	player_sprite.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
	player_name_label.text = player_bp.species.species_name
	player_level_label.text = "Nv%d" % player_bp.level
	player_hp_bar.max_value = player_bp.max_hp
	player_hp_bar.value = player_bp.current_hp
	player_hp_bar.modulate = GBPalette.hp_color(player_bp.hp_ratio())
	player_hp_label.text = "%d/%d" % [player_bp.current_hp, player_bp.max_hp]

	for child in move_grid.get_children():
		child.queue_free()
	for move in player_bp.moves:
		var btn := Button.new()
		btn.text = "%s\n(%s)" % [move.move_name, move.move_type]
		btn.custom_minimum_size = Vector2(0, 48)
		btn.pressed.connect(_on_move_pressed.bind(move))
		move_grid.add_child(btn)

func _on_message_shown(text: String) -> void:
	message_label.text = text
	advance_overlay.show()
	action_menu.hide()
	move_menu.hide()

func _on_hp_changed(is_player: bool, current_hp: int, max_hp: int) -> void:
	var ratio := 0.0 if max_hp <= 0 else float(current_hp) / float(max_hp)
	if is_player:
		player_hp_bar.max_value = max_hp
		player_hp_bar.value = current_hp
		player_hp_bar.modulate = GBPalette.hp_color(ratio)
		player_hp_label.text = "%d/%d" % [current_hp, max_hp]
	else:
		enemy_hp_bar.max_value = max_hp
		enemy_hp_bar.value = current_hp
		enemy_hp_bar.modulate = GBPalette.hp_color(ratio)

func _on_player_menu_ready() -> void:
	advance_overlay.hide()
	_show_action_menu()

func _show_action_menu() -> void:
	message_label.text = "O que %s fará?" % battle_manager.player_pokemon.species.species_name
	move_menu.hide()
	action_menu.show()

func _on_fight_pressed() -> void:
	action_menu.hide()
	move_menu.show()

func _on_move_pressed(move: MoveData) -> void:
	move_menu.hide()
	battle_manager.player_choose_move(move)

func _on_battle_ended(player_won: bool, ran_away: bool) -> void:
	action_menu.hide()
	move_menu.hide()
	advance_overlay.hide()
	await get_tree().create_timer(1.0).timeout
	if ran_away:
		result_label.text = "Você fugiu da batalha."
	elif player_won:
		result_label.text = "Você venceu a batalha!"
	else:
		result_label.text = "Seu Pokémon desmaiou..."
	result_overlay.show()

func _on_back_to_menu_pressed() -> void:
	get_tree().change_scene_to_file("res://scenes/Main.tscn")
