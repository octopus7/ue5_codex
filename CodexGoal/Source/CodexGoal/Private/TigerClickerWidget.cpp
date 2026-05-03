// Copyright Epic Games, Inc. All Rights Reserved.

#include "TigerClickerWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "TigerCurrencyComponent.h"
#include "TigerShopPlayerController.h"

void UTigerClickerWidget::InitializeForPlayer(ATigerShopPlayerController* InPlayerController)
{
	TigerPlayerController = InPlayerController;

	if (UTigerCurrencyComponent* Currency = TigerPlayerController ? TigerPlayerController->GetCurrencyComponent() : nullptr)
	{
		Currency->OnCurrencyChanged.AddUniqueDynamic(this, &UTigerClickerWidget::HandleCurrencyChanged);
	}

	Refresh();
}

void UTigerClickerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("TigerClickerRoot"));
	RootCanvas->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	WidgetTree->RootWidget = RootCanvas;

	UBorder* PanelBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("HudPanelBorder"));
	PanelBorder->SetPadding(FMargin(16.0f, 12.0f));
	PanelBorder->SetBrushColor(FLinearColor(0.05f, 0.035f, 0.02f, 0.82f));

	UVerticalBox* RootBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("HudContent"));
	RootBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	PanelBorder->SetContent(RootBox);

	UCanvasPanelSlot* PanelSlot = RootCanvas->AddChildToCanvas(PanelBorder);
	if (PanelSlot)
	{
		PanelSlot->SetAnchors(FAnchors(0.0f, 0.0f));
		PanelSlot->SetAlignment(FVector2D(0.0f, 0.0f));
		PanelSlot->SetPosition(FVector2D(28.0f, 28.0f));
		PanelSlot->SetAutoSize(true);
	}

	RiceCakeText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("RiceCakeText"));
	ClickPowerText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ClickPowerText"));
	UpgradeButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("UpgradeButton"));
	UpgradeButtonText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("UpgradeButtonText"));

	RiceCakeText->SetText(FText::FromString(TEXT("Rice Cakes: 0")));
	RiceCakeText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.88f, 0.46f)));
	ClickPowerText->SetText(FText::FromString(TEXT("Click Power: 1")));
	ClickPowerText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	UpgradeButtonText->SetText(FText::FromString(TEXT("Upgrade")));
	UpgradeButtonText->SetColorAndOpacity(FSlateColor(FLinearColor(0.04f, 0.025f, 0.015f)));

	RootBox->AddChildToVerticalBox(RiceCakeText);
	RootBox->AddChildToVerticalBox(ClickPowerText);

	UpgradeButton->AddChild(UpgradeButtonText);
	UVerticalBoxSlot* UpgradeSlot = RootBox->AddChildToVerticalBox(UpgradeButton);
	if (UpgradeSlot)
	{
		UpgradeSlot->SetPadding(FMargin(0.0f, 8.0f, 0.0f, 0.0f));
	}

	UpgradeButton->OnClicked.AddDynamic(this, &UTigerClickerWidget::HandleUpgradeClicked);
	Refresh();
}

void UTigerClickerWidget::HandleUpgradeClicked()
{
	if (TigerPlayerController)
	{
		TigerPlayerController->TryBuyClickUpgrade();
	}
}

void UTigerClickerWidget::HandleCurrencyChanged()
{
	Refresh();
}

void UTigerClickerWidget::Refresh()
{
	UTigerCurrencyComponent* Currency = TigerPlayerController ? TigerPlayerController->GetCurrencyComponent() : nullptr;
	if (!Currency)
	{
		return;
	}

	if (RiceCakeText)
	{
		RiceCakeText->SetText(FText::Format(
			FText::FromString(TEXT("Rice Cakes: {0}")),
			FText::AsNumber(Currency->GetRiceCakeCount())));
	}

	if (ClickPowerText)
	{
		ClickPowerText->SetText(FText::Format(
			FText::FromString(TEXT("Click Power: {0}")),
			FText::AsNumber(Currency->GetClickPower())));
	}

	if (UpgradeButtonText)
	{
		UpgradeButtonText->SetText(FText::Format(
			FText::FromString(TEXT("Upgrade +1 - Cost {0}")),
			FText::AsNumber(Currency->GetNextClickUpgradeCost())));
	}

	if (UpgradeButton)
	{
		UpgradeButton->SetIsEnabled(Currency->CanBuyClickUpgrade());
	}
}
