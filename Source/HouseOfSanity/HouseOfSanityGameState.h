#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HouseOfSanityTypes.h"
#include "HouseOfSanityGameState.generated.h"

class UGameOverWidget;

// Owns the day/night clock. Ticks and advances phases only on the server
// (or standalone); clients receive CurrentDay/CurrentPhase via replication
// and react through the OnRep functions, so every sibling's screen agrees
// on what time it is even though each one hallucinates independently.
UCLASS(Config = Game)
class HOUSEOFSANITY_API AHouseOfSanityGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AHouseOfSanityGameState();

	UPROPERTY(EditDefaultsOnly, Config, BlueprintReadWrite, Category = "Cycle")
	float DayLengthSeconds = 240.f;

	UPROPERTY(EditDefaultsOnly, Config, BlueprintReadWrite, Category = "Cycle")
	float DuskLengthSeconds = 20.f;

	UPROPERTY(EditDefaultsOnly, Config, BlueprintReadWrite, Category = "Cycle")
	float NightLengthSeconds = 180.f;

	UPROPERTY(EditDefaultsOnly, Config, BlueprintReadWrite, Category = "Cycle")
	float DawnLengthSeconds = 20.f;

	UPROPERTY(EditDefaultsOnly, Config, BlueprintReadWrite, Category = "Cycle")
	float SurvivalSanityRecovery = 10.f;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentDay, BlueprintReadOnly, Category = "Cycle")
	int32 CurrentDay = 1;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentPhase, BlueprintReadOnly, Category = "Cycle")
	ETimePhase CurrentPhase = ETimePhase::Day;

	UPROPERTY(BlueprintAssignable, Category = "Cycle")
	FOnTimePhaseChanged OnTimePhaseChanged;

	UPROPERTY(BlueprintAssignable, Category = "Cycle")
	FOnNewDayBegin OnNewDayBegin;

	UPROPERTY(BlueprintAssignable, Category = "Cycle")
	FOnNightSurvived OnNightSurvived;

	// AGameModeBase only ever exists on the server - it has no representation
	// on connected clients at all - so the "someone lost their mind" broadcast
	// has to go through the replicated GameState instead, or remote clients in
	// a co-op session would never see the game-over screen.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UGameOverWidget> GameOverWidgetClass;

	UPROPERTY(ReplicatedUsing = OnRep_GameOver, BlueprintReadOnly, Category = "House of Sanity")
	bool bGameOver = false;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure, Category = "Cycle")
	bool IsNight() const { return CurrentPhase == ETimePhase::Night; }

	UFUNCTION(BlueprintCallable, Category = "House of Sanity")
	void NotifyGameOver();

private:
	void AdvancePhase();
	void EnterPhase(ETimePhase NewPhase);
	float GetPhaseLength(ETimePhase Phase) const;
	void RecoverSanityForAllSurvivors();

	UFUNCTION()
	void OnRep_CurrentDay();

	UFUNCTION()
	void OnRep_CurrentPhase();

	UFUNCTION()
	void OnRep_GameOver();

	float PhaseElapsed = 0.f;
	bool bNightSurvivedThisCycle = false;
};
