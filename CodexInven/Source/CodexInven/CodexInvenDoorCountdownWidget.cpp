// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenDoorCountdownWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Styling/SlateColor.h"

void UCodexInvenDoorCountdownWidget::SetRemainingSeconds(const int32 InRemainingSeconds)
{
	BuildWidgetTreeIfNeeded();
	if (CountdownTextBlock == nullptr)
	{
		return;
	}

	CountdownTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%ds"), FMath::Max(0, InRemainingSeconds))));
}

void UCodexInvenDoorCountdownWidget::SetCountdownVisible(const bool bInVisible)
{
	BuildWidgetTreeIfNeeded();
	if (CountdownTextBlock == nullptr)
	{
		return;
	}

	CountdownTextBlock->SetVisibility(bInVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UCodexInvenDoorCountdownWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetIsFocusable(false);
	BuildWidgetTreeIfNeeded();
	SetCountdownVisible(false);
}

void UCodexInvenDoorCountdownWidget::BuildWidgetTreeIfNeeded()
{
	if (WidgetTree == nullptr)
	{
		WidgetTree = NewObject<UWidgetTree>(this, TEXT("DoorCountdownWidgetTree"));
	}

	if (CountdownTextBlock == nullptr)
	{
		if (WidgetTree->RootWidget != nullptr)
		{
			CountdownTextBlock = Cast<UTextBlock>(WidgetTree->RootWidget);
		}

		if (CountdownTextBlock == nullptr)
		{
			CountdownTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CountdownTextBlock"));
			CountdownTextBlock->SetJustification(ETextJustify::Center);
			CountdownTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.95f, 0.72f, 1.0f)));
			CountdownTextBlock->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.85f));
			CountdownTextBlock->SetShadowOffset(FVector2D(1.0f, 1.0f));

			FSlateFontInfo CountdownFont = CountdownTextBlock->GetFont();
			CountdownFont.Size = 20;
			CountdownTextBlock->SetFont(CountdownFont);

			WidgetTree->RootWidget = CountdownTextBlock;
		}
	}
}
