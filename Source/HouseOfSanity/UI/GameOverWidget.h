#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

UCLASS()
class HOUSEOFSANITY_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Over")
	FText DeathMessage = NSLOCTEXT("HouseOfSanity", "DefaultDeathMessage", "Your mind gave out before your body did.");

	UFUNCTION(BlueprintCallable, Category = "Game Over")
	void RestartGame();
};
