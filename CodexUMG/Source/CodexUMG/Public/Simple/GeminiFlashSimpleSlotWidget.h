// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Simple/GeminiFlashSimpleTypes.h"
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
	// Sets the item instance of the slot and updates visuals
	void SetItemInstance(const FGeminiFlashItemInstance& InInstance);

	// Gets current item instance
	const FGeminiFlashItemInstance& GetItemInstance() const { return ItemInstance; }

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

	UPROPERTY(meta = (BindWidget, OptionalWidget = true))
	TObjectPtr<UTextBlock> TXT_Guid;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border_Highlight;

private:
	UPROPERTY(Transient)
	FGeminiFlashItemInstance ItemInstance;
};
