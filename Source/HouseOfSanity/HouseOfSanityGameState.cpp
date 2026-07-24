#include "HouseOfSanityGameState.h"
#include "Components/SanityComponent.h"
#include "UI/GameOverWidget.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"

AHouseOfSanityGameState::AHouseOfSanityGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AHouseOfSanityGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHouseOfSanityGameState, CurrentDay);
	DOREPLIFETIME(AHouseOfSanityGameState, CurrentPhase);
	DOREPLIFETIME(AHouseOfSanityGameState, bGameOver);
}

float AHouseOfSanityGameState::GetPhaseLength(ETimePhase Phase) const
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

void AHouseOfSanityGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority())
	{
		return;
	}

	PhaseElapsed += DeltaSeconds;
	if (PhaseElapsed >= GetPhaseLength(CurrentPhase))
	{
		PhaseElapsed = 0.f;
		AdvancePhase();
	}
}

void AHouseOfSanityGameState::AdvancePhase()
{
	switch (CurrentPhase)
	{
		case ETimePhase::Day: EnterPhase(ETimePhase::Dusk); break;
		case ETimePhase::Dusk: EnterPhase(ETimePhase::Night); break;
		case ETimePhase::Night: EnterPhase(ETimePhase::Dawn); break;
		case ETimePhase::Dawn:
			CurrentDay++;
			OnRep_CurrentDay();
			EnterPhase(ETimePhase::Day);
			break;
	}
}

void AHouseOfSanityGameState::EnterPhase(ETimePhase NewPhase)
{
	CurrentPhase = NewPhase;

	if (NewPhase == ETimePhase::Night)
	{
		bNightSurvivedThisCycle = false;
	}
	else if (NewPhase == ETimePhase::Dawn && !bNightSurvivedThisCycle)
	{
		bNightSurvivedThisCycle = true;
		RecoverSanityForAllSurvivors(); // mutates each pawn's own SanityComponent, so this propagates regardless
		OnNightSurvived.Broadcast(CurrentDay); // server/listen-host only - no client RepNotify backs this one
	}

	OnRep_CurrentPhase();
}

void AHouseOfSanityGameState::RecoverSanityForAllSurvivors()
{
	for (APlayerState* PS : PlayerArray)
	{
		if (!PS)
		{
			continue;
		}
		if (APawn* Pawn = PS->GetPawn())
		{
			if (USanityComponent* Sanity = Pawn->FindComponentByClass<USanityComponent>())
			{
				Sanity->AddSanity(SurvivalSanityRecovery);
			}
		}
	}
}

void AHouseOfSanityGameState::OnRep_CurrentDay()
{
	// Called both by real replication on clients and manually on the server
	// (which never gets its own OnRep) so every listener sees exactly one
	// broadcast per day change regardless of net role.
	OnNewDayBegin.Broadcast(CurrentDay);
}

void AHouseOfSanityGameState::OnRep_CurrentPhase()
{
	OnTimePhaseChanged.Broadcast(CurrentPhase);
}

void AHouseOfSanityGameState::NotifyGameOver()
{
	if (bGameOver)
	{
		return;
	}
	bGameOver = true;
	OnRep_GameOver();
}

void AHouseOfSanityGameState::OnRep_GameOver()
{
	if (!bGameOver)
	{
		return;
	}

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	if (GameOverWidgetClass)
	{
		if (UGameOverWidget* Widget = CreateWidget<UGameOverWidget>(PC, GameOverWidgetClass))
		{
			Widget->AddToViewport();
		}
	}

	FInputModeUIOnly InputMode;
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;
}
