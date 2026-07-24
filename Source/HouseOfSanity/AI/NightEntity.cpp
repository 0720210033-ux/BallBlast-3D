#include "AI/NightEntity.h"
#include "Components/SanityComponent.h"

ANightEntity::ANightEntity()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ANightEntity::SetEntityState(EEntityState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}
	CurrentState = NewState;
	OnStateChanged(NewState);
}

bool ANightEntity::IsAttackOnCooldown() const
{
	return (GetWorld()->GetTimeSeconds() - LastAttackTime) < AttackCooldownSeconds;
}

bool ANightEntity::TryAttack(APawn* Target)
{
	if (!Target || IsAttackOnCooldown())
	{
		return false;
	}

	if (FVector::Dist(GetActorLocation(), Target->GetActorLocation()) > AttackRange)
	{
		return false;
	}

	LastAttackTime = GetWorld()->GetTimeSeconds();

	USanityComponent* Sanity = Target->FindComponentByClass<USanityComponent>();
	if (!Sanity)
	{
		return false;
	}

	if (Sanity->GetSanityTier() == ESanityTier::Lost)
	{
		return true;
	}

	Sanity->AddSanity(-AttackSanityPenalty);
	OnScarePlayer(Target);
	SetEntityState(EEntityState::Wandering);
	return false;
}
