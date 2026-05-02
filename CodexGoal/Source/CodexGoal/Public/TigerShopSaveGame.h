// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "TigerShopSaveGame.generated.h"

UCLASS()
class CODEXGOAL_API UTigerShopSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	int64 RiceCakeCount = 0;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	int32 ClickPower = 1;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	int32 ClickUpgradeLevel = 0;
};
