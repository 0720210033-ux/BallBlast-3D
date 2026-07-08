## Instância "viva" de um Pokémon dentro de uma batalha: guarda HP atual,
## status calculados por nível e o moveset ativo. PokemonSpecies nunca é
## alterado, só lido — BattlePokemon é o estado mutável de uma partida.
class_name BattlePokemon
extends RefCounted

var species: PokemonSpecies
var level: int
var max_hp: int
var current_hp: int
var attack: int
var defense: int
var speed: int
var moves: Array[MoveData] = []

func _init(p_species: PokemonSpecies, p_level: int = 5) -> void:
	species = p_species
	level = max(1, p_level)
	_calculate_stats()
	moves = p_species.moves.duplicate()
	current_hp = max_hp

## Fórmula de status por nível baseada na série principal (simplificada,
## sem IVs/EVs/natureza) — suficiente para a demo e fácil de estender depois.
func _calculate_stats() -> void:
	max_hp = int((2.0 * species.base_hp * level) / 100.0) + level + 10
	attack = int((2.0 * species.base_attack * level) / 100.0) + 5
	defense = int((2.0 * species.base_defense * level) / 100.0) + 5
	speed = int((2.0 * species.base_speed * level) / 100.0) + 5

func is_fainted() -> bool:
	return current_hp <= 0

func take_damage(amount: int) -> void:
	current_hp = clampi(current_hp - amount, 0, max_hp)

func hp_ratio() -> float:
	if max_hp <= 0:
		return 0.0
	return float(current_hp) / float(max_hp)

func heal_full() -> void:
	current_hp = max_hp
