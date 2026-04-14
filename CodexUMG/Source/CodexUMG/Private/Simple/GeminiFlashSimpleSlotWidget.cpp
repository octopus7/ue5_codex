// Copyright Epic Games, Inc. All Rights Reserved.

#include "Simple/GeminiFlashSimpleSlotWidget.h"
#include "Simple/GeminiFlashSimpleDragDropOperation.h"
#include "Simple/GeminiFlashSimplePopupWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

void UGeminiFlashSimpleSlotWidget::SetItemInstance(const FGeminiFlashItemInstance& InInstance)
{
	ItemInstance = InInstance;

	if (TXT_Value)
	{
		TXT_Value->SetText(ItemInstance.DisplayValue > 0 ? FText::AsNumber(ItemInstance.DisplayValue) : FText::GetEmpty());
	}

	if (TXT_Guid)
	{
		TXT_Guid->SetText(FText::FromString(ItemInstance.GetShortGuid()));
		TXT_Guid->SetVisibility(ItemInstance.IsValid() ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UGeminiFlashSimpleSlotWidget::SetHighlight(bool bHighlight)
{
	if (Border_Highlight)
	{
		Border_Highlight->SetBrushColor(bHighlight ? FLinearColor::Yellow : FLinearColor(1, 1, 1, 0.1f));
	}
}

FReply UGeminiFlashSimpleSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// Detect drag if clicking with Left Mouse Button on a non-empty slot
	if (ItemInstance.IsValid() && InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UGeminiFlashSimpleSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	// Create our custom operation
	UGeminiFlashSimpleDragDropOperation* Operation = Cast<UGeminiFlashSimpleDragDropOperation>(
		UWidgetBlueprintLibrary::CreateDragDropOperation(UGeminiFlashSimpleDragDropOperation::StaticClass()));

	if (Operation)
	{
		Operation->DraggedItem = ItemInstance;
		Operation->SourceWidget = this;
		
		// Create a separate visual widget so it doesn't block hit testing
		if (UUserWidget* VisualWidget = CreateWidget<UUserWidget>(this, GetClass()))
		{
			if (UGeminiFlashSimpleSlotWidget* CastVisual = Cast<UGeminiFlashSimpleSlotWidget>(VisualWidget))
			{
				CastVisual->SetItemInstance(ItemInstance);
				CastVisual->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			Operation->DefaultDragVisual = VisualWidget;
		}
		
		Operation->Pivot = EDragPivot::CenterCenter;

		OutOperation = Operation;
	}
}

void UGeminiFlashSimpleSlotWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	if (Cast<UGeminiFlashSimpleDragDropOperation>(InOperation))
	{
		SetHighlight(true);
	}
}

void UGeminiFlashSimpleSlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	SetHighlight(false);
}

bool UGeminiFlashSimpleSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (UGeminiFlashSimpleDragDropOperation* SimpleOp = Cast<UGeminiFlashSimpleDragDropOperation>(InOperation))
	{
		// Find the common parent PopupWidget to handle the swap logic
		if (UGeminiFlashSimplePopupWidget* ParentPopup = GetTypedOuter<UGeminiFlashSimplePopupWidget>())
		{
			if (UGeminiFlashSimpleSlotWidget* SourceSlot = Cast<UGeminiFlashSimpleSlotWidget>(SimpleOp->SourceWidget))
			{
				ParentPopup->HandleSwap(SourceSlot, this);
				SetHighlight(false);
				return true;
			}
		}
	}

	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
