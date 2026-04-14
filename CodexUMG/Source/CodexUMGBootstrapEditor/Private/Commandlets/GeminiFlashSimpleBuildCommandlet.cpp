// Copyright Epic Games, Inc. All Rights Reserved.

#include "Commandlets/GeminiFlashSimpleBuildCommandlet.h"
#include "Simple/GeminiFlashAssetBuilder.h"

UGeminiFlashSimpleBuildCommandlet::UGeminiFlashSimpleBuildCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
	ShowErrorCount = true;
}

int32 UGeminiFlashSimpleBuildCommandlet::Main(const FString& Params)
{
	(void)Params;

	FString ErrorMessage;
	if (!FGeminiFlashAssetBuilder::RunBuild(ErrorMessage))
	{
		UE_LOG(LogTemp, Error, TEXT("GeminiFlash Asset Build Failed: %s"), *ErrorMessage);
		return 1;
	}

	UE_LOG(LogTemp, Display, TEXT("GeminiFlash Asset Build Completed Successfully."));
	return 0;
}
