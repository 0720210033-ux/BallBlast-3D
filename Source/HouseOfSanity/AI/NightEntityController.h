#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "NightEntityController.generated.h"

class ANightEntity;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;

UCLASS()
class HOUSEOFSANITY_API ANightEntityController : public AAIController
{
	GENERATED_BODY()

public:
	ANightEntityController();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entity AI")
	float BehaviorUpdateInterval = 0.5f;

	// How close Stalking is allowed to get before it holds position - it should
	// be seen, not touching, until sanity drops enough to become Hunting.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entity AI")
	float StalkingDistance = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entity AI")
	float SightRadius = 2000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entity AI")
	float SightAge = 5.f;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	void UpdateBehavior();

	UFUNCTION()
	void HandlePerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UPROPERTY()
	TObjectPtr<UAIPerceptionComponent> PerceptionComponent;

	UPROPERTY()
	TObjectPtr<ANightEntity> ControlledEntity;

	FTimerHandle BehaviorTimerHandle;
	bool bCanSeePlayer = false;
};
