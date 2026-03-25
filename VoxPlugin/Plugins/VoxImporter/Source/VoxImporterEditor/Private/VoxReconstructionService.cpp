#include "VoxReconstructionService.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Engine/StaticMesh.h"
#include "MeshDescription.h"
#include "Misc/FeedbackContext.h"
#include "Misc/Paths.h"
#include "UObject/Package.h"
#include "VoxParser.h"
#include "VoxSmoothMeshBuilder.h"
#include "VoxStaticMeshUtilities.h"

namespace VoxReconstructionService
{
	bool GenerateSmoothReconstruction(UStaticMesh* SourceMesh, UObject*& OutCreatedAsset, FString& OutError)
	{
		OutCreatedAsset = nullptr;

		if (!VoxStaticMeshUtilities::IsPrimaryVoxSourceStaticMesh(SourceMesh))
		{
			OutError = TEXT("Selected Static Mesh is not a primary .vox import asset.");
			return false;
		}

		const FString SourceFilename = VoxStaticMeshUtilities::GetVoxSourceFilename(SourceMesh);
		if (SourceFilename.IsEmpty() || !FPaths::FileExists(SourceFilename))
		{
			OutError = TEXT("The original .vox source file could not be found for this asset.");
			return false;
		}

		FVoxModelData Model;
		if (!FVoxParser::ParseFile(SourceFilename, Model, OutError))
		{
			return false;
		}

		FMeshDescription MeshDescription;
		FVoxSmoothBuildMetadata BuildMetadata;
		if (!FVoxSmoothMeshBuilder::BuildSmoothMeshDescription(Model, MeshDescription, OutError, 1.0f, FVoxSmoothBuildSettings(), &BuildMetadata))
		{
			return false;
		}

		FMeshDescription FinalMeshDescription;
		if (!VoxStaticMeshUtilities::SimplifyReconstructedMeshDescription(MeshDescription, Model, 1.0f, 0.03f, FinalMeshDescription, OutError))
		{
			return false;
		}

		FString UniquePackageName;
		FString UniqueAssetName;
		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
		AssetToolsModule.Get().CreateUniqueAssetName(SourceMesh->GetOutermost()->GetName(), TEXT("_Smooth"), UniquePackageName, UniqueAssetName);

		UPackage* Package = CreatePackage(*UniquePackageName);
		if (!Package)
		{
			OutError = TEXT("Failed to create a package for the reconstructed mesh asset.");
			return false;
		}

		UStaticMesh* ReconstructedMesh = NewObject<UStaticMesh>(Package, *UniqueAssetName, RF_Public | RF_Standalone | RF_Transactional);
		if (!ReconstructedMesh)
		{
			OutError = TEXT("Failed to allocate the reconstructed Static Mesh asset.");
			return false;
		}

		const FVoxMeshAssetBuildParams BuildParams
		{
			SourceFilename,
			true,
			BuildMetadata.ResolutionScaleUsed,
			SourceMesh->GetPathName()
		};

		if (!VoxStaticMeshUtilities::BuildStaticMeshAsset(ReconstructedMesh, FinalMeshDescription, BuildParams, GWarn))
		{
			OutError = TEXT("Failed to build the reconstructed Static Mesh asset.");
			return false;
		}

		FAssetRegistryModule::AssetCreated(ReconstructedMesh);
		Package->MarkPackageDirty();
		OutCreatedAsset = ReconstructedMesh;
		return true;
	}
}
