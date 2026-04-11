// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/CodexInteractionTypes.h"
#include "CodexInteractionUIPlaygroundPayload.generated.h"

USTRUCT(BlueprintType)
struct CODEXUMG_API FCodexUIPlaygroundListEntryData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground")
	FText Title = FText::FromString(TEXT("List Entry"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground", meta = (MultiLine = "true"))
	FText Description = FText::FromString(TEXT("List item description."));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground")
	FText StateText = FText::FromString(TEXT("State"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground")
	ECodexUIPlaygroundSection Section = ECodexUIPlaygroundSection::Collection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground")
	FLinearColor TintColor = FLinearColor(0.26f, 0.38f, 0.52f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground")
	bool bIsEnabled = true;
};

USTRUCT(BlueprintType)
struct CODEXUMG_API FCodexUIPlaygroundTileSlotData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground")
	int32 SlotIndex = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground")
	FText Label = FText::FromString(TEXT("Slot"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground")
	int32 Value = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground")
	FLinearColor TintColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground")
	bool bIsEmpty = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground")
	ECodexUIPlaygroundSection Section = ECodexUIPlaygroundSection::Advanced;
};

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class CODEXUMG_API UCodexInteractionUIPlaygroundPayload : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground")
	FText Title = FText::FromString(TEXT("UI Playground"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground", meta = (MultiLine = "true"))
	FText StatusText = FText::FromString(TEXT("Ready"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground")
	ECodexUIPlaygroundSection InitialSection = ECodexUIPlaygroundSection::Basic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground", meta = (MultiLine = "true"))
	FText BasicDescription = FText::FromString(TEXT("Use this popup to exercise UMG controls, list views, and drag/drop slots."));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground")
	FText InputText = FText::FromString(TEXT("Sample text"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground")
	bool bToggleValue = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground")
	float SliderValue = 0.50f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground")
	int32 SpinValue = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground")
	FString SelectedPreset = TEXT("Preset A");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground")
	TArray<FString> PresetOptions =
	{
		TEXT("Preset A"),
		TEXT("Preset B"),
		TEXT("Preset C")
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground")
	TArray<FCodexUIPlaygroundListEntryData> ListEntries;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Codex|Interaction|UIPlayground")
	TArray<FCodexUIPlaygroundTileSlotData> TileSlots;
};
