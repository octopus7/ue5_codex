#include "CodexInvenPickupAssetGenerator.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "CodexInvenPickupData.h"
#include "Engine/StaticMesh.h"
#include "Engine/TextureDefines.h"
#include "Engine/Texture2D.h"
#include "Factories/MaterialFactoryNew.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "GeometryScript/CreateNewAssetUtilityFunctions.h"
#include "GeometryScript/GeometryScriptTypes.h"
#include "GeometryScript/MeshAssetFunctions.h"
#include "GeometryScript/MeshBasicEditFunctions.h"
#include "GeometryScript/MeshNormalsFunctions.h"
#include "GeometryScript/MeshPrimitiveFunctions.h"
#include "GeometryScript/MeshVertexColorFunctions.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionVertexColor.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Misc/PackageName.h"
#include "Misc/ScopedSlowTask.h"
#include "TextureCompiler.h"
#include "UDynamicMesh.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/UObjectGlobals.h"

DEFINE_LOG_CATEGORY_STATIC(LogCodexInvenPickupAssetGenerator, Log, All);

namespace
{
	constexpr int32 IconSize = 64;
	const TCHAR* BaseMaterialPackagePath = TEXT("/Game/Art/Pickups/Materials/M_PickupVertexColor");
	const TCHAR* CommonMaterialInstancePackagePath = TEXT("/Game/Art/Pickups/Materials/MI_PickupVertexColor_Common");
	const TCHAR* GoldMaterialInstancePackagePath = TEXT("/Game/Art/Pickups/Materials/MI_PickupVertexColor_Gold");

	struct FPickupPalette
	{
		FLinearColor BodyColor;
		FLinearColor AccentColor;
		FLinearColor DetailColor;
		FLinearColor HighlightColor;
	};

	FString MakeObjectPath(const FString& InPackagePath)
	{
		return InPackagePath + TEXT(".") + FPackageName::GetLongPackageAssetName(InPackagePath);
	}

	FString GetAssetNameFromPackagePath(const FString& InPackagePath)
	{
		return FPackageName::GetLongPackageAssetName(InPackagePath);
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

	template <typename TObjectType>
	TObjectType* LoadExistingAsset(const FString& InPackagePath)
	{
		return LoadObject<TObjectType>(nullptr, *MakeObjectPath(InPackagePath));
	}

	FPickupPalette BuildPalette(const FCodexInvenPickupDefinition& InDefinition)
	{
		FPickupPalette Palette;
		Palette.AccentColor = InDefinition.TintColor;
		Palette.BodyColor = InDefinition.Rarity == ECodexInvenPickupRarity::Gold
			? FLinearColor(0.18f, 0.18f, 0.19f, 1.0f)
			: FLinearColor(0.41f, 0.43f, 0.46f, 1.0f);
		Palette.DetailColor = InDefinition.Rarity == ECodexInvenPickupRarity::Gold
			? FLinearColor(0.30f, 0.26f, 0.12f, 1.0f)
			: FLinearColor(0.16f, 0.17f, 0.19f, 1.0f);
		Palette.HighlightColor = InDefinition.Rarity == ECodexInvenPickupRarity::Gold
			? FLinearColor(0.90f, 0.76f, 0.28f, 1.0f)
			: FLinearColor(0.72f, 0.74f, 0.78f, 1.0f);
		return Palette;
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
		const FLinearColor& InColor,
		const EGeometryScriptPrimitiveOriginMode InOrigin = EGeometryScriptPrimitiveOriginMode::Center)
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
			InOrigin);
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
		const int32 InRadialSteps = 12,
		const EGeometryScriptPrimitiveOriginMode InOrigin = EGeometryScriptPrimitiveOriginMode::Center)
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
			InOrigin);
		UGeometryScriptLibrary_MeshVertexColorFunctions::SetMeshConstantVertexColor(
			PartMesh,
			InColor,
			FGeometryScriptColorFlags(),
			true);
		UGeometryScriptLibrary_MeshBasicEditFunctions::AppendMesh(&InOutMesh, PartMesh, FTransform::Identity, true);
	}

	void AppendConePart(
		UDynamicMesh& InOutMesh,
		const float InBaseRadius,
		const float InTopRadius,
		const float InHeight,
		const FVector& InLocation,
		const FRotator& InRotation,
		const FLinearColor& InColor,
		const int32 InRadialSteps = 12,
		const EGeometryScriptPrimitiveOriginMode InOrigin = EGeometryScriptPrimitiveOriginMode::Center)
	{
		UDynamicMesh* const PartMesh = MakeTransientDynamicMesh();
		UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendCone(
			PartMesh,
			MakePrimitiveOptions(),
			FTransform(InRotation, InLocation),
			InBaseRadius,
			InTopRadius,
			InHeight,
			InRadialSteps,
			0,
			true,
			InOrigin);
		UGeometryScriptLibrary_MeshVertexColorFunctions::SetMeshConstantVertexColor(
			PartMesh,
			InColor,
			FGeometryScriptColorFlags(),
			true);
		UGeometryScriptLibrary_MeshBasicEditFunctions::AppendMesh(&InOutMesh, PartMesh, FTransform::Identity, true);
	}

	UDynamicMesh* BuildPickupMesh(const FCodexInvenPickupDefinition& InDefinition)
	{
		UDynamicMesh* const Mesh = MakeTransientDynamicMesh();
		const FPickupPalette Palette = BuildPalette(InDefinition);

		switch (InDefinition.Type)
		{
		case ECodexInvenPickupType::CylinderRed:
			AppendBoxPart(*Mesh, FVector(68.0f, 42.0f, 26.0f), FVector(0.0f, 0.0f, 13.0f), FRotator::ZeroRotator, Palette.BodyColor);
			AppendBoxPart(*Mesh, FVector(74.0f, 46.0f, 8.0f), FVector(0.0f, 0.0f, 28.0f), FRotator::ZeroRotator, Palette.DetailColor);
			AppendBoxPart(*Mesh, FVector(12.0f, 28.0f, 4.0f), FVector(0.0f, 0.0f, 33.0f), FRotator::ZeroRotator, Palette.AccentColor);
			AppendBoxPart(*Mesh, FVector(28.0f, 12.0f, 4.0f), FVector(0.0f, 0.0f, 33.0f), FRotator::ZeroRotator, Palette.AccentColor);
			AppendBoxPart(*Mesh, FVector(8.0f, 8.0f, 12.0f), FVector(-34.0f, 0.0f, 12.0f), FRotator::ZeroRotator, Palette.HighlightColor);
			AppendBoxPart(*Mesh, FVector(8.0f, 8.0f, 12.0f), FVector(34.0f, 0.0f, 12.0f), FRotator::ZeroRotator, Palette.HighlightColor);
			AppendBoxPart(*Mesh, FVector(74.0f, 8.0f, 6.0f), FVector(0.0f, 0.0f, 27.0f), FRotator::ZeroRotator, Palette.DetailColor);
			break;

		case ECodexInvenPickupType::CylinderGreen:
			AppendCylinderPart(*Mesh, 18.0f, 56.0f, FVector(0.0f, 0.0f, 28.0f), FRotator::ZeroRotator, Palette.BodyColor, 12);
			AppendCylinderPart(*Mesh, 19.0f, 4.0f, FVector(0.0f, 0.0f, 2.0f), FRotator::ZeroRotator, Palette.HighlightColor, 12);
			AppendCylinderPart(*Mesh, 19.0f, 4.0f, FVector(0.0f, 0.0f, 54.0f), FRotator::ZeroRotator, Palette.HighlightColor, 12);
			AppendCylinderPart(*Mesh, 18.5f, 12.0f, FVector(0.0f, 0.0f, 22.0f), FRotator::ZeroRotator, Palette.AccentColor, 12);
			AppendBoxPart(*Mesh, FVector(14.0f, 10.0f, 2.0f), FVector(0.0f, 0.0f, 59.0f), FRotator::ZeroRotator, Palette.DetailColor);
			AppendBoxPart(*Mesh, FVector(6.0f, 10.0f, 2.0f), FVector(4.0f, 0.0f, 61.0f), FRotator::ZeroRotator, Palette.HighlightColor);
			break;

		case ECodexInvenPickupType::CylinderBlue:
			AppendBoxPart(*Mesh, FVector(72.0f, 46.0f, 18.0f), FVector(0.0f, 0.0f, 9.0f), FRotator::ZeroRotator, Palette.BodyColor);
			AppendBoxPart(*Mesh, FVector(74.0f, 10.0f, 10.0f), FVector(0.0f, 18.0f, 22.0f), FRotator::ZeroRotator, Palette.DetailColor);
			AppendBoxPart(*Mesh, FVector(10.0f, 50.0f, 14.0f), FVector(-31.0f, 0.0f, 7.0f), FRotator::ZeroRotator, Palette.DetailColor);
			AppendBoxPart(*Mesh, FVector(10.0f, 50.0f, 14.0f), FVector(31.0f, 0.0f, 7.0f), FRotator::ZeroRotator, Palette.DetailColor);
			AppendBoxPart(*Mesh, FVector(34.0f, 18.0f, 3.0f), FVector(0.0f, 0.0f, 19.5f), FRotator::ZeroRotator, Palette.AccentColor);
			AppendBoxPart(*Mesh, FVector(16.0f, 8.0f, 6.0f), FVector(0.0f, -18.0f, 17.0f), FRotator::ZeroRotator, Palette.HighlightColor);
			AppendBoxPart(*Mesh, FVector(48.0f, 6.0f, 3.0f), FVector(0.0f, 14.0f, 26.5f), FRotator::ZeroRotator, Palette.HighlightColor);
			break;

		case ECodexInvenPickupType::CylinderGold:
			AppendCylinderPart(*Mesh, 18.0f, 42.0f, FVector(0.0f, 0.0f, 21.0f), FRotator::ZeroRotator, Palette.BodyColor, 10);
			AppendConePart(*Mesh, 18.0f, 14.0f, 8.0f, FVector(0.0f, 0.0f, 46.0f), FRotator::ZeroRotator, Palette.DetailColor, 10);
			AppendCylinderPart(*Mesh, 15.0f, 12.0f, FVector(0.0f, 0.0f, 56.0f), FRotator::ZeroRotator, Palette.AccentColor, 10);
			AppendCylinderPart(*Mesh, 18.5f, 10.0f, FVector(0.0f, 0.0f, 18.0f), FRotator::ZeroRotator, Palette.HighlightColor, 10);
			AppendCylinderPart(*Mesh, 12.0f, 2.0f, FVector(0.0f, 0.0f, 63.0f), FRotator::ZeroRotator, Palette.HighlightColor, 10);
			break;

		case ECodexInvenPickupType::CubeRed:
			AppendConePart(*Mesh, 30.0f, 18.0f, 18.0f, FVector(0.0f, 0.0f, 18.0f), FRotator::ZeroRotator, Palette.BodyColor, 12);
			AppendCylinderPart(*Mesh, 25.0f, 10.0f, FVector(0.0f, 0.0f, 31.0f), FRotator::ZeroRotator, Palette.BodyColor, 12);
			AppendBoxPart(*Mesh, FVector(54.0f, 40.0f, 4.0f), FVector(0.0f, 0.0f, 6.0f), FRotator::ZeroRotator, Palette.DetailColor);
			AppendBoxPart(*Mesh, FVector(32.0f, 16.0f, 4.0f), FVector(0.0f, 16.0f, 9.0f), FRotator::ZeroRotator, Palette.AccentColor);
			AppendBoxPart(*Mesh, FVector(10.0f, 18.0f, 14.0f), FVector(-23.0f, 0.0f, 13.0f), FRotator::ZeroRotator, Palette.HighlightColor);
			AppendBoxPart(*Mesh, FVector(10.0f, 18.0f, 14.0f), FVector(23.0f, 0.0f, 13.0f), FRotator::ZeroRotator, Palette.HighlightColor);
			break;

		case ECodexInvenPickupType::CubeGreen:
			AppendBoxPart(*Mesh, FVector(42.0f, 20.0f, 52.0f), FVector(0.0f, -10.0f, 26.0f), FRotator::ZeroRotator, Palette.BodyColor);
			AppendBoxPart(*Mesh, FVector(34.0f, 22.0f, 48.0f), FVector(0.0f, 12.0f, 24.0f), FRotator::ZeroRotator, Palette.DetailColor);
			AppendBoxPart(*Mesh, FVector(12.0f, 10.0f, 16.0f), FVector(-18.0f, -1.0f, 42.0f), FRotator::ZeroRotator, Palette.BodyColor);
			AppendBoxPart(*Mesh, FVector(12.0f, 10.0f, 16.0f), FVector(18.0f, -1.0f, 42.0f), FRotator::ZeroRotator, Palette.BodyColor);
			AppendBoxPart(*Mesh, FVector(8.0f, 18.0f, 38.0f), FVector(-25.0f, 0.0f, 19.0f), FRotator::ZeroRotator, Palette.HighlightColor);
			AppendBoxPart(*Mesh, FVector(8.0f, 18.0f, 38.0f), FVector(25.0f, 0.0f, 19.0f), FRotator::ZeroRotator, Palette.HighlightColor);
			AppendBoxPart(*Mesh, FVector(10.0f, 8.0f, 18.0f), FVector(-12.0f, 22.0f, 14.0f), FRotator::ZeroRotator, Palette.AccentColor);
			AppendBoxPart(*Mesh, FVector(10.0f, 8.0f, 18.0f), FVector(0.0f, 22.0f, 14.0f), FRotator::ZeroRotator, Palette.AccentColor);
			AppendBoxPart(*Mesh, FVector(10.0f, 8.0f, 18.0f), FVector(12.0f, 22.0f, 14.0f), FRotator::ZeroRotator, Palette.AccentColor);
			break;

		case ECodexInvenPickupType::CubeBlue:
			AppendBoxPart(*Mesh, FVector(44.0f, 28.0f, 56.0f), FVector(0.0f, -2.0f, 28.0f), FRotator::ZeroRotator, Palette.BodyColor);
			AppendBoxPart(*Mesh, FVector(34.0f, 12.0f, 20.0f), FVector(0.0f, 18.0f, 14.0f), FRotator::ZeroRotator, Palette.AccentColor);
			AppendBoxPart(*Mesh, FVector(44.0f, 18.0f, 12.0f), FVector(0.0f, 4.0f, 55.0f), FRotator::ZeroRotator, Palette.DetailColor);
			AppendBoxPart(*Mesh, FVector(10.0f, 16.0f, 24.0f), FVector(-25.0f, 4.0f, 18.0f), FRotator::ZeroRotator, Palette.HighlightColor);
			AppendBoxPart(*Mesh, FVector(10.0f, 16.0f, 24.0f), FVector(25.0f, 4.0f, 18.0f), FRotator::ZeroRotator, Palette.HighlightColor);
			AppendBoxPart(*Mesh, FVector(8.0f, 6.0f, 18.0f), FVector(-14.0f, -18.0f, 34.0f), FRotator::ZeroRotator, Palette.DetailColor);
			AppendBoxPart(*Mesh, FVector(8.0f, 6.0f, 18.0f), FVector(14.0f, -18.0f, 34.0f), FRotator::ZeroRotator, Palette.DetailColor);
			AppendBoxPart(*Mesh, FVector(44.0f, 8.0f, 8.0f), FVector(0.0f, -14.0f, 4.0f), FRotator::ZeroRotator, Palette.HighlightColor);
			break;

		case ECodexInvenPickupType::CubeGold:
			AppendBoxPart(*Mesh, FVector(64.0f, 40.0f, 20.0f), FVector(0.0f, 0.0f, 10.0f), FRotator::ZeroRotator, Palette.BodyColor);
			AppendBoxPart(*Mesh, FVector(64.0f, 40.0f, 8.0f), FVector(0.0f, 0.0f, 24.0f), FRotator::ZeroRotator, Palette.DetailColor);
			AppendBoxPart(*Mesh, FVector(8.0f, 8.0f, 12.0f), FVector(-26.0f, -14.0f, 10.0f), FRotator::ZeroRotator, Palette.HighlightColor);
			AppendBoxPart(*Mesh, FVector(8.0f, 8.0f, 12.0f), FVector(26.0f, -14.0f, 10.0f), FRotator::ZeroRotator, Palette.HighlightColor);
			AppendBoxPart(*Mesh, FVector(8.0f, 8.0f, 12.0f), FVector(-26.0f, 14.0f, 10.0f), FRotator::ZeroRotator, Palette.HighlightColor);
			AppendBoxPart(*Mesh, FVector(8.0f, 8.0f, 12.0f), FVector(26.0f, 14.0f, 10.0f), FRotator::ZeroRotator, Palette.HighlightColor);
			AppendCylinderPart(*Mesh, 3.5f, 18.0f, FVector(-10.0f, 0.0f, 31.0f), FRotator(90.0f, 0.0f, 0.0f), Palette.HighlightColor, 10);
			AppendCylinderPart(*Mesh, 3.5f, 18.0f, FVector(10.0f, 0.0f, 31.0f), FRotator(90.0f, 0.0f, 0.0f), Palette.HighlightColor, 10);
			AppendCylinderPart(*Mesh, 3.5f, 26.0f, FVector(0.0f, 0.0f, 35.0f), FRotator(0.0f, 90.0f, 0.0f), Palette.AccentColor, 10);
			AppendBoxPart(*Mesh, FVector(12.0f, 6.0f, 6.0f), FVector(-18.0f, 22.0f, 18.0f), FRotator::ZeroRotator, Palette.AccentColor);
			AppendBoxPart(*Mesh, FVector(12.0f, 6.0f, 6.0f), FVector(18.0f, 22.0f, 18.0f), FRotator::ZeroRotator, Palette.AccentColor);
			break;
		}

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
				OutError = TEXT("Failed to create the pickup vertex color material.");
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
		Roughness->DefaultValue = 0.58f;
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

	void SetPixel(TArray64<uint8>& InOutPixels, const int32 InX, const int32 InY, const FColor& InColor)
	{
		if (InX < 0 || InX >= IconSize || InY < 0 || InY >= IconSize)
		{
			return;
		}

		const int64 PixelIndex = ((static_cast<int64>(InY) * IconSize) + InX) * 4;
		InOutPixels[PixelIndex + 0] = InColor.B;
		InOutPixels[PixelIndex + 1] = InColor.G;
		InOutPixels[PixelIndex + 2] = InColor.R;
		InOutPixels[PixelIndex + 3] = InColor.A;
	}

	void FillRect(TArray64<uint8>& InOutPixels, const int32 InMinX, const int32 InMinY, const int32 InMaxX, const int32 InMaxY, const FColor& InColor)
	{
		for (int32 Y = InMinY; Y <= InMaxY; ++Y)
		{
			for (int32 X = InMinX; X <= InMaxX; ++X)
			{
				SetPixel(InOutPixels, X, Y, InColor);
			}
		}
	}

	void FillRoundedRect(TArray64<uint8>& InOutPixels, const int32 InMinX, const int32 InMinY, const int32 InMaxX, const int32 InMaxY, const int32 InRadius, const FColor& InColor)
	{
		const int32 RadiusSquared = InRadius * InRadius;
		for (int32 Y = InMinY; Y <= InMaxY; ++Y)
		{
			for (int32 X = InMinX; X <= InMaxX; ++X)
			{
				const int32 ClampedX = FMath::Clamp(X, InMinX + InRadius, InMaxX - InRadius);
				const int32 ClampedY = FMath::Clamp(Y, InMinY + InRadius, InMaxY - InRadius);
				const int32 DeltaX = X - ClampedX;
				const int32 DeltaY = Y - ClampedY;
				if ((DeltaX * DeltaX) + (DeltaY * DeltaY) <= RadiusSquared)
				{
					SetPixel(InOutPixels, X, Y, InColor);
				}
			}
		}
	}

	void FillEllipse(TArray64<uint8>& InOutPixels, const int32 InCenterX, const int32 InCenterY, const int32 InRadiusX, const int32 InRadiusY, const FColor& InColor)
	{
		const float RadiusXSquared = static_cast<float>(InRadiusX * InRadiusX);
		const float RadiusYSquared = static_cast<float>(InRadiusY * InRadiusY);
		for (int32 Y = InCenterY - InRadiusY; Y <= InCenterY + InRadiusY; ++Y)
		{
			for (int32 X = InCenterX - InRadiusX; X <= InCenterX + InRadiusX; ++X)
			{
				const float NormalizedX = static_cast<float>((X - InCenterX) * (X - InCenterX)) / RadiusXSquared;
				const float NormalizedY = static_cast<float>((Y - InCenterY) * (Y - InCenterY)) / RadiusYSquared;
				if (NormalizedX + NormalizedY <= 1.0f)
				{
					SetPixel(InOutPixels, X, Y, InColor);
				}
			}
		}
	}

	void BuildInventoryIconPixels(const FCodexInvenPickupDefinition& InDefinition, TArray64<uint8>& OutPixels)
	{
		OutPixels.Init(0, static_cast<int64>(IconSize) * IconSize * 4);

		const FPickupPalette Palette = BuildPalette(InDefinition);
		const FColor Body = Palette.BodyColor.ToFColorSRGB();
		const FColor Accent = Palette.AccentColor.ToFColorSRGB();
		const FColor Detail = Palette.DetailColor.ToFColorSRGB();
		const FColor Highlight = Palette.HighlightColor.ToFColorSRGB();

		switch (InDefinition.Type)
		{
		case ECodexInvenPickupType::CylinderRed:
			FillRoundedRect(OutPixels, 14, 18, 50, 44, 6, Body);
			FillRoundedRect(OutPixels, 12, 10, 52, 20, 4, Detail);
			FillRect(OutPixels, 28, 22, 36, 40, Accent);
			FillRect(OutPixels, 22, 28, 42, 34, Accent);
			break;

		case ECodexInvenPickupType::CylinderGreen:
			FillRoundedRect(OutPixels, 24, 8, 40, 56, 8, Body);
			FillRect(OutPixels, 21, 24, 43, 34, Accent);
			FillRect(OutPixels, 26, 10, 38, 14, Highlight);
			break;

		case ECodexInvenPickupType::CylinderBlue:
			FillRoundedRect(OutPixels, 12, 18, 52, 44, 5, Body);
			FillRoundedRect(OutPixels, 10, 12, 54, 20, 4, Detail);
			FillRoundedRect(OutPixels, 20, 24, 44, 36, 3, Accent);
			break;

		case ECodexInvenPickupType::CylinderGold:
			FillRoundedRect(OutPixels, 22, 18, 42, 54, 6, Body);
			FillRoundedRect(OutPixels, 24, 10, 40, 20, 3, Accent);
			FillRect(OutPixels, 20, 32, 44, 38, Highlight);
			break;

		case ECodexInvenPickupType::CubeRed:
			FillEllipse(OutPixels, 32, 30, 18, 14, Body);
			FillRect(OutPixels, 20, 32, 44, 42, Detail);
			FillRect(OutPixels, 24, 36, 40, 42, Accent);
			break;

		case ECodexInvenPickupType::CubeGreen:
			FillRoundedRect(OutPixels, 18, 14, 46, 50, 4, Body);
			FillRect(OutPixels, 28, 14, 36, 24, FColor(0, 0, 0, 0));
			FillRect(OutPixels, 22, 28, 42, 46, Detail);
			FillRect(OutPixels, 24, 20, 40, 26, Accent);
			break;

		case ECodexInvenPickupType::CubeBlue:
			FillRoundedRect(OutPixels, 16, 14, 48, 50, 5, Body);
			FillRoundedRect(OutPixels, 18, 10, 46, 20, 4, Detail);
			FillRoundedRect(OutPixels, 20, 24, 44, 36, 3, Accent);
			FillRect(OutPixels, 10, 22, 16, 42, Highlight);
			FillRect(OutPixels, 48, 22, 54, 42, Highlight);
			break;

		case ECodexInvenPickupType::CubeGold:
			FillRoundedRect(OutPixels, 12, 20, 52, 46, 4, Body);
			FillRect(OutPixels, 12, 20, 52, 26, Detail);
			FillRect(OutPixels, 24, 10, 40, 18, Accent);
			FillRect(OutPixels, 18, 28, 24, 34, Highlight);
			FillRect(OutPixels, 40, 28, 46, 34, Highlight);
			break;
		}
	}

	UTexture2D* CreateOrUpdateIconTexture(const FCodexInvenPickupDefinition& InDefinition, FString& OutError)
	{
		const FString PackagePath = CodexInvenPickupData::GetPickupIconPackagePath(InDefinition.Type);
		UTexture2D* Texture = LoadExistingAsset<UTexture2D>(PackagePath);
		if (Texture == nullptr)
		{
			UPackage* const Package = CreatePackage(*PackagePath);
			Texture = NewObject<UTexture2D>(Package, *GetAssetNameFromPackagePath(PackagePath), RF_Public | RF_Standalone);
			if (Texture == nullptr)
			{
				OutError = FString::Printf(TEXT("Failed to create icon texture %s."), *PackagePath);
				return nullptr;
			}

			FAssetRegistryModule::AssetCreated(Texture);
		}

		TArray64<uint8> PixelData;
		BuildInventoryIconPixels(InDefinition, PixelData);

		Texture->MipGenSettings = TMGS_NoMipmaps;
		Texture->NeverStream = true;
		Texture->SRGB = true;
		Texture->LODGroup = TEXTUREGROUP_UI;
		Texture->CompressionSettings = TC_EditorIcon;
		Texture->Filter = TF_Bilinear;
		Texture->Source.Init(IconSize, IconSize, 1, 1, TSF_BGRA8, PixelData.GetData());
		Texture->UpdateResource();
		Texture->PostEditChange();
		FTextureCompilingManager::Get().FinishCompilation({ Texture });
		Texture->MarkPackageDirty();

		if (!SaveAssetPackage(*Texture, OutError))
		{
			return nullptr;
		}

		return Texture;
	}

	bool CreateOrUpdateStaticMeshAsset(
		const FCodexInvenPickupDefinition& InDefinition,
		UMaterialInterface& InAssignedMaterial,
		FString& OutError)
	{
		UDynamicMesh* const GeneratedMesh = BuildPickupMesh(InDefinition);
		if (GeneratedMesh == nullptr)
		{
			OutError = FString::Printf(TEXT("Failed to build the mesh for %s."), *InDefinition.DisplayName);
			return false;
		}

		UStaticMesh* StaticMesh = LoadExistingAsset<UStaticMesh>(CodexInvenPickupData::GetPickupMeshPackagePath(InDefinition.Type));
		if (StaticMesh == nullptr)
		{
			FGeometryScriptCreateNewStaticMeshAssetOptions Options;
			Options.bEnableCollision = false;
			EGeometryScriptOutcomePins Outcome = EGeometryScriptOutcomePins::Failure;
			StaticMesh = UGeometryScriptLibrary_CreateNewAssetFunctions::CreateNewStaticMeshAssetFromMesh(
				GeneratedMesh,
				CodexInvenPickupData::GetPickupMeshPackagePath(InDefinition.Type),
				Options,
				Outcome);
			if (StaticMesh == nullptr || Outcome != EGeometryScriptOutcomePins::Success)
			{
				OutError = FString::Printf(TEXT("Failed to create static mesh asset for %s."), *InDefinition.DisplayName);
				return false;
			}
		}

		FGeometryScriptCopyMeshToAssetOptions CopyOptions;
		CopyOptions.bEmitTransaction = false;
		CopyOptions.bReplaceMaterials = true;
		CopyOptions.GenerateLightmapUVs = EGeometryScriptGenerateLightmapUVOptions::DoNotGenerateLightmapUVs;
		CopyOptions.NewMaterials = { &InAssignedMaterial };
		CopyOptions.NewMaterialSlotNames = { TEXT("PickupVertexColor") };

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
			OutError = FString::Printf(TEXT("Failed to update static mesh asset for %s."), *InDefinition.DisplayName);
			return false;
		}

		StaticMesh->MarkPackageDirty();
		return SaveAssetPackage(*StaticMesh, OutError);
	}
}

bool FCodexInvenPickupAssetGenerator::GenerateAssets(FString& OutMessage)
{
	FScopedSlowTask SlowTask(11.0f, FText::FromString(TEXT("Generating pickup assets...")));
	SlowTask.MakeDialog(false);

	FString ErrorMessage;

	SlowTask.EnterProgressFrame(1.0f, FText::FromString(TEXT("Preparing materials")));
	UMaterial* const BaseMaterial = CreateOrUpdateVertexColorMaterial(ErrorMessage);
	if (BaseMaterial == nullptr)
	{
		OutMessage = ErrorMessage;
		return false;
	}

	UMaterialInstanceConstant* const CommonMaterial = CreateOrUpdateMaterialInstance(
		CommonMaterialInstancePackagePath,
		*BaseMaterial,
		0.0f,
		0.62f,
		0.45f,
		ErrorMessage);
	if (CommonMaterial == nullptr)
	{
		OutMessage = ErrorMessage;
		return false;
	}

	UMaterialInstanceConstant* const GoldMaterial = CreateOrUpdateMaterialInstance(
		GoldMaterialInstancePackagePath,
		*BaseMaterial,
		0.78f,
		0.28f,
		0.62f,
		ErrorMessage);
	if (GoldMaterial == nullptr)
	{
		OutMessage = ErrorMessage;
		return false;
	}

	int32 GeneratedMeshCount = 0;
	int32 GeneratedIconCount = 0;
	for (const ECodexInvenPickupType PickupType : CodexInvenPickupData::GetAllPickupTypes())
	{
		const FCodexInvenPickupDefinition& Definition = CodexInvenPickupData::GetPickupDefinitionChecked(PickupType);

		SlowTask.EnterProgressFrame(1.0f, FText::FromString(FString::Printf(TEXT("Generating %s"), *Definition.DisplayName)));
		if (CreateOrUpdateIconTexture(Definition, ErrorMessage) == nullptr)
		{
			OutMessage = ErrorMessage;
			return false;
		}
		++GeneratedIconCount;

		UMaterialInterface* const AssignedMaterial =
			Definition.Rarity == ECodexInvenPickupRarity::Gold
				? static_cast<UMaterialInterface*>(GoldMaterial)
				: static_cast<UMaterialInterface*>(CommonMaterial);

		if (!CreateOrUpdateStaticMeshAsset(Definition, *AssignedMaterial, ErrorMessage))
		{
			OutMessage = ErrorMessage;
			return false;
		}
		++GeneratedMeshCount;
	}

	OutMessage = FString::Printf(
		TEXT("Generated pickup assets successfully. Meshes: %d, Icons: %d, Materials: 3"),
		GeneratedMeshCount,
		GeneratedIconCount);
	return true;
}
