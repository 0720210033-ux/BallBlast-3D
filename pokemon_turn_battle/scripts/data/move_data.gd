## Representa um golpe (move) simplificado.
## Sem cálculo de efetividade de tipo por enquanto — ponto de expansão futura.
class_name MoveData
extends Resource

@export var move_name: String = "Investida"
@export var move_type: String = "Normal"
@export var power: int = 40
@export var accuracy: int = 100 # 0-100
