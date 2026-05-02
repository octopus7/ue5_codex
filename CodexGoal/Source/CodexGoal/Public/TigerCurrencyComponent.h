// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TigerCurrencyComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTigerCurrencyChanged);

UCLASS(ClassGroup=(TigerShop), meta=(BlueprintSpawnableComponent))
class CODEXGOAL_API UTigerCurrencyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTigerCurrencyComponent();

	UPROPERTY(BlueprintAssignable, Category="Tiger Shop")
	FOnTigerCurrencyChanged OnCurrencyChanged;

	UFUNCTION(BlueprintCallable, Category="Tiger Shop")
	void AddRiceCakes(int64 Amount);

	UFUNCTION(BlueprintCallable, Category="Tiger Shop")
	bool TryBuyClickUpgrade();

	UFUNCTION(BlueprintCallable, Category="Tiger Shop")
	void SetState(int64 InRiceCakeCount, int32 InClickPower, int32 InClickUpgradeLevel);

	UFUNCTION(BlueprintPure, Category="Tiger Shop")
	bool CanBuyClickUpgrade() const;

	UFUNCTION(BlueprintPure, Category="Tiger Shop")
	int64 GetRiceCakeCount() const { return RiceCakeCount; }

	UFUNCTION(BlueprintPure, Category="Tiger Shop")
	int32 GetClickPower() const { return ClickPower; }

	UFUNCTION(BlueprintPure, Category="Tiger Shop")
	int32 GetClickUpgradeLevel() const { return ClickUpgradeLevel; }

	UFUNCTION(BlueprintPure, Category="Tiger Shop")
	int64 GetNextClickUpgradeCost() const;

private:
	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	int64 RiceCakeCount = 0;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	int32 ClickPower = 1;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	int32 ClickUpgradeLevel = 0;

	UPROPERTY(EditDefaultsOnly, Category="Tiger Shop")
	int64 BaseClickUpgradeCost = 10;
};
