// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"
#include "GeminiFlashSimpleTypes.generated.h"

/**
 * A unique instance of an item in the GeminiFlash simple demo.
 * Bundles the identity (GUID) with display properties (caching).
 */
USTRUCT(BlueprintType)
struct FGeminiFlashItemInstance
{
	GENERATED_BODY()

public:
	/** Globally unique identifier for this specific item instance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeminiFlash")
	FGuid ItemId;

	/** The value to display in the UI (cached for performance) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeminiFlash")
	int32 DisplayValue = 0;

	FGeminiFlashItemInstance() = default;

	FGeminiFlashItemInstance(int32 InValue)
		: ItemId(FGuid::NewGuid())
		, DisplayValue(InValue)
	{
	}

	/** Returns true if this instance represents a valid/non-empty item */
	bool IsValid() const
	{
		return ItemId.IsValid() && DisplayValue > 0;
	}

	/** Returns a truncated string of the GUID for UI visualization */
	FString GetShortGuid() const
	{
		if (!ItemId.IsValid()) return TEXT("---");
		return ItemId.ToString().Left(4);
	}
};
