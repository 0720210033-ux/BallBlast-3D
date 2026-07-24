# House of Sanity

Jogo de terror doméstico em primeira pessoa, cooperativo: você e seus irmãos
fazem tarefas de casa comuns (lavar louça, arrumar a cama, varrer o chão,
tirar o lixo) numa casa de dois andares. Errar uma tarefa custa sanidade. De
dia isso não tem efeito nenhum — a casa parece normal. À noite, quanto mais
baixa sua sanidade, mais a realidade começa a mentir para você: vários seres
que só existem enquanto você não confia mais nos próprios olhos passam a
caçar, e até os próprios irmãos podem deixar de parecer quem são. Sobreviver
à noite com a sanidade alta é a única forma de não morrer — e de continuar
enxergando a verdade.

> **Nota sobre a engine**: "Unreal Engine 6" ainda não existe — a versão mais
> recente é a Unreal Engine 5. Este projeto foi escrito para **UE 5.4**.
> Este ambiente é uma sessão de linha de comando sem o Editor instalado, então
> todo o código abaixo foi escrito e revisado por leitura, mas **nunca foi
> compilado ou aberto no Editor**. Abra o `.uproject` numa máquina com UE 5.4
> instalado para gerar os arquivos de projeto, compilar e testar.

Veja também **`LEVEL_DESIGN.md`** para a planta da casa (cômodos, banheiros,
onde encaixar cada chore/entidade/armadilha).

## Loop de jogo

1. **Dia**: percorra a casa e complete as tarefas disponíveis (interagir
   inicia um mini-game de tempo/precisão: um marcador oscila entre 0 e 1,
   interagir de novo dentro da janela de acerto = sucesso). Errar aplica
   `-25` de sanidade (configurável), não importa quantas vezes você erre de
   dia — não há consequência visível.
2. **Anoitecer → Noite**: a sanidade acumulada até aqui passa a valer. Quanto
   mais baixa, mais forte a distorção visual/sonora, mais agressivas as
   entidades da noite (são várias, não uma só) e mais convincentes as
   ilusões ambientais — como uma armadilha específica: um vão na casa que,
   com sanidade baixa, parece ter um pula-pula esperando lá embaixo. Não tem.
   É só o chão, um andar abaixo.
3. **Amanhecer**: sobreviver recupera um pouco de sanidade automaticamente;
   as tarefas resetam para o próximo dia.
4. **Morte**: pode acontecer de duas formas —
   - uma entidade alcança o jogador **e** a sanidade dele já está em 0
     (tier `Lost`). Acima disso, ser pego é um susto (dano de sanidade + a
     entidade recua), não morte instantânea;
   - o jogador cai de uma altura letal (ex.: acreditando na ilusão do
     pula-pula). Isso mata **independente da sanidade** — a ilusão só
     controla se você é convencido a pular; a física da queda é sempre real.
5. **Dia 70 — o resgate**: ao amanhecer do dia 70 (`EscapeDayNumber`, ajustável),
   o pesadelo acaba de forma roteirizada, não por mérito de jogo: a mãe
   aparece, dá uma chinelada, a sanidade de todo mundo volta ao máximo na
   hora, e vocês escapam da casa. É a condição de vitória — não depende de
   quantos erros vocês acumularam ou de quão bem jogaram, só do calendário.

## Cooperativo — irmãos e o sistema de disfarce

Cada jogador conectado controla um dos irmãos. O mecanismo central: **a
sanidade que decide o que você vê é sempre a sua própria** — nunca a do
colega. Se a sua sanidade cair o suficiente à noite, os outros jogadores
passam a aparecer, só para você, como entidades em vez de irmãos. É uma
troca puramente visual e local a cada cliente (não é replicada, nem
precisa ser — dois jogadores olhando pro mesmo personagem, ao mesmo tempo,
podem estar vendo coisas diferentes, e é exatamente esse o ponto).

## Sanidade — números padrão

| Sanidade (de 150) | Tier | Efeito à noite |
|---|---|---|
| 150 – 125 | `Stable` | Entidades dormentes, sem alucinações, irmãos aparecem normalmente |
| 100 – 75 | `Uneasy` | Entidades vagam sem perseguir, sem alucinações |
| 50 | `Disturbed` | Entidades perseguem a distância (`Stalking`), alucinações leves, irmãos passam a parecer entidades pra você |
| 25 | `Critical` | Entidades caçam (`Hunting`), alucinações fortes |
| 0 | `Lost` | Encontro com uma entidade = morte |

Cada erro de tarefa custa 25 pontos (`SanityComponent::MistakePenalty`), então
os degraus acima correspondem a 0–6 erros acumulados. Tudo isso é ajustável
sem recompilar via `DefaultGame.ini` / valores default nas classes.

## Estrutura do código

```
Source/HouseOfSanity/
├── HouseOfSanityTypes.h            Enums e delegates compartilhados
├── HouseOfSanityGameMode.h/.cpp    Wiring do jogo (server-only)
├── HouseOfSanityGameState.h/.cpp   Ciclo dia/noite + game-over, replicados
├── Components/
│   ├── SanityComponent.h/.cpp           Valor de sanidade, tiers, penalidades
│   ├── InteractionComponent.h/.cpp      Line trace + Server RPC de Interact()
│   └── SiblingAppearanceComponent.h/.cpp Disfarce de irmãos (local, por cliente)
├── Interfaces/
│   └── InteractableInterface.h          Contrato de objetos interagíveis
├── Chores/
│   ├── ChoreBase.h/.cpp                  Mini-game genérico de tarefa (replicado)
│   └── ChoreWashDishes / ChoreMakeBed / ChoreSweepFloor / ChoreTakeOutTrash
├── Character/
│   └── HouseOfSanityCharacter.h/.cpp     Pawn primeira pessoa (Enhanced Input)
├── Systems/
│   └── HallucinationSubsystem.h/.cpp     Eventos de alucinação + parâmetro de distorção (local)
├── AI/
│   ├── NightEntity.h/.cpp                Pawn da entidade (ataque/estado)
│   └── NightEntityController.h/.cpp      FSM: Dormant/Wandering/Stalking/Hunting, mira no irmão mais vulnerável que enxerga
├── Environment/
│   ├── FallHallucinationTrap.h/.cpp      Ilusão do "pula-pula" (local, por cliente)
│   └── FallDeathVolume.h/.cpp            Chão real embaixo (sempre letal, servidor)
└── UI/
    ├── MainHUDWidget.h/.cpp              Base C++ do HUD (bind de sanidade/fase)
    ├── GameOverWidget.h/.cpp             Base C++ da tela de morte
    └── VictoryWidget.h/.cpp              Base C++ da tela de fuga (dia 70)
```

Todas as classes de IA usam `AAIController::MoveToActor`/`MoveToLocation`
diretamente (sem Behavior Tree/Blackboard), exatamente para que todo o
comportamento fique em C++ puro e não dependa de assets binários que eu não
consigo criar neste ambiente.

## Arquitetura de multiplayer (o que é replicado, o que é só local)

- **`AHouseOfSanityGameState`** é a única fonte de verdade sobre dia/noite:
  só o servidor avança o relógio (`Tick` sai cedo se `!HasAuthority()`), e
  `CurrentDay`/`CurrentPhase`/`bGameOver`/`bHasEscaped` são `Replicated` para
  todo mundo ver a mesma coisa. Isso substitui uma versão anterior baseada
  num `WorldSubsystem` local — que funcionaria sozinho, mas cada cliente
  teria seu próprio relógio dessincronizado assim que houvesse mais de um
  jogador. Ao chegar no `EscapeDayNumber` (dia 70 por padrão), o relógio
  chama `TriggerMotherRescue()` e para de vez (`Tick` sai cedo se
  `bHasEscaped`) — não tem mais dia depois disso, o jogo terminou.
- **`AHouseOfSanityGameMode` só existe no servidor** (comportamento padrão da
  Unreal) — por isso a tela de game over não pode viver lá: `HandlePlayerDeath()`
  apenas repassa a decisão para `GameState->NotifyGameOver()`, que É replicado
  e dispara a apresentação (widget, input mode) em cada cliente
  independentemente via `OnRep_GameOver`. A fuga do dia 70 segue exatamente
  o mesmo padrão (`bHasEscaped`/`OnRep_HasEscaped`/`VictoryWidgetClass`),
  só que é o próprio `GameState` que decide disparar sozinho, sem passar
  pelo GameMode — não existe um "evento de morte" externo para escutar aqui,
  é só o calendário chegando no dia certo.
- **Sanidade de cada jogador é local por natureza**: o que decide se você
  vê alucinações, se um irmão vira entidade pros seus olhos, ou se o
  pula-pula parece real, é sempre a sanidade do jogador que está *olhando* —
  nunca a de quem está sendo olhado. Por isso `HallucinationSubsystem`,
  `SiblingAppearanceComponent` e `FallHallucinationTrap` não replicam nada:
  cada cliente calcula sozinho, a partir do próprio jogador local
  (`UGameplayStatics::GetPlayerPawn(GetWorld(), 0)`), sem precisar saber a
  sanidade de ninguém mais.
- **Tarefas (`AChoreBase`) são replicadas** (`bIsCompleted`/`bIsFailed`/
  `bInProgress`) para todos verem o mesmo estado, e a interação em si passa
  por um `Server RPC` (`UInteractionComponent::ServerTryInteract`) antes de
  mutar esse estado — do contrário, um cliente completando uma tarefa
  localmente nunca se propagaria para o resto do grupo.
- **Decisão consciente de escopo**: o RPC de interação confia no alvo que o
  cliente informa (sem revalidar distância/linha de visão no servidor). Para
  jogar com amigos de confiança isso é suficiente; um servidor público
  hostil precisaria de validação extra ali.
- **Morte por queda (`FallDeathVolume`) é sempre autoritativa no servidor**
  e não depende de sanidade nenhuma — é a física real por trás da ilusão.

## O que falta fazer no Editor (assets binários — não dá para gerar por CLI)

Nada disso é opcional: sem esses passos o projeto abre mas não é jogável.

1. **Gerar os arquivos de projeto**: clique direito em `HouseOfSanity.uproject`
   → *Generate Visual Studio project files* (Windows) ou rode
   `RunUAT.sh GenerateProjectFiles` / abra direto no Editor no Linux/Mac.
2. **Criar o nível** `Content/Maps/HouseInterior.umap` (é o mapa referenciado
   em `DefaultEngine.ini`) seguindo a planta de `LEVEL_DESIGN.md` (2 andares,
   5 quartos no total, banheiros, escritórios, cozinha+dispensa), e adicionar
   um `NavMeshBoundsVolume` cobrindo os cômodos para as entidades conseguirem
   navegar (`UNavigationSystemV1`/`MoveToActor` dependem disso).
3. **Enhanced Input**: criar `IMC_Default` (Input Mapping Context) e as ações
   `IA_Move` (Axis2D), `IA_Look` (Axis2D), `IA_Interact` (Digital), mapear
   WASD/mouse/gamepad, e atribuir os quatro assets nos campos
   `DefaultMappingContext/MoveAction/LookAction/InteractAction` de
   `AHouseOfSanityCharacter` (direto nos defaults da classe ou num Blueprint
   filho `BP_HouseOfSanityCharacter`).
4. **Malha do personagem**: atribuir um Skeletal Mesh em `BP_HouseOfSanityCharacter`
   com um socket chamado `head` (padrão do template First Person da Epic) —
   ou trocar o `SetupAttachment` em `HouseOfSanityCharacter.cpp` se preferir
   anexar a câmera em outro lugar. Atribuir também a `DisguiseMesh` uma malha
   de entidade compatível com o mesmo esqueleto, para o disfarce funcionar.
5. **Entidades da noite (várias)**: criar `BP_NightEntity` (filho de
   `ANightEntity`, com malha/animações) e `BP_NightEntityController` (filho
   de `ANightEntityController`), e colocar **várias instâncias** espalhadas
   pela casa (ver `LEVEL_DESIGN.md`). Ajustar `SightRadius`,
   `StalkingDistance`, `AttackRange`, `WanderRadius` conforme o cômodo/andar
   de cada uma.
6. **Tarefas**: criar Blueprints filhos das 4 classes em `Chores/` (malha da
   pia/cama/vassoura/lixo, posição na casa) e implementar os eventos
   `OnMarkerUpdated`/`OnChoreOutcome` para tocar VFX/SFX/animação.
7. **UI**: criar `WBP_MainHUD` (filho de `UMainHUDWidget`), `WBP_GameOver`
   (filho de `UGameOverWidget`) e `WBP_Victory` (filho de `UVictoryWidget`)
   em UMG; atribuir `WBP_GameOver`/`WBP_Victory` nos campos
   `GameOverWidgetClass`/`VictoryWidgetClass` de um `BP_HouseOfSanityGameState`
   (filho de `AHouseOfSanityGameState`), e configurar esse Blueprint como
   GameState padrão do mapa/projeto. `WBP_Victory` é a tela do dia 70 — a
   chinelada da mãe e a fuga.
8. **Distorção visual**: criar uma `Material Parameter Collection` chamada
   `MPC_Sanity` com um parâmetro escalar `Distortion`, atribuir em
   `HallucinationSubsystem::SanityMPC` (via `DefaultGame.ini` ou Blueprint), e
   criar um material de post-process que leia esse parâmetro para efeitos
   (chromatic aberration, vinheta, ruído) crescentes com a sanidade baixa.
9. **Armadilha do pula-pula**: posicionar `AFallHallucinationTrap` (com a
   malha do pula-pula em `IllusionMesh`) e `AFallDeathVolume` conforme
   `LEVEL_DESIGN.md`, e implementar os eventos `OnIllusionVisibilityChanged`/
   `OnFallImpact` para VFX/SFX (som convidativo quando a ilusão aparece, tela
   escurecendo/som de impacto na queda real).
10. Ligar os eventos `OnHallucinationTriggered` (som falso, prompt de tarefa
    falso, entidade "disfarçada" de inofensiva) a atores/VFX/SFX reais via
    Blueprint ou uma subclasse C++.
11. **Multiplayer**: posicionar múltiplos `PlayerStart` (um por irmão) e
    testar com "Number of Players" > 1 no PIE (Play In Editor) para checar
    o disfarce entre irmãos e a sincronização do ciclo dia/noite.
12. **Cutscene da chinelada**: ligar `GameState->OnMotherRescue` (dispara com
    o número do dia) a uma sequência real — Level Sequence com a mãe
    entrando, animação da chinelada, e então `WBP_Victory` na tela. Ajustar
    `EscapeDayNumber` em `DefaultGame.ini` se 70 dias for longo demais para
    testar no editor (reduzir temporariamente para validar o fluxo).

## Por que não há Behavior Tree / Blackboard

Decisão deliberada: BTs e Blackboards são assets binários (`.uasset`) que só
podem ser criados dentro do Editor. Toda a lógica da entidade
(`NightEntityController::UpdateBehavior`) roda em C++ com um timer simples,
então o comportamento existe e é revisável em texto sem precisar de nenhum
asset extra para funcionar — só precisa do NavMesh do passo 2 acima.
