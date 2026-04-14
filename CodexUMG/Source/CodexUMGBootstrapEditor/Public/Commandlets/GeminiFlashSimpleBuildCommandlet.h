// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Commandlets/Commandlet.h"
#include "GeminiFlashSimpleBuildCommandlet.generated.h"

/**
 * Commandlet to build GeminiFlash simplified UMG assets.
 * Usage: -run=GeminiFlashSimpleBuild
 */
UCLASS()
class UGeminiFlashSimpleBuildCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UGeminiFlashSimpleBuildCommandlet();

	virtual int32 Main(const FString& Params) override;
};
