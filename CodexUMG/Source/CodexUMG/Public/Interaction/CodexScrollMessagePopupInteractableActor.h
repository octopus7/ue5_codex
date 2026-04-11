// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Interaction/CodexPopupInteractableActor.h"
#include "CodexScrollMessagePopupInteractableActor.generated.h"

UCLASS(Blueprintable)
class CODEXUMG_API ACodexScrollMessagePopupInteractableActor : public ACodexPopupInteractableActor
{
	GENERATED_BODY()

public:
	ACodexScrollMessagePopupInteractableActor();

	virtual ECodexInteractionPopupStyle GetPopupStyle() const override;
	virtual bool AllowsPopupControllerClose() const override;
};
