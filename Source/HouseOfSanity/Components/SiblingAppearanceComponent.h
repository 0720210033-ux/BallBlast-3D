#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HouseOfSanityTypes.h"
#include "SiblingAppearanceComponent.generated.h"

// Attached to every sibling character. Each client independently checks its
// OWN local player's sanity (never the target's) to decide whether THIS
// other sibling should be shown as themselves or as a disguised entity -
// there is no replicated "who sees what" state, because there can't be:
// two clients looking at the same character are meant to see different
// things once one of them is losing their mind and the other isn't.
UCLASS(ClassGroup = (HouseOfSanity), meta = (BlueprintSpawnableComponent))
class HOUSEOFSANITY_API USiblingAppearanceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USiblingAppearanceComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Appearance")
	ESanityTier MinimumTierToDisguise = ESanityTier::Disturbed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Appearance")
	float EvaluationIntervalSeconds = 0.5f;

protected:
	virtual void BeginPlay() override;

private:
	void EvaluateLocalAppearance();

	FTimerHandle EvaluationTimerHandle;
	bool bIsShowingDisguise = false;
};
