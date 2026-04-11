// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Interaction/CodexPopupInteractableActor.h"
#include "CodexUIPlaygroundPopupInteractableActor.generated.h"

class UCodexInteractionUIPlaygroundPayload;

UCLASS(Blueprintable)
class CODEXUMG_API ACodexUIPlaygroundPopupInteractableActor : public ACodexPopupInteractableActor
{
	GENERATED_BODY()

public:
	ACodexUIPlaygroundPopupInteractableActor();

	virtual ECodexInteractionPopupStyle GetPopupStyle() const override;
	virtual void PopulatePopupRequest(FCodexInteractionPopupRequest& PopupRequest) const override;

protected:
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "Codex|Interaction|Popup")
	TObjectPtr<UCodexInteractionUIPlaygroundPayload> UIPlaygroundPayload;
};
