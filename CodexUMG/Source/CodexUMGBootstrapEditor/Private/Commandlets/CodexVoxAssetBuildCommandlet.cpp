#include "Commandlets/CodexVoxAssetBuildCommandlet.h"

#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Vox/CodexVoxAssetGenerator.h"
#include "Vox/CodexVoxTypes.h"

UCodexVoxAssetBuildCommandlet::UCodexVoxAssetBuildCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
	ShowErrorCount = true;
}

int32 UCodexVoxAssetBuildCommandlet::Main(const FString& Params)
{
	CodexVox::FBuildOptions BuildOptions;
	BuildOptions.ManifestPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("SourceArt/Vox/VoxAssetManifest.json"));
	BuildOptions.bVerbose = FParse::Param(*Params, TEXT("Verbose"));
	BuildOptions.bNoOverwrite = FParse::Param(*Params, TEXT("NoOverwrite"));
	BuildOptions.bSkipMaterialUpdate = FParse::Param(*Params, TEXT("SkipMaterialUpdate"));

	FParse::Value(*Params, TEXT("Manifest="), BuildOptions.ManifestPath);
	BuildOptions.ManifestPath = FPaths::ConvertRelativePathToFull(BuildOptions.ManifestPath);

	FString ErrorMessage;
	if (!FCodexVoxAssetGenerator::RunBuild(BuildOptions, ErrorMessage))
	{
		UE_LOG(LogCodexVox, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	UE_LOG(LogCodexVox, Display, TEXT("VOX asset build completed successfully."));
	return 0;
}
