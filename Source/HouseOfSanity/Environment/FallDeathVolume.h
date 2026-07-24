#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FallDeathVolume.generated.h"

class UBoxComponent;

// The real floor under the fake bounce house. Whether or not a given
// player was fooled by AFallHallucinationTrap into thinking this was a
// soft landing, physics doesn't negotiate: landing here hard enough is
// lethal regardless of sanity tier. Server-authoritative - the fall
// itself is the hazard, not a perception check.
UCLASS()
class HOUSEOFSANITY_API AFallDeathVolume : public AActor
{
	GENERATED_BODY()

public:
	AFallDeathVolume();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hazard")
	TObjectPtr<UBoxComponent> LethalVolume;

	// Downward speed (uu/s) above which a landing pawn is considered to have
	// fallen from lethal height, rather than just walked in.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hazard")
	float LethalImpactSpeed = 1200.f;

	UFUNCTION(BlueprintNativeEvent, Category = "Hazard")
	void OnFallImpact(APawn* Victim);
	virtual void OnFallImpact_Implementation(APawn* Victim) {}

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
