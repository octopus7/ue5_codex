// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CodexInteractionDualTileTransferTileEntryWidget.h"

#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Interaction/CodexInteractionAssetPaths.h"
#include "Interaction/CodexInteractionDualTileTransferPopupWidget.h"
#include "Interaction/CodexInteractionDualTileTransferTileItem.h"
#include "Interaction/CodexTileTransferDragDropOperation.h"

namespace
{
	const FLinearColor EmptySlotTint(1.0f, 1.0f, 1.0f, 0.14f);
	const FLinearColor SelectedOutlineTint(1.0f, 1.0f, 1.0f, 0.92f);
	const FLinearColor DropTargetOutlineTint(1.0f, 0.97f, 0.72f, 1.0f);
}

FReply UCodexInteractionDualTileTransferTileEntryWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (ActiveItem != nullptr
		&& ActiveItem->HasNumber()
		&& InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

bool UCodexInteractionDualTileTransferTileEntryWidget::NativeOnDragOver(
	const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	(void)InGeometry;
	(void)InDragDropEvent;
	(void)InOperation;
	return Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
}

bool UCodexInteractionDualTileTransferTileEntryWidget::NativeOnDrop(
	const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	(void)InGeometry;
	(void)InDragDropEvent;
	(void)InOperation;
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UCodexInteractionDualTileTransferTileEntryWidget::NativeOnDragLeave(
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	(void)InDragDropEvent;
	(void)InOperation;
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
}

void UCodexInteractionDualTileTransferTileEntryWidget::NativeOnDragDetected(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (ActiveItem == nullptr || !ActiveItem->HasNumber())
	{
		return;
	}

	UCodexTileTransferDragDropOperation* Operation = Cast<UCodexTileTransferDragDropOperation>(
		UWidgetBlueprintLibrary::CreateDragDropOperation(UCodexTileTransferDragDropOperation::StaticClass()));
	if (Operation == nullptr)
	{
		return;
	}

	Operation->Payload = ActiveItem;
	Operation->Item = ActiveItem;
	Operation->Number = ActiveItem->Number;
	Operation->SourcePanelSide = ActiveItem->PanelSide;
	Operation->SourceIndex = ActiveItem->SlotIndex;
	Operation->Pivot = EDragPivot::CenterCenter;

	if (UCodexInteractionDualTileTransferPopupWidget* PopupWidget = ResolvePopupWidget())
	{
		PopupWidget->NotifyTileDragStarted();
	}

	if (const TSubclassOf<UUserWidget> PreviewClass = ResolveTileEntryWidgetClass())
	{
		if (UCodexInteractionDualTileTransferTileEntryWidget* PreviewWidget =
			CreateWidget<UCodexInteractionDualTileTransferTileEntryWidget>(GetOwningPlayer(), PreviewClass))
		{
			PreviewWidget->ApplyPreviewItem(ActiveItem);
			PreviewWidget->SetRenderOpacity(0.95f);
			Operation->DefaultDragVisual = PreviewWidget;
		}
	}

	OutOperation = Operation;
}

void UCodexInteractionDualTileTransferTileEntryWidget::ApplyPreviewItem(UCodexInteractionDualTileTransferTileItem* InPreviewItem)
{
	ActiveItem = InPreviewItem;
	bIsDropTargetHighlighted = false;
	RefreshVisualState();
}

UCodexInteractionDualTileTransferTileItem* UCodexInteractionDualTileTransferTileEntryWidget::GetTileItem() const
{
	return ActiveItem;
}

void UCodexInteractionDualTileTransferTileEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	ActiveItem = Cast<UCodexInteractionDualTileTransferTileItem>(ListItemObject);
	bIsDropTargetHighlighted = false;
	RefreshVisualState();
}

void UCodexInteractionDualTileTransferTileEntryWidget::NativeOnItemSelectionChanged(const bool bIsSelected)
{
	IUserListEntry::NativeOnItemSelectionChanged(bIsSelected);

	if (ActiveItem != nullptr)
	{
		ActiveItem->bIsSelected = bIsSelected;
	}

	RefreshVisualState();
}

void UCodexInteractionDualTileTransferTileEntryWidget::NativeOnEntryReleased()
{
	IUserListEntry::NativeOnEntryReleased();
	ActiveItem = nullptr;
	bIsDropTargetHighlighted = false;
	RefreshVisualState();
}

UCodexInteractionDualTileTransferPopupWidget* UCodexInteractionDualTileTransferTileEntryWidget::ResolvePopupWidget() const
{
	if (ActiveItem != nullptr)
	{
		if (UCodexInteractionDualTileTransferPopupWidget* PopupWidget =
			ActiveItem->GetTypedOuter<UCodexInteractionDualTileTransferPopupWidget>())
		{
			return PopupWidget;
		}
	}

	return GetTypedOuter<UCodexInteractionDualTileTransferPopupWidget>();
}

void UCodexInteractionDualTileTransferTileEntryWidget::SetDropTargetHighlighted(const bool bHighlighted)
{
	if (bIsDropTargetHighlighted == bHighlighted)
	{
		return;
	}

	bIsDropTargetHighlighted = bHighlighted;
	RefreshVisualState();
}

TSubclassOf<UUserWidget> UCodexInteractionDualTileTransferTileEntryWidget::ResolveTileEntryWidgetClass() const
{
	return LoadClass<UUserWidget>(
		nullptr,
		*CodexInteractionAssetPaths::MakeGeneratedClassObjectPath(CodexInteractionAssetPaths::DualTileTransferTileEntryWidgetObjectPath));
}

void UCodexInteractionDualTileTransferTileEntryWidget::RefreshVisualState() const
{
	if (TXT_Number != nullptr)
	{
		const FText NumberText =
			ActiveItem != nullptr && ActiveItem->HasNumber() ? FText::AsNumber(ActiveItem->Number) : FText::GetEmpty();
		TXT_Number->SetText(NumberText);
	}

	if (IMG_TileBackground != nullptr)
	{
		const FLinearColor BackgroundTint =
			ActiveItem != nullptr && ActiveItem->HasNumber() ? ActiveItem->TintColor : EmptySlotTint;
		IMG_TileBackground->SetColorAndOpacity(BackgroundTint);
	}

	if (Border_SelectedOutline != nullptr)
	{
		const bool bShowSelection = ActiveItem != nullptr && ActiveItem->HasNumber() && ActiveItem->bIsSelected;
		const bool bShowOutline = bShowSelection || bIsDropTargetHighlighted;
		Border_SelectedOutline->SetBrushColor(bIsDropTargetHighlighted ? DropTargetOutlineTint : SelectedOutlineTint);
		Border_SelectedOutline->SetVisibility(bShowOutline ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}
}
