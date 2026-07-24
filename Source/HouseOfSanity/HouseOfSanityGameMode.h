#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HouseOfSanityGameMode.generated.h"

class UGameOverWidget;

UCLASS()
class HOUSEOFSANITY_API AHouseOfSanityGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHouseOfSanityGameMode();

	// Assign a Blueprint widget (subclassing UGameOverWidget) in a Blueprint
	// child of this GameMode - the widget asset itself can't be authored here.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UGameOverWidget> GameOverWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "House of Sanity")
	void HandlePlayerDeath();

	UPROPERTY(BlueprintReadOnly, Category = "House of Sanity")
	bool bGameOver = false;

private:
	UPROPERTY()
	TObjectPtr<UGameOverWidget> GameOverWidgetInstance;
};
