// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CodexScrollMessagePopupInteractableActor.h"

ACodexScrollMessagePopupInteractableActor::ACodexScrollMessagePopupInteractableActor()
{
	SetPopupButtonLayout(ECodexPopupButtonLayout::Ok);
}

ECodexInteractionPopupStyle ACodexScrollMessagePopupInteractableActor::GetPopupStyle() const
{
	return ECodexInteractionPopupStyle::ScrollMessage;
}

bool ACodexScrollMessagePopupInteractableActor::AllowsPopupControllerClose() const
{
	return false;
}
