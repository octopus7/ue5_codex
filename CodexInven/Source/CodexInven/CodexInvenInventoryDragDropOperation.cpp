// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenInventoryDragDropOperation.h"

#include "CodexInvenPlayerHudWidget.h"

void UCodexInvenInventoryDragDropOperation::InitializeOperation(
	const int32 InSourceSlotIndex,
	const FCodexInvenInventorySlotData& InSourceSlotData,
	UTexture2D* InIconTexture,
	UCodexInvenPlayerHudWidget* InOwningHudWidget)
{
	SourceSlotIndex = InSourceSlotIndex;
	SourceSlotData = InSourceSlotData;
	IconTexture = InIconTexture;
	OwningHudWidget = InOwningHudWidget;
}

void UCodexInvenInventoryDragDropOperation::Drop_Implementation(const FPointerEvent& InPointerEvent)
{
	Super::Drop_Implementation(InPointerEvent);

	if (OwningHudWidget != nullptr)
	{
		OwningHudWidget->EndInventoryDrag();
	}
}

void UCodexInvenInventoryDragDropOperation::DragCancelled_Implementation(const FPointerEvent& InPointerEvent)
{
	Super::DragCancelled_Implementation(InPointerEvent);

	if (OwningHudWidget != nullptr)
	{
		OwningHudWidget->EndInventoryDrag();
	}
}
