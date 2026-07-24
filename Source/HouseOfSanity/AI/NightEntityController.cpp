#include "AI/NightEntityController.h"
#include "AI/NightEntity.h"
#include "HouseOfSanityGameState.h"
#include "Components/SanityComponent.h"
#include "HouseOfSanityGameMode.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "GameFramework/PlayerState.h"
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
	PatrolOrigin = InPawn ? InPawn->GetActorLocation() : FVector::ZeroVector;
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
	APawn* SensedPawn = Cast<APawn>(Actor);
	if (!SensedPawn)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		PerceivedPawns.AddUnique(SensedPawn);
	}
	else
	{
		PerceivedPawns.RemoveAll([SensedPawn](const TWeakObjectPtr<APawn>& Weak) { return Weak.Get() == SensedPawn; });
	}
}

APawn* ANightEntityController::FindMostVulnerablePerceivedPawn() const
{
	APawn* BestPawn = nullptr;
	ESanityTier BestTier = ESanityTier::Stable;
	float BestDistance = TNumericLimits<float>::Max();

	for (const TWeakObjectPtr<APawn>& WeakPawn : PerceivedPawns)
	{
		APawn* Pawn = WeakPawn.Get();
		if (!Pawn)
		{
			continue;
		}

		const USanityComponent* Sanity = Pawn->FindComponentByClass<USanityComponent>();
		if (!Sanity)
		{
			continue;
		}

		const ESanityTier Tier = Sanity->GetSanityTier();
		const float Distance = ControlledEntity ? FVector::Dist(ControlledEntity->GetActorLocation(), Pawn->GetActorLocation()) : 0.f;

		if (!BestPawn || Tier > BestTier || (Tier == BestTier && Distance < BestDistance))
		{
			BestPawn = Pawn;
			BestTier = Tier;
			BestDistance = Distance;
		}
	}

	return BestPawn;
}

void ANightEntityController::UpdateBehavior()
{
	if (!ControlledEntity)
	{
		return;
	}

	const AHouseOfSanityGameState* GameState = GetWorld()->GetGameState<AHouseOfSanityGameState>();
	if (!GameState || !GameState->IsNight())
	{
		ControlledEntity->SetEntityState(EEntityState::Dormant);
		StopMovement();
		return;
	}

	APawn* TargetPawn = FindMostVulnerablePerceivedPawn();
	USanityComponent* TargetSanity = TargetPawn ? TargetPawn->FindComponentByClass<USanityComponent>() : nullptr;

	EEntityState DesiredState = EEntityState::Wandering;
	if (TargetSanity)
	{
		switch (TargetSanity->GetSanityTier())
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
	}

	ControlledEntity->SetEntityState(DesiredState);

	switch (DesiredState)
	{
		case EEntityState::Wandering:
		{
			if (GetMoveStatus() == EPathFollowingStatus::Idle)
			{
				if (UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld()))
				{
					FNavLocation RandomLocation;
					if (NavSystem->GetRandomReachablePointInRadius(PatrolOrigin, WanderRadius, RandomLocation))
					{
						MoveToLocation(RandomLocation.Location);
					}
				}
			}
			break;
		}
		case EEntityState::Stalking:
		{
			const float DistanceToTarget = FVector::Dist(ControlledEntity->GetActorLocation(), TargetPawn->GetActorLocation());
			if (DistanceToTarget > StalkingDistance)
			{
				MoveToActor(TargetPawn, StalkingDistance);
			}
			else
			{
				StopMovement();
			}
			break;
		}
		case EEntityState::Hunting:
		{
			MoveToActor(TargetPawn, ControlledEntity->AttackRange * 0.8f);
			const float DistanceToTarget = FVector::Dist(ControlledEntity->GetActorLocation(), TargetPawn->GetActorLocation());
			if (DistanceToTarget <= ControlledEntity->AttackRange)
			{
				if (ControlledEntity->TryAttack(TargetPawn))
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
