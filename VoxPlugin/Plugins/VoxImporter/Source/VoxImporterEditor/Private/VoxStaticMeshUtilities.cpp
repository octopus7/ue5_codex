#include "VoxStaticMeshUtilities.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "EditorFramework/AssetImportData.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Factories/MaterialFactoryNew.h"
#include "IMeshReductionInterfaces.h"
#include "IMeshReductionManagerModule.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionVertexColor.h"
#include "Materials/MaterialInterface.h"
#include "MeshDescription.h"
#include "MeshReductionSettings.h"
#include "Misc/FeedbackContext.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "OverlappingCorners.h"
#include "PhysicsEngine/BodySetup.h"
#include "StaticMeshAttributes.h"
#include "StaticMeshOperations.h"
#include "VoxModel.h"
#include "VoxImportedAssetUserData.h"

namespace
{
	struct FVoxColorGrid
	{
		FIntVector Size = FIntVector::ZeroValue;
		TArray<uint8> Cells;

		void Initialize(const FIntVector& InSize)
		{
			Size = InSize;
			Cells.Init(0, Size.X * Size.Y * Size.Z);
		}

		bool IsInside(int32 X, int32 Y, int32 Z) const
		{
			return X >= 0 && Y >= 0 && Z >= 0 && X < Size.X && Y < Size.Y && Z < Size.Z;
		}

		int32 ToIndex(int32 X, int32 Y, int32 Z) const
		{
			return X + (Y * Size.X) + (Z * Size.X * Size.Y);
		}

		uint8 Get(int32 X, int32 Y, int32 Z) const
		{
			if (!IsInside(X, Y, Z))
			{
				return 0;
			}

			return Cells[ToIndex(X, Y, Z)];
		}

		void Set(int32 X, int32 Y, int32 Z, uint8 ColorIndex)
		{
			Cells[ToIndex(X, Y, Z)] = ColorIndex;
		}
	};

	const TCHAR* GVoxelMaterialPackagePath = TEXT("/Game/VoxImporter/Materials/M_VoxVertexColor");
	const TCHAR* GVoxelMaterialObjectPath = TEXT("/Game/VoxImporter/Materials/M_VoxVertexColor.M_VoxVertexColor");
	const TCHAR* GVoxelMaterialAssetName = TEXT("M_VoxVertexColor");

	UMaterialInterface* CreateVoxelProjectMaterial()
	{
		UPackage* Package = CreatePackage(GVoxelMaterialPackagePath);
		if (!Package)
		{
			return nullptr;
		}

		UMaterialFactoryNew* MaterialFactory = NewObject<UMaterialFactoryNew>();
		UMaterial* Material = Cast<UMaterial>(
			MaterialFactory->FactoryCreateNew(
				UMaterial::StaticClass(),
				Package,
				*FString(GVoxelMaterialAssetName),
				RF_Standalone | RF_Public | RF_Transactional,
				nullptr,
				GWarn));

		if (!Material)
		{
			return nullptr;
		}

		Material->PreEditChange(nullptr);

		UMaterialEditorOnlyData* MaterialEditorOnly = Material->GetEditorOnlyData();

		UMaterialExpressionVertexColor* VertexColor = NewObject<UMaterialExpressionVertexColor>(Material);
		VertexColor->MaterialExpressionEditorX = -400;
		VertexColor->MaterialExpressionEditorY = -220;
		Material->GetExpressionCollection().AddExpression(VertexColor);
		MaterialEditorOnly->BaseColor.Connect(0, VertexColor);

		auto AddConstantExpression = [Material](float Value, int32 X, int32 Y)
		{
			UMaterialExpressionConstant* Constant = NewObject<UMaterialExpressionConstant>(Material);
			Constant->R = Value;
			Constant->MaterialExpressionEditorX = X;
			Constant->MaterialExpressionEditorY = Y;
			Material->GetExpressionCollection().AddExpression(Constant);
			return Constant;
		};

		MaterialEditorOnly->Roughness.Connect(0, AddConstantExpression(1.0f, -400, 0));
		MaterialEditorOnly->Specular.Connect(0, AddConstantExpression(0.0f, -400, 120));
		MaterialEditorOnly->Metallic.Connect(0, AddConstantExpression(0.0f, -400, 240));

		Material->PostEditChange();
		Material->MarkPackageDirty();
		Package->SetDirtyFlag(true);
		FAssetRegistryModule::AssetCreated(Material);
		Material->ForceRecompileForRendering();

		return Material;
	}

	bool HasVoxSourceImportPath(const UStaticMesh* StaticMesh, FString* OutFilename = nullptr)
	{
		if (!StaticMesh || !StaticMesh->GetAssetImportData())
		{
			return false;
		}

		const FString Filename = StaticMesh->GetAssetImportData()->GetFirstFilename();
		if (Filename.IsEmpty() || !FPaths::GetExtension(Filename).Equals(TEXT("vox"), ESearchCase::IgnoreCase))
		{
			return false;
		}

		if (OutFilename)
		{
			*OutFilename = Filename;
		}

		return true;
	}

	void UpdateVoxImportedAssetUserData(UStaticMesh* StaticMesh, const FVoxMeshAssetBuildParams& BuildParams)
	{
		if (!StaticMesh)
		{
			return;
		}

		StaticMesh->RemoveUserDataOfClass(UVoxImportedAssetUserData::StaticClass());

		UVoxImportedAssetUserData* UserData = NewObject<UVoxImportedAssetUserData>(StaticMesh, UVoxImportedAssetUserData::StaticClass(), NAME_None, RF_Transactional);
		UserData->SourceFilename = BuildParams.SourceFilename;
		UserData->bIsSmoothReconstruction = BuildParams.bIsSmoothReconstruction;
		UserData->ReconstructionResolutionScale = BuildParams.ReconstructionResolutionScale;
		UserData->GeneratedFromAssetPath = BuildParams.GeneratedFromAssetPath;
		StaticMesh->AddAssetUserData(UserData);
	}

	void ComputeOccupiedBounds(const FVoxModelData& Model, FIntVector& OutMin, FIntVector& OutMax)
	{
		OutMin = FIntVector(INT32_MAX, INT32_MAX, INT32_MAX);
		OutMax = FIntVector(INT32_MIN, INT32_MIN, INT32_MIN);

		for (const FVoxVoxel& Voxel : Model.Voxels)
		{
			OutMin.X = FMath::Min(OutMin.X, Voxel.X);
			OutMin.Y = FMath::Min(OutMin.Y, Voxel.Y);
			OutMin.Z = FMath::Min(OutMin.Z, Voxel.Z);

			OutMax.X = FMath::Max(OutMax.X, Voxel.X + 1);
			OutMax.Y = FMath::Max(OutMax.Y, Voxel.Y + 1);
			OutMax.Z = FMath::Max(OutMax.Z, Voxel.Z + 1);
		}
	}

	void BuildColorGrid(const FVoxModelData& Model, FVoxColorGrid& OutGrid)
	{
		OutGrid.Initialize(Model.Size);
		for (const FVoxVoxel& Voxel : Model.Voxels)
		{
			if (OutGrid.IsInside(Voxel.X, Voxel.Y, Voxel.Z))
			{
				OutGrid.Set(Voxel.X, Voxel.Y, Voxel.Z, Voxel.ColorIndex);
			}
		}
	}

	float SquaredDistanceToVoxelBounds(const FVector& Position, int32 X, int32 Y, int32 Z)
	{
		const double MinX = static_cast<double>(X);
		const double MinY = static_cast<double>(Y);
		const double MinZ = static_cast<double>(Z);
		const double MaxX = MinX + 1.0;
		const double MaxY = MinY + 1.0;
		const double MaxZ = MinZ + 1.0;

		const double DX = Position.X < MinX ? MinX - Position.X : (Position.X > MaxX ? Position.X - MaxX : 0.0);
		const double DY = Position.Y < MinY ? MinY - Position.Y : (Position.Y > MaxY ? Position.Y - MaxY : 0.0);
		const double DZ = Position.Z < MinZ ? MinZ - Position.Z : (Position.Z > MaxZ ? Position.Z - MaxZ : 0.0);

		return static_cast<float>((DX * DX) + (DY * DY) + (DZ * DZ));
	}

	uint8 FindNearestColorIndex(const FVoxColorGrid& Grid, TConstArrayView<FVoxVoxel> Voxels, const FVector& Position)
	{
		const FIntVector Base(
			FMath::FloorToInt(Position.X),
			FMath::FloorToInt(Position.Y),
			FMath::FloorToInt(Position.Z));

		for (int32 Radius = 1; Radius <= 3; ++Radius)
		{
			float BestDistanceSquared = TNumericLimits<float>::Max();
			uint8 BestColorIndex = 0;

			for (int32 Z = Base.Z - Radius; Z <= Base.Z + Radius; ++Z)
			{
				for (int32 Y = Base.Y - Radius; Y <= Base.Y + Radius; ++Y)
				{
					for (int32 X = Base.X - Radius; X <= Base.X + Radius; ++X)
					{
						const uint8 ColorIndex = Grid.Get(X, Y, Z);
						if (ColorIndex == 0)
						{
							continue;
						}

						const float DistanceSquared = SquaredDistanceToVoxelBounds(Position, X, Y, Z);
						if (DistanceSquared < BestDistanceSquared)
						{
							BestDistanceSquared = DistanceSquared;
							BestColorIndex = ColorIndex;
						}
					}
				}
			}

			if (BestColorIndex != 0)
			{
				return BestColorIndex;
			}
		}

		float BestDistanceSquared = TNumericLimits<float>::Max();
		uint8 BestColorIndex = 0;
		for (const FVoxVoxel& Voxel : Voxels)
		{
			const float DistanceSquared = SquaredDistanceToVoxelBounds(Position, Voxel.X, Voxel.Y, Voxel.Z);
			if (DistanceSquared < BestDistanceSquared)
			{
				BestDistanceSquared = DistanceSquared;
				BestColorIndex = Voxel.ColorIndex;
			}
		}

		return BestColorIndex;
	}

	void NeutralizeMeshDescriptionAttributes(FMeshDescription& MeshDescription)
	{
		FStaticMeshAttributes Attributes(MeshDescription);
		Attributes.Register();

		TVertexInstanceAttributesRef<FVector2f> VertexUVs = Attributes.GetVertexInstanceUVs();
		TVertexInstanceAttributesRef<FVector3f> VertexNormals = Attributes.GetVertexInstanceNormals();
		TVertexInstanceAttributesRef<FVector3f> VertexTangents = Attributes.GetVertexInstanceTangents();
		TVertexInstanceAttributesRef<float> VertexBinormalSigns = Attributes.GetVertexInstanceBinormalSigns();
		TVertexInstanceAttributesRef<FVector4f> VertexColors = Attributes.GetVertexInstanceColors();

		VertexUVs.SetNumChannels(1);

		for (const FVertexInstanceID VertexInstanceID : MeshDescription.VertexInstances().GetElementIDs())
		{
			VertexUVs.Set(VertexInstanceID, 0, FVector2f::ZeroVector);
			VertexNormals[VertexInstanceID] = FVector3f::UpVector;
			VertexTangents[VertexInstanceID] = FVector3f::RightVector;
			VertexBinormalSigns[VertexInstanceID] = 1.0f;
			VertexColors[VertexInstanceID] = FVector4f(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	void RecomputeMeshSurfaceAttributes(FMeshDescription& MeshDescription)
	{
		FStaticMeshAttributes Attributes(MeshDescription);
		Attributes.Register();
		Attributes.GetVertexInstanceUVs().SetNumChannels(1);

		FStaticMeshOperations::ComputeTriangleTangentsAndNormals(MeshDescription, 0.0f, TEXT("VoxImporterReconstruction"));
		FStaticMeshOperations::ComputeTangentsAndNormals(
			MeshDescription,
			EComputeNTBsFlags::Normals | EComputeNTBsFlags::Tangents | EComputeNTBsFlags::BlendOverlappingNormals);
	}
}

UMaterialInterface* VoxStaticMeshUtilities::ResolveVoxelMaterial()
{
	if (UMaterialInterface* ExistingMaterial = LoadObject<UMaterialInterface>(nullptr, GVoxelMaterialObjectPath))
	{
		return ExistingMaterial;
	}

	if (UMaterialInterface* CreatedMaterial = CreateVoxelProjectMaterial())
	{
		return CreatedMaterial;
	}

	if (GEngine && GEngine->VertexColorViewModeMaterial_ColorOnly)
	{
		return GEngine->VertexColorViewModeMaterial_ColorOnly;
	}

	return LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/EngineDebugMaterials/VertexColorViewMode_ColorOnly.VertexColorViewMode_ColorOnly"));
}

const UVoxImportedAssetUserData* VoxStaticMeshUtilities::GetVoxImportedAssetUserData(const UStaticMesh* StaticMesh)
{
	if (!StaticMesh)
	{
		return nullptr;
	}

	if (const TArray<UAssetUserData*>* AssetUserData = StaticMesh->GetAssetUserDataArray())
	{
		for (const UAssetUserData* UserData : *AssetUserData)
		{
			if (const UVoxImportedAssetUserData* VoxUserData = Cast<UVoxImportedAssetUserData>(UserData))
			{
				return VoxUserData;
			}
		}
	}

	return nullptr;
}

bool VoxStaticMeshUtilities::IsVoxImportedStaticMesh(const UStaticMesh* StaticMesh)
{
	return GetVoxImportedAssetUserData(StaticMesh) != nullptr || HasVoxSourceImportPath(StaticMesh);
}

bool VoxStaticMeshUtilities::IsPrimaryVoxSourceStaticMesh(const UStaticMesh* StaticMesh)
{
	if (const UVoxImportedAssetUserData* UserData = GetVoxImportedAssetUserData(StaticMesh))
	{
		return !UserData->bIsSmoothReconstruction;
	}

	return HasVoxSourceImportPath(StaticMesh);
}

FString VoxStaticMeshUtilities::GetVoxSourceFilename(const UStaticMesh* StaticMesh)
{
	if (const UVoxImportedAssetUserData* UserData = GetVoxImportedAssetUserData(StaticMesh))
	{
		if (!UserData->SourceFilename.IsEmpty())
		{
			return UserData->SourceFilename;
		}
	}

	FString SourceFilename;
	HasVoxSourceImportPath(StaticMesh, &SourceFilename);
	return SourceFilename;
}

bool VoxStaticMeshUtilities::SimplifyMeshDescription(
	const FMeshDescription& SourceMeshDescription,
	float TargetPercentTriangles,
	FMeshDescription& OutMeshDescription,
	FString& OutError)
{
	const float ClampedTargetPercent = FMath::Clamp(TargetPercentTriangles, 0.0f, 1.0f);
	if (ClampedTargetPercent >= 1.0f - KINDA_SMALL_NUMBER)
	{
		OutMeshDescription = SourceMeshDescription;
		return true;
	}

	if (ClampedTargetPercent <= KINDA_SMALL_NUMBER)
	{
		OutError = TEXT("Simplify target percentage must be greater than zero.");
		return false;
	}

	FMeshDescription ReductionSourceMeshDescription = SourceMeshDescription;
	NeutralizeMeshDescriptionAttributes(ReductionSourceMeshDescription);

	IMeshReduction* Reduction = FModuleManager::Get().LoadModuleChecked<IMeshReductionManagerModule>(TEXT("MeshReductionInterface")).GetStaticMeshReductionInterface();
	if (!Reduction || !Reduction->IsSupported())
	{
		OutError = TEXT("Static mesh reduction is not available in this editor build.");
		return false;
	}

	FOverlappingCorners OverlappingCorners;
	FStaticMeshOperations::FindOverlappingCorners(OverlappingCorners, ReductionSourceMeshDescription, THRESH_POINTS_ARE_SAME);

	FMeshReductionSettings ReductionSettings;
	ReductionSettings.TerminationCriterion = EStaticMeshReductionTerimationCriterion::Triangles;
	ReductionSettings.PercentTriangles = FMath::Max(0.001f, ClampedTargetPercent);
	ReductionSettings.PercentVertices = 1.0f;
	ReductionSettings.MaxNumOfTriangles = MAX_uint32;
	ReductionSettings.MaxNumOfVerts = MAX_uint32;
	ReductionSettings.TextureImportance = EMeshFeatureImportance::Off;
	ReductionSettings.ShadingImportance = EMeshFeatureImportance::High;
	ReductionSettings.SilhouetteImportance = EMeshFeatureImportance::Highest;
	ReductionSettings.VertexColorImportance = EMeshFeatureImportance::Highest;

	FStaticMeshAttributes OutputAttributes(OutMeshDescription);
	OutputAttributes.Register();

	float MaxDeviation = 0.0f;
	Reduction->ReduceMeshDescription(OutMeshDescription, MaxDeviation, ReductionSourceMeshDescription, OverlappingCorners, ReductionSettings);

	if (OutMeshDescription.Triangles().Num() == 0)
	{
		OutError = TEXT("Mesh reduction produced an empty result.");
		return false;
	}

	return true;
}

bool VoxStaticMeshUtilities::ReapplyVoxelVertexColors(const FVoxModelData& Model, FMeshDescription& MeshDescription, float VoxelSize, FString& OutError)
{
	if (VoxelSize <= 0.0f)
	{
		OutError = TEXT("Voxel size must be greater than zero.");
		return false;
	}

	if (Model.Voxels.IsEmpty())
	{
		OutError = TEXT("Model has no voxels.");
		return false;
	}

	RecomputeMeshSurfaceAttributes(MeshDescription);

	FStaticMeshAttributes Attributes(MeshDescription);
	TVertexAttributesConstRef<FVector3f> VertexPositions = MeshDescription.VertexAttributes().GetAttributesRef<FVector3f>(MeshAttribute::Vertex::Position);
	TVertexInstanceAttributesRef<FVector4f> VertexColors = Attributes.GetVertexInstanceColors();

	FVoxColorGrid ColorGrid;
	BuildColorGrid(Model, ColorGrid);

	FIntVector BoundsMin;
	FIntVector BoundsMax;
	ComputeOccupiedBounds(Model, BoundsMin, BoundsMax);
	const FVector PivotOffset = (FVector(BoundsMin) + FVector(BoundsMax)) * 0.5f * VoxelSize;

	for (const FVertexInstanceID VertexInstanceID : MeshDescription.VertexInstances().GetElementIDs())
	{
		const FVertexID VertexID = MeshDescription.GetVertexInstanceVertex(VertexInstanceID);
		const FVector SamplePosition = (FVector(VertexPositions[VertexID]) + PivotOffset) / VoxelSize;
		const uint8 ColorIndex = FindNearestColorIndex(ColorGrid, Model.Voxels, SamplePosition);
		VertexColors[VertexInstanceID] = FVector4f(FLinearColor::FromSRGBColor(Model.GetColor(ColorIndex)));
	}

	return true;
}

bool VoxStaticMeshUtilities::SimplifyReconstructedMeshDescription(
	const FMeshDescription& SourceMeshDescription,
	const FVoxModelData& Model,
	float VoxelSize,
	float TargetPercentTriangles,
	FMeshDescription& OutMeshDescription,
	FString& OutError)
{
	FMeshDescription SimplifiedGeometry;
	if (!SimplifyMeshDescription(SourceMeshDescription, TargetPercentTriangles, SimplifiedGeometry, OutError))
	{
		return false;
	}

	if (!ReapplyVoxelVertexColors(Model, SimplifiedGeometry, VoxelSize, OutError))
	{
		return false;
	}

	OutMeshDescription = MoveTemp(SimplifiedGeometry);
	return true;
}

bool VoxStaticMeshUtilities::BuildStaticMeshAsset(UStaticMesh* StaticMesh, const FMeshDescription& MeshDescription, const FVoxMeshAssetBuildParams& BuildParams, FFeedbackContext* Warn)
{
	if (!StaticMesh)
	{
		if (Warn)
		{
			Warn->Logf(ELogVerbosity::Error, TEXT("Static Mesh asset target was null."));
		}
		return false;
	}

	StaticMesh->SetFlags(RF_Transactional);
	StaticMesh->Modify();
	StaticMesh->SetImportVersion(EImportStaticMeshVersion::LastVersion);
	StaticMesh->SetNumSourceModels(1);

	FStaticMeshSourceModel& SourceModel = StaticMesh->GetSourceModel(0);
	SourceModel.BuildSettings.bRecomputeNormals = false;
	SourceModel.BuildSettings.bRecomputeTangents = false;
	SourceModel.BuildSettings.bGenerateLightmapUVs = false;
	SourceModel.BuildSettings.bUseFullPrecisionUVs = true;
	SourceModel.BuildSettings.bUseHighPrecisionTangentBasis = false;

	const FName MaterialSlotName(TEXT("VoxelMaterial"));
	StaticMesh->GetStaticMaterials().Reset();
	StaticMesh->GetStaticMaterials().Add(FStaticMaterial(ResolveVoxelMaterial(), MaterialSlotName, MaterialSlotName));

	TArray<const FMeshDescription*> MeshDescriptions;
	MeshDescriptions.Add(&MeshDescription);

	UStaticMesh::FBuildMeshDescriptionsParams BuildMeshParams;
	BuildMeshParams.bCommitMeshDescription = true;
	BuildMeshParams.bFastBuild = false;
	BuildMeshParams.bBuildSimpleCollision = false;
	BuildMeshParams.bMarkPackageDirty = true;

	if (!StaticMesh->BuildFromMeshDescriptions(MeshDescriptions, BuildMeshParams))
	{
		if (Warn)
		{
			Warn->Logf(ELogVerbosity::Error, TEXT("Failed to build Static Mesh render data for %s."), *StaticMesh->GetName());
		}
		return false;
	}

	StaticMesh->CreateBodySetup();
	if (UBodySetup* BodySetup = StaticMesh->GetBodySetup())
	{
		BodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
		BodySetup->InvalidatePhysicsData();
		BodySetup->CreatePhysicsMeshes();
	}

	if (!StaticMesh->GetAssetImportData())
	{
		StaticMesh->SetAssetImportData(NewObject<UAssetImportData>(StaticMesh, TEXT("AssetImportData")));
	}

	if (!BuildParams.SourceFilename.IsEmpty())
	{
		StaticMesh->GetAssetImportData()->Update(BuildParams.SourceFilename);
	}

	UpdateVoxImportedAssetUserData(StaticMesh, BuildParams);

	StaticMesh->MarkPackageDirty();
	StaticMesh->PostEditChange();
	return true;
}
