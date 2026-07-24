#include "Components/SanityComponent.h"

USanityComponent::USanityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USanityComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentSanity = MaxSanity;
}

void USanityComponent::ApplyMistakePenalty()
{
	SetSanity(CurrentSanity - MistakePenalty);
}

void USanityComponent::AddSanity(float Amount)
{
	SetSanity(CurrentSanity + Amount);
}

void USanityComponent::SetSanity(float NewValue)
{
	const float Clamped = FMath::Clamp(NewValue, 0.f, MaxSanity);
	const float Delta = Clamped - CurrentSanity;
	if (FMath::IsNearlyZero(Delta))
	{
		return;
	}

	CurrentSanity = Clamped;
	OnSanityChanged.Broadcast(CurrentSanity, Delta);

	if (CurrentSanity <= 0.f)
	{
		if (!bHasNotifiedDepletion)
		{
			bHasNotifiedDepletion = true;
			OnSanityDepleted.Broadcast();
		}
	}
	else
	{
		bHasNotifiedDepletion = false;
	}
}

float USanityComponent::GetSanityPercent() const
{
	return MaxSanity > 0.f ? CurrentSanity / MaxSanity : 0.f;
}

ESanityTier USanityComponent::GetSanityTier() const
{
	const float Pct = GetSanityPercent();

	if (CurrentSanity <= 0.f)
	{
		return ESanityTier::Lost;
	}
	if (Pct >= StableThresholdPct)
	{
		return ESanityTier::Stable;
	}
	if (Pct >= UneasyThresholdPct)
	{
		return ESanityTier::Uneasy;
	}
	if (Pct >= DisturbedThresholdPct)
	{
		return ESanityTier::Disturbed;
	}
	return ESanityTier::Critical;
}
