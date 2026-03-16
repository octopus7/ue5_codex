// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenInventoryDragVisualWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateColor.h"

namespace
{
	constexpr float DragVisualIconSize = 64.0f;
	constexpr int32 DragVisualNameFontSize = 8;
	constexpr int32 DragVisualQuantityFontSize = 14;

	FText BuildInventorySlotLabel(const FCodexInvenInventorySlotData& InSlotData)
	{
		if (!InSlotData.bStackable && InSlotData.UniqueInstanceId != INDEX_NONE)
		{
			return FText::Format(FText::FromString(TEXT("{0} #{1}")), InSlotData.DisplayName, FText::AsNumber(InSlotData.UniqueInstanceId));
		}

		return InSlotData.DisplayName;
	}
}

void UCodexInvenInventoryDragVisualWidget::InitializeFromSlotData(const FCodexInvenInventorySlotData& InSlotData, UTexture2D* InIconTexture)
{
	BuildWidgetTreeIfNeeded();

	if (IconImage == nullptr || NameTextBlock == nullptr || QuantityTextBlock == nullptr)
	{
		return;
	}

	IconImage->SetBrushFromTexture(InIconTexture, true);
	NameTextBlock->SetText(BuildInventorySlotLabel(InSlotData));

	if (InSlotData.bStackable)
	{
		QuantityTextBlock->SetText(FText::AsNumber(InSlotData.Quantity));
		QuantityTextBlock->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		QuantityTextBlock->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCodexInvenInventoryDragVisualWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetIsFocusable(false);
	SetVisibility(ESlateVisibility::HitTestInvisible);
	SetRenderOpacity(0.72f);
	BuildWidgetTreeIfNeeded();
}

void UCodexInvenInventoryDragVisualWidget::BuildWidgetTreeIfNeeded()
{
	if (WidgetTree == nullptr)
	{
		WidgetTree = NewObject<UWidgetTree>(this, TEXT("InventoryDragVisualWidgetTree"));
	}

	if (RootBox == nullptr)
	{
		RootBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("RootBox"));
		WidgetTree->RootWidget = RootBox;
	}
	else if (WidgetTree->RootWidget == nullptr)
	{
		WidgetTree->RootWidget = RootBox;
	}

	if (RootBox == nullptr || RootBox->GetChildrenCount() > 0)
	{
		return;
	}

	UOverlay* IconOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("IconOverlay"));
	if (UVerticalBoxSlot* IconOverlaySlot = RootBox->AddChildToVerticalBox(IconOverlay))
	{
		IconOverlaySlot->SetHorizontalAlignment(HAlign_Center);
		IconOverlaySlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 6.0f));
	}

	USizeBox* IconSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("IconSizeBox"));
	IconSizeBox->SetWidthOverride(DragVisualIconSize);
	IconSizeBox->SetHeightOverride(DragVisualIconSize);
	if (UOverlaySlot* IconSizeSlot = IconOverlay->AddChildToOverlay(IconSizeBox))
	{
		IconSizeSlot->SetHorizontalAlignment(HAlign_Center);
		IconSizeSlot->SetVerticalAlignment(VAlign_Center);
	}

	IconImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("IconImage"));
	IconSizeBox->SetContent(IconImage);

	QuantityTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("QuantityTextBlock"));
	QuantityTextBlock->SetJustification(ETextJustify::Right);
	QuantityTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	QuantityTextBlock->SetShadowColorAndOpacity(FLinearColor::Black);
	QuantityTextBlock->SetShadowOffset(FVector2D(1.0f, 1.0f));
	QuantityTextBlock->SetVisibility(ESlateVisibility::Collapsed);
	QuantityTextBlock->SetFont(FSlateFontInfo(QuantityTextBlock->GetFont().FontObject, DragVisualQuantityFontSize, QuantityTextBlock->GetFont().TypefaceFontName));
	if (UOverlaySlot* QuantitySlot = IconOverlay->AddChildToOverlay(QuantityTextBlock))
	{
		QuantitySlot->SetHorizontalAlignment(HAlign_Right);
		QuantitySlot->SetVerticalAlignment(VAlign_Bottom);
		QuantitySlot->SetPadding(FMargin(0.0f, 0.0f, 2.0f, 2.0f));
	}

	NameTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("NameTextBlock"));
	NameTextBlock->SetJustification(ETextJustify::Center);
	NameTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	NameTextBlock->SetAutoWrapText(true);
	NameTextBlock->SetMinDesiredWidth(72.0f);
	NameTextBlock->SetFont(FSlateFontInfo(NameTextBlock->GetFont().FontObject, DragVisualNameFontSize, NameTextBlock->GetFont().TypefaceFontName));
	if (UVerticalBoxSlot* NameSlot = RootBox->AddChildToVerticalBox(NameTextBlock))
	{
		NameSlot->SetHorizontalAlignment(HAlign_Fill);
		NameSlot->SetVerticalAlignment(VAlign_Bottom);
	}
}
