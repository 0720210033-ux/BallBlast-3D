#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HouseOfSanityTypes.h"
#include "FallHallucinationTrap.generated.h"

class UStaticMeshComponent;

// The bounce-house-over-a-real-drop trick: purely a local illusion. Every
// client independently decides, from its OWN local player's sanity, whether
// this looks like a soft landing or is simply absent (leaving the drop
// looking exactly like what it is - a hole). No replication involved -
// two siblings standing in the same spot can legitimately see different
// things here. The floor below (AFallDeathVolume) is real either way.
UCLASS()
class HOUSEOFSANITY_API AFallHallucinationTrap : public AActor
{
	GENERATED_BODY()

public:
	AFallHallucinationTrap();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hallucination")
	TObjectPtr<UStaticMeshComponent> IllusionMesh;

	// Sanity tier at/above which the illusion becomes convincing enough to
	// show (Stable/Uneasy always just see the real, empty drop).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hallucination")
	ESanityTier MinimumTierToDeceive = ESanityTier::Disturbed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hallucination")
	float EvaluationIntervalSeconds = 0.5f;

	UFUNCTION(BlueprintNativeEvent, Category = "Hallucination")
	void OnIllusionVisibilityChanged(bool bVisible);
	virtual void OnIllusionVisibilityChanged_Implementation(bool bVisible) {}

protected:
	virtual void BeginPlay() override;

private:
	void EvaluateLocalVisibility();

	FTimerHandle EvaluationTimerHandle;
	bool bIsShowingIllusion = false;
};
