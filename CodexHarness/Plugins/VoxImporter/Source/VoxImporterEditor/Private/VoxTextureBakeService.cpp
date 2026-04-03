#include "VoxTextureBakeService.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "IMaterialBakingModule.h"
#include "MaterialBakingStructures.h"
#include "MaterialPropertyEx.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MeshDescription.h"
#include "Misc/PackageName.h"
#include "Modules/ModuleManager.h"
#include "UObject/Package.h"
#include "VoxImporterEditorConstants.h"
#include "VoxStaticMeshUtilities.h"

namespace
{
	const TCHAR* GVoxBakedTextureParameterName = TEXT("BaseColorTexture");

	FString MakeSiblingAssetPackagePath(const UStaticMesh* StaticMesh, const FString& AssetName)
	{
		const FString PackagePath = FPackageName::GetLongPackagePath(StaticMesh->GetOutermost()->GetName());
		return PackagePath + TEXT("/") + AssetName;
	}

	template <typename TObjectType>
	TObjectType* LoadSiblingAsset(const UStaticMesh* StaticMesh, const FString& AssetName)
	{
		const FString PackagePath = MakeSiblingAssetPackagePath(StaticMesh, AssetName);
		const FString ObjectPath = PackagePath + TEXT(".") + AssetName;
		return LoadObject<TObjectType>(nullptr, *ObjectPath);
	}

	void WriteTexturePixels(UTexture2D* Texture, const TArray<FColor>& Pixels, const FIntPoint& Size)
	{
		Texture->Source.Init(Size.X, Size.Y, 1, 1, TSF_BGRA8);

		uint8* MipData = Texture->Source.LockMip(0);
		for (int32 Y = 0; Y < Size.Y; ++Y)
		{
			uint8* DestPtr = &MipData[static_cast<int64>(Y) * Size.X * sizeof(FColor)];
			const FColor* SrcPtr = &Pixels[static_cast<int64>(Y) * Size.X];
			for (int32 X = 0; X < Size.X; ++X)
			{
				*DestPtr++ = SrcPtr->B;
				*DestPtr++ = SrcPtr->G;
				*DestPtr++ = SrcPtr->R;
				*DestPtr++ = SrcPtr->A;
				++SrcPtr;
			}
		}
		Texture->Source.UnlockMip(0);
	}

	UTexture2D* CreateOrUpdateBakedTexture(UStaticMesh* StaticMesh, const TArray<FColor>& Pixels, const FIntPoint& Size, bool bSRGB)
	{
		const FString TextureAssetName = FString::Printf(TEXT("T_%s_BaseColor"), *StaticMesh->GetName());
		const FString TexturePackagePath = MakeSiblingAssetPackagePath(StaticMesh, TextureAssetName);

		bool bIsNewAsset = false;
		UTexture2D* Texture = LoadSiblingAsset<UTexture2D>(StaticMesh, TextureAssetName);
		if (!Texture)
		{
			UPackage* Package = CreatePackage(*TexturePackagePath);
			if (!Package)
			{
				return nullptr;
			}

			Texture = NewObject<UTexture2D>(Package, *TextureAssetName, RF_Public | RF_Standalone | RF_Transactional);
			bIsNewAsset = Texture != nullptr;
		}

		if (!Texture)
		{
			return nullptr;
		}

		Texture->Modify();
		WriteTexturePixels(Texture, Pixels, Size);
		Texture->SRGB = bSRGB;
		Texture->CompressionSettings = TC_Default;
		Texture->CompressionNoAlpha = true;
		Texture->DeferCompression = false;
		Texture->MipGenSettings = TMGS_FromTextureGroup;
		Texture->LODGroup = TEXTUREGROUP_World;
		Texture->VirtualTextureStreaming = false;
		Texture->NeverStream = false;
		Texture->SetModernSettingsForNewOrChangedTexture();
		Texture->PostEditChange();
		Texture->MarkPackageDirty();
		Texture->GetOutermost()->MarkPackageDirty();

		if (bIsNewAsset)
		{
			FAssetRegistryModule::AssetCreated(Texture);
		}

		return Texture;
	}

	UMaterialInstanceConstant* CreateOrUpdateBakedMaterialInstance(UStaticMesh* StaticMesh, UTexture2D* Texture, FString& OutError)
	{
		UMaterialInterface* MasterMaterial = VoxStaticMeshUtilities::ResolveVoxelBakedMaterial();
		if (!MasterMaterial)
		{
			OutError = TEXT("Failed to resolve the shared baked-color material.");
			return nullptr;
		}

		const FString MaterialAssetName = FString::Printf(TEXT("MI_%s_Baked"), *StaticMesh->GetName());
		const FString MaterialPackagePath = MakeSiblingAssetPackagePath(StaticMesh, MaterialAssetName);

		bool bIsNewAsset = false;
		UMaterialInstanceConstant* MaterialInstance = LoadSiblingAsset<UMaterialInstanceConstant>(StaticMesh, MaterialAssetName);
		if (!MaterialInstance)
		{
			UPackage* Package = CreatePackage(*MaterialPackagePath);
			if (!Package)
			{
				OutError = TEXT("Failed to create a package for the baked material instance.");
				return nullptr;
			}

			UMaterialInstanceConstantFactoryNew* Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
			Factory->InitialParent = MasterMaterial;
			MaterialInstance = Cast<UMaterialInstanceConstant>(
				Factory->FactoryCreateNew(
					UMaterialInstanceConstant::StaticClass(),
					Package,
					*MaterialAssetName,
					RF_Public | RF_Standalone | RF_Transactional,
					nullptr,
					GWarn));
			bIsNewAsset = MaterialInstance != nullptr;
		}

		if (!MaterialInstance)
		{
			OutError = TEXT("Failed to create the baked material instance.");
			return nullptr;
		}

		MaterialInstance->Modify();
		MaterialInstance->SetParentEditorOnly(MasterMaterial);
		MaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(GVoxBakedTextureParameterName), Texture);
		MaterialInstance->PostEditChange();
		MaterialInstance->MarkPackageDirty();
		MaterialInstance->GetOutermost()->MarkPackageDirty();

		if (bIsNewAsset)
		{
			FAssetRegistryModule::AssetCreated(MaterialInstance);
		}

		return MaterialInstance;
	}

	bool BakeBaseColorTexture(
		UStaticMesh* StaticMesh,
		const FMeshDescription& MeshDescription,
		TConstArrayView<FVector2D> GeneratedUVs,
		TArray<FColor>& OutPixels,
		FIntPoint& OutSize,
		bool& OutSRGB,
		FString& OutError)
	{
		UMaterialInterface* VertexColorMaterial = VoxStaticMeshUtilities::ResolveVoxelMaterial();
		if (!VertexColorMaterial)
		{
			OutError = TEXT("Failed to resolve the vertex-color source material.");
			return false;
		}

		FMeshData MeshSettings;
		MeshSettings.MeshDescription = &MeshDescription;
		MeshSettings.Mesh = StaticMesh;
		MeshSettings.TextureCoordinateBox = FBox2D(FVector2D::ZeroVector, FVector2D(1.0f, 1.0f));
		MeshSettings.TextureCoordinateIndex = 0;
		MeshSettings.CustomTextureCoordinates.Append(GeneratedUVs.GetData(), GeneratedUVs.Num());
		MeshSettings.PrimitiveData = FPrimitiveData(StaticMesh);

		for (const FPolygonGroupID PolygonGroupID : MeshDescription.PolygonGroups().GetElementIDs())
		{
			MeshSettings.MaterialIndices.Add(PolygonGroupID.GetValue());
		}

		if (MeshSettings.MaterialIndices.IsEmpty())
		{
			MeshSettings.MaterialIndices.Add(0);
		}

		FMaterialDataEx MaterialSettings;
		MaterialSettings.Material = VertexColorMaterial;
		MaterialSettings.PropertySizes.Add(
			FMaterialPropertyEx(MP_BaseColor),
			FIntPoint(VoxImporterEditorConstants::BakeTextureResolution, VoxImporterEditorConstants::BakeTextureResolution));
		MaterialSettings.bPerformBorderSmear = true;
		MaterialSettings.bPerformShrinking = false;
		MaterialSettings.BackgroundColor = FColor::Black;

		TArray<FMeshData*> MeshSettingsPtrs;
		MeshSettingsPtrs.Add(&MeshSettings);

		TArray<FMaterialDataEx*> MaterialSettingsPtrs;
		MaterialSettingsPtrs.Add(&MaterialSettings);

		TArray<FBakeOutputEx> BakeOutputs;
		IMaterialBakingModule& MaterialBakingModule = FModuleManager::LoadModuleChecked<IMaterialBakingModule>("MaterialBaking");
		MaterialBakingModule.SetLinearBake(true);
		MaterialBakingModule.BakeMaterials(MaterialSettingsPtrs, MeshSettingsPtrs, BakeOutputs);
		OutSRGB = !MaterialBakingModule.IsLinearBake(FMaterialPropertyEx(MP_BaseColor));
		MaterialBakingModule.SetLinearBake(false);

		if (BakeOutputs.IsEmpty())
		{
			OutError = TEXT("Material baking returned no output.");
			return false;
		}

		FBakeOutputEx& BakeOutput = BakeOutputs[0];
		const TArray<FColor>* PropertyPixels = BakeOutput.PropertyData.Find(FMaterialPropertyEx(MP_BaseColor));
		const FIntPoint* PropertySize = BakeOutput.PropertySizes.Find(FMaterialPropertyEx(MP_BaseColor));
		if (!PropertyPixels || !PropertySize || PropertyPixels->IsEmpty())
		{
			OutError = TEXT("Material baking did not produce Base Color pixels.");
			return false;
		}

		OutPixels = *PropertyPixels;
		OutSize = *PropertySize;
		for (FColor& Pixel : OutPixels)
		{
			Pixel.A = 255;
		}

		return true;
	}
}

namespace VoxTextureBakeService
{
	bool BakeVertexColorToTexture(UStaticMesh* StaticMesh, TArray<UObject*>& OutCreatedAssets, FString& OutError)
	{
		OutCreatedAssets.Reset();

		if (!VoxStaticMeshUtilities::IsVoxImportedStaticMesh(StaticMesh))
		{
			OutError = TEXT("Selected Static Mesh was not imported by the VOX importer.");
			return false;
		}

		FMeshDescription SourceMeshDescription;
		if (!StaticMesh->CloneMeshDescription(0, SourceMeshDescription))
		{
			OutError = TEXT("Failed to clone the source mesh description from the selected Static Mesh.");
			return false;
		}

		FMeshDescription PreparedMeshDescription;
		TArray<FVector2D> GeneratedUVs;
		if (!VoxStaticMeshUtilities::PrepareVertexColorTextureBakeMeshDescription(SourceMeshDescription, PreparedMeshDescription, GeneratedUVs, OutError))
		{
			return false;
		}

		TArray<FColor> BakedPixels;
		FIntPoint BakedSize = FIntPoint::ZeroValue;
		bool bTextureSRGB = true;
		if (!BakeBaseColorTexture(StaticMesh, SourceMeshDescription, GeneratedUVs, BakedPixels, BakedSize, bTextureSRGB, OutError))
		{
			return false;
		}

		UTexture2D* BakedTexture = CreateOrUpdateBakedTexture(StaticMesh, BakedPixels, BakedSize, bTextureSRGB);
		if (!BakedTexture)
		{
			OutError = TEXT("Failed to create or update the baked color texture asset.");
			return false;
		}

		UMaterialInstanceConstant* BakedMaterialInstance = CreateOrUpdateBakedMaterialInstance(StaticMesh, BakedTexture, OutError);
		if (!BakedMaterialInstance)
		{
			return false;
		}

		const FVoxMeshAssetBuildParams BuildParams = VoxStaticMeshUtilities::MakeBuildParamsFromStaticMesh(StaticMesh);
		if (!VoxStaticMeshUtilities::BuildStaticMeshAsset(StaticMesh, PreparedMeshDescription, BuildParams, GWarn))
		{
			OutError = TEXT("Failed to rebuild the Static Mesh with baked UVs and cleared vertex colors.");
			return false;
		}

		const FName MaterialSlotName(TEXT("VoxelMaterial"));
		StaticMesh->GetStaticMaterials().Reset();
		StaticMesh->GetStaticMaterials().Add(FStaticMaterial(BakedMaterialInstance, MaterialSlotName, MaterialSlotName));
		StaticMesh->MarkPackageDirty();
		StaticMesh->PostEditChange();

		OutCreatedAssets.AddUnique(BakedTexture);
		OutCreatedAssets.AddUnique(BakedMaterialInstance);
		return true;
	}
}
