// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CodexInvenGameInstance.generated.h"

class UCodexInvenInputConfigDataAsset;
class UInputAction;
class UInputMappingContext;

UENUM(BlueprintType)
enum class ECodexInvenConfiguredInputAction : uint8
{
	Move,
	Look,
	Jump,
	Fire
};

UCLASS()
class CODEXINVEN_API UCodexInvenGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UCodexInvenInputConfigDataAsset> DefaultInputConfig = nullptr;

	UFUNCTION(BlueprintPure, Category = "Input")
	const UCodexInvenInputConfigDataAsset* GetInputConfig() const;

	UFUNCTION(BlueprintPure, Category = "Input")
	const UInputMappingContext* GetInputMappingContext() const;

	UFUNCTION(BlueprintPure, Category = "Input")
	const UInputAction* GetInputAction(ECodexInvenConfiguredInputAction InAction) const;
};
