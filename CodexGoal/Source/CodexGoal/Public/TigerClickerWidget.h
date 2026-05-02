// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TigerClickerWidget.generated.h"

class ATigerShopPlayerController;
class UButton;
class UTextBlock;

UCLASS()
class CODEXGOAL_API UTigerClickerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeForPlayer(ATigerShopPlayerController* InPlayerController);

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void HandleUpgradeClicked();

	UFUNCTION()
	void HandleCurrencyChanged();

	void Refresh();

	UPROPERTY()
	TObjectPtr<ATigerShopPlayerController> TigerPlayerController;

	UPROPERTY()
	TObjectPtr<UTextBlock> RiceCakeText;

	UPROPERTY()
	TObjectPtr<UTextBlock> ClickPowerText;

	UPROPERTY()
	TObjectPtr<UTextBlock> UpgradeButtonText;

	UPROPERTY()
	TObjectPtr<UButton> UpgradeButton;
};
