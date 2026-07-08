## Núcleo do sistema de batalha por turnos, ao estilo Pokémon FireRed.
## Não conhece nada de UI/cena — só recebe dois BattlePokemon e emite sinais.
## A camada visual (battle.gd) escuta esses sinais e atualiza a tela.
##
## Fluxo de um turno:
##   1. Jogador escolhe um golpe (player_choose_move) ou foge (player_run).
##   2. Quem tem maior Velocidade ataca primeiro.
##   3. Cada ataque calcula dano, aplica, atualiza HP, checa desmaio.
##   4. Se ninguém desmaiou, volta para o menu do jogador.
class_name BattleManager
extends Node

signal message_shown(text: String)
signal hp_changed(is_player: bool, current_hp: int, max_hp: int)
signal player_menu_ready
signal battle_ended(player_won: bool, ran_away: bool)

enum State { IDLE, INTRO, PLAYER_MENU, RESOLVING_TURN, ENDED }

var player_pokemon: BattlePokemon
var enemy_pokemon: BattlePokemon
var state: State = State.IDLE

var _awaiting_ack: bool = false
signal _ack_received

func start_battle(p_player: BattlePokemon, p_enemy: BattlePokemon) -> void:
	player_pokemon = p_player
	enemy_pokemon = p_enemy
	state = State.INTRO
	await _show_message("Um %s selvagem apareceu!" % enemy_pokemon.species.species_name)
	await _show_message("Vai, %s!" % player_pokemon.species.species_name)
	_enter_player_menu()

## Chamado pela UI quando o jogador toca na caixa de diálogo para continuar.
func acknowledge_message() -> void:
	if _awaiting_ack:
		_awaiting_ack = false
		_ack_received.emit()

func player_choose_move(move: MoveData) -> void:
	if state != State.PLAYER_MENU:
		return
	state = State.RESOLVING_TURN
	await _resolve_turn(move)

func player_run() -> void:
	if state != State.PLAYER_MENU:
		return
	state = State.RESOLVING_TURN
	await _show_message("Você fugiu da batalha com segurança!")
	_end_battle(false, true)

func _enter_player_menu() -> void:
	if state == State.ENDED:
		return
	state = State.PLAYER_MENU
	player_menu_ready.emit()

func _resolve_turn(player_move: MoveData) -> void:
	var enemy_move: MoveData = enemy_pokemon.moves[randi() % enemy_pokemon.moves.size()]
	var player_first: bool = player_pokemon.speed >= enemy_pokemon.speed

	if player_first:
		await _execute_move(player_pokemon, enemy_pokemon, player_move, true)
		if await _check_faint():
			return
		await _execute_move(enemy_pokemon, player_pokemon, enemy_move, false)
		if await _check_faint():
			return
	else:
		await _execute_move(enemy_pokemon, player_pokemon, enemy_move, false)
		if await _check_faint():
			return
		await _execute_move(player_pokemon, enemy_pokemon, player_move, true)
		if await _check_faint():
			return

	_enter_player_menu()

func _execute_move(attacker: BattlePokemon, defender: BattlePokemon, move: MoveData, attacker_is_player: bool) -> void:
	await _show_message("%s usou %s!" % [attacker.species.species_name, move.move_name])

	if randf() * 100.0 > move.accuracy:
		await _show_message("O ataque errou!")
		return

	var damage := calculate_damage(attacker, defender, move)
	defender.take_damage(damage)
	hp_changed.emit(not attacker_is_player, defender.current_hp, defender.max_hp)
	await _show_message("Causou %d de dano em %s!" % [damage, defender.species.species_name])

## Fórmula SIMPLES de Ataque vs Defesa, conforme pedido para a demo.
## Escalada pelo poder do golpe e por uma pequena variação aleatória
## (85%-100%), igual ao "roll" de dano dos jogos originais.
## Ponto de expansão futura: efetividade de tipo, STAB, golpes críticos,
## Ataque/Defesa especiais, status (paralisia, veneno etc).
func calculate_damage(attacker: BattlePokemon, defender: BattlePokemon, move: MoveData) -> int:
	var attack_defense_ratio := float(attacker.attack) / float(max(1, defender.defense))
	var raw_damage := attack_defense_ratio * move.power * 0.5
	var random_factor := randf_range(0.85, 1.0)
	return max(1, int(round(raw_damage * random_factor)))

func _check_faint() -> bool:
	if enemy_pokemon.is_fainted():
		await _show_message("%s inimigo desmaiou!" % enemy_pokemon.species.species_name)
		_end_battle(true, false)
		return true
	if player_pokemon.is_fainted():
		await _show_message("%s desmaiou!" % player_pokemon.species.species_name)
		_end_battle(false, false)
		return true
	return false

func _end_battle(player_won: bool, ran_away: bool) -> void:
	state = State.ENDED
	battle_ended.emit(player_won, ran_away)

func _show_message(text: String) -> void:
	message_shown.emit(text)
	_awaiting_ack = true
	await _ack_received
