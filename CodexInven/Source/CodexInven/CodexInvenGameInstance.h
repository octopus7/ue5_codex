// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CodexInvenGameInstance.generated.h"

class ACodexInvenProjectile;
class UCodexInvenAttendanceConfigDataAsset;
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
	UCodexInvenGameInstance();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UCodexInvenInputConfigDataAsset> DefaultInputConfig = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<ACodexInvenProjectile> DefaultProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attendance")
	TObjectPtr<UCodexInvenAttendanceConfigDataAsset> AttendanceConfig = nullptr;

	UFUNCTION(BlueprintPure, Category = "Input")
	const UCodexInvenInputConfigDataAsset* GetInputConfig() const;

	UFUNCTION(BlueprintPure, Category = "Input")
	const UInputMappingContext* GetInputMappingContext() const;

	UFUNCTION(BlueprintPure, Category = "Input")
	const UInputAction* GetInputAction(ECodexInvenConfiguredInputAction InAction) const;

	UFUNCTION(BlueprintPure, Category = "Combat")
	TSubclassOf<ACodexInvenProjectile> GetProjectileClass() const;

	UFUNCTION(BlueprintPure, Category = "Attendance")
	const UCodexInvenAttendanceConfigDataAsset* GetAttendanceConfig() const;

private:
	UPROPERTY(Transient)
	mutable TObjectPtr<UCodexInvenAttendanceConfigDataAsset> RuntimeResolvedAttendanceConfig = nullptr;
};
