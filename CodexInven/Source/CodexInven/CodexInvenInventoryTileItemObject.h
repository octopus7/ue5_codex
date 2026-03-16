// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CodexInvenOwnershipComponent.h"
#include "UObject/Object.h"
#include "CodexInvenInventoryTileItemObject.generated.h"

class UTexture2D;

UCLASS()
class CODEXINVEN_API UCodexInvenInventoryTileItemObject : public UObject
{
	GENERATED_BODY()

public:
	void InitializeItem(const FCodexInvenInventorySlotData& InSlotData, UTexture2D* InIconTexture, UTexture2D* InBackgroundTexture);

	const FCodexInvenInventorySlotData& GetSlotData() const
	{
		return SlotData;
	}

	UTexture2D* GetIconTexture() const
	{
		return IconTexture;
	}

	UTexture2D* GetBackgroundTexture() const
	{
		return BackgroundTexture;
	}

private:
	UPROPERTY(Transient)
	FCodexInvenInventorySlotData SlotData;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> IconTexture = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> BackgroundTexture = nullptr;
};
