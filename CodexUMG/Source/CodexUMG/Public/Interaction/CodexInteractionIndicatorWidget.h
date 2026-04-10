// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Interaction/CodexInteractionTypes.h"
#include "CodexInteractionIndicatorWidget.generated.h"

class UBorder;
class UImage;
class UTextBlock;

UCLASS(Abstract, Blueprintable)
class CODEXUMG_API UCodexInteractionIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void ApplyInteractionState(ECodexInteractionWidgetState NewState, const FText& NewPromptText);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IMG_FilledCircle;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IMG_OuterRing;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> BOR_PromptBackground;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Prompt;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Codex|Interaction", meta = (ClampMin = "0.01"))
	float TransitionDuration = 0.18f;

private:
	void RefreshVisualState() const;

	ECodexInteractionWidgetState TargetState = ECodexInteractionWidgetState::Hidden;
	float CurrentVisibleAlpha = 0.0f;
	float CurrentPromptAlpha = 0.0f;
	FText CurrentPromptText;
};
