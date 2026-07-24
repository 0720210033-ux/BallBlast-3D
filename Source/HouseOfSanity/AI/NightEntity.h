#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HouseOfSanityTypes.h"
#include "NightEntity.generated.h"

// The manifestation of the player's fading sanity. It only ever kills when
// the player is already in the Lost tier - everywhere above that, a catch is
// a scare (sanity damage + retreat), not a death, which keeps sanity legible
// as the thing standing between the player and death rather than a hidden stat.
UCLASS()
class HOUSEOFSANITY_API ANightEntity : public ACharacter
{
	GENERATED_BODY()

public:
	ANightEntity();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entity")
	float AttackRange = 150.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entity")
	float AttackSanityPenalty = 25.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entity")
	float AttackCooldownSeconds = 4.f;

	UPROPERTY(BlueprintReadOnly, Category = "Entity")
	EEntityState CurrentState = EEntityState::Dormant;

	UFUNCTION(BlueprintCallable, Category = "Entity")
	void SetEntityState(EEntityState NewState);

	// Returns true if the attack killed the player (only possible at Lost tier).
	UFUNCTION(BlueprintCallable, Category = "Entity")
	bool TryAttack(APawn* Target);

	UFUNCTION(BlueprintPure, Category = "Entity")
	bool IsAttackOnCooldown() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Entity")
	void OnStateChanged(EEntityState NewState);
	virtual void OnStateChanged_Implementation(EEntityState NewState) {}

	UFUNCTION(BlueprintNativeEvent, Category = "Entity")
	void OnScarePlayer(APawn* Target);
	virtual void OnScarePlayer_Implementation(APawn* Target) {}

private:
	float LastAttackTime = -1000.f;
};
