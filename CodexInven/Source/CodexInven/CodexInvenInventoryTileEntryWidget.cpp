// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenInventoryTileEntryWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "CodexInvenInventoryDragDropOperation.h"
#include "CodexInvenInventoryDragVisualWidget.h"
#include "CodexInvenInventoryTileItemObject.h"
#include "CodexInvenPlayerHudWidget.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/Texture2D.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateColor.h"

namespace
{
	constexpr float InventoryTileIconSize = 64.0f;
	constexpr int32 InventoryTileNameFontSize = 8;
	constexpr int32 InventoryTileQuantityFontSize = 14;

	FText BuildInventorySlotLabel(const FCodexInvenInventorySlotData& InSlotData)
	{
		if (!InSlotData.bStackable && InSlotData.UniqueInstanceId != INDEX_NONE)
		{
			return FText::Format(FText::FromString(TEXT("{0} #{1}")), InSlotData.DisplayName, FText::AsNumber(InSlotData.UniqueInstanceId));
		}

		return InSlotData.DisplayName;
	}
}

void UCodexInvenInventoryTileEntryWidget::SetTileItemObject(UCodexInvenInventoryTileItemObject* InItemObject)
{
	BuildWidgetTreeIfNeeded();
	RefreshFromItemObject(InItemObject);
}

void UCodexInvenInventoryTileEntryWidget::SetOwningHudWidget(UCodexInvenPlayerHudWidget* InOwningHudWidget)
{
	OwningHudWidget = InOwningHudWidget;
}

void UCodexInvenInventoryTileEntryWidget::SetVisualState(const bool bInIsDragSource, const bool bInIsDropTarget)
{
	bIsDragSource = bInIsDragSource;
	bIsDropTarget = bInIsDropTarget;
	ApplyVisualState();
}

int32 UCodexInvenInventoryTileEntryWidget::GetSlotIndex() const
{
	const UCodexInvenInventoryTileItemObject* ItemObject = GetTileItemObject();
	return ItemObject != nullptr ? ItemObject->GetSlotData().SlotIndex : INDEX_NONE;
}

void UCodexInvenInventoryTileEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetIsFocusable(false);
	BuildWidgetTreeIfNeeded();
}

void UCodexInvenInventoryTileEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	BuildWidgetTreeIfNeeded();
	RefreshFromItemObject(Cast<UCodexInvenInventoryTileItemObject>(ListItemObject));
}

FReply UCodexInvenInventoryTileEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	const UCodexInvenInventoryTileItemObject* ItemObject = GetTileItemObject();
	if (ItemObject == nullptr || ItemObject->GetSlotData().bIsEmpty)
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

void UCodexInvenInventoryTileEntryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	const UCodexInvenInventoryTileItemObject* ItemObject = GetTileItemObject();
	if (ItemObject == nullptr || OwningHudWidget == nullptr)
	{
		return;
	}

	const FCodexInvenInventorySlotData& SlotData = ItemObject->GetSlotData();
	if (SlotData.bIsEmpty)
	{
		return;
	}

	UCodexInvenInventoryDragDropOperation* DragOperation = Cast<UCodexInvenInventoryDragDropOperation>(
		UWidgetBlueprintLibrary::CreateDragDropOperation(UCodexInvenInventoryDragDropOperation::StaticClass()));
	if (DragOperation == nullptr)
	{
		return;
	}

	UCodexInvenInventoryDragVisualWidget* DragVisual = CreateWidget<UCodexInvenInventoryDragVisualWidget>(
		GetOwningPlayer(),
		UCodexInvenInventoryDragVisualWidget::StaticClass());
	if (DragVisual != nullptr)
	{
		DragVisual->InitializeFromSlotData(SlotData, ItemObject->GetIconTexture());
		DragOperation->DefaultDragVisual = DragVisual;
	}

	DragOperation->Pivot = EDragPivot::MouseDown;
	DragOperation->InitializeOperation(SlotData.SlotIndex, SlotData, ItemObject->GetIconTexture(), OwningHudWidget);
	OwningHudWidget->BeginInventoryDrag(SlotData.SlotIndex);
	OutOperation = DragOperation;
}

void UCodexInvenInventoryTileEntryWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	const UCodexInvenInventoryDragDropOperation* DragOperation = Cast<UCodexInvenInventoryDragDropOperation>(InOperation);
	if (DragOperation == nullptr || OwningHudWidget == nullptr)
	{
		return;
	}

	OwningHudWidget->SetHoveredInventoryDropTarget(DragOperation->GetSourceSlotIndex() == GetSlotIndex() ? INDEX_NONE : GetSlotIndex());
}

void UCodexInvenInventoryTileEntryWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	const UCodexInvenInventoryDragDropOperation* DragOperation = Cast<UCodexInvenInventoryDragDropOperation>(InOperation);
	if (DragOperation == nullptr || OwningHudWidget == nullptr)
	{
		return;
	}

	OwningHudWidget->ClearHoveredInventoryDropTarget(GetSlotIndex());
}

bool UCodexInvenInventoryTileEntryWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	const UCodexInvenInventoryDragDropOperation* DragOperation = Cast<UCodexInvenInventoryDragDropOperation>(InOperation);
	if (DragOperation == nullptr || OwningHudWidget == nullptr)
	{
		return false;
	}

	return OwningHudWidget->HandleInventorySlotDrop(DragOperation->GetSourceSlotIndex(), GetSlotIndex());
}

void UCodexInvenInventoryTileEntryWidget::BuildWidgetTreeIfNeeded()
{
	if (WidgetTree == nullptr)
	{
		WidgetTree = NewObject<UWidgetTree>(this, TEXT("InventoryTileEntryWidgetTree"));
	}

	if (RootBorder == nullptr)
	{
		RootBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("RootBorder"));
		RootBorder->SetPadding(FMargin(8.0f));
		RootBorder->SetBrushColor(FLinearColor(0.08f, 0.10f, 0.12f, 0.92f));
		WidgetTree->RootWidget = RootBorder;
	}
	else if (WidgetTree->RootWidget == nullptr)
	{
		WidgetTree->RootWidget = RootBorder;
	}

	if (RootBorder == nullptr || RootBorder->GetContent() != nullptr)
	{
		return;
	}

	UOverlay* RootOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("RootOverlay"));
	RootBorder->SetContent(RootOverlay);

	RarityBackgroundImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("RarityBackgroundImage"));
	RarityBackgroundImage->SetVisibility(ESlateVisibility::Collapsed);
	if (UOverlaySlot* BackgroundSlot = RootOverlay->AddChildToOverlay(RarityBackgroundImage))
	{
		BackgroundSlot->SetHorizontalAlignment(HAlign_Fill);
		BackgroundSlot->SetVerticalAlignment(VAlign_Fill);
	}

	ContentBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ContentBox"));
	if (UOverlaySlot* ContentSlot = RootOverlay->AddChildToOverlay(ContentBox))
	{
		ContentSlot->SetHorizontalAlignment(HAlign_Fill);
		ContentSlot->SetVerticalAlignment(VAlign_Fill);
	}

	USizeBox* IconSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("IconSizeBox"));
	IconSizeBox->SetWidthOverride(InventoryTileIconSize);
	IconSizeBox->SetHeightOverride(InventoryTileIconSize);
	if (UVerticalBoxSlot* IconSlot = ContentBox->AddChildToVerticalBox(IconSizeBox))
	{
		IconSlot->SetHorizontalAlignment(HAlign_Center);
		IconSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 6.0f));
	}

	IconImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("IconImage"));
	IconSizeBox->SetContent(IconImage);

	NameTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("NameTextBlock"));
	NameTextBlock->SetJustification(ETextJustify::Center);
	NameTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	NameTextBlock->SetAutoWrapText(true);
	NameTextBlock->SetMinDesiredWidth(72.0f);
	NameTextBlock->SetFont(FSlateFontInfo(NameTextBlock->GetFont().FontObject, InventoryTileNameFontSize, NameTextBlock->GetFont().TypefaceFontName));
	if (UVerticalBoxSlot* NameSlot = ContentBox->AddChildToVerticalBox(NameTextBlock))
	{
		NameSlot->SetHorizontalAlignment(HAlign_Fill);
		NameSlot->SetVerticalAlignment(VAlign_Bottom);
		NameSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}

	QuantityTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("QuantityTextBlock"));
	QuantityTextBlock->SetJustification(ETextJustify::Right);
	QuantityTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	QuantityTextBlock->SetShadowColorAndOpacity(FLinearColor::Black);
	QuantityTextBlock->SetShadowOffset(FVector2D(1.0f, 1.0f));
	QuantityTextBlock->SetVisibility(ESlateVisibility::Collapsed);
	QuantityTextBlock->SetFont(FSlateFontInfo(QuantityTextBlock->GetFont().FontObject, InventoryTileQuantityFontSize, QuantityTextBlock->GetFont().TypefaceFontName));
	if (UOverlaySlot* QuantitySlot = RootOverlay->AddChildToOverlay(QuantityTextBlock))
	{
		QuantitySlot->SetHorizontalAlignment(HAlign_Right);
		QuantitySlot->SetVerticalAlignment(VAlign_Bottom);
		QuantitySlot->SetPadding(FMargin(0.0f, 0.0f, 2.0f, 2.0f));
	}
}

void UCodexInvenInventoryTileEntryWidget::RefreshFromItemObject(UCodexInvenInventoryTileItemObject* InItemObject)
{
	TileItemObject = InItemObject;

	if (RarityBackgroundImage == nullptr || IconImage == nullptr || NameTextBlock == nullptr || QuantityTextBlock == nullptr)
	{
		return;
	}

	if (InItemObject == nullptr)
	{
		RarityBackgroundImage->SetBrush(FSlateBrush());
		RarityBackgroundImage->SetVisibility(ESlateVisibility::Collapsed);
		IconImage->SetBrush(FSlateBrush());
		IconImage->SetVisibility(ESlateVisibility::Collapsed);
		NameTextBlock->SetText(FText::GetEmpty());
		QuantityTextBlock->SetVisibility(ESlateVisibility::Collapsed);
		ApplyVisualState();
		return;
	}

	const FCodexInvenInventorySlotData& SlotData = InItemObject->GetSlotData();

	if (SlotData.bIsEmpty)
	{
		RarityBackgroundImage->SetBrush(FSlateBrush());
		RarityBackgroundImage->SetVisibility(ESlateVisibility::Collapsed);
		IconImage->SetBrush(FSlateBrush());
		IconImage->SetVisibility(ESlateVisibility::Collapsed);
		NameTextBlock->SetText(FText::GetEmpty());
		QuantityTextBlock->SetVisibility(ESlateVisibility::Collapsed);
		ApplyVisualState();
		return;
	}

	if (UTexture2D* BackgroundTexture = InItemObject->GetBackgroundTexture())
	{
		RarityBackgroundImage->SetBrushFromTexture(BackgroundTexture, true);
		RarityBackgroundImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		RarityBackgroundImage->SetBrush(FSlateBrush());
		RarityBackgroundImage->SetVisibility(ESlateVisibility::Collapsed);
	}

	IconImage->SetBrushFromTexture(InItemObject->GetIconTexture(), true);
	IconImage->SetVisibility(ESlateVisibility::Visible);
	NameTextBlock->SetText(BuildInventorySlotLabel(SlotData));

	if (SlotData.bStackable)
	{
		QuantityTextBlock->SetText(FText::AsNumber(SlotData.Quantity));
		QuantityTextBlock->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		QuantityTextBlock->SetVisibility(ESlateVisibility::Collapsed);
	}

	ApplyVisualState();
}

void UCodexInvenInventoryTileEntryWidget::ApplyVisualState() const
{
	if (RootBorder == nullptr || RarityBackgroundImage == nullptr || IconImage == nullptr || NameTextBlock == nullptr || QuantityTextBlock == nullptr)
	{
		return;
	}

	const UCodexInvenInventoryTileItemObject* ItemObject = GetTileItemObject();
	const bool bIsEmptySlot = ItemObject == nullptr || ItemObject->GetSlotData().bIsEmpty;
	const bool bIsGoldRarity = !bIsEmptySlot && ItemObject->GetSlotData().Rarity == ECodexInvenPickupRarity::Gold;

	FLinearColor BorderColor = bIsEmptySlot
		? FLinearColor(0.03f, 0.04f, 0.05f, 0.92f)
		: (bIsGoldRarity ? FLinearColor(0.12f, 0.10f, 0.05f, 0.94f) : FLinearColor(0.08f, 0.10f, 0.12f, 0.92f));

	if (bIsDropTarget)
	{
		BorderColor = bIsEmptySlot
			? FLinearColor(0.10f, 0.14f, 0.18f, 0.96f)
			: FLinearColor(0.14f, 0.20f, 0.28f, 0.98f);
	}

	RootBorder->SetBrushColor(BorderColor);
	RarityBackgroundImage->SetRenderOpacity(bIsGoldRarity ? 0.82f : 1.0f);

	const float ContentOpacity = bIsDragSource && !bIsEmptySlot ? 0.35f : 1.0f;
	IconImage->SetRenderOpacity(ContentOpacity);
	NameTextBlock->SetRenderOpacity(ContentOpacity);
	QuantityTextBlock->SetRenderOpacity(ContentOpacity);
}

const UCodexInvenInventoryTileItemObject* UCodexInvenInventoryTileEntryWidget::GetTileItemObject() const
{
	return TileItemObject;
}
