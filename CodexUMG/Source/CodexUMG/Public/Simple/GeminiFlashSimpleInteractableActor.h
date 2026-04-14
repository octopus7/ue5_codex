// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Interaction/CodexPopupInteractableActor.h"
#include "GeminiFlashSimpleInteractableActor.generated.h"

/**
 * Simplified interactable actor that triggers the GeminiFlash popup.
 */
UCLASS(Blueprintable)
class CODEXUMG_API AGeminiFlashSimpleInteractableActor : public ACodexPopupInteractableActor
{
	GENERATED_BODY()

public:
	AGeminiFlashSimpleInteractableActor();

	// ACodexPopupInteractableActor overrides
	virtual ECodexInteractionPopupStyle GetPopupStyle() const override;
};
