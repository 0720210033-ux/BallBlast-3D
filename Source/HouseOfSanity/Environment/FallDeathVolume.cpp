#include "Environment/FallDeathVolume.h"
#include "HouseOfSanityGameMode.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

AFallDeathVolume::AFallDeathVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	LethalVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("LethalVolume"));
	RootComponent = LethalVolume;
	LethalVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LethalVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	LethalVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AFallDeathVolume::BeginPlay()
{
	Super::BeginPlay();
	LethalVolume->OnComponentBeginOverlap.AddDynamic(this, &AFallDeathVolume::HandleBeginOverlap);
}

void AFallDeathVolume::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character)
	{
		return;
	}

	if (Character->GetVelocity().Z > -LethalImpactSpeed)
	{
		return;
	}

	OnFallImpact(Character);

	if (AHouseOfSanityGameMode* GameMode = GetWorld()->GetAuthGameMode<AHouseOfSanityGameMode>())
	{
		GameMode->HandlePlayerDeath();
	}
}
