#include "Commandlets/CodexInteractionAssetBuildCommandlet.h"

#include "Interaction/CodexInteractionAssetBuilder.h"

UCodexInteractionAssetBuildCommandlet::UCodexInteractionAssetBuildCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
	ShowErrorCount = true;
}

int32 UCodexInteractionAssetBuildCommandlet::Main(const FString& Params)
{
	(void)Params;

	FString ErrorMessage;
	if (!FCodexInteractionAssetBuilder::RunBuild(ErrorMessage))
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	UE_LOG(LogTemp, Display, TEXT("Interaction asset build completed successfully."));
	return 0;
}
