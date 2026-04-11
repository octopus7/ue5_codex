// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/DragDropOperation.h"
#include "Interaction/CodexInteractionTypes.h"
#include "CodexUIPlaygroundDragDropOperation.generated.h"

class UCodexInteractionUIPlaygroundTileItem;

UCLASS(BlueprintType)
class CODEXUMG_API UCodexUIPlaygroundDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction|UIPlayground")
	TObjectPtr<UCodexInteractionUIPlaygroundTileItem> Item = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction|UIPlayground")
	int32 SourceSlotIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction|UIPlayground")
	ECodexUIPlaygroundSection Section = ECodexUIPlaygroundSection::Advanced;
};
