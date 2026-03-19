#include "FarmAnimals/CodexInvenFarmAnimalAssetGenerator.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "FarmAnimals/CodexInvenFarmAnimalBuilders.h"
#include "Engine/StaticMesh.h"
#include "Factories/MaterialFactoryNew.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "GeometryScript/CreateNewAssetUtilityFunctions.h"
#include "GeometryScript/MeshAssetFunctions.h"
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

namespace
{
	const TCHAR* BaseMaterialPackagePath = TEXT("/Game/TestGenerated/FarmAnimals/Materials/M_FarmAnimalVertexColor");
	const TCHAR* RetroMaterialInstancePackagePath = TEXT("/Game/TestGenerated/FarmAnimals/Materials/MI_FarmAnimalVertexColor_Retro");
	const TCHAR* RefinedMaterialInstancePackagePath = TEXT("/Game/TestGenerated/FarmAnimals/Materials/MI_FarmAnimalVertexColor_Refined");
	const TCHAR* ToyMaterialInstancePackagePath = TEXT("/Game/TestGenerated/FarmAnimals/Materials/MI_FarmAnimalVertexColor_Toy");

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
				OutError = TEXT("Failed to create the farm animal vertex color material.");
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
		Roughness->DefaultValue = 0.62f;
		Roughness->MaterialExpressionEditorX = -420;
		Roughness->MaterialExpressionEditorY = 160;
		Material->GetExpressionCollection().AddExpression(Roughness);
		EditorOnlyData->Roughness.Expression = Roughness;

		UMaterialExpressionScalarParameter* const Specular = NewObject<UMaterialExpressionScalarParameter>(Material);
		Specular->ParameterName = TEXT("Specular");
		Specular->DefaultValue = 0.45f;
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

	UMaterialInterface* GetStyleMaterial(
		const ECodexInvenFarmAnimalStyle InStyle,
		UMaterialInstanceConstant& InRetroMaterial,
		UMaterialInstanceConstant& InRefinedMaterial,
		UMaterialInstanceConstant& InToyMaterial)
	{
		switch (InStyle)
		{
		case ECodexInvenFarmAnimalStyle::Retro:
			return &InRetroMaterial;
		case ECodexInvenFarmAnimalStyle::Refined:
			return &InRefinedMaterial;
		case ECodexInvenFarmAnimalStyle::Toy:
			return &InToyMaterial;
		}

		return &InRefinedMaterial;
	}

	bool CreateOrUpdateStaticMeshAsset(
		const ECodexInvenFarmAnimalType InAnimalType,
		const ECodexInvenFarmAnimalStyle InStyle,
		UMaterialInterface& InAssignedMaterial,
		FString& OutError)
	{
		UDynamicMesh* const GeneratedMesh = CodexInvenFarmAnimals::BuildAnimalMesh(InAnimalType, InStyle);
		if (GeneratedMesh == nullptr)
		{
			OutError = TEXT("Failed to build the farm animal mesh.");
			return false;
		}

		const FString PackagePath = CodexInvenFarmAnimals::GetMeshPackagePath(InAnimalType, InStyle);
		UStaticMesh* StaticMesh = LoadExistingAsset<UStaticMesh>(PackagePath);
		if (StaticMesh == nullptr)
		{
			FGeometryScriptCreateNewStaticMeshAssetOptions Options;
			Options.bEnableCollision = false;
			EGeometryScriptOutcomePins Outcome = EGeometryScriptOutcomePins::Failure;
			StaticMesh = UGeometryScriptLibrary_CreateNewAssetFunctions::CreateNewStaticMeshAssetFromMesh(
				GeneratedMesh,
				PackagePath,
				Options,
				Outcome);
			if (StaticMesh == nullptr || Outcome != EGeometryScriptOutcomePins::Success)
			{
				OutError = FString::Printf(TEXT("Failed to create static mesh asset %s."), *PackagePath);
				return false;
			}
		}

		FGeometryScriptCopyMeshToAssetOptions CopyOptions;
		CopyOptions.bEmitTransaction = false;
		CopyOptions.bReplaceMaterials = true;
		CopyOptions.GenerateLightmapUVs = EGeometryScriptGenerateLightmapUVOptions::DoNotGenerateLightmapUVs;
		CopyOptions.NewMaterials = { &InAssignedMaterial };
		CopyOptions.NewMaterialSlotNames = { TEXT("FarmAnimalVertexColor") };

		FGeometryScriptMeshWriteLOD TargetLod;
		TargetLod.bWriteHiResSource = false;
		TargetLod.LODIndex = 0;

		EGeometryScriptOutcomePins Outcome = EGeometryScriptOutcomePins::Failure;
		UGeometryScriptLibrary_StaticMeshFunctions::CopyMeshToStaticMesh(
			GeneratedMesh,
			StaticMesh,
			CopyOptions,
			TargetLod,
			Outcome);
		if (Outcome != EGeometryScriptOutcomePins::Success)
		{
			OutError = FString::Printf(TEXT("Failed to update static mesh asset %s."), *PackagePath);
			return false;
		}

		StaticMesh->MarkPackageDirty();
		return SaveAssetPackage(*StaticMesh, OutError);
	}
}

bool FCodexInvenFarmAnimalAssetGenerator::GenerateAssets(FString& OutMessage)
{
	const int32 MeshCount = CodexInvenFarmAnimals::GetAllAnimalTypes().Num() * CodexInvenFarmAnimals::GetAllStyles().Num();
	FScopedSlowTask SlowTask(static_cast<float>(MeshCount + 1), FText::FromString(TEXT("Generating test farm animal meshes...")));
	SlowTask.MakeDialog(false);

	FString ErrorMessage;

	SlowTask.EnterProgressFrame(1.0f, FText::FromString(TEXT("Preparing farm animal materials")));
	UMaterial* const BaseMaterial = CreateOrUpdateVertexColorMaterial(ErrorMessage);
	if (BaseMaterial == nullptr)
	{
		OutMessage = ErrorMessage;
		return false;
	}

	UMaterialInstanceConstant* const RetroMaterial = CreateOrUpdateMaterialInstance(
		RetroMaterialInstancePackagePath,
		*BaseMaterial,
		0.0f,
		0.82f,
		0.26f,
		ErrorMessage);
	if (RetroMaterial == nullptr)
	{
		OutMessage = ErrorMessage;
		return false;
	}

	UMaterialInstanceConstant* const RefinedMaterial = CreateOrUpdateMaterialInstance(
		RefinedMaterialInstancePackagePath,
		*BaseMaterial,
		0.0f,
		0.58f,
		0.42f,
		ErrorMessage);
	if (RefinedMaterial == nullptr)
	{
		OutMessage = ErrorMessage;
		return false;
	}

	UMaterialInstanceConstant* const ToyMaterial = CreateOrUpdateMaterialInstance(
		ToyMaterialInstancePackagePath,
		*BaseMaterial,
		0.0f,
		0.34f,
		0.64f,
		ErrorMessage);
	if (ToyMaterial == nullptr)
	{
		OutMessage = ErrorMessage;
		return false;
	}

	int32 GeneratedMeshCount = 0;
	for (const ECodexInvenFarmAnimalStyle Style : CodexInvenFarmAnimals::GetAllStyles())
	{
		for (const ECodexInvenFarmAnimalType AnimalType : CodexInvenFarmAnimals::GetAllAnimalTypes())
		{
			const FText ProgressText = FText::FromString(FString::Printf(
				TEXT("Generating %s %s"),
				*CodexInvenFarmAnimals::GetStyleDisplayName(Style).ToString(),
				*CodexInvenFarmAnimals::GetAnimalDisplayName(AnimalType).ToString()));

			SlowTask.EnterProgressFrame(1.0f, ProgressText);
			UMaterialInterface* const AssignedMaterial = GetStyleMaterial(Style, *RetroMaterial, *RefinedMaterial, *ToyMaterial);
			if (!CreateOrUpdateStaticMeshAsset(AnimalType, Style, *AssignedMaterial, ErrorMessage))
			{
				OutMessage = ErrorMessage;
				return false;
			}

			++GeneratedMeshCount;
		}
	}

	OutMessage = FString::Printf(
		TEXT("Generated farm animal test assets successfully. Meshes: %d, Materials: 4"),
		GeneratedMeshCount);
	return true;
}
