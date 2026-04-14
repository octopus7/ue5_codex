// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/DragDropOperation.h"
#include "GeminiFlashSimpleDragDropOperation.generated.h"

/**
 * Simplified drag-and-drop operation for learning purposes.
 */
UCLASS(BlueprintType)
class CODEXUMG_API UGeminiFlashSimpleDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	// The value being transferred
	UPROPERTY(BlueprintReadOnly, Category = "GeminiFlash|Simple")
	int32 Value = 0;

	// The slot widget where the drag started
	UPROPERTY(BlueprintReadOnly, Category = "GeminiFlash|Simple")
	TObjectPtr<UUserWidget> SourceWidget = nullptr;
};
