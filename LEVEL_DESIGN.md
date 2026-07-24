# House of Sanity — planta da casa

Documento de referência para quem for montar o `Content/Maps/HouseInterior.umap`
no editor. Nada aqui é geometria pronta (isso é asset binário, não dá para
gerar por código) — é a especificação de cômodos, adjacência e onde encaixar
cada sistema já implementado em C++.

## Térreo

| Cômodo | Notas |
|---|---|
| Hall de entrada | Ponto de spawn dos irmãos (vários `PlayerStart`, um por sibling); vão de escada visível para o 1º andar |
| Cozinha | Chore `AChoreWashDishes` na pia |
| Dispensa | Anexa à cozinha, porta própria; boa candidata para um chore extra futuro ou só ambientação |
| Escritório 1 | Sem chore obrigatório — espaço de exploração/hallucinations |
| Escritório 2 | Idem |
| 1 quarto (suíte) | Tem banheiro próprio anexo; chore `AChoreMakeBed` |
| Banheiro da suíte térrea | Privativo, acesso só pelo quarto térreo |
| Sala / corredor central | Conecta todos os cômodos do térreo e a escada; `AChoreSweepFloor` faz sentido aqui |
| Área de serviço / lavanderia (opcional) | `AChoreTakeOutTrash` perto da saída dos fundos |

## 1º andar (4 quartos)

- **Quarto A + Quarto B**: compartilham um banheiro "Jack-and-Jill" — uma porta
  de cada quarto dando no mesmo banheiro, sem acesso pelo corredor.
- **Quarto C**: banheiro próprio (suíte).
- **Quarto D**: banheiro próprio (suíte).

Total no 1º andar: 4 quartos, 3 banheiros (1 compartilhado + 2 privativos).
Todos os 4 quartos têm acesso a um banheiro, nenhum fica sem.

Corredor do 1º andar conecta os 4 quartos e a escada. É o lugar natural para o
**vão da armadilha do pula-pula** (ver seção abaixo).

## A armadilha do pula-pula (`AFallHallucinationTrap` + `AFallDeathVolume`)

Pedido original: "com sanidade baixa vc vê um pula-pula embaixo, vc pula lá de
cima, mas na verdade é só chão mesmo."

Sugestão de posicionamento: um vão aberto no corredor do 1º andar (ou um
"poço de luz" central de dois andares) olhando para o hall de entrada, no
térreo, bem abaixo.

- `AFallDeathVolume` fica no **térreo**, exatamente no ponto de impacto real
  (o chão do hall). É sempre real, sempre sólido, sempre letal em queda de
  ~1 andar — não depende de sanidade.
- `AFallHallucinationTrap` fica no mesmo ponto (ou próximo), mas sua malha
  (`IllusionMesh`, o "pula-pula") só aparece para um cliente específico
  quando **a sanidade daquele jogador** está em `Disturbed` ou pior **e** é
  noite. Cada jogador decide isso localmente e de forma independente — dois
  irmãos podem estar olhando pro mesmo vão e um ver chão vazio, o outro ver
  um colchão inflável convidativo.
- Resultado: um jogador com sanidade baixa pode literalmente pular a
  convite da própria mente e morrer na queda, enquanto os outros (com
  sanidade estável) claramente veem que ali é só um buraco.

Configure `MinimumTierToDeceive` (padrão `Disturbed`) e `LethalImpactSpeed`
(padrão 1200 uu/s) conforme a altura real do vão que vocês construírem.

## Múltiplas entidades

O jogo tem **várias** entidades, não uma só. Cada `ANightEntity` +
`ANightEntityController` já suporta isso sem nenhuma mudança extra: basta
colocar várias instâncias na cena, cada uma em uma área diferente da casa
(ex.: uma no térreo, uma ou duas no 1º andar). `WanderRadius` e o
`PatrolOrigin` (capturado automaticamente na posição onde a entidade foi
colocada) mantêm cada uma vagando pela sua própria região em vez de todas
convergirem para o mesmo canto da casa. Cada entidade avalia
independentemente, entre os irmãos que ela consegue *ver*, qual está com a
sanidade pior, e persegue esse.

## Cooperativo — irmãos e o sistema de disfarce

Cada jogador conectado controla um irmão (`AHouseOfSanityCharacter`,
`SiblingIndex` só para identidade visual/cosmética — atribuam por
`PlayerStart` ou no `GameMode`). O pulo aqui: **a sanidade que importa para
decidir o que você vê é sempre a SUA própria**, nunca a do outro jogador.

`USiblingAppearanceComponent` (em cada personagem) roda localmente em cada
cliente e decide, a cada meio segundo, se aquele OUTRO irmão deve aparecer
com a aparência real (`GetMesh()`) ou com `DisguiseMesh` (uma entidade) —
com base na sanidade do jogador local. Isso não é replicado nem precisa ser:
é puramente visual e por cliente, então dois jogadores olhando para o mesmo
irmão, ao mesmo tempo, podem estar vendo coisas diferentes. Falta no editor:
atribuir a `DisguiseMesh` uma malha de entidade (mesmo esqueleto da malha
real do personagem, para a animação continuar funcionando).

## Resumo de sistemas já existentes por cômodo

| Sistema | Onde faz sentido |
|---|---|
| `AChoreWashDishes` | Cozinha |
| `AChoreMakeBed` | Qualquer quarto (recomenda-se 1 por quarto, ou revezar) |
| `AChoreSweepFloor` | Corredores/sala |
| `AChoreTakeOutTrash` | Cozinha/área de serviço, perto de uma saída |
| `AFallHallucinationTrap` + `AFallDeathVolume` | Vão do corredor do 1º andar sobre o hall |
| `ANightEntity` + `ANightEntityController` (múltiplas) | Uma por andar/ala, espalhadas |
