#pragma once

#include "CoreMinimal.h"

class UObject;
class UPackage;

namespace CMWEditorAssetUtils
{
	FString GetAssetNameFromPackagePath(const FString& PackagePath);
	FString GetObjectPathFromPackagePath(const FString& PackagePath);
	UPackage* FindOrCreatePackage(const FString& PackagePath);
	bool SaveAsset(UObject* Asset, FString* OutError = nullptr);

	template <typename AssetType>
	AssetType* LoadAsset(const FString& PackagePath)
	{
		return LoadObject<AssetType>(nullptr, *GetObjectPathFromPackagePath(PackagePath));
	}

	template <typename AssetType, typename CreateFuncType>
	AssetType* LoadOrCreateAsset(const FString& PackagePath, CreateFuncType&& CreateFunc)
	{
		if (AssetType* ExistingAsset = LoadAsset<AssetType>(PackagePath))
		{
			return ExistingAsset;
		}

		UPackage* Package = FindOrCreatePackage(PackagePath);
		const FString AssetName = GetAssetNameFromPackagePath(PackagePath);
		return CreateFunc(Package, *AssetName);
	}
}
