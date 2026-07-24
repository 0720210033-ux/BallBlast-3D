#include "Environment/FallHallucinationTrap.h"
#include "HouseOfSanityGameState.h"
#include "Components/SanityComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

AFallHallucinationTrap::AFallHallucinationTrap()
{
	PrimaryActorTick.bCanEverTick = false;

	IllusionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IllusionMesh"));
	RootComponent = IllusionMesh;
	IllusionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	IllusionMesh->SetVisibility(false);
}

void AFallHallucinationTrap::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(EvaluationTimerHandle, this, &AFallHallucinationTrap::EvaluateLocalVisibility, EvaluationIntervalSeconds, true);
}

void AFallHallucinationTrap::EvaluateLocalVisibility()
{
	const AHouseOfSanityGameState* GameState = GetWorld()->GetGameState<AHouseOfSanityGameState>();
	bool bShouldShow = false;

	if (GameState && GameState->IsNight())
	{
		if (const APawn* LocalPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
		{
			if (const USanityComponent* Sanity = LocalPawn->FindComponentByClass<USanityComponent>())
			{
				bShouldShow = Sanity->GetSanityTier() >= MinimumTierToDeceive;
			}
		}
	}

	if (bShouldShow != bIsShowingIllusion)
	{
		bIsShowingIllusion = bShouldShow;
		IllusionMesh->SetVisibility(bShouldShow, true);
		OnIllusionVisibilityChanged(bShouldShow);
	}
}
