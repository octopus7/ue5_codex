// Copyright Epic Games, Inc. All Rights Reserved.

#include "TigerClickerWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
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

	UVerticalBox* RootBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("TigerClickerRoot"));
	RootBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	WidgetTree->RootWidget = RootBox;

	RiceCakeText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("RiceCakeText"));
	ClickPowerText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ClickPowerText"));
	UpgradeButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("UpgradeButton"));
	UpgradeButtonText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("UpgradeButtonText"));

	RiceCakeText->SetText(FText::FromString(TEXT("Rice Cakes: 0")));
	ClickPowerText->SetText(FText::FromString(TEXT("Click Power: 1")));
	UpgradeButtonText->SetText(FText::FromString(TEXT("Upgrade")));

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
