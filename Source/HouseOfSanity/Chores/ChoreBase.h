#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"
#include "HouseOfSanityTypes.h"
#include "ChoreBase.generated.h"

class USanityComponent;

// Generic household chore: interacting starts a timing skill-check (a marker
// sweeps back and forth; interacting again while it's inside the target
// window succeeds). Subclasses only need to set flavor/difficulty and react
// to the outcome - the skill-check itself lives here so every chore behaves
// consistently.
UCLASS(Abstract)
class HOUSEOFSANITY_API AChoreBase : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	AChoreBase();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Chore")
	FText ChoreName;

	// Fraction (0-1) of the sweep centered on 0.5 that counts as a success.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Chore", meta = (ClampMin = "0.05", ClampMax = "1.0"))
	float SuccessWindowSize = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Chore")
	float SweepSpeed = 2.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Chore")
	bool bIsCompleted = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Chore")
	bool bIsFailed = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Chore")
	bool bInProgress = false;

	UPROPERTY(BlueprintAssignable, Category = "Chore")
	FOnChoreResolved OnChoreResolved;

	virtual void Interact_Implementation(APawn* Instigator) override;
	virtual FText GetInteractionPrompt_Implementation() const override;

	UFUNCTION(BlueprintPure, Category = "Chore")
	float GetMarkerPosition() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Chore")
	void OnMarkerUpdated(float Position);
	virtual void OnMarkerUpdated_Implementation(float Position) {}

	UFUNCTION(BlueprintNativeEvent, Category = "Chore")
	void OnChoreOutcome(bool bSuccess);
	virtual void OnChoreOutcome_Implementation(bool bSuccess) {}

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void BeginChore(APawn* Instigator);
	void ResolveChore();

	UFUNCTION()
	void HandleNewDay(int32 DayNumber);

private:
	TWeakObjectPtr<APawn> CurrentInstigator;
	float ElapsedTime = 0.f;
};
