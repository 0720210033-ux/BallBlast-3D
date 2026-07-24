#include "HouseOfSanityGameMode.h"
#include "Character/HouseOfSanityCharacter.h"
#include "UI/GameOverWidget.h"
#include "Kismet/GameplayStatics.h"

AHouseOfSanityGameMode::AHouseOfSanityGameMode()
{
	DefaultPawnClass = AHouseOfSanityCharacter::StaticClass();
}

void AHouseOfSanityGameMode::HandlePlayerDeath()
{
	if (bGameOver)
	{
		return;
	}
	bGameOver = true;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	if (GameOverWidgetClass && PC)
	{
		GameOverWidgetInstance = CreateWidget<UGameOverWidget>(PC, GameOverWidgetClass);
		if (GameOverWidgetInstance)
		{
			GameOverWidgetInstance->AddToViewport();
		}
	}

	if (PC)
	{
		FInputModeUIOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}

	UGameplayStatics::SetGamePaused(this, true);
}
