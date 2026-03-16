// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenInventoryTileEntryWidget.h"

#include "Blueprint/WidgetTree.h"
#include "CodexInvenInventoryTileItemObject.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateColor.h"

namespace
{
	constexpr float InventoryTileIconSize = 64.0f;
	constexpr int32 InventoryTileNameFontSize = 8;
	constexpr int32 InventoryTileQuantityFontSize = 14;
}

void UCodexInvenInventoryTileEntryWidget::SetTileItemObject(UCodexInvenInventoryTileItemObject* InItemObject)
{
	BuildWidgetTreeIfNeeded();
	RefreshFromItemObject(InItemObject);
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

	UVerticalBox* ContentBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ContentBox"));
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

void UCodexInvenInventoryTileEntryWidget::RefreshFromItemObject(UCodexInvenInventoryTileItemObject* InItemObject) const
{
	if (IconImage == nullptr || NameTextBlock == nullptr || QuantityTextBlock == nullptr)
	{
		return;
	}

	if (InItemObject == nullptr)
	{
		RootBorder->SetBrushColor(FLinearColor(0.03f, 0.04f, 0.05f, 0.92f));
		IconImage->SetBrush(FSlateBrush());
		IconImage->SetVisibility(ESlateVisibility::Collapsed);
		NameTextBlock->SetText(FText::GetEmpty());
		QuantityTextBlock->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	const FCodexInvenInventorySlotData& SlotData = InItemObject->GetSlotData();

	if (SlotData.bIsEmpty)
	{
		RootBorder->SetBrushColor(FLinearColor(0.03f, 0.04f, 0.05f, 0.92f));
		IconImage->SetBrush(FSlateBrush());
		IconImage->SetVisibility(ESlateVisibility::Collapsed);
		NameTextBlock->SetText(FText::GetEmpty());
		QuantityTextBlock->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	RootBorder->SetBrushColor(FLinearColor(0.08f, 0.10f, 0.12f, 0.92f));

	IconImage->SetBrushFromTexture(InItemObject->GetIconTexture(), true);
	IconImage->SetVisibility(ESlateVisibility::Visible);

	FText EntryDisplayLabel = SlotData.DisplayName;
	if (!SlotData.bStackable && SlotData.UniqueInstanceId != INDEX_NONE)
	{
		EntryDisplayLabel = FText::Format(FText::FromString(TEXT("{0} #{1}")), SlotData.DisplayName, FText::AsNumber(SlotData.UniqueInstanceId));
	}

	NameTextBlock->SetText(EntryDisplayLabel);

	if (SlotData.bStackable)
	{
		QuantityTextBlock->SetText(FText::AsNumber(SlotData.Quantity));
		QuantityTextBlock->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		QuantityTextBlock->SetVisibility(ESlateVisibility::Collapsed);
	}
}
