// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CodexMoveInputConsumer.generated.h"

UINTERFACE(BlueprintType)
class CODEXUMG_API UCodexMoveInputReceiver : public UInterface
{
	GENERATED_BODY()
};

class CODEXUMG_API ICodexMoveInputReceiver
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Codex|Input")
	void ConsumeMoveInput(FVector2D MoveAxis);
};
