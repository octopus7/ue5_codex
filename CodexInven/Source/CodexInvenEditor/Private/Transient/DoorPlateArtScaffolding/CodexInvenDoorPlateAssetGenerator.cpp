#include "Transient/DoorPlateArtScaffolding/CodexInvenDoorPlateAssetGenerator.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/StaticMesh.h"
#include "Factories/MaterialFactoryNew.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "GeometryScript/CreateNewAssetUtilityFunctions.h"
#include "GeometryScript/GeometryScriptTypes.h"
#include "GeometryScript/MeshAssetFunctions.h"
#include "GeometryScript/MeshBasicEditFunctions.h"
#include "GeometryScript/MeshNormalsFunctions.h"
#include "GeometryScript/MeshPrimitiveFunctions.h"
#include "GeometryScript/MeshVertexColorFunctions.h"
#include "HAL/FileManager.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionVertexColor.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/ScopedSlowTask.h"
#include "UDynamicMesh.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/UObjectGlobals.h"

DEFINE_LOG_CATEGORY_STATIC(LogCodexInvenDoorPlateAssetGenerator, Log, All);

namespace
{
	const TCHAR* BaseMaterialPackagePath = TEXT("/Game/Art/Interactables/DoorPlate/Materials/M_DoorPlateVertexColor");
	const TCHAR* DoorMaterialInstancePackagePath = TEXT("/Game/Art/Interactables/DoorPlate/Materials/MI_DoorPlateVertexColor_Door");
	const TCHAR* PressurePlateMaterialInstancePackagePath = TEXT("/Game/Art/Interactables/DoorPlate/Materials/MI_DoorPlateVertexColor_PressurePlate");
	const TCHAR* DoorMeshPackagePath = TEXT("/Game/Art/Interactables/DoorPlate/Meshes/SM_DoorPanel_Prototype");
	const TCHAR* PressurePlateMeshPackagePath = TEXT("/Game/Art/Interactables/DoorPlate/Meshes/SM_PressurePlate_Prototype");

	struct FDoorPalette
	{
		FLinearColor MainWoodColor;
		FLinearColor DarkTrimColor;
		FLinearColor MetalAccentColor;
		FLinearColor HandleColor;
	};

	struct FPressurePlatePalette
	{
		FLinearColor BaseColor;
		FLinearColor TopPanelColor;
		FLinearColor AccentColor;
		FLinearColor EdgeColor;
	};

	FString MakeObjectPath(const FString& InPackagePath)
	{
		return InPackagePath + TEXT(".") + FPackageName::GetLongPackageAssetName(InPackagePath);
	}

	FString GetAssetNameFromPackagePath(const FString& InPackagePath)
	{
		return FPackageName::GetLongPackageAssetName(InPackagePath);
	}

	template <typename TObjectType>
	TObjectType* LoadExistingAsset(const FString& InPackagePath)
	{
		const FString Filename = FPackageName::LongPackageNameToFilename(InPackagePath, FPackageName::GetAssetPackageExtension());
		if (!FPaths::FileExists(Filename))
		{
			return nullptr;
		}

		return LoadObject<TObjectType>(nullptr, *MakeObjectPath(InPackagePath));
	}

	bool SaveAssetPackage(UObject& InAsset, FString& OutError)
	{
		UPackage* const Package = InAsset.GetPackage();
		if (Package == nullptr)
		{
			OutError = FString::Printf(TEXT("Asset %s has no package."), *InAsset.GetName());
			return false;
		}

		const FString Filename = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
		IFileManager::Get().MakeDirectory(*FPaths::GetPath(Filename), true);

		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.SaveFlags = SAVE_NoError;
		SaveArgs.Error = GError;
		if (!UPackage::SavePackage(Package, &InAsset, *Filename, SaveArgs))
		{
			OutError = FString::Printf(TEXT("Failed to save package %s."), *Package->GetName());
			return false;
		}

		return true;
	}

	FGeometryScriptPrimitiveOptions MakePrimitiveOptions()
	{
		FGeometryScriptPrimitiveOptions Options;
		Options.PolygroupMode = EGeometryScriptPrimitivePolygroupMode::PerFace;
		Options.MaterialID = 0;
		return Options;
	}

	UDynamicMesh* MakeTransientDynamicMesh()
	{
		return NewObject<UDynamicMesh>(GetTransientPackage(), NAME_None, RF_Transient);
	}

	void AppendBoxPart(
		UDynamicMesh& InOutMesh,
		const FVector& InDimensions,
		const FVector& InLocation,
		const FRotator& InRotation,
		const FLinearColor& InColor)
	{
		UDynamicMesh* const PartMesh = MakeTransientDynamicMesh();
		UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendBox(
			PartMesh,
			MakePrimitiveOptions(),
			FTransform(InRotation, InLocation),
			InDimensions.X,
			InDimensions.Y,
			InDimensions.Z,
			0,
			0,
			0,
			EGeometryScriptPrimitiveOriginMode::Center);
		UGeometryScriptLibrary_MeshVertexColorFunctions::SetMeshConstantVertexColor(
			PartMesh,
			InColor,
			FGeometryScriptColorFlags(),
			true);
		UGeometryScriptLibrary_MeshBasicEditFunctions::AppendMesh(&InOutMesh, PartMesh, FTransform::Identity, true);
	}

	void AppendCylinderPart(
		UDynamicMesh& InOutMesh,
		const float InRadius,
		const float InHeight,
		const FVector& InLocation,
		const FRotator& InRotation,
		const FLinearColor& InColor,
		const int32 InRadialSteps = 16)
	{
		UDynamicMesh* const PartMesh = MakeTransientDynamicMesh();
		UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendCylinder(
			PartMesh,
			MakePrimitiveOptions(),
			FTransform(InRotation, InLocation),
			InRadius,
			InHeight,
			InRadialSteps,
			0,
			true,
			EGeometryScriptPrimitiveOriginMode::Center);
		UGeometryScriptLibrary_MeshVertexColorFunctions::SetMeshConstantVertexColor(
			PartMesh,
			InColor,
			FGeometryScriptColorFlags(),
			true);
		UGeometryScriptLibrary_MeshBasicEditFunctions::AppendMesh(&InOutMesh, PartMesh, FTransform::Identity, true);
	}

	FDoorPalette BuildDoorPalette()
	{
		FDoorPalette Palette;
		Palette.MainWoodColor = FLinearColor(0.37f, 0.21f, 0.08f, 1.0f);
		Palette.DarkTrimColor = FLinearColor(0.17f, 0.11f, 0.06f, 1.0f);
		Palette.MetalAccentColor = FLinearColor(0.33f, 0.35f, 0.38f, 1.0f);
		Palette.HandleColor = FLinearColor(0.73f, 0.59f, 0.19f, 1.0f);
		return Palette;
	}

	FPressurePlatePalette BuildPressurePlatePalette()
	{
		FPressurePlatePalette Palette;
		Palette.BaseColor = FLinearColor(0.12f, 0.14f, 0.16f, 1.0f);
		Palette.TopPanelColor = FLinearColor(0.36f, 0.41f, 0.46f, 1.0f);
		Palette.AccentColor = FLinearColor(0.78f, 0.26f, 0.18f, 1.0f);
		Palette.EdgeColor = FLinearColor(0.61f, 0.64f, 0.69f, 1.0f);
		return Palette;
	}

	UDynamicMesh* BuildDoorMesh()
	{
		UDynamicMesh* const Mesh = MakeTransientDynamicMesh();
		const FDoorPalette Palette = BuildDoorPalette();
		constexpr float DoorFrontOverlayCenterX = 6.75f;
		constexpr float DoorFrontOverlayThickness = 2.5f;

		AppendBoxPart(*Mesh, FVector(12.0f, 140.0f, 300.0f), FVector(0.0f, 0.0f, 150.0f), FRotator::ZeroRotator, Palette.MainWoodColor);
		AppendBoxPart(*Mesh, FVector(DoorFrontOverlayThickness, 18.0f, 280.0f), FVector(DoorFrontOverlayCenterX, -52.0f, 150.0f), FRotator::ZeroRotator, Palette.DarkTrimColor);
		AppendBoxPart(*Mesh, FVector(DoorFrontOverlayThickness, 18.0f, 280.0f), FVector(DoorFrontOverlayCenterX, 52.0f, 150.0f), FRotator::ZeroRotator, Palette.DarkTrimColor);
		AppendBoxPart(*Mesh, FVector(DoorFrontOverlayThickness, 110.0f, 20.0f), FVector(DoorFrontOverlayCenterX, 0.0f, 52.0f), FRotator::ZeroRotator, Palette.DarkTrimColor);
		AppendBoxPart(*Mesh, FVector(DoorFrontOverlayThickness, 110.0f, 20.0f), FVector(DoorFrontOverlayCenterX, 0.0f, 248.0f), FRotator::ZeroRotator, Palette.DarkTrimColor);
		AppendBoxPart(*Mesh, FVector(DoorFrontOverlayThickness, 110.0f, 18.0f), FVector(DoorFrontOverlayCenterX, 0.0f, 160.0f), FRotator::ZeroRotator, Palette.MetalAccentColor);
		AppendBoxPart(*Mesh, FVector(DoorFrontOverlayThickness, 14.0f, 190.0f), FVector(DoorFrontOverlayCenterX, 0.0f, 150.0f), FRotator(0.0f, 0.0f, 32.0f), Palette.DarkTrimColor);
		AppendBoxPart(*Mesh, FVector(8.0f, 18.0f, 16.0f), FVector(10.0f, 34.0f, 150.0f), FRotator::ZeroRotator, Palette.HandleColor);
		AppendBoxPart(*Mesh, FVector(12.0f, 10.0f, 10.0f), FVector(20.0f, 34.0f, 150.0f), FRotator::ZeroRotator, Palette.HandleColor);
		AppendBoxPart(*Mesh, FVector(4.0f, 42.0f, 6.0f), FVector(8.0f, 0.0f, 150.0f), FRotator::ZeroRotator, Palette.MetalAccentColor);
		UGeometryScriptLibrary_MeshNormalsFunctions::SetPerFaceNormals(Mesh);
		return Mesh;
	}

	UDynamicMesh* BuildPressurePlateMesh()
	{
		UDynamicMesh* const Mesh = MakeTransientDynamicMesh();
		const FPressurePlatePalette Palette = BuildPressurePlatePalette();

		AppendBoxPart(*Mesh, FVector(160.0f, 160.0f, 18.0f), FVector(0.0f, 0.0f, 9.0f), FRotator::ZeroRotator, Palette.BaseColor);
		AppendBoxPart(*Mesh, FVector(148.0f, 148.0f, 8.0f), FVector(0.0f, 0.0f, 20.0f), FRotator::ZeroRotator, Palette.EdgeColor);
		AppendBoxPart(*Mesh, FVector(132.0f, 132.0f, 10.0f), FVector(0.0f, 0.0f, 29.0f), FRotator::ZeroRotator, Palette.TopPanelColor);
		AppendBoxPart(*Mesh, FVector(96.0f, 96.0f, 6.0f), FVector(0.0f, 0.0f, 37.0f), FRotator::ZeroRotator, Palette.AccentColor);
		AppendBoxPart(*Mesh, FVector(132.0f, 12.0f, 4.0f), FVector(0.0f, 0.0f, 41.0f), FRotator::ZeroRotator, Palette.EdgeColor);
		AppendBoxPart(*Mesh, FVector(12.0f, 132.0f, 4.0f), FVector(0.0f, 0.0f, 41.0f), FRotator::ZeroRotator, Palette.EdgeColor);
		AppendBoxPart(*Mesh, FVector(18.0f, 18.0f, 12.0f), FVector(-60.0f, -60.0f, 14.0f), FRotator::ZeroRotator, Palette.BaseColor);
		AppendBoxPart(*Mesh, FVector(18.0f, 18.0f, 12.0f), FVector(60.0f, -60.0f, 14.0f), FRotator::ZeroRotator, Palette.BaseColor);
		AppendBoxPart(*Mesh, FVector(18.0f, 18.0f, 12.0f), FVector(-60.0f, 60.0f, 14.0f), FRotator::ZeroRotator, Palette.BaseColor);
		AppendBoxPart(*Mesh, FVector(18.0f, 18.0f, 12.0f), FVector(60.0f, 60.0f, 14.0f), FRotator::ZeroRotator, Palette.BaseColor);
		AppendCylinderPart(*Mesh, 10.0f, 6.0f, FVector(0.0f, 0.0f, 43.0f), FRotator::ZeroRotator, Palette.EdgeColor, 16);
		UGeometryScriptLibrary_MeshNormalsFunctions::SetPerFaceNormals(Mesh);
		return Mesh;
	}

	UMaterial* CreateOrUpdateVertexColorMaterial(FString& OutError)
	{
		UMaterial* Material = LoadExistingAsset<UMaterial>(BaseMaterialPackagePath);
		if (Material == nullptr)
		{
			UPackage* const Package = CreatePackage(BaseMaterialPackagePath);
			UMaterialFactoryNew* const Factory = NewObject<UMaterialFactoryNew>();
			Material = Cast<UMaterial>(Factory->FactoryCreateNew(
				UMaterial::StaticClass(),
				Package,
				*GetAssetNameFromPackagePath(BaseMaterialPackagePath),
				RF_Public | RF_Standalone,
				nullptr,
				GWarn));
			if (Material == nullptr)
			{
				OutError = TEXT("Failed to create the door/plate vertex color material.");
				return nullptr;
			}

			FAssetRegistryModule::AssetCreated(Material);
		}

		Material->PreEditChange(nullptr);
		Material->BlendMode = BLEND_Opaque;
		Material->GetExpressionCollection().Empty();

		UMaterialEditorOnlyData* const EditorOnlyData = Material->GetEditorOnlyData();
		EditorOnlyData->BaseColor.Expression = nullptr;
		EditorOnlyData->Metallic.Expression = nullptr;
		EditorOnlyData->Roughness.Expression = nullptr;
		EditorOnlyData->Specular.Expression = nullptr;

		UMaterialExpressionVertexColor* const VertexColor = NewObject<UMaterialExpressionVertexColor>(Material);
		VertexColor->MaterialExpressionEditorX = -420;
		VertexColor->MaterialExpressionEditorY = -120;
		Material->GetExpressionCollection().AddExpression(VertexColor);
		EditorOnlyData->BaseColor.Expression = VertexColor;

		UMaterialExpressionScalarParameter* const Metallic = NewObject<UMaterialExpressionScalarParameter>(Material);
		Metallic->ParameterName = TEXT("Metallic");
		Metallic->DefaultValue = 0.0f;
		Metallic->MaterialExpressionEditorX = -420;
		Metallic->MaterialExpressionEditorY = 20;
		Material->GetExpressionCollection().AddExpression(Metallic);
		EditorOnlyData->Metallic.Expression = Metallic;

		UMaterialExpressionScalarParameter* const Roughness = NewObject<UMaterialExpressionScalarParameter>(Material);
		Roughness->ParameterName = TEXT("Roughness");
		Roughness->DefaultValue = 0.56f;
		Roughness->MaterialExpressionEditorX = -420;
		Roughness->MaterialExpressionEditorY = 160;
		Material->GetExpressionCollection().AddExpression(Roughness);
		EditorOnlyData->Roughness.Expression = Roughness;

		UMaterialExpressionScalarParameter* const Specular = NewObject<UMaterialExpressionScalarParameter>(Material);
		Specular->ParameterName = TEXT("Specular");
		Specular->DefaultValue = 0.5f;
		Specular->MaterialExpressionEditorX = -420;
		Specular->MaterialExpressionEditorY = 300;
		Material->GetExpressionCollection().AddExpression(Specular);
		EditorOnlyData->Specular.Expression = Specular;

		Material->PostEditChange();
		Material->MarkPackageDirty();

		if (!SaveAssetPackage(*Material, OutError))
		{
			return nullptr;
		}

		return Material;
	}

	UMaterialInstanceConstant* CreateOrUpdateMaterialInstance(
		const FString& InPackagePath,
		UMaterialInterface& InParent,
		const float InMetallic,
		const float InRoughness,
		const float InSpecular,
		FString& OutError)
	{
		UMaterialInstanceConstant* MaterialInstance = LoadExistingAsset<UMaterialInstanceConstant>(InPackagePath);
		if (MaterialInstance == nullptr)
		{
			UPackage* const Package = CreatePackage(*InPackagePath);
			UMaterialInstanceConstantFactoryNew* const Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
			Factory->InitialParent = &InParent;
			MaterialInstance = Cast<UMaterialInstanceConstant>(Factory->FactoryCreateNew(
				UMaterialInstanceConstant::StaticClass(),
				Package,
				*GetAssetNameFromPackagePath(InPackagePath),
				RF_Public | RF_Standalone,
				nullptr,
				GWarn));
			if (MaterialInstance == nullptr)
			{
				OutError = FString::Printf(TEXT("Failed to create material instance %s."), *InPackagePath);
				return nullptr;
			}

			FAssetRegistryModule::AssetCreated(MaterialInstance);
		}

		MaterialInstance->SetParentEditorOnly(&InParent);
		MaterialInstance->SetScalarParameterValueEditorOnly(TEXT("Metallic"), InMetallic);
		MaterialInstance->SetScalarParameterValueEditorOnly(TEXT("Roughness"), InRoughness);
		MaterialInstance->SetScalarParameterValueEditorOnly(TEXT("Specular"), InSpecular);
		MaterialInstance->PostEditChange();
		MaterialInstance->MarkPackageDirty();

		if (!SaveAssetPackage(*MaterialInstance, OutError))
		{
			return nullptr;
		}

		return MaterialInstance;
	}

	bool CreateOrUpdateStaticMeshAsset(
		const FString& InPackagePath,
		UDynamicMesh& InGeneratedMesh,
		UMaterialInterface& InAssignedMaterial,
		const FName InMaterialSlotName,
		FString& OutError)
	{
		UStaticMesh* StaticMesh = LoadExistingAsset<UStaticMesh>(InPackagePath);
		if (StaticMesh == nullptr)
		{
			FGeometryScriptCreateNewStaticMeshAssetOptions Options;
			Options.bEnableCollision = false;
			EGeometryScriptOutcomePins Outcome = EGeometryScriptOutcomePins::Failure;
			StaticMesh = UGeometryScriptLibrary_CreateNewAssetFunctions::CreateNewStaticMeshAssetFromMesh(
				&InGeneratedMesh,
				InPackagePath,
				Options,
				Outcome);
			if (StaticMesh == nullptr || Outcome != EGeometryScriptOutcomePins::Success)
			{
				OutError = FString::Printf(TEXT("Failed to create static mesh asset %s."), *InPackagePath);
				return false;
			}
		}

		FGeometryScriptCopyMeshToAssetOptions CopyOptions;
		CopyOptions.bEmitTransaction = false;
		CopyOptions.bReplaceMaterials = true;
		CopyOptions.GenerateLightmapUVs = EGeometryScriptGenerateLightmapUVOptions::DoNotGenerateLightmapUVs;
		CopyOptions.NewMaterials = { &InAssignedMaterial };
		CopyOptions.NewMaterialSlotNames = { InMaterialSlotName };

		FGeometryScriptMeshWriteLOD TargetLod;
		TargetLod.bWriteHiResSource = false;
		TargetLod.LODIndex = 0;

		EGeometryScriptOutcomePins Outcome = EGeometryScriptOutcomePins::Failure;
		UGeometryScriptLibrary_StaticMeshFunctions::CopyMeshToStaticMesh(
			&InGeneratedMesh,
			StaticMesh,
			CopyOptions,
			TargetLod,
			Outcome);
		if (Outcome != EGeometryScriptOutcomePins::Success)
		{
			OutError = FString::Printf(TEXT("Failed to update static mesh asset %s."), *InPackagePath);
			return false;
		}

		StaticMesh->MarkPackageDirty();
		return SaveAssetPackage(*StaticMesh, OutError);
	}
}

bool FCodexInvenDoorPlateAssetGenerator::GenerateAssets(FString& OutMessage)
{
	FScopedSlowTask SlowTask(4.0f, FText::FromString(TEXT("Generating door and pressure plate prototype assets...")));
	SlowTask.MakeDialog(false);

	FString ErrorMessage;

	SlowTask.EnterProgressFrame(1.0f, FText::FromString(TEXT("Preparing vertex color materials")));
	UMaterial* const BaseMaterial = CreateOrUpdateVertexColorMaterial(ErrorMessage);
	if (BaseMaterial == nullptr)
	{
		OutMessage = ErrorMessage;
		return false;
	}

	UMaterialInstanceConstant* const DoorMaterial = CreateOrUpdateMaterialInstance(
		DoorMaterialInstancePackagePath,
		*BaseMaterial,
		0.0f,
		0.78f,
		0.34f,
		ErrorMessage);
	if (DoorMaterial == nullptr)
	{
		OutMessage = ErrorMessage;
		return false;
	}

	UMaterialInstanceConstant* const PressurePlateMaterial = CreateOrUpdateMaterialInstance(
		PressurePlateMaterialInstancePackagePath,
		*BaseMaterial,
		0.16f,
		0.42f,
		0.58f,
		ErrorMessage);
	if (PressurePlateMaterial == nullptr)
	{
		OutMessage = ErrorMessage;
		return false;
	}

	SlowTask.EnterProgressFrame(1.0f, FText::FromString(TEXT("Building door prototype mesh")));
	UDynamicMesh* const DoorMesh = BuildDoorMesh();
	if (DoorMesh == nullptr)
	{
		OutMessage = TEXT("Failed to build the door prototype mesh.");
		return false;
	}

	if (!CreateOrUpdateStaticMeshAsset(DoorMeshPackagePath, *DoorMesh, *DoorMaterial, TEXT("DoorVertexColor"), ErrorMessage))
	{
		OutMessage = ErrorMessage;
		return false;
	}

	SlowTask.EnterProgressFrame(1.0f, FText::FromString(TEXT("Building pressure plate prototype mesh")));
	UDynamicMesh* const PressurePlateMesh = BuildPressurePlateMesh();
	if (PressurePlateMesh == nullptr)
	{
		OutMessage = TEXT("Failed to build the pressure plate prototype mesh.");
		return false;
	}

	if (!CreateOrUpdateStaticMeshAsset(PressurePlateMeshPackagePath, *PressurePlateMesh, *PressurePlateMaterial, TEXT("PressurePlateVertexColor"), ErrorMessage))
	{
		OutMessage = ErrorMessage;
		return false;
	}

	SlowTask.EnterProgressFrame(1.0f, FText::FromString(TEXT("Finalizing generated interactable art assets")));
	OutMessage = FString::Printf(
		TEXT("Door/plate prototype assets are ready. Meshes: [%s, %s], Materials: [%s, %s, %s]"),
		DoorMeshPackagePath,
		PressurePlateMeshPackagePath,
		BaseMaterialPackagePath,
		DoorMaterialInstancePackagePath,
		PressurePlateMaterialInstancePackagePath);
	return true;
}
