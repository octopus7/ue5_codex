// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/CodexInteractionUIPlaygroundPayload.h"
#include "UObject/Object.h"
#include "CodexInteractionUIPlaygroundListItem.generated.h"

UCLASS(BlueprintType)
class CODEXUMG_API UCodexInteractionUIPlaygroundListItem : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction|UIPlayground")
	FText Title;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction|UIPlayground", meta = (MultiLine = "true"))
	FText Description;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction|UIPlayground")
	FText StateText;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction|UIPlayground")
	ECodexUIPlaygroundSection Section = ECodexUIPlaygroundSection::Collection;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction|UIPlayground")
	FLinearColor TintColor = FLinearColor::White;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction|UIPlayground")
	bool bIsEnabled = true;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction|UIPlayground")
	bool bIsSelected = false;

	void ApplyData(const FCodexUIPlaygroundListEntryData& Data);
};
