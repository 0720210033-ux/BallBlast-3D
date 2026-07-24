#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "HouseOfSanityTypes.h"
#include "DayNightSubsystem.generated.h"

class USanityComponent;

// Drives the day/night loop. Chores reset at the start of each day; the
// night phase is when accumulated sanity damage actually matters, since
// UHallucinationSubsystem and ANightEntityController only escalate then.
UCLASS(Config = Game)
class HOUSEOFSANITY_API UDayNightSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Config, BlueprintReadWrite, Category = "Cycle")
	float DayLengthSeconds = 240.f;

	UPROPERTY(EditDefaultsOnly, Config, BlueprintReadWrite, Category = "Cycle")
	float DuskLengthSeconds = 20.f;

	UPROPERTY(EditDefaultsOnly, Config, BlueprintReadWrite, Category = "Cycle")
	float NightLengthSeconds = 180.f;

	UPROPERTY(EditDefaultsOnly, Config, BlueprintReadWrite, Category = "Cycle")
	float DawnLengthSeconds = 20.f;

	// Sanity restored automatically for surviving a night, on top of whatever
	// chores the player completes the next day.
	UPROPERTY(EditDefaultsOnly, Config, BlueprintReadWrite, Category = "Cycle")
	float SurvivalSanityRecovery = 10.f;

	UPROPERTY(BlueprintReadOnly, Category = "Cycle")
	int32 CurrentDay = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Cycle")
	ETimePhase CurrentPhase = ETimePhase::Day;

	UPROPERTY(BlueprintAssignable, Category = "Cycle")
	FOnTimePhaseChanged OnTimePhaseChanged;

	UPROPERTY(BlueprintAssignable, Category = "Cycle")
	FOnNewDayBegin OnNewDayBegin;

	UPROPERTY(BlueprintAssignable, Category = "Cycle")
	FOnNightSurvived OnNightSurvived;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;

	UFUNCTION(BlueprintCallable, Category = "Cycle")
	void NotifyNightSurvived();

	UFUNCTION(BlueprintPure, Category = "Cycle")
	bool IsNight() const { return CurrentPhase == ETimePhase::Night; }

private:
	void AdvancePhase();
	void EnterPhase(ETimePhase NewPhase);
	float GetPhaseLength(ETimePhase Phase) const;

	float PhaseElapsed = 0.f;
	bool bNightSurvivedThisCycle = false;
};
