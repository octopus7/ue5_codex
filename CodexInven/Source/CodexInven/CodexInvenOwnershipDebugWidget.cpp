// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenOwnershipDebugWidget.h"

#include "Blueprint/WidgetTree.h"
#include "CodexInvenOwnershipComponent.h"
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
		ObservedOwnershipComponent->OnInventoryChanged.AddUObject(this, &ThisClass::HandleInventoryChanged);
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

	if (OwnershipTextBlock == nullptr)
	{
		OwnershipTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("OwnershipTextBlock"));
		OwnershipTextBlock->SetJustification(ETextJustify::Right);
		OwnershipTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		OwnershipTextBlock->SetShadowColorAndOpacity(FLinearColor::Black);
		OwnershipTextBlock->SetShadowOffset(FVector2D(1.0f, 1.0f));
		OwnershipTextBlock->SetAutoWrapText(false);
		OwnershipTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		WidgetTree->RootWidget = OwnershipTextBlock;
	}
	else if (WidgetTree->RootWidget == nullptr)
	{
		WidgetTree->RootWidget = OwnershipTextBlock;
	}
	else if (OwnershipTextBlock == nullptr)
	{
		OwnershipTextBlock = Cast<UTextBlock>(WidgetTree->RootWidget);
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
		ObservedOwnershipComponent->OnInventoryChanged.RemoveAll(this);
		ObservedOwnershipComponent = nullptr;
	}
}

void UCodexInvenOwnershipDebugWidget::HandleInventoryChanged()
{
	RefreshOwnershipText();
}
