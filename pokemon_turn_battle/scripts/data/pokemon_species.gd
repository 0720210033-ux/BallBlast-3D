## Dados "de fábrica" de uma espécie (equivalente à Pokédex).
## Os status aqui são base stats (nível-independentes); o cálculo por nível
## acontece em BattlePokemon.
class_name PokemonSpecies
extends Resource

@export var id: int = 0
@export var species_name: String = "???"
@export var region: String = "Kanto"
@export var type1: String = "Normal"
@export var type2: String = "" # vazio = mono-tipo
@export var base_hp: int = 45
@export var base_attack: int = 45
@export var base_defense: int = 45
@export var base_speed: int = 45
@export var moves: Array[MoveData] = []
@export var sprite_color: Color = Color.WHITE

func get_type_label() -> String:
	if type2.is_empty():
		return type1
	return "%s/%s" % [type1, type2]
