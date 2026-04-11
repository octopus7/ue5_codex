// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Interaction/CodexInteractableActor.h"
#include "CodexPopupInteractableActor.generated.h"

UCLASS(Blueprintable)
class CODEXUMG_API ACodexPopupInteractableActor : public ACodexInteractableActor
{
	GENERATED_BODY()

public:
	ACodexPopupInteractableActor();

	virtual void HandleInteractionRequested_Implementation(const FCodexInteractionRequest& Request) override;
	virtual ECodexInteractionPopupStyle GetPopupStyle() const;
	virtual bool AllowsPopupControllerClose() const;

	const FText& GetPopupTitle() const { return PopupTitle; }
	const FText& GetPopupMessage() const { return PopupMessage; }
	ECodexPopupButtonLayout GetPopupButtonLayout() const { return PopupButtonLayout; }

	void SetPopupTitle(const FText& NewPopupTitle);
	void SetPopupMessage(const FText& NewPopupMessage);
	void SetPopupButtonLayout(ECodexPopupButtonLayout NewPopupButtonLayout);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Codex|Interaction|Popup")
	FText PopupTitle = FText::FromString(TEXT("Notice"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Codex|Interaction|Popup", meta = (MultiLine = "true"))
	FText PopupMessage = FText::FromString(TEXT("This sign has no message."));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Codex|Interaction|Popup")
	ECodexPopupButtonLayout PopupButtonLayout = ECodexPopupButtonLayout::Ok;
};
