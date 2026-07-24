#include "AI/NightEntityController.h"
#include "AI/NightEntity.h"
#include "Systems/DayNightSubsystem.h"
#include "Components/SanityComponent.h"
#include "HouseOfSanityGameMode.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

ANightEntityController::ANightEntityController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SetPerceptionComponent(*PerceptionComponent);

	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = SightRadius * 1.25f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	SightConfig->SetMaxAge(SightAge);
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;

	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ANightEntityController::HandlePerceptionUpdated);
}

void ANightEntityController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledEntity = Cast<ANightEntity>(InPawn);
	GetWorldTimerManager().SetTimer(BehaviorTimerHandle, this, &ANightEntityController::UpdateBehavior, BehaviorUpdateInterval, true);
}

void ANightEntityController::OnUnPossess()
{
	GetWorldTimerManager().ClearTimer(BehaviorTimerHandle);
	ControlledEntity = nullptr;
	Super::OnUnPossess();
}

void ANightEntityController::HandlePerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Actor && Actor->IsA<APawn>())
	{
		bCanSeePlayer = Stimulus.WasSuccessfullySensed();
	}
}

void ANightEntityController::UpdateBehavior()
{
	if (!ControlledEntity)
	{
		return;
	}

	const UDayNightSubsystem* DayNight = GetWorld()->GetSubsystem<UDayNightSubsystem>();
	if (!DayNight || !DayNight->IsNight())
	{
		ControlledEntity->SetEntityState(EEntityState::Dormant);
		StopMovement();
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	USanityComponent* Sanity = PlayerPawn ? PlayerPawn->FindComponentByClass<USanityComponent>() : nullptr;
	if (!PlayerPawn || !Sanity)
	{
		return;
	}

	EEntityState DesiredState = EEntityState::Wandering;
	switch (Sanity->GetSanityTier())
	{
		case ESanityTier::Stable:
		case ESanityTier::Uneasy:
			DesiredState = EEntityState::Wandering;
			break;
		case ESanityTier::Disturbed:
			DesiredState = EEntityState::Stalking;
			break;
		case ESanityTier::Critical:
		case ESanityTier::Lost:
			DesiredState = EEntityState::Hunting;
			break;
	}

	// Without a perceived player, the entity searches rather than beelines -
	// low sanity raises the stakes, it doesn't grant omniscience.
	if (!bCanSeePlayer && DesiredState != EEntityState::Wandering)
	{
		DesiredState = EEntityState::Wandering;
	}

	ControlledEntity->SetEntityState(DesiredState);

	const float DistanceToPlayer = FVector::Dist(ControlledEntity->GetActorLocation(), PlayerPawn->GetActorLocation());

	switch (DesiredState)
	{
		case EEntityState::Wandering:
		{
			if (GetMoveStatus() == EPathFollowingStatus::Idle)
			{
				if (UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld()))
				{
					FNavLocation RandomLocation;
					if (NavSystem->GetRandomReachablePointInRadius(ControlledEntity->GetActorLocation(), 1500.f, RandomLocation))
					{
						MoveToLocation(RandomLocation.Location);
					}
				}
			}
			break;
		}
		case EEntityState::Stalking:
		{
			if (DistanceToPlayer > StalkingDistance)
			{
				MoveToActor(PlayerPawn, StalkingDistance);
			}
			else
			{
				StopMovement();
			}
			break;
		}
		case EEntityState::Hunting:
		{
			MoveToActor(PlayerPawn, ControlledEntity->AttackRange * 0.8f);
			if (DistanceToPlayer <= ControlledEntity->AttackRange)
			{
				if (ControlledEntity->TryAttack(PlayerPawn))
				{
					if (AHouseOfSanityGameMode* GameMode = GetWorld()->GetAuthGameMode<AHouseOfSanityGameMode>())
					{
						GameMode->HandlePlayerDeath();
					}
				}
			}
			break;
		}
		default:
			break;
	}
}
