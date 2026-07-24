#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VictoryWidget.generated.h"

UCLASS()
class HOUSEOFSANITY_API UVictoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Victory")
	FText EscapeMessage = NSLOCTEXT("HouseOfSanity", "DefaultEscapeMessage", "Her slipper hurts less than the seventy days did. You're out.");
};
