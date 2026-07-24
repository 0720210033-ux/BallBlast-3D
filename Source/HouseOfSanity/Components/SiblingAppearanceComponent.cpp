#include "Components/SiblingAppearanceComponent.h"
#include "Character/HouseOfSanityCharacter.h"
#include "Components/SanityComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

USiblingAppearanceComponent::USiblingAppearanceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USiblingAppearanceComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(EvaluationTimerHandle, this, &USiblingAppearanceComponent::EvaluateLocalAppearance, EvaluationIntervalSeconds, true);
}

void USiblingAppearanceComponent::EvaluateLocalAppearance()
{
	AHouseOfSanityCharacter* OwnerCharacter = Cast<AHouseOfSanityCharacter>(GetOwner());
	if (!OwnerCharacter || OwnerCharacter->IsLocallyControlled())
	{
		return;
	}

	const APawn* LocalPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	const USanityComponent* LocalSanity = LocalPawn ? LocalPawn->FindComponentByClass<USanityComponent>() : nullptr;

	const bool bShouldDisguise = LocalSanity && LocalSanity->GetSanityTier() >= MinimumTierToDisguise;
	if (bShouldDisguise == bIsShowingDisguise)
	{
		return;
	}
	bIsShowingDisguise = bShouldDisguise;

	if (USkeletalMeshComponent* RealMesh = OwnerCharacter->GetMesh())
	{
		RealMesh->SetVisibility(!bShouldDisguise, true);
	}
	if (USkeletalMeshComponent* Disguise = OwnerCharacter->DisguiseMesh)
	{
		Disguise->SetVisibility(bShouldDisguise, true);
	}
}
