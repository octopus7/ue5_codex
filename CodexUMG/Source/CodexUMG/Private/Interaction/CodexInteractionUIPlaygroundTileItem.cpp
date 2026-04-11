// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CodexInteractionUIPlaygroundTileItem.h"

void UCodexInteractionUIPlaygroundTileItem::ApplyData(const FCodexUIPlaygroundTileSlotData& Data)
{
	SlotIndex = Data.SlotIndex;
	Label = Data.Label;
	Value = Data.Value;
	TintColor = Data.TintColor;
	bIsEmpty = Data.bIsEmpty;
	Section = Data.Section;
}
