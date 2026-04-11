// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CodexPopupInteractableActor.h"

ACodexPopupInteractableActor::ACodexPopupInteractableActor()
{
	ConsumeDelaySeconds = 0.0f;
}

void ACodexPopupInteractableActor::HandleInteractionRequested_Implementation(const FCodexInteractionRequest& Request)
{
	ReceiveInteractionRequested(Request);
}

void ACodexPopupInteractableActor::SetPopupTitle(const FText& NewPopupTitle)
{
	PopupTitle = NewPopupTitle;
}

void ACodexPopupInteractableActor::SetPopupMessage(const FText& NewPopupMessage)
{
	PopupMessage = NewPopupMessage;
}

void ACodexPopupInteractableActor::SetPopupButtonLayout(const ECodexPopupButtonLayout NewPopupButtonLayout)
{
	PopupButtonLayout = NewPopupButtonLayout;
}
