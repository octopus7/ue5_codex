// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenOwnershipDebugWidget.h"

#include "Blueprint/WidgetTree.h"
#include "CodexInvenOwnershipComponent.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Styling/SlateColor.h"

void UCodexInvenOwnershipDebugWidget::SetObservedOwnershipComponent(UCodexInvenOwnershipComponent* InComponent)
{
	BuildWidgetTreeIfNeeded();

	if (ObservedOwnershipComponent == InComponent)
	{
		RefreshOwnershipText();
		return;
	}

	UnbindObservedOwnershipComponent();
	ObservedOwnershipComponent = InComponent;

	if (ObservedOwnershipComponent != nullptr)
	{
		ObservedOwnershipComponent->OnOwnershipChanged.AddUObject(this, &ThisClass::HandleOwnershipChanged);
	}

	RefreshOwnershipText();
}

void UCodexInvenOwnershipDebugWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetIsFocusable(false);
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	BuildWidgetTreeIfNeeded();
	RefreshOwnershipText();
}

void UCodexInvenOwnershipDebugWidget::NativeDestruct()
{
	UnbindObservedOwnershipComponent();

	Super::NativeDestruct();
}

void UCodexInvenOwnershipDebugWidget::BuildWidgetTreeIfNeeded()
{
	if (WidgetTree == nullptr)
	{
		WidgetTree = NewObject<UWidgetTree>(this, TEXT("OwnershipDebugWidgetTree"));
	}

	if (WidgetTree->RootWidget == nullptr)
	{
		RootCanvasPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvasPanel"));
		WidgetTree->RootWidget = RootCanvasPanel;
	}
	else if (RootCanvasPanel == nullptr)
	{
		RootCanvasPanel = Cast<UCanvasPanel>(WidgetTree->RootWidget);
	}

	if (RootCanvasPanel == nullptr)
	{
		return;
	}

	if (OwnershipTextBlock == nullptr)
	{
		OwnershipTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("OwnershipTextBlock"));
		OwnershipTextBlock->SetJustification(ETextJustify::Right);
		OwnershipTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		OwnershipTextBlock->SetShadowColorAndOpacity(FLinearColor::Black);
		OwnershipTextBlock->SetShadowOffset(FVector2D(1.0f, 1.0f));
		OwnershipTextBlock->SetAutoWrapText(false);
		OwnershipTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (UCanvasPanelSlot* TextSlot = RootCanvasPanel->AddChildToCanvas(OwnershipTextBlock))
		{
			TextSlot->SetAutoSize(true);
			TextSlot->SetAnchors(FAnchors(1.0f, 0.0f, 1.0f, 0.0f));
			TextSlot->SetAlignment(FVector2D(1.0f, 0.0f));
			TextSlot->SetPosition(FVector2D(-24.0f, 24.0f));
		}
	}
}

void UCodexInvenOwnershipDebugWidget::RefreshOwnershipText() const
{
	if (OwnershipTextBlock == nullptr)
	{
		return;
	}

	if (ObservedOwnershipComponent == nullptr)
	{
		OwnershipTextBlock->SetText(FText::FromString(TEXT("Ownership Debug")));
		return;
	}

	OwnershipTextBlock->SetText(ObservedOwnershipComponent->BuildDebugOwnershipText());
}

void UCodexInvenOwnershipDebugWidget::UnbindObservedOwnershipComponent()
{
	if (ObservedOwnershipComponent != nullptr)
	{
		ObservedOwnershipComponent->OnOwnershipChanged.RemoveAll(this);
		ObservedOwnershipComponent = nullptr;
	}
}

void UCodexInvenOwnershipDebugWidget::HandleOwnershipChanged(const ECodexInvenPickupType InPickupType, const int32 InDelta, const int32 InNewTotal)
{
	static_cast<void>(InPickupType);
	static_cast<void>(InDelta);
	static_cast<void>(InNewTotal);

	RefreshOwnershipText();
}
