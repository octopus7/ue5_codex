#pragma once

#include "CoreMinimal.h"

namespace OctoDenAssetNaming
{
	FString SanitizeAssetName(const FString& RawName, const FString& FallbackStem);
	FString NormalizePackageFolder(const FString& RawFolder, const FString& FallbackFolder);
	FString BuildPackagePath(const FString& FolderPath, const FString& AssetName);
	FString BuildObjectPath(const FString& FolderPath, const FString& AssetName);
	FString ToPascalIdentifier(const FString& RawName, const FString& FallbackStem, const TCHAR* RequiredPrefix = nullptr);
}
