#include "UI/GameOverWidget.h"
#include "Kismet/GameplayStatics.h"

void UGameOverWidget::RestartGame()
{
	UGameplayStatics::SetGamePaused(this, false);
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()));
}
