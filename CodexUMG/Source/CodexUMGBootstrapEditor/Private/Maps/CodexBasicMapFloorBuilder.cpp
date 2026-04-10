#include "Maps/CodexBasicMapFloorBuilder.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Components/StaticMeshComponent.h"
#include "Editor.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "Factories/MaterialFactoryNew.h"
#include "Factories/Texture2dFactoryNew.h"
#include "FileHelpers.h"
#include "HAL/FileManager.h"
#include "IImageWrapperModule.h"
#include "MaterialEditingLibrary.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionDDX.h"
#include "Materials/MaterialExpressionDDY.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionMaterialFunctionCall.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialFunctionInterface.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Subsystems/EditorAssetSubsystem.h"
#include "UObject/Package.h"

namespace
{
	static const TCHAR* const BasicMapAssetPath = TEXT("/Game/Maps/BasicMap");
	static const TCHAR* const FloorActorLabel = TEXT("Floor");
	static const TCHAR* const MaterialsPath = TEXT("/Game/Materials");
	static const TCHAR* const MaterialAssetName = TEXT("M_BasicMapFloor_StylizedGrassDirt01");
	static const TCHAR* const SourceTextureRelativePath = TEXT("SourceArt/T_Stylized_Grass_Dirt_01.png");
	static const TCHAR* const TextureAssetName = TEXT("T_Stylized_Grass_Dirt_01");
	static const TCHAR* const TextureVariationFunctionPath = TEXT("/Engine/Functions/Engine_MaterialFunctions03/Texturing/TextureVariation.TextureVariation");

	template <typename AssetType>
	AssetType* LoadAsset(const FString& ObjectPath)
	{
		return LoadObject<AssetType>(nullptr, *ObjectPath);
	}

	FString MakeObjectPath(const FString& PackagePath, const FString& AssetName)
	{
		return FString::Printf(TEXT("%s/%s.%s"), *PackagePath, *AssetName, *AssetName);
	}

	FString ResolveSourceTexturePath()
	{
		return FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / SourceTextureRelativePath);
	}

	UTexture2D* CreateTextureAsset()
	{
		const FString ObjectPath = MakeObjectPath(MaterialsPath, TextureAssetName);
		if (UTexture2D* ExistingTexture = LoadAsset<UTexture2D>(ObjectPath))
		{
			return ExistingTexture;
		}

		UTexture2DFactoryNew* TextureFactory = NewObject<UTexture2DFactoryNew>();
		TextureFactory->bEditAfterNew = false;

		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
		return Cast<UTexture2D>(AssetToolsModule.Get().CreateAsset(
			TextureAssetName,
			MaterialsPath,
			UTexture2D::StaticClass(),
			TextureFactory));
	}

	void EnsureDirectory(const FString& DirectoryPath)
	{
		if (GEditor == nullptr)
		{
			return;
		}

		if (UEditorAssetSubsystem* AssetSubsystem = GEditor->GetEditorSubsystem<UEditorAssetSubsystem>())
		{
			if (!AssetSubsystem->DoesDirectoryExist(DirectoryPath))
			{
				AssetSubsystem->MakeDirectory(DirectoryPath);
			}
		}
	}

	bool SaveAssets(const TArray<UObject*>& AssetsToSave)
	{
		if (GEditor == nullptr)
		{
			return false;
		}

		if (UEditorAssetSubsystem* AssetSubsystem = GEditor->GetEditorSubsystem<UEditorAssetSubsystem>())
		{
			return AssetSubsystem->SaveLoadedAssets(AssetsToSave, false);
		}

		return false;
	}

	UEditorActorSubsystem* GetEditorActorSubsystem()
	{
		return GEditor ? GEditor->GetEditorSubsystem<UEditorActorSubsystem>() : nullptr;
	}

	AActor* FindActorByLabel(UEditorActorSubsystem& ActorSubsystem, const FString& ActorLabel)
	{
		for (AActor* Actor : ActorSubsystem.GetAllLevelActors())
		{
			if (IsValid(Actor) && Actor->GetActorLabel() == ActorLabel)
			{
				return Actor;
			}
		}

		return nullptr;
	}
}

bool FCodexBasicMapFloorBuilder::RunBuild(FString& OutError)
{
	if (GEditor == nullptr)
	{
		OutError = TEXT("Editor is unavailable.");
		return false;
	}

	EnsureDirectory(MaterialsPath);

	UTexture2D* Texture = nullptr;
	if (!CreateOrUpdateTexture(Texture, OutError))
	{
		return false;
	}

	UMaterial* Material = nullptr;
	if (!CreateOrUpdateMaterial(*Texture, Material, OutError))
	{
		return false;
	}

	if (!SaveAssets({Texture, Material}))
	{
		OutError = TEXT("Failed to save imported floor texture and material assets.");
		return false;
	}

	if (!ApplyMaterialToBasicMapFloor(*Material, OutError))
	{
		return false;
	}

	return true;
}

bool FCodexBasicMapFloorBuilder::CreateOrUpdateTexture(UTexture2D*& OutTexture, FString& OutError)
{
	const FString SourceFilename = ResolveSourceTexturePath();
	if (!IFileManager::Get().FileExists(*SourceFilename))
	{
		OutError = FString::Printf(TEXT("%s was not found."), *SourceFilename);
		return false;
	}

	TArray64<uint8> CompressedImageData;
	if (!FFileHelper::LoadFileToArray(CompressedImageData, *SourceFilename) || CompressedImageData.IsEmpty())
	{
		OutError = FString::Printf(TEXT("Failed to read %s."), *SourceFilename);
		return false;
	}

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
	FImage DecodedImage;
	if (!ImageWrapperModule.DecompressImage(CompressedImageData.GetData(), CompressedImageData.Num(), DecodedImage))
	{
		OutError = FString::Printf(TEXT("Failed to decode %s."), *SourceFilename);
		return false;
	}

	DecodedImage.ChangeFormat(ERawImageFormat::BGRA8, EGammaSpace::sRGB);
	if (OutTexture == nullptr)
	{
		OutTexture = CreateTextureAsset();
	}

	if (OutTexture == nullptr)
	{
		OutError = FString::Printf(TEXT("Failed to import or load %s/%s."), MaterialsPath, TextureAssetName);
		return false;
	}

	OutTexture->Modify();
	OutTexture->Source.Init(DecodedImage.SizeX, DecodedImage.SizeY, 1, 1, TSF_BGRA8, DecodedImage.RawData.GetData());
	OutTexture->CompressionSettings = TC_Default;
	OutTexture->MipGenSettings = TMGS_FromTextureGroup;
	OutTexture->LODGroup = TEXTUREGROUP_World;
	OutTexture->SRGB = true;
	OutTexture->AddressX = TA_Wrap;
	OutTexture->AddressY = TA_Wrap;
	OutTexture->MarkPackageDirty();
	OutTexture->PostEditChange();
	OutTexture->UpdateResource();
	return true;
}

bool FCodexBasicMapFloorBuilder::CreateOrUpdateMaterial(UTexture2D& Texture, UMaterial*& OutMaterial, FString& OutError)
{
	const FString PackageName = FString::Printf(TEXT("%s/%s"), MaterialsPath, MaterialAssetName);
	const FString ObjectPath = MakeObjectPath(MaterialsPath, MaterialAssetName);

	UPackage* Package = CreatePackage(*PackageName);
	Package->FullyLoad();

	OutMaterial = LoadAsset<UMaterial>(ObjectPath);
	const bool bIsNewMaterial = OutMaterial == nullptr;
	if (bIsNewMaterial)
	{
		UMaterialFactoryNew* Factory = NewObject<UMaterialFactoryNew>();
		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
		OutMaterial = Cast<UMaterial>(AssetToolsModule.Get().CreateAsset(
			MaterialAssetName,
			MaterialsPath,
			UMaterial::StaticClass(),
			Factory));

		if (OutMaterial == nullptr)
		{
			OutError = FString::Printf(TEXT("Failed to create or load %s."), *ObjectPath);
			return false;
		}
	}

	OutMaterial->Modify();
	OutMaterial->BlendMode = BLEND_Opaque;
	OutMaterial->SetShadingModel(MSM_DefaultLit);
	UMaterialEditingLibrary::DeleteAllMaterialExpressions(OutMaterial);

	UMaterialExpressionTextureCoordinate* TextureCoordinateExpression = Cast<UMaterialExpressionTextureCoordinate>(
		UMaterialEditingLibrary::CreateMaterialExpression(
			OutMaterial,
			UMaterialExpressionTextureCoordinate::StaticClass(),
			-980,
			-80));
	UMaterialExpressionMaterialFunctionCall* TextureVariationExpression = Cast<UMaterialExpressionMaterialFunctionCall>(
		UMaterialEditingLibrary::CreateMaterialExpression(
			OutMaterial,
			UMaterialExpressionMaterialFunctionCall::StaticClass(),
			-700,
			0));
	UMaterialExpressionDDX* DdxExpression = Cast<UMaterialExpressionDDX>(
		UMaterialEditingLibrary::CreateMaterialExpression(
			OutMaterial,
			UMaterialExpressionDDX::StaticClass(),
			-420,
			-150));
	UMaterialExpressionDDY* DdyExpression = Cast<UMaterialExpressionDDY>(
		UMaterialEditingLibrary::CreateMaterialExpression(
			OutMaterial,
			UMaterialExpressionDDY::StaticClass(),
			-420,
			10));
	UMaterialExpressionTextureSample* TextureSampleExpression = Cast<UMaterialExpressionTextureSample>(
		UMaterialEditingLibrary::CreateMaterialExpression(
			OutMaterial,
			UMaterialExpressionTextureSample::StaticClass(),
			-120,
			0));
	UMaterialExpressionConstant* RoughnessExpression = Cast<UMaterialExpressionConstant>(
		UMaterialEditingLibrary::CreateMaterialExpression(
			OutMaterial,
			UMaterialExpressionConstant::StaticClass(),
			-160,
			220));
	UMaterialExpressionConstant* SpecularExpression = Cast<UMaterialExpressionConstant>(
		UMaterialEditingLibrary::CreateMaterialExpression(
			OutMaterial,
			UMaterialExpressionConstant::StaticClass(),
			-160,
			360));

	if (TextureCoordinateExpression == nullptr ||
		TextureVariationExpression == nullptr ||
		DdxExpression == nullptr ||
		DdyExpression == nullptr ||
		TextureSampleExpression == nullptr ||
		RoughnessExpression == nullptr ||
		SpecularExpression == nullptr)
	{
		OutError = FString::Printf(TEXT("Failed to author material graph for %s."), *ObjectPath);
		return false;
	}

	UMaterialFunctionInterface* TextureVariationFunction = LoadAsset<UMaterialFunctionInterface>(TextureVariationFunctionPath);
	if (TextureVariationFunction == nullptr || !TextureVariationExpression->SetMaterialFunction(TextureVariationFunction))
	{
		OutError = FString::Printf(TEXT("Failed to load material function %s."), TextureVariationFunctionPath);
		return false;
	}

	TextureCoordinateExpression->CoordinateIndex = 0;
	TextureCoordinateExpression->UTiling = 4.0f;
	TextureCoordinateExpression->VTiling = 4.0f;
	TextureSampleExpression->Texture = &Texture;
	TextureSampleExpression->MipValueMode = TMVM_Derivative;
	RoughnessExpression->R = 1.0f;
	SpecularExpression->R = 0.0f;

	if (!UMaterialEditingLibrary::ConnectMaterialExpressions(TextureCoordinateExpression, TEXT(""), TextureVariationExpression, TEXT("UVs")))
	{
		OutError = FString::Printf(TEXT("Failed to connect TextureCoordinate to TextureVariation.UVs for %s."), *ObjectPath);
		return false;
	}

	if (!UMaterialEditingLibrary::ConnectMaterialExpressions(TextureVariationExpression, TEXT("Raw UVs"), DdxExpression, TEXT("Value")))
	{
		OutError = FString::Printf(TEXT("Failed to connect TextureVariation.Raw UVs to DDX for %s."), *ObjectPath);
		return false;
	}

	if (!UMaterialEditingLibrary::ConnectMaterialExpressions(TextureVariationExpression, TEXT("Raw UVs"), DdyExpression, TEXT("Value")))
	{
		OutError = FString::Printf(TEXT("Failed to connect TextureVariation.Raw UVs to DDY for %s."), *ObjectPath);
		return false;
	}

	if (!UMaterialEditingLibrary::ConnectMaterialExpressions(TextureVariationExpression, TEXT(""), TextureSampleExpression, TEXT("UVs")))
	{
		OutError = FString::Printf(TEXT("Failed to connect TextureVariation.Shifted UVs to TextureSample.UVs for %s."), *ObjectPath);
		return false;
	}

	if (!UMaterialEditingLibrary::ConnectMaterialExpressions(DdxExpression, TEXT(""), TextureSampleExpression, TEXT("DDX(UVs)")))
	{
		OutError = FString::Printf(TEXT("Failed to connect DDX to TextureSample.DDX(UVs) for %s."), *ObjectPath);
		return false;
	}

	if (!UMaterialEditingLibrary::ConnectMaterialExpressions(DdyExpression, TEXT(""), TextureSampleExpression, TEXT("DDY(UVs)")))
	{
		OutError = FString::Printf(TEXT("Failed to connect DDY to TextureSample.DDY(UVs) for %s."), *ObjectPath);
		return false;
	}

	if (!UMaterialEditingLibrary::ConnectMaterialProperty(TextureSampleExpression, TEXT(""), MP_BaseColor) ||
		!UMaterialEditingLibrary::ConnectMaterialProperty(RoughnessExpression, TEXT(""), MP_Roughness) ||
		!UMaterialEditingLibrary::ConnectMaterialProperty(SpecularExpression, TEXT(""), MP_Specular))
	{
		OutError = FString::Printf(TEXT("Failed to connect material graph for %s."), *ObjectPath);
		return false;
	}

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

bool FCodexBasicMapFloorBuilder::ApplyMaterialToBasicMapFloor(UMaterial& Material, FString& OutError)
{
	FString MapFilename;
	if (!FPackageName::TryConvertLongPackageNameToFilename(BasicMapAssetPath, MapFilename, FPackageName::GetMapPackageExtension()))
	{
		OutError = TEXT("Failed to resolve BasicMap filename.");
		return false;
	}

	UWorld* World = UEditorLoadingAndSavingUtils::LoadMap(MapFilename);
	if (World == nullptr)
	{
		OutError = TEXT("Failed to load /Game/Maps/BasicMap.");
		return false;
	}

	UEditorActorSubsystem* ActorSubsystem = GetEditorActorSubsystem();
	if (ActorSubsystem == nullptr)
	{
		OutError = TEXT("Editor actor subsystem is unavailable.");
		return false;
	}

	AActor* FloorActor = FindActorByLabel(*ActorSubsystem, FloorActorLabel);
	if (FloorActor == nullptr)
	{
		OutError = TEXT("Failed to find actor labeled Floor in BasicMap.");
		return false;
	}

	AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(FloorActor);
	if (StaticMeshActor == nullptr)
	{
		OutError = TEXT("Actor labeled Floor is not a StaticMeshActor.");
		return false;
	}

	UStaticMeshComponent* StaticMeshComponent = StaticMeshActor->GetStaticMeshComponent();
	if (StaticMeshComponent == nullptr)
	{
		OutError = TEXT("Actor labeled Floor is missing a StaticMeshComponent.");
		return false;
	}

	const int32 NumMaterials = StaticMeshComponent->GetNumMaterials();
	if (NumMaterials <= 0)
	{
		OutError = TEXT("Floor StaticMeshComponent has no material slots to override.");
		return false;
	}

	StaticMeshActor->Modify();
	StaticMeshComponent->Modify();

	for (int32 MaterialIndex = 0; MaterialIndex < NumMaterials; ++MaterialIndex)
	{
		StaticMeshComponent->SetMaterial(MaterialIndex, &Material);
	}

	StaticMeshComponent->MarkPackageDirty();
	StaticMeshComponent->MarkRenderStateDirty();
	StaticMeshActor->MarkPackageDirty();
	World->MarkPackageDirty();

	if (!UEditorLoadingAndSavingUtils::SaveMap(World, BasicMapAssetPath))
	{
		OutError = TEXT("Failed to save /Game/Maps/BasicMap after applying floor material.");
		return false;
	}

	return true;
}
