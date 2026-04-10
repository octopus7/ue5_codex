// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CodexGameInstance.generated.h"

class UCodexTopDownInputConfigDataAsset;
class UCodexProjectileConfigDataAsset;

UCLASS(Blueprintable)
class CODEXUMG_API UCodexGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Codex|Input")
	const UCodexTopDownInputConfigDataAsset* GetTopDownInputConfig() const;

	UFUNCTION(BlueprintCallable, Category = "Codex|Input")
	void SetTopDownInputConfig(UCodexTopDownInputConfigDataAsset* NewConfig);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Codex|Projectile")
	const UCodexProjectileConfigDataAsset* GetPlayerProjectileConfig() const;

	UFUNCTION(BlueprintCallable, Category = "Codex|Projectile")
	void SetPlayerProjectileConfig(UCodexProjectileConfigDataAsset* NewConfig);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Codex|Input")
	TObjectPtr<UCodexTopDownInputConfigDataAsset> TopDownInputConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Codex|Projectile")
	TObjectPtr<UCodexProjectileConfigDataAsset> PlayerProjectileConfig;
};
