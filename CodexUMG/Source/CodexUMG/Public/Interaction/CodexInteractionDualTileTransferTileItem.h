// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/CodexInteractionTypes.h"
#include "UObject/Object.h"
#include "CodexInteractionDualTileTransferTileItem.generated.h"

UCLASS(BlueprintType)
class CODEXUMG_API UCodexInteractionDualTileTransferTileItem : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	int32 SlotIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	int32 Number = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	FLinearColor TintColor = FLinearColor::White;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	bool bIsEmpty = true;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	bool bIsSelected = false;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	ECodexTileTransferPanelSide PanelSide = ECodexTileTransferPanelSide::Left;

	bool HasNumber() const
	{
		return !bIsEmpty && Number > 0;
	}
};
