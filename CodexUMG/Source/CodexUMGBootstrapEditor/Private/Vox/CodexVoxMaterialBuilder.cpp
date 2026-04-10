#include "Vox/CodexVoxMaterialBuilder.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Factories/MaterialFactoryNew.h"
#include "MaterialEditingLibrary.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialExpressionVertexColor.h"
#include "Misc/PackageName.h"
#include "Modules/ModuleManager.h"
#include "UObject/Package.h"
#include "Vox/CodexVoxTypes.h"

bool FCodexVoxMaterialBuilder::CreateOrUpdateSharedMaterial(UMaterial*& OutMaterial, FString& OutError)
{
	const FString PackageName = FString::Printf(TEXT("%s/%s"), *CodexVox::MaterialPackagePath, *CodexVox::MaterialAssetName);
	const FString ObjectPath = CodexVox::MakeObjectPath(CodexVox::MaterialPackagePath, CodexVox::MaterialAssetName);

	UPackage* Package = CreatePackage(*PackageName);
	Package->FullyLoad();

	OutMaterial = LoadObject<UMaterial>(nullptr, *ObjectPath);
	const bool bIsNewMaterial = OutMaterial == nullptr;
	if (bIsNewMaterial)
	{
		UMaterialFactoryNew* Factory = NewObject<UMaterialFactoryNew>();
		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
		OutMaterial = Cast<UMaterial>(AssetToolsModule.Get().CreateAsset(
			CodexVox::MaterialAssetName,
			CodexVox::MaterialPackagePath,
			UMaterial::StaticClass(),
			Factory));

		if (OutMaterial == nullptr)
		{
			OutError = FString::Printf(TEXT("Failed to create shared material '%s'."), *ObjectPath);
			return false;
		}
	}

	OutMaterial->Modify();
	OutMaterial->BlendMode = BLEND_Opaque;
	UMaterialEditingLibrary::DeleteAllMaterialExpressions(OutMaterial);

	UMaterialExpressionVertexColor* VertexColorExpression = Cast<UMaterialExpressionVertexColor>(
		UMaterialEditingLibrary::CreateMaterialExpression(
			OutMaterial,
			UMaterialExpressionVertexColor::StaticClass(),
			-600,
			0));
	UMaterialExpressionCustom* VertexColorDecodeExpression = Cast<UMaterialExpressionCustom>(
		UMaterialEditingLibrary::CreateMaterialExpression(
			OutMaterial,
			UMaterialExpressionCustom::StaticClass(),
			-260,
			0));
	UMaterialExpressionConstant* RoughnessExpression = Cast<UMaterialExpressionConstant>(
		UMaterialEditingLibrary::CreateMaterialExpression(
			OutMaterial,
			UMaterialExpressionConstant::StaticClass(),
			-400,
			220));
	UMaterialExpressionConstant* SpecularExpression = Cast<UMaterialExpressionConstant>(
		UMaterialEditingLibrary::CreateMaterialExpression(
			OutMaterial,
			UMaterialExpressionConstant::StaticClass(),
			-400,
			360));

	if (VertexColorExpression == nullptr || VertexColorDecodeExpression == nullptr || RoughnessExpression == nullptr || SpecularExpression == nullptr)
	{
		OutError = FString::Printf(TEXT("Failed to author material graph for '%s'."), *ObjectPath);
		return false;
	}

	VertexColorDecodeExpression->Description = TEXT("Decode vertex-color bytes from sRGB to linear");
	VertexColorDecodeExpression->Code = TEXT("float3 low = InColor * (1.0 / 12.92);\nfloat3 high = pow(abs(InColor) * (1.0 / 1.055) + 0.0521327, 2.4);\nreturn lerp(low, high, step(0.04045, InColor));");
	VertexColorDecodeExpression->OutputType = CMOT_Float3;
	VertexColorDecodeExpression->Inputs.Reset();
	FCustomInput& DecodeInput = VertexColorDecodeExpression->Inputs.AddDefaulted_GetRef();
	DecodeInput.InputName = TEXT("InColor");
#if WITH_EDITOR
	VertexColorDecodeExpression->RebuildOutputs();
#endif

	RoughnessExpression->R = 1.0f;
	SpecularExpression->R = 0.0f;

	UMaterialEditingLibrary::ConnectMaterialExpressions(VertexColorExpression, TEXT(""), VertexColorDecodeExpression, TEXT("InColor"));
	UMaterialEditingLibrary::ConnectMaterialProperty(VertexColorDecodeExpression, TEXT(""), MP_BaseColor);
	UMaterialEditingLibrary::ConnectMaterialProperty(RoughnessExpression, TEXT(""), MP_Roughness);
	UMaterialEditingLibrary::ConnectMaterialProperty(SpecularExpression, TEXT(""), MP_Specular);
	UMaterialEditingLibrary::LayoutMaterialExpressions(OutMaterial);
	UMaterialEditingLibrary::RecompileMaterial(OutMaterial);

	OutMaterial->MarkPackageDirty();
	Package->MarkPackageDirty();

	if (bIsNewMaterial)
	{
		FAssetRegistryModule::AssetCreated(OutMaterial);
	}

	return true;
}
