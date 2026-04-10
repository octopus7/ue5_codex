// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CodexGameInstance.generated.h"

class UCodexTopDownInputConfigDataAsset;

UCLASS(Blueprintable)
class CODEXUMG_API UCodexGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Codex|Input")
	const UCodexTopDownInputConfigDataAsset* GetTopDownInputConfig() const;

	UFUNCTION(BlueprintCallable, Category = "Codex|Input")
	void SetTopDownInputConfig(UCodexTopDownInputConfigDataAsset* NewConfig);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Codex|Input")
	TObjectPtr<UCodexTopDownInputConfigDataAsset> TopDownInputConfig;
};
