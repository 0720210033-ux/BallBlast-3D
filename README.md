# House of Sanity

Jogo de terror doméstico em primeira pessoa: você faz tarefas de casa comuns
(lavar louça, arrumar a cama, varrer o chão, tirar o lixo). Errar uma tarefa
custa sanidade. De dia isso não tem efeito nenhum — a casa parece normal. À
noite, quanto mais baixa sua sanidade, mais a realidade começa a mentir para
você, e uma presença que só existe enquanto você não confia mais nos próprios
olhos passa a caçar. Sobreviver à noite com a sanidade alta é a única forma de
não morrer.

> **Nota sobre a engine**: "Unreal Engine 6" ainda não existe — a versão mais
> recente é a Unreal Engine 5. Este projeto foi escrito para **UE 5.4**.
> Este ambiente é uma sessão de linha de comando sem o Editor instalado, então
> todo o código abaixo foi escrito e revisado por leitura, mas **nunca foi
> compilado ou aberto no Editor**. Abra o `.uproject` numa máquina com UE 5.4
> instalado para gerar os arquivos de projeto, compilar e testar.

## Loop de jogo

1. **Dia**: percorra a casa e complete as tarefas disponíveis (interagir
   inicia um mini-game de tempo/precisão: um marcador oscila entre 0 e 1,
   interagir de novo dentro da janela de acerto = sucesso). Errar aplica
   `-25` de sanidade (configurável), não importa quantas vezes você erre de
   dia — não há consequência visível.
2. **Anoitecer → Noite**: a sanidade acumulada até aqui passa a valer. Quanto
   mais baixa, mais forte a distorção visual/sonora e mais agressiva a
   entidade da noite.
3. **Amanhecer**: sobreviver recupera um pouco de sanidade automaticamente;
   as tarefas resetam para o próximo dia.
4. **Morte**: só acontece se a entidade alcançar o jogador **e** a sanidade
   já estiver em 0 (tier `Lost`). Acima disso, ser pega é um susto (dano de
   sanidade + a entidade recua), não uma morte instantânea — a sanidade é
   literalmente o que impede a morte, não um contador escondido.

## Sanidade — números padrão

| Sanidade (de 150) | Tier | Efeito à noite |
|---|---|---|
| 150 – 125 | `Stable` | Entidade dormente, sem alucinações |
| 100 – 75 | `Uneasy` | Entidade vaga sem perseguir, sem alucinações |
| 50 | `Disturbed` | Entidade persegue a distância (`Stalking`), alucinações leves |
| 25 | `Critical` | Entidade caça (`Hunting`), alucinações fortes |
| 0 | `Lost` | Encontro com a entidade = morte |

Cada erro de tarefa custa 25 pontos (`SanityComponent::MistakePenalty`), então
os degraus acima correspondem a 0–6 erros acumulados. Tudo isso é ajustável
sem recompilar via `DefaultGame.ini` / valores default nas classes.

## Estrutura do código

```
Source/HouseOfSanity/
├── HouseOfSanityTypes.h          Enums e delegates compartilhados
├── HouseOfSanityGameMode.h/.cpp  Wiring do jogo + tela de game over
├── Components/
│   ├── SanityComponent.h/.cpp        Valor de sanidade, tiers, penalidades
│   └── InteractionComponent.h/.cpp   Line trace + chamada de Interact()
├── Interfaces/
│   └── InteractableInterface.h       Contrato de objetos interagíveis
├── Chores/
│   ├── ChoreBase.h/.cpp               Mini-game genérico de tarefa
│   ├── ChoreWashDishes / ChoreMakeBed / ChoreSweepFloor / ChoreTakeOutTrash
├── Character/
│   └── HouseOfSanityCharacter.h/.cpp  Pawn primeira pessoa (Enhanced Input)
├── Systems/
│   ├── DayNightSubsystem.h/.cpp       Ciclo Dia/Dusk/Noite/Amanhecer
│   └── HallucinationSubsystem.h/.cpp  Eventos de alucinação + parâmetro de distorção
├── AI/
│   ├── NightEntity.h/.cpp             Pawn da entidade (ataque/estado)
│   └── NightEntityController.h/.cpp   FSM: Dormant/Wandering/Stalking/Hunting
└── UI/
    ├── MainHUDWidget.h/.cpp           Base C++ do HUD (bind de sanidade/fase)
    └── GameOverWidget.h/.cpp          Base C++ da tela de morte
```

Todas as classes de IA usam `AAIController::MoveToActor`/`MoveToLocation`
diretamente (sem Behavior Tree/Blackboard), exatamente para que todo o
comportamento fique em C++ puro e não dependa de assets binários que eu não
consigo criar neste ambiente.

## O que falta fazer no Editor (assets binários — não dá para gerar por CLI)

Nada disso é opcional: sem esses passos o projeto abre mas não é jogável.

1. **Gerar os arquivos de projeto**: clique direito em `HouseOfSanity.uproject`
   → *Generate Visual Studio project files* (Windows) ou rode
   `RunUAT.sh GenerateProjectFiles` / abra direto no Editor no Linux/Mac.
2. **Criar o nível** `Content/Maps/HouseInterior.umap` (é o mapa referenciado
   em `DefaultEngine.ini`) com a geometria da casa, e adicionar um
   `NavMeshBoundsVolume` cobrindo os cômodos para a IA da entidade conseguir
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
   anexar a câmera em outro lugar.
5. **Entidade da noite**: criar `BP_NightEntity` (filho de `ANightEntity`, com
   malha/animações) e `BP_NightEntityController` (filho de
   `ANightEntityController`), e colocar uma instância na cena. Ajustar
   `SightRadius`, `StalkingDistance`, `AttackRange` conforme o tamanho da casa.
6. **Tarefas**: criar Blueprints filhos das 4 classes em `Chores/` (malha do
   pia/cama/vassoura/lixo, posição na casa) e implementar os eventos
   `OnMarkerUpdated`/`OnChoreOutcome` para tocar VFX/SFX/animação.
7. **UI**: criar `WBP_MainHUD` (filho de `UMainHUDWidget`) e `WBP_GameOver`
   (filho de `UGameOverWidget`) em UMG; atribuir `WBP_GameOver` no campo
   `GameOverWidgetClass` de um `BP_HouseOfSanityGameMode` (filho de
   `AHouseOfSanityGameMode`), e configurar esse Blueprint como GameMode padrão
   do mapa/projeto.
8. **Distorção visual**: criar uma `Material Parameter Collection` chamada
   `MPC_Sanity` com um parâmetro escalar `Distortion`, atribuir em
   `HallucinationSubsystem::SanityMPC` (via `DefaultGame.ini` ou Blueprint), e
   criar um material de post-process que leia esse parâmetro para efeitos
   (chromatic aberration, vinheta, ruído) crescentes com a sanidade baixa.
9. Ligar os eventos `OnHallucinationTriggered` (som falso, prompt de tarefa
   falso, entidade "disfarçada" de inofensiva) a atores/VFX/SFX reais via
   Blueprint ou uma subclasse C++.

## Por que não há Behavior Tree / Blackboard

Decisão deliberada: BTs e Blackboards são assets binários (`.uasset`) que só
podem ser criados dentro do Editor. Toda a lógica da entidade
(`NightEntityController::UpdateBehavior`) roda em C++ com um timer simples,
então o comportamento existe e é revisável em texto sem precisar de nenhum
asset extra para funcionar — só precisa do NavMesh do passo 2 acima.
