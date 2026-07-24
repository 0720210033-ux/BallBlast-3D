#include "Chores/ChoreBase.h"
#include "Components/SanityComponent.h"
#include "Components/SceneComponent.h"
#include "Systems/DayNightSubsystem.h"
#include "GameFramework/Pawn.h"

AChoreBase::AChoreBase()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void AChoreBase::BeginPlay()
{
	Super::BeginPlay();

	if (UDayNightSubsystem* Subsystem = GetWorld()->GetSubsystem<UDayNightSubsystem>())
	{
		Subsystem->OnNewDayBegin.AddDynamic(this, &AChoreBase::HandleNewDay);
	}
}

void AChoreBase::HandleNewDay(int32 DayNumber)
{
	bIsCompleted = false;
	bIsFailed = false;
	bInProgress = false;
	ElapsedTime = 0.f;
	SetActorTickEnabled(false);
}

FText AChoreBase::GetInteractionPrompt_Implementation() const
{
	if (bIsCompleted)
	{
		return FText::Format(NSLOCTEXT("HouseOfSanity", "ChoreDone", "{0} (done)"), ChoreName);
	}
	if (bInProgress)
	{
		return NSLOCTEXT("HouseOfSanity", "ChoreResolve", "Press again to finish");
	}
	return ChoreName;
}

void AChoreBase::Interact_Implementation(APawn* Instigator)
{
	if (bIsCompleted)
	{
		return;
	}

	if (!bInProgress)
	{
		BeginChore(Instigator);
	}
	else
	{
		ResolveChore();
	}
}

void AChoreBase::BeginChore(APawn* Instigator)
{
	CurrentInstigator = Instigator;
	bInProgress = true;
	bIsFailed = false;
	ElapsedTime = 0.f;
	SetActorTickEnabled(true);
}

float AChoreBase::GetMarkerPosition() const
{
	// Sweeps smoothly between 0 and 1, centered target at 0.5.
	return 0.5f + 0.5f * FMath::Sin(ElapsedTime * SweepSpeed);
}

void AChoreBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bInProgress)
	{
		return;
	}

	ElapsedTime += DeltaSeconds;
	OnMarkerUpdated(GetMarkerPosition());
}

void AChoreBase::ResolveChore()
{
	const float Marker = GetMarkerPosition();
	const bool bSuccess = FMath::Abs(Marker - 0.5f) <= (SuccessWindowSize * 0.5f);

	bInProgress = false;
	SetActorTickEnabled(false);

	if (bSuccess)
	{
		bIsCompleted = true;
		bIsFailed = false;
	}
	else
	{
		bIsFailed = true;
		if (APawn* Instigator = CurrentInstigator.Get())
		{
			if (USanityComponent* Sanity = Instigator->FindComponentByClass<USanityComponent>())
			{
				Sanity->ApplyMistakePenalty();
			}
		}
	}

	OnChoreOutcome(bSuccess);
	OnChoreResolved.Broadcast(bSuccess);
}
