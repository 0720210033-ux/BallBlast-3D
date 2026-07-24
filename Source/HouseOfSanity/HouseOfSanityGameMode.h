#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HouseOfSanityGameMode.generated.h"

UCLASS()
class HOUSEOFSANITY_API AHouseOfSanityGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHouseOfSanityGameMode();

	// The actual game-over presentation lives on AHouseOfSanityGameState,
	// since that's what's replicated to every connected sibling; this just
	// forwards the server's authoritative decision that someone died.
	UFUNCTION(BlueprintCallable, Category = "House of Sanity")
	void HandlePlayerDeath();
};
