#include "UI/MainHUDWidget.h"
#include "Components/SanityComponent.h"
#include "Systems/DayNightSubsystem.h"
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

	if (UDayNightSubsystem* DayNight = GetWorld()->GetSubsystem<UDayNightSubsystem>())
	{
		DayNight->OnTimePhaseChanged.AddDynamic(this, &UMainHUDWidget::HandleTimePhaseChanged);
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
	if (const UDayNightSubsystem* DayNight = GetWorld()->GetSubsystem<UDayNightSubsystem>())
	{
		OnTimePhaseUpdated(NewPhase, DayNight->CurrentDay);
	}
}
