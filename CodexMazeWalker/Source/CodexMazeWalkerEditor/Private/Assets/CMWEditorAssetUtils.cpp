#include "Assets/CMWEditorAssetUtils.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/PackageName.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

namespace CMWEditorAssetUtils
{
	FString GetAssetNameFromPackagePath(const FString& PackagePath)
	{
		return FPackageName::GetLongPackageAssetName(PackagePath);
	}

	FString GetObjectPathFromPackagePath(const FString& PackagePath)
	{
		const FString AssetName = GetAssetNameFromPackagePath(PackagePath);
		return FString::Printf(TEXT("%s.%s"), *PackagePath, *AssetName);
	}

	UPackage* FindOrCreatePackage(const FString& PackagePath)
	{
		UPackage* Package = CreatePackage(*PackagePath);
		Package->FullyLoad();
		return Package;
	}

	bool SaveAsset(UObject* Asset, FString* OutError)
	{
		if (!Asset)
		{
			if (OutError)
			{
				*OutError = TEXT("Asset pointer was null.");
			}
			return false;
		}

		UPackage* Package = Asset->GetOutermost();
		const FString PackageName = Package->GetName();
		FString PackageFilename;
		if (!FPackageName::TryConvertLongPackageNameToFilename(PackageName, PackageFilename, FPackageName::GetAssetPackageExtension()))
		{
			if (OutError)
			{
				*OutError = FString::Printf(TEXT("Failed to convert package name '%s' to filename."), *PackageName);
			}
			return false;
		}

		Package->MarkPackageDirty();
		FAssetRegistryModule::AssetCreated(Asset);

		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.SaveFlags = SAVE_None;
		return UPackage::SavePackage(Package, Asset, *PackageFilename, SaveArgs);
	}
}
