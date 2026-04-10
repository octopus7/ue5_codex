// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CodexFireInputConsumer.generated.h"

UINTERFACE(BlueprintType)
class CODEXUMG_API UCodexFireInputReceiver : public UInterface
{
	GENERATED_BODY()
};

class CODEXUMG_API ICodexFireInputReceiver
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Codex|Input")
	void ConsumeFireInput();
};
