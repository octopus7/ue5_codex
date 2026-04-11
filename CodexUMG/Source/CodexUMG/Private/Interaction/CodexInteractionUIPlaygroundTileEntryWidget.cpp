// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CodexInteractionUIPlaygroundTileEntryWidget.h"

#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Interaction/CodexInteractionAssetPaths.h"
#include "Interaction/CodexInteractionUIPlaygroundPopupWidget.h"
#include "Interaction/CodexInteractionUIPlaygroundTileItem.h"
#include "Interaction/CodexUIPlaygroundDragDropOperation.h"

namespace
{
	const FLinearColor EmptyTargetTint(1.0f, 1.0f, 1.0f, 0.08f);
	const FLinearColor HighlightTint(1.0f, 0.98f, 0.76f, 1.0f);
}

FReply UCodexInteractionUIPlaygroundTileEntryWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (ActiveItem != nullptr
		&& ActiveItem->HasContent()
		&& InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

bool UCodexInteractionUIPlaygroundTileEntryWidget::NativeOnDragOver(
	const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	(void)InGeometry;
	(void)InDragDropEvent;

	const UCodexUIPlaygroundDragDropOperation* Operation = Cast<UCodexUIPlaygroundDragDropOperation>(InOperation);
	if (UCodexInteractionUIPlaygroundPopupWidget* PopupWidget = ResolvePopupWidget())
	{
		const bool bCanDrop = PopupWidget->CanDropOnSlot(ActiveItem, Operation);
		SetDropTargetHighlighted(bCanDrop);
		return bCanDrop;
	}

	return Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
}

bool UCodexInteractionUIPlaygroundTileEntryWidget::NativeOnDrop(
	const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	(void)InGeometry;
	(void)InDragDropEvent;

	if (UCodexInteractionUIPlaygroundPopupWidget* PopupWidget = ResolvePopupWidget())
	{
		const bool bHandled = PopupWidget->TryHandleDropOnSlot(ActiveItem, InOperation);
		SetDropTargetHighlighted(false);
		return bHandled;
	}

	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UCodexInteractionUIPlaygroundTileEntryWidget::NativeOnDragLeave(
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	(void)InDragDropEvent;
	(void)InOperation;

	SetDropTargetHighlighted(false);
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
}

void UCodexInteractionUIPlaygroundTileEntryWidget::NativeOnDragDetected(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (ActiveItem == nullptr || !ActiveItem->HasValue())
	{
		return;
	}

	UCodexUIPlaygroundDragDropOperation* Operation = Cast<UCodexUIPlaygroundDragDropOperation>(
		UWidgetBlueprintLibrary::CreateDragDropOperation(UCodexUIPlaygroundDragDropOperation::StaticClass()));
	if (Operation == nullptr)
	{
		return;
	}

	Operation->Item = ActiveItem;
	Operation->Payload = ActiveItem;
	Operation->SourceSlotIndex = ActiveItem->SlotIndex;
	Operation->Section = ActiveItem->Section;
	Operation->Pivot = EDragPivot::CenterCenter;

	if (UCodexInteractionUIPlaygroundPopupWidget* PopupWidget = ResolvePopupWidget())
	{
		PopupWidget->NotifyTileDragStarted();
	}

	if (const TSubclassOf<UUserWidget> PreviewClass = ResolveTileEntryWidgetClass())
	{
		if (UCodexInteractionUIPlaygroundTileEntryWidget* PreviewWidget =
			CreateWidget<UCodexInteractionUIPlaygroundTileEntryWidget>(GetOwningPlayer(), PreviewClass))
		{
			PreviewWidget->ApplyPreviewItem(ActiveItem);
			PreviewWidget->SetRenderOpacity(0.95f);
			Operation->DefaultDragVisual = PreviewWidget;
		}
	}

	OutOperation = Operation;
}

void UCodexInteractionUIPlaygroundTileEntryWidget::ApplyPreviewItem(UCodexInteractionUIPlaygroundTileItem* InItem)
{
	ActiveItem = InItem;
	bIsDropTargetHighlighted = false;
	RefreshVisualState();
}

void UCodexInteractionUIPlaygroundTileEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	ActiveItem = Cast<UCodexInteractionUIPlaygroundTileItem>(ListItemObject);
	bIsDropTargetHighlighted = false;
	RefreshVisualState();
}

void UCodexInteractionUIPlaygroundTileEntryWidget::NativeOnItemSelectionChanged(const bool bIsSelected)
{
	IUserListEntry::NativeOnItemSelectionChanged(bIsSelected);

	if (ActiveItem != nullptr)
	{
		ActiveItem->bIsSelected = bIsSelected;
	}

	RefreshVisualState();
}

void UCodexInteractionUIPlaygroundTileEntryWidget::SetDropTargetHighlighted(const bool bHighlighted)
{
	if (bIsDropTargetHighlighted == bHighlighted)
	{
		return;
	}

	bIsDropTargetHighlighted = bHighlighted;
	RefreshVisualState();
}

void UCodexInteractionUIPlaygroundTileEntryWidget::NativeOnEntryReleased()
{
	IUserListEntry::NativeOnEntryReleased();

	if (ActiveItem != nullptr)
	{
		ActiveItem->bIsSelected = false;
	}

	ActiveItem = nullptr;
	bIsDropTargetHighlighted = false;
	RefreshVisualState();
}

UCodexInteractionUIPlaygroundPopupWidget* UCodexInteractionUIPlaygroundTileEntryWidget::ResolvePopupWidget() const
{
	if (ActiveItem != nullptr)
	{
		if (UCodexInteractionUIPlaygroundPopupWidget* PopupWidget = ActiveItem->GetTypedOuter<UCodexInteractionUIPlaygroundPopupWidget>())
		{
			return PopupWidget;
		}
	}

	return GetTypedOuter<UCodexInteractionUIPlaygroundPopupWidget>();
}

TSubclassOf<UUserWidget> UCodexInteractionUIPlaygroundTileEntryWidget::ResolveTileEntryWidgetClass() const
{
	const TSubclassOf<UUserWidget> WidgetClass = LoadClass<UUserWidget>(
		nullptr,
		*CodexInteractionAssetPaths::MakeGeneratedClassObjectPath(CodexInteractionAssetPaths::UIPlaygroundTileEntryWidgetObjectPath));
	if (WidgetClass != nullptr)
	{
		return WidgetClass;
	}

	return StaticClass();
}

void UCodexInteractionUIPlaygroundTileEntryWidget::RefreshVisualState() const
{
	if (TXT_Label != nullptr)
	{
		if (ActiveItem == nullptr)
		{
			TXT_Label->SetText(FText::GetEmpty());
		}
		else if (ActiveItem->HasValue())
		{
			TXT_Label->SetText(ActiveItem->Label);
		}
		else
		{
			TXT_Label->SetText(FText::FromString(TEXT("Empty")));
		}
	}

	if (TXT_Value != nullptr)
	{
		TXT_Value->SetText(ActiveItem != nullptr && ActiveItem->HasValue() ? FText::AsNumber(ActiveItem->Value) : FText::GetEmpty());
	}

	if (IMG_TileBackground != nullptr)
	{
		const FLinearColor BackgroundTint = ActiveItem != nullptr && ActiveItem->HasValue() ? ActiveItem->TintColor : EmptyTargetTint;
		IMG_TileBackground->SetColorAndOpacity(BackgroundTint);
	}

	if (Border_TileRoot != nullptr)
	{
		const FLinearColor BrushColor = ActiveItem != nullptr && ActiveItem->HasValue() ? ActiveItem->TintColor : EmptyTargetTint;
		Border_TileRoot->SetBrushColor(BrushColor);
	}

	if (Border_Outline != nullptr)
	{
		Border_Outline->SetBrushColor(HighlightTint);
		Border_Outline->SetVisibility(bIsDropTargetHighlighted || (ActiveItem != nullptr && ActiveItem->bIsSelected)
			? ESlateVisibility::SelfHitTestInvisible
			: ESlateVisibility::Collapsed);
	}
}
