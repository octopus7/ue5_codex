// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/DragDropOperation.h"
#include "Interaction/CodexInteractionTypes.h"
#include "CodexTileTransferDragDropOperation.generated.h"

class UCodexInteractionDualTileTransferTileItem;

UCLASS(BlueprintType)
class CODEXUMG_API UCodexTileTransferDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	int32 Number = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	ECodexTileTransferPanelSide SourcePanelSide = ECodexTileTransferPanelSide::Left;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	int32 SourceIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	TObjectPtr<UCodexInteractionDualTileTransferTileItem> Item = nullptr;
};
