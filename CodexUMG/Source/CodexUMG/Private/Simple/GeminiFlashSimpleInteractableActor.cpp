// Copyright Epic Games, Inc. All Rights Reserved.

#include "Simple/GeminiFlashSimpleInteractableActor.h"

AGeminiFlashSimpleInteractableActor::AGeminiFlashSimpleInteractableActor()
{
	PopupTitle = FText::FromString(TEXT("GeminiFlash Test"));
	PopupMessage = FText::FromString(TEXT("This is a simplified drag-and-drop test."));
}

ECodexInteractionPopupStyle AGeminiFlashSimpleInteractableActor::GetPopupStyle() const
{
	return ECodexInteractionPopupStyle::GeminiFlashSimple;
}
