// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenInventoryTileItemObject.h"

void UCodexInvenInventoryTileItemObject::InitializeItem(const FCodexInvenInventorySlotData& InSlotData, UTexture2D* InIconTexture, UTexture2D* InBackgroundTexture)
{
	SlotData = InSlotData;
	IconTexture = InIconTexture;
	BackgroundTexture = InBackgroundTexture;
}
