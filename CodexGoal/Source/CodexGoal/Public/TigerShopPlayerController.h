// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TigerShopPlayerController.generated.h"

class ARiceCakeWorkstation;
class ATemporaryTigerCharacter;
class UTigerClickerWidget;
class UTigerCurrencyComponent;

UCLASS()
class CODEXGOAL_API ATigerShopPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATigerShopPlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UTigerCurrencyComponent* GetCurrencyComponent() const { return CurrencyComponent; }
	bool TryBuyClickUpgrade();

	void HandleWorkstationClicked(ARiceCakeWorkstation* Workstation);
	void RegisterTemporaryTiger(ATemporaryTigerCharacter* InTemporaryTiger);

private:
	UFUNCTION()
	void HandleCurrencyChangedForSave();

	void HandlePrimaryClick();
	void BuildClickerWidget();
	void LoadProgress();
	void SaveProgress() const;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<UTigerCurrencyComponent> CurrencyComponent;

	UPROPERTY()
	TObjectPtr<UTigerClickerWidget> ClickerWidget;

	UPROPERTY()
	TObjectPtr<ATemporaryTigerCharacter> TemporaryTiger;
};
