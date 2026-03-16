// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CodexInvenPickupData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CodexInvenInventoryIconSubsystem.generated.h"

class UTexture2D;

UCLASS()
class CODEXINVEN_API UCodexInvenInventoryIconSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UTexture2D* GetInventoryIcon(ECodexInvenPickupType InPickupType) const;

private:
	UTexture2D* CreateInventoryIconTexture(const FCodexInvenPickupDefinition& InDefinition) const;
	static void BuildInventoryIconPixels(const FCodexInvenPickupDefinition& InDefinition, TArray64<uint8>& OutPixels);

	UPROPERTY(Transient)
	TMap<ECodexInvenPickupType, TObjectPtr<UTexture2D>> InventoryIcons;
};
