## Gera um "sprite" de monstro em pixel art proceduralmente, a partir de um
## id (semente) e uma cor base. Evita depender de arquivos de imagem
## externos na demo e mantém tudo determinístico (mesmo id = mesmo sprite).
## Pode ser substituído depois por sprites de arte feitos à mão sem mexer
## no resto do jogo — só trocar esta função por um carregamento de textura.
class_name PixelSpriteGenerator
extends RefCounted

const GRID: int = 16

static func generate(seed_value: int, base_color: Color) -> ImageTexture:
	var rng := RandomNumberGenerator.new()
	rng.seed = seed_value
	var img := Image.create_empty(GRID, GRID, false, Image.FORMAT_RGBA8)
	img.fill(Color(0, 0, 0, 0))

	var half := GRID / 2
	var outline := base_color.darkened(0.6)

	for y in range(GRID):
		for x in range(half):
			# margem transparente para não colar nas bordas do grid
			var edge_margin := (x < 1) or (y < 1) or (y > GRID - 2)
			var density := 0.62 if not edge_margin else 0.3
			if rng.randf() < density:
				var shade := base_color.lightened(rng.randf_range(-0.15, 0.25))
				_set_mirrored(img, x, y, shade, half)

	_apply_outline(img, outline)
	var tex := ImageTexture.create_from_image(img)
	return tex

static func _set_mirrored(img: Image, x: int, y: int, color: Color, half: int) -> void:
	img.set_pixel(x, y, color)
	img.set_pixel(img.get_width() - 1 - x, y, color)

static func _apply_outline(img: Image, outline_color: Color) -> void:
	var w := img.get_width()
	var h := img.get_height()
	var original: Image = img.duplicate()
	for y in range(h):
		for x in range(w):
			if original.get_pixel(x, y).a > 0.0:
				continue
			if _has_opaque_neighbor(original, x, y, w, h):
				img.set_pixel(x, y, outline_color)

static func _has_opaque_neighbor(img: Image, x: int, y: int, w: int, h: int) -> bool:
	var offsets := [Vector2i(1, 0), Vector2i(-1, 0), Vector2i(0, 1), Vector2i(0, -1)]
	for off in offsets:
		var nx: int = x + off.x
		var ny: int = y + off.y
		if nx < 0 or ny < 0 or nx >= w or ny >= h:
			continue
		if img.get_pixel(nx, ny).a > 0.0:
			return true
	return false
