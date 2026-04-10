// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/CodexInteractionTypes.h"
#include "UObject/Interface.h"
#include "CodexInteractionTarget.generated.h"

UINTERFACE(BlueprintType)
class CODEXUMG_API UCodexInteractionTarget : public UInterface
{
	GENERATED_BODY()
};

class CODEXUMG_API ICodexInteractionTarget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Codex|Interaction")
	void HandleInteractionRequested(const FCodexInteractionRequest& Request);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Codex|Interaction")
	void HandleInteractionEnded(const FCodexInteractionRequest& Request);
};
