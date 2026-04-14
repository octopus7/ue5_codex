// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "GeminiFlashSimpleSlotWidget.generated.h"

class UTextBlock;
class UBorder;

/**
 * Minimal slot widget that can detect drag and handle drop.
 */
UCLASS(Abstract, Blueprintable)
class CODEXUMG_API UGeminiFlashSimpleSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Sets the value of the slot and updates visuals
	void SetValue(int32 InValue);

	// Gets current value
	int32 GetValue() const { return Value; }

	// Toggles highlight border
	void SetHighlight(bool bHighlight);

protected:
	// UUserWidget Overrides
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	/** UI Bindings */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Value;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border_Highlight;

private:
	int32 Value = 0;
};
