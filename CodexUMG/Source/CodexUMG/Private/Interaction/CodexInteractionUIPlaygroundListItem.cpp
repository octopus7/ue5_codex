// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CodexInteractionUIPlaygroundListItem.h"

void UCodexInteractionUIPlaygroundListItem::ApplyData(const FCodexUIPlaygroundListEntryData& Data)
{
	Title = Data.Title;
	Description = Data.Description;
	StateText = Data.StateText;
	Section = Data.Section;
	TintColor = Data.TintColor;
	bIsEnabled = Data.bIsEnabled;
	bIsSelected = false;
}
