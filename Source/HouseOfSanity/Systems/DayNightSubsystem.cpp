#include "Systems/DayNightSubsystem.h"
#include "Components/SanityComponent.h"
#include "Kismet/GameplayStatics.h"

void UDayNightSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CurrentDay = 1;
	CurrentPhase = ETimePhase::Day;
	PhaseElapsed = 0.f;
	bNightSurvivedThisCycle = false;
}

bool UDayNightSubsystem::DoesSupportWorldType(EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

TStatId UDayNightSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UDayNightSubsystem, STATGROUP_Tickables);
}

float UDayNightSubsystem::GetPhaseLength(ETimePhase Phase) const
{
	switch (Phase)
	{
		case ETimePhase::Day: return DayLengthSeconds;
		case ETimePhase::Dusk: return DuskLengthSeconds;
		case ETimePhase::Night: return NightLengthSeconds;
		case ETimePhase::Dawn: return DawnLengthSeconds;
	}
	return 0.f;
}

void UDayNightSubsystem::Tick(float DeltaTime)
{
	PhaseElapsed += DeltaTime;
	if (PhaseElapsed >= GetPhaseLength(CurrentPhase))
	{
		PhaseElapsed = 0.f;
		AdvancePhase();
	}
}

void UDayNightSubsystem::AdvancePhase()
{
	switch (CurrentPhase)
	{
		case ETimePhase::Day: EnterPhase(ETimePhase::Dusk); break;
		case ETimePhase::Dusk: EnterPhase(ETimePhase::Night); break;
		case ETimePhase::Night: EnterPhase(ETimePhase::Dawn); break;
		case ETimePhase::Dawn:
			CurrentDay++;
			EnterPhase(ETimePhase::Day);
			OnNewDayBegin.Broadcast(CurrentDay);
			break;
	}
}

void UDayNightSubsystem::EnterPhase(ETimePhase NewPhase)
{
	CurrentPhase = NewPhase;

	if (NewPhase == ETimePhase::Night)
	{
		bNightSurvivedThisCycle = false;
	}
	else if (NewPhase == ETimePhase::Dawn)
	{
		NotifyNightSurvived();
	}

	OnTimePhaseChanged.Broadcast(NewPhase);
}

void UDayNightSubsystem::NotifyNightSurvived()
{
	if (bNightSurvivedThisCycle)
	{
		return;
	}
	bNightSurvivedThisCycle = true;

	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		if (USanityComponent* Sanity = PlayerPawn->FindComponentByClass<USanityComponent>())
		{
			Sanity->AddSanity(SurvivalSanityRecovery);
		}
	}

	OnNightSurvived.Broadcast(CurrentDay);
}
