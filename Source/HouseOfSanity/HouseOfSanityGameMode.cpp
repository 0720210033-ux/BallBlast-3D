#include "HouseOfSanityGameMode.h"
#include "HouseOfSanityGameState.h"
#include "Character/HouseOfSanityCharacter.h"

AHouseOfSanityGameMode::AHouseOfSanityGameMode()
{
	DefaultPawnClass = AHouseOfSanityCharacter::StaticClass();
	GameStateClass = AHouseOfSanityGameState::StaticClass();
}

void AHouseOfSanityGameMode::HandlePlayerDeath()
{
	if (AHouseOfSanityGameState* GameState = GetGameState<AHouseOfSanityGameState>())
	{
		GameState->NotifyGameOver();
	}
}
