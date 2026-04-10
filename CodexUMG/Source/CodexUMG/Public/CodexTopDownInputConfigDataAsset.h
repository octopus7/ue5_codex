// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CodexTopDownInputConfigDataAsset.generated.h"

class UInputAction;
class UInputMappingContext;

UCLASS(BlueprintType)
class CODEXUMG_API UCodexTopDownInputConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Codex|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Codex|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Codex|Input", meta = (ClampMin = "0"))
	int32 MappingPriority = 0;

	const UInputMappingContext* GetDefaultMappingContext() const;
	const UInputAction* GetMoveAction() const;
	int32 GetMappingPriority() const;
};
