// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/CodexInteractionUIPlaygroundPayload.h"
#include "UObject/Object.h"
#include "CodexInteractionUIPlaygroundTileItem.generated.h"

UCLASS(BlueprintType)
class CODEXUMG_API UCodexInteractionUIPlaygroundTileItem : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction|UIPlayground")
	int32 SlotIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction|UIPlayground")
	FText Label;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction|UIPlayground")
	int32 Value = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction|UIPlayground")
	FLinearColor TintColor = FLinearColor::White;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction|UIPlayground")
	bool bIsEmpty = true;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction|UIPlayground")
	bool bIsSelected = false;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction|UIPlayground")
	ECodexUIPlaygroundSection Section = ECodexUIPlaygroundSection::Advanced;

	bool HasValue() const { return !bIsEmpty && Value > 0; }
	bool HasContent() const { return HasValue(); }

	void ApplyData(const FCodexUIPlaygroundTileSlotData& Data);
};
