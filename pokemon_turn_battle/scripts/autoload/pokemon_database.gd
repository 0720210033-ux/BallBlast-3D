## Autoload (Singleton). Guarda a "Pokédex" da demo: os iniciais de cada
## região, agrupados para a tela de seleção, além de uma lista geral usada
## para sortear o time do time inimigo.
##
## Os status base usados aqui são aproximações dos jogos oficiais, apenas
## para fins de demonstração/educacionais — não representam dados extraídos
## de nenhum jogo.
extends Node

var regions_order: Array[String] = [
	"Kanto", "Johto", "Hoenn", "Sinnoh", "Unova",
	"Kalos", "Alola", "Galar", "Hisui", "Paldea",
]

# region -> Array[PokemonSpecies]
var starters_by_region: Dictionary = {}

# lista plana com todas as espécies cadastradas (usada para sortear inimigos)
var all_species: Array[PokemonSpecies] = []

func _ready() -> void:
	_build_database()

func get_starters(region: String) -> Array[PokemonSpecies]:
	return starters_by_region.get(region, [])

func get_random_species(exclude_id: int = -1) -> PokemonSpecies:
	var candidates: Array[PokemonSpecies] = all_species.filter(
		func(s: PokemonSpecies) -> bool: return s.id != exclude_id
	)
	if candidates.is_empty():
		candidates = all_species
	return candidates[randi() % candidates.size()]

func _make_move(name: String, type: String, power: int, accuracy: int = 100) -> MoveData:
	var m := MoveData.new()
	m.move_name = name
	m.move_type = type
	m.power = power
	m.accuracy = accuracy
	return m

func _make_species(
	id: int, name: String, region: String, type1: String, type2: String,
	hp: int, atk: int, def: int, spd: int, color: Color,
	type_move_name: String, type_move_power: int
) -> PokemonSpecies:
	var s := PokemonSpecies.new()
	s.id = id
	s.species_name = name
	s.region = region
	s.type1 = type1
	s.type2 = type2
	s.base_hp = hp
	s.base_attack = atk
	s.base_defense = def
	s.base_speed = spd
	s.sprite_color = color
	s.moves = [
		_make_move("Investida", "Normal", 35, 100),
		_make_move(type_move_name, type1, type_move_power, 95),
	]
	all_species.append(s)
	return s

func _build_database() -> void:
	var id := 1

	# --- Kanto ---
	var kanto: Array[PokemonSpecies] = [
		_make_species(id, "Bulbasaur", "Kanto", "Grama", "Veneno", 45, 49, 49, 45, Color(0.35, 0.75, 0.4), "Chicote de Vinha", 45),
		_make_species(id + 1, "Charmander", "Kanto", "Fogo", "", 39, 52, 43, 65, Color(0.95, 0.45, 0.25), "Brasa", 40),
		_make_species(id + 2, "Squirtle", "Kanto", "Água", "", 44, 48, 65, 43, Color(0.35, 0.55, 0.95), "Jato d'Água", 40),
	]
	starters_by_region["Kanto"] = kanto
	id += 3

	# --- Johto ---
	var johto: Array[PokemonSpecies] = [
		_make_species(id, "Chikorita", "Johto", "Grama", "", 45, 49, 65, 45, Color(0.4, 0.8, 0.45), "Folha Navalha", 45),
		_make_species(id + 1, "Cyndaquil", "Johto", "Fogo", "", 39, 52, 43, 65, Color(0.9, 0.5, 0.2), "Brasa", 40),
		_make_species(id + 2, "Totodile", "Johto", "Água", "", 50, 65, 64, 43, Color(0.25, 0.5, 0.85), "Investida d'Água", 45),
	]
	starters_by_region["Johto"] = johto
	id += 3

	# --- Hoenn ---
	var hoenn: Array[PokemonSpecies] = [
		_make_species(id, "Treecko", "Hoenn", "Grama", "", 40, 45, 35, 70, Color(0.3, 0.75, 0.5), "Chicote de Vinha", 45),
		_make_species(id + 1, "Torchic", "Hoenn", "Fogo", "", 45, 60, 40, 45, Color(0.95, 0.6, 0.2), "Brasa", 40),
		_make_species(id + 2, "Mudkip", "Hoenn", "Água", "", 50, 70, 50, 40, Color(0.3, 0.55, 0.9), "Jato d'Água", 40),
	]
	starters_by_region["Hoenn"] = hoenn
	id += 3

	# --- Sinnoh ---
	var sinnoh: Array[PokemonSpecies] = [
		_make_species(id, "Turtwig", "Sinnoh", "Grama", "", 55, 68, 64, 31, Color(0.45, 0.7, 0.35), "Investida de Folhas", 45),
		_make_species(id + 1, "Chimchar", "Sinnoh", "Fogo", "", 44, 58, 44, 61, Color(0.9, 0.55, 0.25), "Brasa", 40),
		_make_species(id + 2, "Piplup", "Sinnoh", "Água", "", 53, 51, 53, 40, Color(0.3, 0.5, 0.85), "Bolha", 40),
	]
	starters_by_region["Sinnoh"] = sinnoh
	id += 3

	# --- Unova ---
	var unova: Array[PokemonSpecies] = [
		_make_species(id, "Snivy", "Unova", "Grama", "", 45, 45, 55, 63, Color(0.4, 0.8, 0.4), "Chicote de Vinha", 45),
		_make_species(id + 1, "Tepig", "Unova", "Fogo", "", 65, 63, 45, 45, Color(0.9, 0.45, 0.25), "Brasa", 40),
		_make_species(id + 2, "Oshawott", "Unova", "Água", "", 55, 55, 45, 45, Color(0.3, 0.55, 0.9), "Jato d'Água", 40),
	]
	starters_by_region["Unova"] = unova
	id += 3

	# --- Kalos ---
	var kalos: Array[PokemonSpecies] = [
		_make_species(id, "Chespin", "Kalos", "Grama", "", 56, 61, 65, 38, Color(0.55, 0.75, 0.3), "Investida de Folhas", 45),
		_make_species(id + 1, "Fennekin", "Kalos", "Fogo", "", 40, 45, 40, 60, Color(0.95, 0.65, 0.35), "Brasa", 40),
		_make_species(id + 2, "Froakie", "Kalos", "Água", "", 41, 56, 40, 71, Color(0.3, 0.6, 0.85), "Jato d'Água", 40),
	]
	starters_by_region["Kalos"] = kalos
	id += 3

	# --- Alola ---
	var alola: Array[PokemonSpecies] = [
		_make_species(id, "Rowlet", "Alola", "Grama", "Voador", 68, 55, 55, 42, Color(0.45, 0.7, 0.4), "Investida de Folhas", 45),
		_make_species(id + 1, "Litten", "Alola", "Fogo", "", 45, 65, 40, 70, Color(0.85, 0.35, 0.3), "Brasa", 40),
		_make_species(id + 2, "Popplio", "Alola", "Água", "", 50, 54, 54, 40, Color(0.35, 0.65, 0.9), "Jato d'Água", 40),
	]
	starters_by_region["Alola"] = alola
	id += 3

	# --- Galar ---
	var galar: Array[PokemonSpecies] = [
		_make_species(id, "Grookey", "Galar", "Grama", "", 50, 65, 50, 65, Color(0.4, 0.8, 0.35), "Investida de Folhas", 45),
		_make_species(id + 1, "Scorbunny", "Galar", "Fogo", "", 50, 71, 40, 69, Color(0.95, 0.5, 0.3), "Brasa", 40),
		_make_species(id + 2, "Sobble", "Galar", "Água", "", 50, 40, 40, 70, Color(0.4, 0.6, 0.9), "Jato d'Água", 40),
	]
	starters_by_region["Galar"] = galar
	id += 3

	# --- Hisui (Legends: Arceus) — formas regionais evoluídas ---
	var hisui: Array[PokemonSpecies] = [
		_make_species(id, "Decidueye de Hisui", "Hisui", "Grama", "Lutador", 78, 107, 75, 100, Color(0.35, 0.6, 0.35), "Folha Navalha", 55),
		_make_species(id + 1, "Typhlosion de Hisui", "Hisui", "Fogo", "Fantasma", 73, 84, 78, 100, Color(0.55, 0.35, 0.75), "Chama Fantasma", 55),
		_make_species(id + 2, "Samurott de Hisui", "Hisui", "Água", "Sombrio", 90, 108, 80, 85, Color(0.25, 0.4, 0.55), "Corte Noturno", 55),
	]
	starters_by_region["Hisui"] = hisui
	id += 3

	# --- Paldea (geração mais recente incluída nesta demo) ---
	var paldea: Array[PokemonSpecies] = [
		_make_species(id, "Sprigatito", "Paldea", "Grama", "", 40, 61, 54, 65, Color(0.4, 0.75, 0.45), "Arranhão", 45),
		_make_species(id + 1, "Fuecoco", "Paldea", "Fogo", "", 67, 45, 59, 36, Color(0.9, 0.4, 0.3), "Brasa", 40),
		_make_species(id + 2, "Quaxly", "Paldea", "Água", "", 55, 65, 45, 50, Color(0.3, 0.65, 0.85), "Bicada", 40),
	]
	starters_by_region["Paldea"] = paldea
	id += 3
