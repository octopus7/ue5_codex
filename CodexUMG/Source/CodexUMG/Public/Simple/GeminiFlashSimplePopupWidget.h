// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "GeminiFlashSimplePopupWidget.generated.h"

class UUniformGridPanel;
class UGeminiFlashSimpleSlotWidget;

/**
 * Main container for the simplified drag-and-drop demo.
 * Manages a 4x2 grid of slots.
 */
UCLASS(Abstract, Blueprintable)
class CODEXUMG_API UGeminiFlashSimplePopupWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/** Initializes the widget with request data */
	void ApplyPopupRequest(const struct FCodexInteractionPopupRequest& Request, class UCodexInteractionSubsystem& Subsystem);

	// Swaps values between two slots
	void HandleSwap(UGeminiFlashSimpleSlotWidget* FromSlot, UGeminiFlashSimpleSlotWidget* ToSlot);

protected:
	/** UI Bindings */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> Grid_Slots;

private:
	// All managed slots in order
	UPROPERTY(Transient)
	TArray<TObjectPtr<UGeminiFlashSimpleSlotWidget>> Slots;
};
