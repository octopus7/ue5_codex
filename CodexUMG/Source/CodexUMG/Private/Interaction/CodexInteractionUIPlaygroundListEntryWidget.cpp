// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CodexInteractionUIPlaygroundListEntryWidget.h"

#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Interaction/CodexInteractionUIPlaygroundListItem.h"

namespace
{
	const FLinearColor DisabledTint(1.0f, 1.0f, 1.0f, 0.38f);
	const FLinearColor SelectionOutlineTint(1.0f, 1.0f, 1.0f, 0.92f);
}

void UCodexInteractionUIPlaygroundListEntryWidget::ApplyPreviewItem(UCodexInteractionUIPlaygroundListItem* InPreviewItem)
{
	ActiveItem = InPreviewItem;
	RefreshVisualState();
}

void UCodexInteractionUIPlaygroundListEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	ActiveItem = Cast<UCodexInteractionUIPlaygroundListItem>(ListItemObject);
	RefreshVisualState();
}

void UCodexInteractionUIPlaygroundListEntryWidget::NativeOnItemSelectionChanged(const bool bIsSelected)
{
	IUserListEntry::NativeOnItemSelectionChanged(bIsSelected);

	if (ActiveItem != nullptr)
	{
		ActiveItem->bIsSelected = bIsSelected;
	}

	RefreshVisualState();
}

void UCodexInteractionUIPlaygroundListEntryWidget::NativeOnEntryReleased()
{
	IUserListEntry::NativeOnEntryReleased();

	if (ActiveItem != nullptr)
	{
		ActiveItem->bIsSelected = false;
	}

	ActiveItem = nullptr;
	RefreshVisualState();
}

void UCodexInteractionUIPlaygroundListEntryWidget::RefreshVisualState() const
{
	if (TXT_Label != nullptr)
	{
		TXT_Label->SetText(ActiveItem != nullptr ? ActiveItem->Title : FText::GetEmpty());
	}

	if (TXT_Description != nullptr)
	{
		TXT_Description->SetText(ActiveItem != nullptr ? ActiveItem->Description : FText::GetEmpty());
	}

	if (Border_Root != nullptr)
	{
		const FLinearColor BaseColor = ActiveItem != nullptr ? ActiveItem->TintColor : FLinearColor(1.0f, 1.0f, 1.0f, 0.08f);
		Border_Root->SetBrushColor(ActiveItem != nullptr && ActiveItem->bIsEnabled ? BaseColor : DisabledTint);
	}

	if (Border_Selection != nullptr)
	{
		const bool bShowSelection = ActiveItem != nullptr && ActiveItem->bIsSelected;
		Border_Selection->SetBrushColor(SelectionOutlineTint);
		Border_Selection->SetVisibility(bShowSelection ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}

	if (TXT_State != nullptr)
	{
		TXT_State->SetText(ActiveItem != nullptr ? ActiveItem->StateText : FText::GetEmpty());
	}
}
