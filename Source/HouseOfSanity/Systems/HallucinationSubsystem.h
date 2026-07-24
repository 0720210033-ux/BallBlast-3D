#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "HouseOfSanityTypes.h"
#include "HallucinationSubsystem.generated.h"

class UMaterialParameterCollection;

// Turns low sanity into deniable horror. Only active at night: rolls for a
// hallucination event on an interval that shortens as the player's sanity
// tier worsens, and drives a Material Parameter Collection scalar so any
// post-process material can react without C++ needing to own the VFX.
UCLASS(Config = Game)
class HOUSEOFSANITY_API UHallucinationSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Config, BlueprintReadWrite, Category = "Hallucination")
	TSoftObjectPtr<UMaterialParameterCollection> SanityMPC;

	UPROPERTY(EditDefaultsOnly, Config, BlueprintReadWrite, Category = "Hallucination")
	FName DistortionParamName = "Distortion";

	UPROPERTY(EditDefaultsOnly, Config, BlueprintReadWrite, Category = "Hallucination")
	float CheckIntervalSeconds = 1.f;

	// Chance of an event firing per check, per tier (Stable/Uneasy get no rolls).
	UPROPERTY(EditDefaultsOnly, Config, BlueprintReadWrite, Category = "Hallucination")
	float DisturbedEventChance = 0.15f;

	UPROPERTY(EditDefaultsOnly, Config, BlueprintReadWrite, Category = "Hallucination")
	float CriticalEventChance = 0.35f;

	UPROPERTY(EditDefaultsOnly, Config, BlueprintReadWrite, Category = "Hallucination")
	float LostEventChance = 0.6f;

	UPROPERTY(BlueprintAssignable, Category = "Hallucination")
	FOnHallucinationTriggered OnHallucinationTriggered;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;

	// True once sanity tier is Disturbed or worse and it's night - the state
	// where reality can no longer be fully trusted.
	UFUNCTION(BlueprintPure, Category = "Hallucination")
	bool IsPlayerDeceivable() const;

private:
	void EvaluateHallucinationRisk();
	float GetEventChanceForTier(ESanityTier Tier) const;
	void UpdateDistortionParameter(float DeltaTime);

	float TimeSinceLastCheck = 0.f;
	float CurrentDistortion = 0.f;
};
