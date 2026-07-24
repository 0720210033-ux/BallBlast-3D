#pragma once

#include "CoreMinimal.h"
#include "HouseOfSanityTypes.generated.h"

UENUM(BlueprintType)
enum class ETimePhase : uint8
{
	Day,
	Dusk,
	Night,
	Dawn
};

// Ordered worst-to-best is deliberately avoided: gameplay code checks tiers
// top-down from Stable, so declaration order here doesn't drive behavior.
UENUM(BlueprintType)
enum class ESanityTier : uint8
{
	Stable,
	Uneasy,
	Disturbed,
	Critical,
	Lost
};

UENUM(BlueprintType)
enum class EEntityState : uint8
{
	Dormant,
	Wandering,
	Stalking,
	Hunting
};

UENUM(BlueprintType)
enum class EHallucinationType : uint8
{
	FakeSound,
	FakeChorePrompt,
	VisualDistortion,
	FalseSafeEntity
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSanityChanged, float, NewSanity, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSanityDepleted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimePhaseChanged, ETimePhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewDayBegin, int32, DayNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNightSurvived, int32, DayNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMotherRescue, int32, DayNumber);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHallucinationTriggered, EHallucinationType, Type);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChoreResolved, bool, bSuccess);
