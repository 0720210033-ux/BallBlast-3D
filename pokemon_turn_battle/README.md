# Pokémon Turn Battle — Demo (Godot 4)

Demo de um sistema de batalha por turnos estilo **Pokémon FireRed**, feito em
Godot 4 / GDScript, com visual retrô "Game Boy" e pensado para rodar tanto no
desktop quanto no celular (touch).

> Projeto educacional/fã-feito. Os nomes das espécies são usados apenas como
> referência de gameplay; os "sprites" são gerados proceduralmente em código
> (pixel art simétrica, sem usar nenhuma arte extraída de jogos oficiais).

## Jogar agora (sem instalar nada)

Este ambiente não tem o editor/templates do Godot instalados nem acesso de
rede para baixá-los, então junto do projeto Godot também existe um **build
jogável em HTML/JS** que reproduz o mesmo jogo (mesmos dados, mesma lógica de
batalha, mesmo visual) e roda direto no navegador:

- Abra `web_build/index.html` em qualquer navegador (PC ou celular) — é um
  arquivo único, sem dependências, sem servidor, sem instalação.

Esse build é a versão "já extraída" para jogar. O projeto Godot continua
sendo a fonte principal para quem quiser abrir no editor e continuar
expandindo.

## Como abrir o projeto Godot

1. Abra o Godot 4 (recomendado 4.3+).
2. "Importar" → selecione a pasta `pokemon_turn_battle/` (o arquivo
   `project.godot` está na raiz dela).
3. Rode a cena `res://scenes/Main.tscn` (F5).

No celular: o projeto já está configurado em modo paisagem (`landscape`),
com `stretch mode = canvas_items` para escalar bem em qualquer proporção de
tela, e o renderer está fixado em `gl_compatibility` para rodar até em
aparelhos mais fracos. Para exportar de verdade (APK/IPA/HTML5 oficial do
Godot), use os presets padrão de Android/iOS/Web do Godot — dependem de
SDKs/templates locais que este ambiente não tem.

## Fluxo do jogo

`Main.tscn` (menu) → `StarterSelect.tscn` (escolha do inicial) →
`Battle.tscn` (batalha contra um Pokémon aleatório).

## Arquitetura

```
scripts/
  data/
    move_data.gd        # Resource: um golpe (nome, tipo, poder, precisão)
    pokemon_species.gd  # Resource: dados fixos de uma espécie (Pokédex)
    battle_pokemon.gd   # Instância em batalha (HP atual, status por nível)
  autoload/
    pokemon_database.gd # Singleton: "Pokédex" da demo (iniciais por região)
    game_state.gd        # Singleton: ponte simples entre telas
  battle/
    battle_manager.gd    # Lógica pura de turnos (sem UI, só sinais)
    battle_ui.gd          # Liga a UI da cena aos sinais do BattleManager
  menu/
    main_menu.gd
    starter_select.gd     # Gera os cartões de região/inicial em código
  util/
    gameboy_palette.gd    # Paleta DMG (4 tons de verde) + cor de HP
    pixel_sprite_generator.gd # Gera sprites em pixel art proceduralmente
scenes/
  Main.tscn
  StarterSelect.tscn
  Battle.tscn
web_build/
  index.html          # build jogável em HTML/JS puro (mesma lógica, sem Godot)
```

`BattleManager` é o "motor" da batalha: não sabe nada sobre botões, labels ou
barras de HP — só recebe dois `BattlePokemon` e conversa com o resto do jogo
via sinais (`message_shown`, `hp_changed`, `player_menu_ready`,
`battle_ended`). Isso separa regra de jogo da apresentação, então dá para
trocar toda a UI (ou escrever testes automatizados da lógica) sem tocar no
`BattleManager`.

## A fórmula de dano (conforme pedido: simples, Ataque vs Defesa)

```gdscript
func calculate_damage(attacker, defender, move) -> int:
    var attack_defense_ratio = float(attacker.attack) / float(max(1, defender.defense))
    var raw_damage = attack_defense_ratio * move.power * 0.5
    var random_factor = randf_range(0.85, 1.0)
    return max(1, int(round(raw_damage * random_factor)))
```

Ou seja: `dano ≈ (Ataque / Defesa) * Poder do golpe / 2`, com uma variação
aleatória de 85%-100% (igual ao "roll" de dano dos jogos oficiais). A ordem
de turno é decidida pela **Velocidade** — quem for mais rápido ataca
primeiro, como no FireRed.

Os status (`attack`, `defense`, `speed`, `max_hp`) de cada `BattlePokemon`
são calculados a partir do nível usando a fórmula real da série principal
(simplificada, sem IVs/EVs/natureza) em `battle_pokemon.gd`.

## Pokémon incluídos nesta demo

Iniciais (estágio 1) de **Kanto, Johto, Hoenn, Sinnoh, Unova, Kalos, Alola,
Galar e Paldea** (geração mais recente incluída aqui), além de um bônus de
**Hisui** com as formas regionais evoluídas de *Legends: Arceus*
(Decidueye, Typhlosion e Samurott de Hisui). O jogador pode escolher
qualquer um desses como inicial na tela de seleção.

> Não encontrei nenhum jogo oficial da franquia chamado "Winds e Waves" — se
> você tinha um título específico em mente (ou uma fan game/ROM hack), me
> diga o nome exato dos Pokémon/região que quer incluir e eu adiciono no
> banco de dados (`pokemon_database.gd`) do mesmo jeito que os outros.

## Roadmap de expansão (a demo foi feita para crescer)

- [ ] Efetividade de tipo (tabela fraco/forte contra) no `calculate_damage`
- [ ] Golpes críticos, STAB, ataques especiais (Atq./Def. especiais)
- [ ] Status (paralisia, veneno, sono, confusão...)
- [ ] Time de até 6 Pokémon + tela de troca (`POKÉMON` já existe como botão
      desabilitado na batalha, só falta implementar)
- [ ] Itens/mochila (`MOCHILA` também já está no menu, desabilitado)
- [ ] Evolução por nível/experiência
- [ ] Sprites desenhados à mão substituindo `pixel_sprite_generator.gd`
      (basta trocar a textura carregada, a interface não muda)
- [ ] Música/efeitos sonoros (não incluídos nesta demo)
- [ ] Fonte pixelada (ex.: "Press Start 2P") para reforçar o visual retrô

## Sobre o web_build/

`web_build/index.html` é uma reimplementação independente em JavaScript puro
(sem frameworks) da mesma Pokédex, da mesma fórmula de dano e do mesmo fluxo
de batalha do projeto Godot — pensada para ser aberta direto no navegador
sem qualquer instalação. Ela não lê os arquivos `.gd`/`.tscn`; os dados foram
duplicados manualmente lá dentro. Se você alterar a Pokédex ou a fórmula de
dano no lado Godot (`pokemon_database.gd` / `battle_manager.gd`), replique a
mudança em `web_build/index.html` para os dois ficarem em sincronia.

## Notas técnicas

- Toda a interface usa `Control` com âncoras em porcentagem (não pixels
  fixos), então ela se adapta a qualquer proporção de tela — celular
  incluso. Botões são `Button` comuns, que já respondem a toque nativamente
  no Godot.
- Os "sprites" são gerados em runtime (`PixelSpriteGenerator`), determinísticos
  por `id` da espécie — mesmo Pokémon sempre gera o mesmo desenho.
