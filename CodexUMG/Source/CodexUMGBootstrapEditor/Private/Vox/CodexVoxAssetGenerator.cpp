#include "Vox/CodexVoxAssetGenerator.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Dom/JsonObject.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Factories/MaterialFactoryNew.h"
#include "HAL/FileManager.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionVertexColor.h"
#include "MeshDescription.h"
#include "MeshDescriptionBuilder.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "PhysicsEngine/BodySetup.h"
#include "Serialization/JsonSerializer.h"
#include "StaticMeshAttributes.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

DEFINE_LOG_CATEGORY_STATIC(LogCodexVoxAssetGenerator, Log, All);

namespace CodexVoxAssetGenerator
{
	const FString SharedMaterialPackagePath = TEXT("/Game/Vox/Materials");
	const FString SharedMaterialAssetName = TEXT("M_VoxVertexColor");
	const FName SharedMaterialSlotName(TEXT("Vox"));
	constexpr float VoxelUnitSize = 5.0f;

	template <typename AssetType>
	AssetType* LoadAsset(const FString& PackagePath, const FString& AssetName)
	{
		const FString ObjectPath = FString::Printf(TEXT("%s/%s.%s"), *PackagePath, *AssetName, *AssetName);
		return LoadObject<AssetType>(nullptr, *ObjectPath);
	}

	FString MakeObjectPath(const FString& PackagePath, const FString& AssetName)
	{
		return FString::Printf(TEXT("%s/%s.%s"), *PackagePath, *AssetName, *AssetName);
	}

	bool SaveAssetPackage(UObject& Asset, FString& OutError)
	{
		UPackage* Package = Asset.GetOutermost();
		if (Package == nullptr)
		{
			OutError = FString::Printf(TEXT("Failed to save asset '%s': missing outer package."), *Asset.GetName());
			return false;
		}

		const FString PackageName = Package->GetName();
		const FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
		IFileManager::Get().MakeDirectory(*FPaths::GetPath(PackageFileName), true);

		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.Error = GWarn;
		SaveArgs.SaveFlags = SAVE_NoError;
		if (!UPackage::SavePackage(Package, &Asset, *PackageFileName, SaveArgs))
		{
			OutError = FString::Printf(TEXT("Failed to save package '%s'. This can happen if an editor session is still holding the asset or the file is locked."), *PackageName);
			return false;
		}

		return true;
	}

	bool EnsureManifestEntryValue(const TSharedPtr<FJsonObject>& JsonObject, const TCHAR* FieldName, FString& OutValue, FString& OutError)
	{
		if (!JsonObject->TryGetStringField(FieldName, OutValue) || OutValue.IsEmpty())
		{
			OutError = FString::Printf(TEXT("Manifest entry is missing required field '%s'."), FieldName);
			return false;
		}

		return true;
	}

	bool LoadManifest(const FString& ManifestPath, TArray<FVoxAssetManifestEntry>& OutEntries, FString& OutError)
	{
		FString ManifestText;
		if (!FFileHelper::LoadFileToString(ManifestText, *ManifestPath))
		{
			OutError = FString::Printf(TEXT("Failed to read manifest file '%s'."), *ManifestPath);
			return false;
		}

		TSharedPtr<FJsonObject> RootObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ManifestText);
		if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
		{
			OutError = FString::Printf(TEXT("Failed to parse manifest JSON '%s'."), *ManifestPath);
			return false;
		}

		const TArray<TSharedPtr<FJsonValue>>* AssetArray = nullptr;
		if (!RootObject->TryGetArrayField(TEXT("assets"), AssetArray) || AssetArray == nullptr || AssetArray->Num() == 0)
		{
			OutError = TEXT("Manifest must contain a non-empty 'assets' array.");
			return false;
		}

		for (const TSharedPtr<FJsonValue>& Value : *AssetArray)
		{
			const TSharedPtr<FJsonObject> EntryObject = Value.IsValid() ? Value->AsObject() : nullptr;
			if (!EntryObject.IsValid())
			{
				OutError = TEXT("Manifest contains an invalid asset entry.");
				return false;
			}

			FVoxAssetManifestEntry Entry;
			if (!EnsureManifestEntryValue(EntryObject, TEXT("id"), Entry.Id, OutError) ||
				!EnsureManifestEntryValue(EntryObject, TEXT("displayName"), Entry.DisplayName, OutError) ||
				!EnsureManifestEntryValue(EntryObject, TEXT("sourceVoxFile"), Entry.SourceVoxFile, OutError) ||
				!EnsureManifestEntryValue(EntryObject, TEXT("targetPackagePath"), Entry.TargetPackagePath, OutError) ||
				!EnsureManifestEntryValue(EntryObject, TEXT("targetAssetName"), Entry.TargetAssetName, OutError))
			{
				return false;
			}

			EntryObject->TryGetStringField(TEXT("category"), Entry.Category);
			EntryObject->TryGetBoolField(TEXT("repeatable"), Entry.bRepeatable);
			EntryObject->TryGetStringField(TEXT("pivotRule"), Entry.PivotRule);
			EntryObject->TryGetStringField(TEXT("collisionType"), Entry.CollisionType);
			EntryObject->TryGetStringField(TEXT("notes"), Entry.Notes);

			if (!Entry.TargetPackagePath.StartsWith(TEXT("/Game")))
			{
				OutError = FString::Printf(TEXT("Manifest entry '%s' has unsupported targetPackagePath '%s'. Expected a /Game path."), *Entry.Id, *Entry.TargetPackagePath);
				return false;
			}

			OutEntries.Add(MoveTemp(Entry));
		}

		return true;
	}

	bool ReadExact(const TArray<uint8>& Data, int32& Offset, void* Destination, int32 BytesToRead, FString& OutError)
	{
		if (Offset < 0 || BytesToRead < 0 || Offset + BytesToRead > Data.Num())
		{
			OutError = TEXT("Unexpected end of VOX file.");
			return false;
		}

		FMemory::Memcpy(Destination, Data.GetData() + Offset, BytesToRead);
		Offset += BytesToRead;
		return true;
	}

	bool ReadInt32(const TArray<uint8>& Data, int32& Offset, int32& OutValue, FString& OutError)
	{
		return ReadExact(Data, Offset, &OutValue, sizeof(int32), OutError);
	}

	bool ReadChunkId(const TArray<uint8>& Data, int32& Offset, FString& OutId, FString& OutError)
	{
		ANSICHAR Buffer[5] = {};
		if (!ReadExact(Data, Offset, Buffer, 4, OutError))
		{
			return false;
		}

		OutId = ANSI_TO_TCHAR(Buffer);
		return true;
	}

	bool LoadVoxFile(const FString& VoxFilePath, FVoxModel& OutModel, FString& OutError)
	{
		TArray<uint8> FileData;
		if (!FFileHelper::LoadFileToArray(FileData, *VoxFilePath))
		{
			OutError = FString::Printf(TEXT("Failed to load VOX file '%s'."), *VoxFilePath);
			return false;
		}

		int32 Offset = 0;
		FString ChunkId;
		if (!ReadChunkId(FileData, Offset, ChunkId, OutError) || ChunkId != TEXT("VOX "))
		{
			OutError = FString::Printf(TEXT("File '%s' is not a valid VOX file."), *VoxFilePath);
			return false;
		}

		int32 Version = 0;
		if (!ReadInt32(FileData, Offset, Version, OutError))
		{
			return false;
		}

		if (!ReadChunkId(FileData, Offset, ChunkId, OutError) || ChunkId != TEXT("MAIN"))
		{
			OutError = FString::Printf(TEXT("File '%s' is missing MAIN chunk."), *VoxFilePath);
			return false;
		}

		int32 MainContentSize = 0;
		int32 MainChildrenSize = 0;
		if (!ReadInt32(FileData, Offset, MainContentSize, OutError) ||
			!ReadInt32(FileData, Offset, MainChildrenSize, OutError))
		{
			return false;
		}

		Offset += MainContentSize;
		if (Offset > FileData.Num())
		{
			OutError = FString::Printf(TEXT("File '%s' has a truncated MAIN chunk."), *VoxFilePath);
			return false;
		}

		OutModel.Palette.Init(FColor::White, 256);

		bool bFoundSize = false;
		bool bFoundVoxelData = false;
		bool bFoundPalette = false;

		while (Offset < FileData.Num())
		{
			int32 ChunkStartOffset = Offset;
			if (!ReadChunkId(FileData, Offset, ChunkId, OutError))
			{
				return false;
			}

			int32 ContentSize = 0;
			int32 ChildrenSize = 0;
			if (!ReadInt32(FileData, Offset, ContentSize, OutError) ||
				!ReadInt32(FileData, Offset, ChildrenSize, OutError))
			{
				return false;
			}

			const int32 ContentOffset = Offset;
			const int32 ContentEndOffset = ContentOffset + ContentSize;
			if (ContentEndOffset > FileData.Num())
			{
				OutError = FString::Printf(TEXT("File '%s' has a truncated %s chunk."), *VoxFilePath, *ChunkId);
				return false;
			}

			if (ChunkId == TEXT("SIZE"))
			{
				if (!ReadInt32(FileData, Offset, OutModel.SizeX, OutError) ||
					!ReadInt32(FileData, Offset, OutModel.SizeY, OutError) ||
					!ReadInt32(FileData, Offset, OutModel.SizeZ, OutError))
				{
					return false;
				}

				bFoundSize = true;
			}
			else if (ChunkId == TEXT("XYZI"))
			{
				int32 VoxelCount = 0;
				if (!ReadInt32(FileData, Offset, VoxelCount, OutError))
				{
					return false;
				}

				OutModel.Voxels.Reserve(VoxelCount);
				for (int32 Index = 0; Index < VoxelCount; ++Index)
				{
					FVoxVoxel Voxel;
					if (!ReadExact(FileData, Offset, &Voxel, sizeof(FVoxVoxel), OutError))
					{
						return false;
					}

					if (Voxel.ColorIndex == 0)
					{
						OutError = FString::Printf(TEXT("File '%s' contains a voxel with palette index 0, which is unsupported in this pipeline."), *VoxFilePath);
						return false;
					}

					OutModel.Voxels.Add(Voxel);
				}

				bFoundVoxelData = true;
			}
			else if (ChunkId == TEXT("RGBA"))
			{
				OutModel.Palette.Reset(256);
				for (int32 ColorIndex = 0; ColorIndex < 256; ++ColorIndex)
				{
					uint8 RGBA[4] = {};
					if (!ReadExact(FileData, Offset, RGBA, 4, OutError))
					{
						return false;
					}

					OutModel.Palette.Add(FColor(RGBA[0], RGBA[1], RGBA[2], RGBA[3]));
				}

				bFoundPalette = true;
			}

			Offset = ContentEndOffset + ChildrenSize;
			if (Offset > FileData.Num())
			{
				OutError = FString::Printf(TEXT("File '%s' has an invalid child chunk size near offset %d."), *VoxFilePath, ChunkStartOffset);
				return false;
			}
		}

		if (!bFoundSize || !bFoundVoxelData)
		{
			OutError = FString::Printf(TEXT("File '%s' is missing required SIZE or XYZI chunks."), *VoxFilePath);
			return false;
		}

		if (!bFoundPalette)
		{
			OutError = FString::Printf(TEXT("File '%s' is missing RGBA chunk. This pipeline currently expects explicit palettes."), *VoxFilePath);
			return false;
		}

		return true;
	}

	FVector ResolvePivotInVoxelUnits(const FVoxModel& Model, const FString& PivotRule)
	{
		int32 MinX = Model.SizeX;
		int32 MinY = Model.SizeY;
		int32 MinZ = Model.SizeZ;
		int32 MaxX = 0;
		int32 MaxY = 0;
		int32 MaxZ = 0;

		for (const FVoxVoxel& Voxel : Model.Voxels)
		{
			MinX = FMath::Min(MinX, static_cast<int32>(Voxel.X));
			MinY = FMath::Min(MinY, static_cast<int32>(Voxel.Y));
			MinZ = FMath::Min(MinZ, static_cast<int32>(Voxel.Z));
			MaxX = FMath::Max(MaxX, static_cast<int32>(Voxel.X));
			MaxY = FMath::Max(MaxY, static_cast<int32>(Voxel.Y));
			MaxZ = FMath::Max(MaxZ, static_cast<int32>(Voxel.Z));
		}

		const FVector BoundsMin(MinX, MinY, MinZ);
		const FVector BoundsMax(MaxX + 1.0f, MaxY + 1.0f, MaxZ + 1.0f);
		if (PivotRule.Equals(TEXT("Center"), ESearchCase::IgnoreCase))
		{
			return (BoundsMin + BoundsMax) * 0.5f;
		}

		return FVector(
			(BoundsMin.X + BoundsMax.X) * 0.5f,
			(BoundsMin.Y + BoundsMax.Y) * 0.5f,
			BoundsMin.Z);
	}

	struct FVoxFaceDefinition
	{
		FIntVector NeighborOffset;
		FVector Normal;
		FVector Tangent;
		FVector Corners[4];
	};

	const FVoxFaceDefinition& GetFaceDefinition(int32 FaceIndex)
	{
		static const FVoxFaceDefinition Definitions[6] =
		{
			{ FIntVector(1, 0, 0), FVector(1, 0, 0), FVector(0, 1, 0), { FVector(1, 0, 0), FVector(1, 1, 0), FVector(1, 1, 1), FVector(1, 0, 1) } },
			{ FIntVector(-1, 0, 0), FVector(-1, 0, 0), FVector(0, 1, 0), { FVector(0, 0, 0), FVector(0, 0, 1), FVector(0, 1, 1), FVector(0, 1, 0) } },
			{ FIntVector(0, 1, 0), FVector(0, 1, 0), FVector(1, 0, 0), { FVector(0, 1, 0), FVector(0, 1, 1), FVector(1, 1, 1), FVector(1, 1, 0) } },
			{ FIntVector(0, -1, 0), FVector(0, -1, 0), FVector(1, 0, 0), { FVector(0, 0, 0), FVector(1, 0, 0), FVector(1, 0, 1), FVector(0, 0, 1) } },
			{ FIntVector(0, 0, 1), FVector(0, 0, 1), FVector(1, 0, 0), { FVector(0, 0, 1), FVector(1, 0, 1), FVector(1, 1, 1), FVector(0, 1, 1) } },
			{ FIntVector(0, 0, -1), FVector(0, 0, -1), FVector(1, 0, 0), { FVector(0, 0, 0), FVector(0, 1, 0), FVector(1, 1, 0), FVector(1, 0, 0) } }
		};

		return Definitions[FaceIndex];
	}

	FString MakeVoxelKey(int32 X, int32 Y, int32 Z)
	{
		return FString::Printf(TEXT("%d_%d_%d"), X, Y, Z);
	}

	FVector ToLocalPosition(const FVector& VoxelCornerPosition, const FVector& PivotInVoxelUnits)
	{
		return (VoxelCornerPosition - PivotInVoxelUnits) * VoxelUnitSize;
	}

	bool BuildMeshDescription(const FVoxModel& Model, const FString& PivotRule, FMeshDescription& OutMeshDescription, FString& OutError)
	{
		FStaticMeshAttributes Attributes(OutMeshDescription);
		Attributes.Register();

		FMeshDescriptionBuilder Builder;
		Builder.SetMeshDescription(&OutMeshDescription);
		Builder.SetNumUVLayers(1);
		Builder.EnablePolyGroups();

		const FPolygonGroupID PolygonGroup = Builder.AppendPolygonGroup(SharedMaterialSlotName);
		const FVector PivotInVoxelUnits = ResolvePivotInVoxelUnits(Model, PivotRule);

		TSet<FString> OccupiedVoxels;
		for (const FVoxVoxel& Voxel : Model.Voxels)
		{
			OccupiedVoxels.Add(MakeVoxelKey(Voxel.X, Voxel.Y, Voxel.Z));
		}

		const FVector2D UVs[4] =
		{
			FVector2D(0.0f, 0.0f),
			FVector2D(1.0f, 0.0f),
			FVector2D(1.0f, 1.0f),
			FVector2D(0.0f, 1.0f)
		};

		for (const FVoxVoxel& Voxel : Model.Voxels)
		{
			if (!Model.Palette.IsValidIndex(Voxel.ColorIndex))
			{
				OutError = TEXT("VOX palette index is out of range.");
				return false;
			}

			const FColor Color = Model.Palette[Voxel.ColorIndex];
			const FVector4f VertexColor(
				Color.R / 255.0f,
				Color.G / 255.0f,
				Color.B / 255.0f,
				Color.A / 255.0f);

			for (int32 FaceIndex = 0; FaceIndex < 6; ++FaceIndex)
			{
				const FVoxFaceDefinition& Face = GetFaceDefinition(FaceIndex);
				const FIntVector Neighbor(
					static_cast<int32>(Voxel.X) + Face.NeighborOffset.X,
					static_cast<int32>(Voxel.Y) + Face.NeighborOffset.Y,
					static_cast<int32>(Voxel.Z) + Face.NeighborOffset.Z);

				if (OccupiedVoxels.Contains(MakeVoxelKey(Neighbor.X, Neighbor.Y, Neighbor.Z)))
				{
					continue;
				}

				const FVector BasePosition(Voxel.X, Voxel.Y, Voxel.Z);
				const int32 Triangles[2][3] =
				{
					{ 0, 1, 2 },
					{ 0, 2, 3 }
				};

				for (int32 TriangleIndex = 0; TriangleIndex < 2; ++TriangleIndex)
				{
					FVertexInstanceID TriangleInstances[3];
					for (int32 CornerIndex = 0; CornerIndex < 3; ++CornerIndex)
					{
						const int32 FaceCornerIndex = Triangles[TriangleIndex][CornerIndex];
						const FVector Position = ToLocalPosition(BasePosition + Face.Corners[FaceCornerIndex], PivotInVoxelUnits);

						const FVertexID VertexId = Builder.AppendVertex(Position);
						const FVertexInstanceID InstanceId = Builder.AppendInstance(VertexId);
						Builder.SetInstanceTangentSpace(InstanceId, Face.Normal, Face.Tangent, 1.0f);
						Builder.SetInstanceColor(InstanceId, VertexColor);
						Builder.SetInstanceUV(InstanceId, UVs[FaceCornerIndex], 0);
						TriangleInstances[CornerIndex] = InstanceId;
					}

					Builder.AppendTriangle(TriangleInstances[0], TriangleInstances[1], TriangleInstances[2], PolygonGroup);
				}
			}
		}

		if (OutMeshDescription.Polygons().Num() == 0)
		{
			OutError = TEXT("VOX model did not generate any visible faces.");
			return false;
		}

		return true;
	}

	UMaterial* EnsureSharedMaterial(FString& OutError)
	{
		if (UMaterial* ExistingMaterial = LoadAsset<UMaterial>(SharedMaterialPackagePath, SharedMaterialAssetName))
		{
			return ExistingMaterial;
		}

		UMaterialFactoryNew* MaterialFactory = NewObject<UMaterialFactoryNew>();
		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
		UMaterial* Material = Cast<UMaterial>(
			AssetToolsModule.Get().CreateAsset(
				SharedMaterialAssetName,
				SharedMaterialPackagePath,
				UMaterial::StaticClass(),
				MaterialFactory));

		if (Material == nullptr)
		{
			OutError = TEXT("Failed to create shared material M_VoxVertexColor.");
			return nullptr;
		}

		Material->BlendMode = BLEND_Opaque;
		Material->TwoSided = false;

		UMaterialEditorOnlyData* EditorOnly = Material->GetEditorOnlyData();
		if (EditorOnly == nullptr)
		{
			OutError = TEXT("Failed to access editor-only material data.");
			return nullptr;
		}

		UMaterialExpressionVertexColor* VertexColorExpression = NewObject<UMaterialExpressionVertexColor>(Material);
		VertexColorExpression->MaterialExpressionEditorX = -320;
		VertexColorExpression->MaterialExpressionEditorY = -40;
		Material->GetExpressionCollection().AddExpression(VertexColorExpression);
		EditorOnly->BaseColor.Expression = VertexColorExpression;

		UMaterialExpressionConstant* RoughnessExpression = NewObject<UMaterialExpressionConstant>(Material);
		RoughnessExpression->R = 1.0f;
		RoughnessExpression->MaterialExpressionEditorX = -320;
		RoughnessExpression->MaterialExpressionEditorY = 160;
		Material->GetExpressionCollection().AddExpression(RoughnessExpression);
		EditorOnly->Roughness.Expression = RoughnessExpression;

		Material->PreEditChange(nullptr);
		Material->PostEditChange();
		Material->ForceRecompileForRendering();
		Material->MarkPackageDirty();

		FAssetRegistryModule::AssetCreated(Material);
		if (!SaveAssetPackage(*Material, OutError))
		{
			return nullptr;
		}

		return Material;
	}

	bool BuildStaticMeshAsset(
		const FVoxAssetManifestEntry& Entry,
		const FVoxModel& Model,
		UMaterialInterface& SharedMaterial,
		const bool bOverwriteExisting,
		FString& OutGeneratedAssetPath,
		FString& OutError)
	{
		FMeshDescription MeshDescription;
		if (!BuildMeshDescription(Model, Entry.PivotRule, MeshDescription, OutError))
		{
			return false;
		}

		const FString ObjectPath = MakeObjectPath(Entry.TargetPackagePath, Entry.TargetAssetName);
		UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, *ObjectPath);
		const bool bCreatedNewAsset = (StaticMesh == nullptr);

		if (StaticMesh != nullptr && !bOverwriteExisting)
		{
			OutGeneratedAssetPath = ObjectPath;
			return true;
		}

		if (StaticMesh == nullptr)
		{
			UPackage* Package = CreatePackage(*FString::Printf(TEXT("%s/%s"), *Entry.TargetPackagePath, *Entry.TargetAssetName));
			if (Package == nullptr)
			{
				OutError = FString::Printf(TEXT("Failed to create package for '%s'."), *Entry.TargetAssetName);
				return false;
			}

			StaticMesh = NewObject<UStaticMesh>(Package, *Entry.TargetAssetName, RF_Public | RF_Standalone);
			if (StaticMesh == nullptr)
			{
				OutError = FString::Printf(TEXT("Failed to create static mesh asset '%s'."), *Entry.TargetAssetName);
				return false;
			}

			StaticMesh->InitResources();
			StaticMesh->SetLightingGuid();
			FAssetRegistryModule::AssetCreated(StaticMesh);
		}
		else
		{
			StaticMesh->Modify();
			StaticMesh->PreEditChange(nullptr);
		}

		StaticMesh->SetNumSourceModels(0);
		FStaticMeshSourceModel& SourceModel = StaticMesh->AddSourceModel();
		SourceModel.BuildSettings.bRecomputeNormals = false;
		SourceModel.BuildSettings.bRecomputeTangents = false;
		SourceModel.BuildSettings.bRemoveDegenerates = false;
		SourceModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
		SourceModel.BuildSettings.bUseFullPrecisionUVs = false;
		SourceModel.BuildSettings.bGenerateLightmapUVs = false;

		StaticMesh->GetStaticMaterials().Reset();
		StaticMesh->GetStaticMaterials().Add(FStaticMaterial(&SharedMaterial, SharedMaterialSlotName, SharedMaterialSlotName));
		StaticMesh->SetImportVersion(EImportStaticMeshVersion::LastVersion);
		StaticMesh->SetLightMapCoordinateIndex(0);
		StaticMesh->GetSectionInfoMap().Set(0, 0, FMeshSectionInfo(0));
		StaticMesh->GetOriginalSectionInfoMap().CopyFrom(StaticMesh->GetSectionInfoMap());

		StaticMesh->CreateMeshDescription(0, MoveTemp(MeshDescription));
		UStaticMesh::FCommitMeshDescriptionParams CommitParams;
		CommitParams.bMarkPackageDirty = false;
		StaticMesh->CommitMeshDescription(0, CommitParams);
		StaticMesh->Build(false);
		StaticMesh->CreateBodySetup();
		if (UBodySetup* BodySetup = StaticMesh->GetBodySetup())
		{
			BodySetup->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;
			BodySetup->DefaultInstance.SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
		}

		StaticMesh->MarkPackageDirty();
		StaticMesh->PostEditChange();

		if (!SaveAssetPackage(*StaticMesh, OutError))
		{
			return false;
		}

		OutGeneratedAssetPath = ObjectPath;
		return true;
	}
}

bool FCodexVoxAssetGenerator::Run(const FVoxAssetBuildSettings& Settings, FVoxAssetBuildResult& OutResult, FString& OutError)
{
	using namespace CodexVoxAssetGenerator;

	if (Settings.ManifestPath.IsEmpty())
	{
		OutError = TEXT("Manifest path is empty.");
		return false;
	}

	TArray<FVoxAssetManifestEntry> Entries;
	if (!LoadManifest(Settings.ManifestPath, Entries, OutError))
	{
		return false;
	}

	UMaterial* SharedMaterial = EnsureSharedMaterial(OutError);
	if (SharedMaterial == nullptr)
	{
		return false;
	}

	const FString ManifestDirectory = FPaths::GetPath(Settings.ManifestPath);
	for (const FVoxAssetManifestEntry& Entry : Entries)
	{
		++OutResult.ProcessedCount;

		const FString VoxFilePath = FPaths::ConvertRelativePathToFull(ManifestDirectory, Entry.SourceVoxFile);
		FVoxModel Model;
		if (!LoadVoxFile(VoxFilePath, Model, OutError))
		{
			OutError = FString::Printf(TEXT("%s Entry='%s'"), *OutError, *Entry.Id);
			return false;
		}

		if (!Model.HasExpectedResolution())
		{
			OutError = FString::Printf(TEXT("VOX file '%s' has unsupported resolution %d x %d x %d. Expected 32 x 32 x 32."), *VoxFilePath, Model.SizeX, Model.SizeY, Model.SizeZ);
			return false;
		}

		FString GeneratedAssetPath;
		if (!BuildStaticMeshAsset(Entry, Model, *SharedMaterial, Settings.bOverwriteExisting, GeneratedAssetPath, OutError))
		{
			return false;
		}

		++OutResult.BuiltCount;
		OutResult.GeneratedAssetPaths.Add(GeneratedAssetPath);

		if (Settings.bVerbose)
		{
			UE_LOG(LogCodexVoxAssetGenerator, Display, TEXT("Built %s from %s"), *GeneratedAssetPath, *VoxFilePath);
		}
	}

	return true;
}
