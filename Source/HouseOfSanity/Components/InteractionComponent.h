#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/EngineTypes.h"
#include "InteractionComponent.generated.h"

UCLASS(ClassGroup = (HouseOfSanity), meta = (BlueprintSpawnableComponent))
class HOUSEOFSANITY_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interaction")
	float TraceDistance = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	// Called every tick from the owning pawn so the HUD can show a prompt for
	// whatever is currently in front of the camera.
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AActor* FindFocusedInteractable() const;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryInteract();

protected:
	virtual void BeginPlay() override;

private:
	FVector GetTraceStart() const;
	FVector GetTraceDirection() const;
};
