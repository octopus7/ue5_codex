// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CodexInteractionIndicatorWidget.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCodexInteractionIndicatorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TargetState = ECodexInteractionWidgetState::Hidden;
	CurrentVisibleAlpha = 0.0f;
	CurrentPromptAlpha = 0.0f;
	RefreshVisualState();
}

void UCodexInteractionIndicatorWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const float InterpSpeed = 1.0f / FMath::Max(TransitionDuration, KINDA_SMALL_NUMBER);
	const float TargetVisibleAlpha = TargetState == ECodexInteractionWidgetState::Hidden ? 0.0f : 1.0f;
	const float TargetPromptAlpha = TargetState == ECodexInteractionWidgetState::Interactable ? 1.0f : 0.0f;

	CurrentVisibleAlpha = FMath::FInterpConstantTo(CurrentVisibleAlpha, TargetVisibleAlpha, InDeltaTime, InterpSpeed);
	CurrentPromptAlpha = FMath::FInterpConstantTo(CurrentPromptAlpha, TargetPromptAlpha, InDeltaTime, InterpSpeed);

	RefreshVisualState();
}

void UCodexInteractionIndicatorWidget::ApplyInteractionState(const ECodexInteractionWidgetState NewState, const FText& NewPromptText)
{
	TargetState = NewState;
	CurrentPromptText = NewPromptText;
	RefreshVisualState();
}

void UCodexInteractionIndicatorWidget::RefreshVisualState() const
{
	const bool bShouldTickMarker = TargetState != ECodexInteractionWidgetState::Hidden || CurrentVisibleAlpha > KINDA_SMALL_NUMBER;
	const bool bShouldTickPrompt = TargetState == ECodexInteractionWidgetState::Interactable || CurrentPromptAlpha > KINDA_SMALL_NUMBER;
	const bool bAnyVisible = bShouldTickMarker || bShouldTickPrompt;
	const_cast<UCodexInteractionIndicatorWidget*>(this)->SetVisibility(bAnyVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);

	if (IMG_FilledCircle)
	{
		IMG_FilledCircle->SetVisibility(bShouldTickMarker ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		IMG_FilledCircle->SetRenderOpacity(CurrentVisibleAlpha);
	}

	if (IMG_OuterRing)
	{
		IMG_OuterRing->SetVisibility(bShouldTickMarker ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		IMG_OuterRing->SetRenderOpacity(CurrentVisibleAlpha * 0.45f);
		const float RingScale = FMath::Lerp(1.6f, 1.0f, CurrentVisibleAlpha);
		IMG_OuterRing->SetRenderScale(FVector2D(RingScale, RingScale));
	}

	if (BOR_PromptBackground)
	{
		BOR_PromptBackground->SetVisibility(bShouldTickPrompt ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		BOR_PromptBackground->SetRenderOpacity(CurrentPromptAlpha);
		BOR_PromptBackground->SetRenderTranslation(FVector2D(FMath::Lerp(12.0f, 0.0f, CurrentPromptAlpha), 0.0f));
	}

	if (TXT_Prompt)
	{
		TXT_Prompt->SetText(CurrentPromptText);
		TXT_Prompt->SetVisibility(bShouldTickPrompt ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		TXT_Prompt->SetRenderOpacity(CurrentPromptAlpha);
	}
}
