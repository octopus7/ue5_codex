#pragma once

#include "CoreMinimal.h"
#include "MeshDescription.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCodexVox, Log, All);

namespace CodexVox
{
	inline constexpr int32 SourceResolution = 32;
	inline constexpr float DefaultVoxelSize = 5.0f;

	inline const FString MaterialPackagePath(TEXT("/Game/Vox/Materials"));
	inline const FString MaterialAssetName(TEXT("M_VoxVertexColor"));
	inline const FName MaterialSlotName(TEXT("Vox"));

	inline FString MakeObjectPath(const FString& PackagePath, const FString& AssetName)
	{
		return FString::Printf(TEXT("%s/%s.%s"), *PackagePath, *AssetName, *AssetName);
	}

	inline FVector TransformVectorToUnreal(const FVector& VoxVector)
	{
		return FVector(VoxVector.X, -VoxVector.Z, VoxVector.Y);
	}

	enum class EPivotRule : uint8
	{
		GroundCentered,
		Centered
	};

	enum class ECollisionType : uint8
	{
		None,
		SimpleBox,
		UseComplexAsSimple,
		SimpleAndComplex
	};

	struct FVoxel
	{
		FIntVector Position = FIntVector::ZeroValue;
		uint8 ColorIndex = 0;
	};

	struct FParsedModel
	{
		FIntVector Size = FIntVector::ZeroValue;
		TArray<FVoxel> Voxels;
		TArray<FColor> Palette;
		bool bHasPalette = false;
	};

	struct FManifestEntry
	{
		FString Id;
		FString DisplayName;
		FString SourceVoxFile;
		FString TargetPackagePath;
		FString TargetAssetName;
		FString Category;
		bool bRepeatable = false;
		EPivotRule PivotRule = EPivotRule::GroundCentered;
		ECollisionType CollisionType = ECollisionType::UseComplexAsSimple;
		FString Notes;
	};

	struct FBuildOptions
	{
		FString ManifestPath;
		bool bVerbose = false;
		bool bNoOverwrite = false;
		float VoxelSize = DefaultVoxelSize;
	};

	struct FMeshBuildInput
	{
		const FParsedModel* Model = nullptr;
		EPivotRule PivotRule = EPivotRule::GroundCentered;
		float VoxelSize = DefaultVoxelSize;
	};

	struct FMeshBuildOutput
	{
		FMeshDescription MeshDescription;
		FBox LocalBounds = FBox(EForceInit::ForceInit);
		int32 ExposedFaceCount = 0;
	};

	inline bool TryParsePivotRule(const FString& Value, EPivotRule& OutPivotRule)
	{
		if (Value.Equals(TEXT("GroundCentered"), ESearchCase::IgnoreCase))
		{
			OutPivotRule = EPivotRule::GroundCentered;
			return true;
		}

		if (Value.Equals(TEXT("Centered"), ESearchCase::IgnoreCase))
		{
			OutPivotRule = EPivotRule::Centered;
			return true;
		}

		return false;
	}

	inline bool TryParseCollisionType(const FString& Value, ECollisionType& OutCollisionType)
	{
		if (Value.Equals(TEXT("None"), ESearchCase::IgnoreCase))
		{
			OutCollisionType = ECollisionType::None;
			return true;
		}

		if (Value.Equals(TEXT("SimpleBox"), ESearchCase::IgnoreCase))
		{
			OutCollisionType = ECollisionType::SimpleBox;
			return true;
		}

		if (Value.Equals(TEXT("UseComplexAsSimple"), ESearchCase::IgnoreCase))
		{
			OutCollisionType = ECollisionType::UseComplexAsSimple;
			return true;
		}

		if (Value.Equals(TEXT("SimpleAndComplex"), ESearchCase::IgnoreCase))
		{
			OutCollisionType = ECollisionType::SimpleAndComplex;
			return true;
		}

		return false;
	}
}
