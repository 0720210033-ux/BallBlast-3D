#include "UI/MainHUDWidget.h"
#include "Components/SanityComponent.h"
#include "HouseOfSanityGameState.h"
#include "Kismet/GameplayStatics.h"

void UMainHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		if (USanityComponent* Sanity = PlayerPawn->FindComponentByClass<USanityComponent>())
		{
			BindToSanityComponent(Sanity);
		}
	}

	if (AHouseOfSanityGameState* GameState = GetWorld()->GetGameState<AHouseOfSanityGameState>())
	{
		GameState->OnTimePhaseChanged.AddDynamic(this, &UMainHUDWidget::HandleTimePhaseChanged);
	}
}

void UMainHUDWidget::BindToSanityComponent(USanityComponent* InSanityComponent)
{
	if (BoundSanityComponent)
	{
		BoundSanityComponent->OnSanityChanged.RemoveDynamic(this, &UMainHUDWidget::HandleSanityChanged);
	}

	BoundSanityComponent = InSanityComponent;

	if (BoundSanityComponent)
	{
		BoundSanityComponent->OnSanityChanged.AddDynamic(this, &UMainHUDWidget::HandleSanityChanged);
		HandleSanityChanged(BoundSanityComponent->CurrentSanity, 0.f);
	}
}

void UMainHUDWidget::HandleSanityChanged(float NewSanity, float Delta)
{
	if (BoundSanityComponent)
	{
		OnSanityUpdated(NewSanity, BoundSanityComponent->MaxSanity, BoundSanityComponent->GetSanityTier());
	}
}

void UMainHUDWidget::HandleTimePhaseChanged(ETimePhase NewPhase)
{
	if (const AHouseOfSanityGameState* GameState = GetWorld()->GetGameState<AHouseOfSanityGameState>())
	{
		OnTimePhaseUpdated(NewPhase, GameState->CurrentDay);
	}
}
