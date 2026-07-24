#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HouseOfSanityTypes.h"
#include "SanityComponent.generated.h"

// Tracks the owning pawn's sanity. Mistakes made during chores cost a flat
// amount regardless of time of day; only the *consequences* of low sanity
// differ between day and night (see UHallucinationSubsystem / ANightEntityController).
UCLASS(ClassGroup = (HouseOfSanity), meta = (BlueprintSpawnableComponent))
class HOUSEOFSANITY_API USanityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USanityComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sanity")
	float MaxSanity = 150.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sanity")
	float MistakePenalty = 25.f;

	// Sanity tier breakpoints, expressed as a fraction of MaxSanity.
	// Defaults line up with mistake steps of 25 on a 150 max: 150/125 Stable,
	// 100/75 Uneasy, 50 Disturbed, 25 Critical, 0 Lost.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sanity|Tiers")
	float StableThresholdPct = 0.8f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sanity|Tiers")
	float UneasyThresholdPct = 0.45f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sanity|Tiers")
	float DisturbedThresholdPct = 0.3f;

	UPROPERTY(BlueprintReadOnly, Category = "Sanity")
	float CurrentSanity = 0.f;

	UPROPERTY(BlueprintAssignable, Category = "Sanity")
	FOnSanityChanged OnSanityChanged;

	UPROPERTY(BlueprintAssignable, Category = "Sanity")
	FOnSanityDepleted OnSanityDepleted;

	UFUNCTION(BlueprintCallable, Category = "Sanity")
	void ApplyMistakePenalty();

	UFUNCTION(BlueprintCallable, Category = "Sanity")
	void AddSanity(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Sanity")
	void SetSanity(float NewValue);

	UFUNCTION(BlueprintPure, Category = "Sanity")
	float GetSanityPercent() const;

	UFUNCTION(BlueprintPure, Category = "Sanity")
	ESanityTier GetSanityTier() const;

protected:
	virtual void BeginPlay() override;

private:
	bool bHasNotifiedDepletion = false;
};
