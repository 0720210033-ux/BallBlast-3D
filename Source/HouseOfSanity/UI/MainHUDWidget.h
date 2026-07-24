#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HouseOfSanityTypes.h"
#include "MainHUDWidget.generated.h"

class USanityComponent;

// Base class for the in-game HUD's UMG Blueprint. Exposes what the widget
// needs to bind to - the actual layout/visuals are built in a WBP_ subclass.
UCLASS()
class HOUSEOFSANITY_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void BindToSanityComponent(USanityComponent* InSanityComponent);

	// Sanity is deliberately hidden during the day in the default design -
	// mistakes don't visibly cost you until night makes them matter.
	UFUNCTION(BlueprintNativeEvent, Category = "HUD")
	void OnSanityUpdated(float NewSanity, float MaxSanity, ESanityTier Tier);
	virtual void OnSanityUpdated_Implementation(float NewSanity, float MaxSanity, ESanityTier Tier) {}

	UFUNCTION(BlueprintNativeEvent, Category = "HUD")
	void OnTimePhaseUpdated(ETimePhase NewPhase, int32 DayNumber);
	virtual void OnTimePhaseUpdated_Implementation(ETimePhase NewPhase, int32 DayNumber) {}

	UFUNCTION(BlueprintNativeEvent, Category = "HUD")
	void OnInteractionPromptChanged(const FText& Prompt, bool bVisible);
	virtual void OnInteractionPromptChanged_Implementation(const FText& Prompt, bool bVisible) {}

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleSanityChanged(float NewSanity, float Delta);

	UFUNCTION()
	void HandleTimePhaseChanged(ETimePhase NewPhase);

private:
	UPROPERTY()
	TObjectPtr<USanityComponent> BoundSanityComponent;
};
