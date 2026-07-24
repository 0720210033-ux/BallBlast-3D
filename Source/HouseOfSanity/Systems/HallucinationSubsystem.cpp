#include "Systems/HallucinationSubsystem.h"
#include "Systems/DayNightSubsystem.h"
#include "Components/SanityComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialParameterCollection.h"

void UHallucinationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

bool UHallucinationSubsystem::DoesSupportWorldType(EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

TStatId UHallucinationSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UHallucinationSubsystem, STATGROUP_Tickables);
}

USanityComponent* GetPlayerSanityComponent(const UWorld* World)
{
	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0))
	{
		return PlayerPawn->FindComponentByClass<USanityComponent>();
	}
	return nullptr;
}

bool UHallucinationSubsystem::IsPlayerDeceivable() const
{
	const UDayNightSubsystem* DayNight = GetWorld()->GetSubsystem<UDayNightSubsystem>();
	if (!DayNight || !DayNight->IsNight())
	{
		return false;
	}

	const USanityComponent* Sanity = GetPlayerSanityComponent(GetWorld());
	return Sanity && Sanity->GetSanityTier() >= ESanityTier::Disturbed;
}

float UHallucinationSubsystem::GetEventChanceForTier(ESanityTier Tier) const
{
	switch (Tier)
	{
		case ESanityTier::Disturbed: return DisturbedEventChance;
		case ESanityTier::Critical: return CriticalEventChance;
		case ESanityTier::Lost: return LostEventChance;
		default: return 0.f;
	}
}

void UHallucinationSubsystem::EvaluateHallucinationRisk()
{
	const UDayNightSubsystem* DayNight = GetWorld()->GetSubsystem<UDayNightSubsystem>();
	if (!DayNight || !DayNight->IsNight())
	{
		return;
	}

	const USanityComponent* Sanity = GetPlayerSanityComponent(GetWorld());
	if (!Sanity)
	{
		return;
	}

	const float Chance = GetEventChanceForTier(Sanity->GetSanityTier());
	if (Chance <= 0.f || FMath::FRand() > Chance)
	{
		return;
	}

	const EHallucinationType Type = static_cast<EHallucinationType>(FMath::RandRange(0, 3));
	OnHallucinationTriggered.Broadcast(Type);
}

void UHallucinationSubsystem::UpdateDistortionParameter(float DeltaTime)
{
	const UDayNightSubsystem* DayNight = GetWorld()->GetSubsystem<UDayNightSubsystem>();
	const USanityComponent* Sanity = GetPlayerSanityComponent(GetWorld());

	float Target = 0.f;
	if (DayNight && DayNight->IsNight() && Sanity)
	{
		Target = FMath::Clamp((1.f - Sanity->GetSanityPercent()) * 1.2f, 0.f, 1.f);
	}

	CurrentDistortion = FMath::FInterpTo(CurrentDistortion, Target, DeltaTime, 1.5f);

	if (UMaterialParameterCollection* MPC = SanityMPC.LoadSynchronous())
	{
		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPC, DistortionParamName, CurrentDistortion);
	}
}

void UHallucinationSubsystem::Tick(float DeltaTime)
{
	UpdateDistortionParameter(DeltaTime);

	TimeSinceLastCheck += DeltaTime;
	if (TimeSinceLastCheck >= CheckIntervalSeconds)
	{
		TimeSinceLastCheck = 0.f;
		EvaluateHallucinationRisk();
	}
}
