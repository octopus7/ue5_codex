#include "UI/PSVGameOverWidget.h"

#include "Components/Button.h"
#include "GameFramework/PlayerController.h"

void UPSVGameOverWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (OkButton)
    {
        OkButton->OnClicked.AddDynamic(this, &UPSVGameOverWidget::HandleOkClicked);
    }
}

void UPSVGameOverWidget::HandleOkClicked()
{
    ResumeGame();
    RemoveFromParent();
}

void UPSVGameOverWidget::ResumeGame()
{
    if (APlayerController* PlayerController = GetOwningPlayer())
    {
        PlayerController->SetPause(false);

        FInputModeGameOnly InputMode;
        PlayerController->SetInputMode(InputMode);
        PlayerController->bShowMouseCursor = false;
    }
}
