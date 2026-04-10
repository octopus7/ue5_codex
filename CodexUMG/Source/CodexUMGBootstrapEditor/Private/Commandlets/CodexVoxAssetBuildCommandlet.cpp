#include "Commandlets/CodexVoxAssetBuildCommandlet.h"

#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Vox/CodexVoxAssetGenerator.h"

DEFINE_LOG_CATEGORY_STATIC(LogCodexVoxAssetBuildCommandlet, Log, All);

UCodexVoxAssetBuildCommandlet::UCodexVoxAssetBuildCommandlet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	IsClient = false;
	IsServer = false;
	IsEditor = true;
	LogToConsole = true;
	ShowErrorCount = true;
	UseCommandletResultAsExitCode = true;
}

int32 UCodexVoxAssetBuildCommandlet::Main(const FString& Params)
{
	FString ManifestPath;
	if (!FParse::Value(*Params, TEXT("Manifest="), ManifestPath))
	{
		UE_LOG(LogCodexVoxAssetBuildCommandlet, Error, TEXT("Missing required -Manifest=... argument."));
		return 1;
	}

	FVoxAssetBuildSettings Settings;
	Settings.ManifestPath = FPaths::ConvertRelativePathToFull(ManifestPath);
	Settings.bVerbose = FParse::Param(*Params, TEXT("Verbose"));
	Settings.bOverwriteExisting = !FParse::Param(*Params, TEXT("NoOverwrite"));

	FVoxAssetBuildResult Result;
	FString ErrorMessage;
	if (!FCodexVoxAssetGenerator::Run(Settings, Result, ErrorMessage))
	{
		UE_LOG(LogCodexVoxAssetBuildCommandlet, Error, TEXT("%s"), *ErrorMessage);
		return ErrorMessage.Contains(TEXT("editor session"), ESearchCase::IgnoreCase) ? 3 : 4;
	}

	UE_LOG(
		LogCodexVoxAssetBuildCommandlet,
		Display,
		TEXT("VOX asset build completed. Processed=%d Built=%d"),
		Result.ProcessedCount,
		Result.BuiltCount);

	return 0;
}
