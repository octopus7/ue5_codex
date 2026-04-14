// Copyright Epic Games, Inc. All Rights Reserved.

#include "Simple/GeminiFlashSimpleSlotWidget.h"
#include "Simple/GeminiFlashSimpleDragDropOperation.h"
#include "Simple/GeminiFlashSimplePopupWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

void UGeminiFlashSimpleSlotWidget::SetValue(int32 InValue)
{
	Value = InValue;
	if (TXT_Value)
	{
		TXT_Value->SetText(Value > 0 ? FText::AsNumber(Value) : FText::GetEmpty());
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
	if (Value > 0 && InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
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
		Operation->Value = Value;
		Operation->SourceWidget = this;
		Operation->DefaultDragVisual = this; // Use itself as visual for simplicity
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
