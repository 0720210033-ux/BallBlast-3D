## Paleta clássica "DMG" (Game Boy original, 4 tons de verde), usada em toda
## a interface para dar a identidade visual retro.
class_name GBPalette
extends RefCounted

const DARKEST: Color = Color("0f380f")
const DARK: Color = Color("306230")
const LIGHT: Color = Color("8bac0f")
const LIGHTEST: Color = Color("9bbc0f")

const HP_GREEN: Color = Color("58d068")
const HP_YELLOW: Color = Color("f8d030")
const HP_RED: Color = Color("f83030")

static func hp_color(ratio: float) -> Color:
	if ratio > 0.5:
		return HP_GREEN
	if ratio > 0.2:
		return HP_YELLOW
	return HP_RED
