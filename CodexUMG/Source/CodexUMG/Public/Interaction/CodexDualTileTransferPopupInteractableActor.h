// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Interaction/CodexPopupInteractableActor.h"
#include "CodexDualTileTransferPopupInteractableActor.generated.h"

UCLASS(Blueprintable)
class CODEXUMG_API ACodexDualTileTransferPopupInteractableActor : public ACodexPopupInteractableActor
{
	GENERATED_BODY()

public:
	ACodexDualTileTransferPopupInteractableActor();

	virtual ECodexInteractionPopupStyle GetPopupStyle() const override;
	virtual bool AllowsPopupControllerClose() const override;
	virtual void PopulatePopupRequest(FCodexInteractionPopupRequest& PopupRequest) const override;

	const TArray<int32>& GetLeftNumbers() const { return LeftNumbers; }
	const TArray<int32>& GetRightNumbers() const { return RightNumbers; }
	bool AllowsDuplicateNumbers() const { return bAllowDuplicateNumbers; }

	void SetLeftNumbers(const TArray<int32>& NewLeftNumbers);
	void SetRightNumbers(const TArray<int32>& NewRightNumbers);
	void SetAllowDuplicateNumbers(bool bInAllowDuplicateNumbers);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Codex|Interaction|Popup")
	TArray<int32> LeftNumbers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Codex|Interaction|Popup")
	TArray<int32> RightNumbers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Codex|Interaction|Popup")
	bool bAllowDuplicateNumbers = false;
};
