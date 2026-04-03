#include "Commandlets/CodexHarnessHeadlessSetupCommandlet.h"

#include "AssetImportTask.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "CodexHarnessCharacter.h"
#include "CodexHarnessEnemyCharacter.h"
#include "CodexHarnessGameInstance.h"
#include "CodexHarnessGameMode.h"
#include "CodexHarnessHUD.h"
#include "CodexHarnessPlayerController.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Blueprint.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Effects/CodexHarnessEffectsConfigDataAsset.h"
#include "Effects/CodexHarnessPlayerHitCameraShake.h"
#include "Factories/DataAssetFactory.h"
#include "Factories/MaterialFactoryNew.h"
#include "FileHelpers.h"
#include "GameFramework/WorldSettings.h"
#include "GameMapsSettings.h"
#include "Input/CodexHarnessInputConfigDataAsset.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "InputEditorModule.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionVertexColor.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "NiagaraSystem.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/UnrealType.h"
#include "VoxStaticMeshFactory.h"

namespace
{
	const FString PlayerVoxSourceRelativePath = TEXT("SourceArt/Vox/SM_Vox_PlayerChicken_White.vox");
	const FString EnemyVoxSourceRelativePath = TEXT("SourceArt/Vox/SM_Vox_EnemyChicken_Red.vox");
	const FString ReportRelativePath = TEXT("Saved/HeadlessSetup/CodexHarnessHeadlessSetupReport.txt");
	const FString PlayerVoxMeshPackagePath = TEXT("/Game/CodexHarness/Vox/SM_Vox_PlayerChicken_White");
	const FString EnemyVoxMeshPackagePath = TEXT("/Game/CodexHarness/Vox/SM_Vox_EnemyChicken_Red");
	const FString VoxBaseMaterialPackagePath = TEXT("/Game/CodexHarness/Materials/M_VoxBase");
	const FString InputActionsFolder = TEXT("/Game/CodexHarness/Input/Actions");
	const FString InputContextsFolder = TEXT("/Game/CodexHarness/Input/Contexts");
	const FString InputConfigsFolder = TEXT("/Game/CodexHarness/Input/Configs");
	const FString EffectsFolder = TEXT("/Game/CodexHarness/Effects");
	const FString BlueprintCoreFolder = TEXT("/Game/CodexHarness/Blueprints/Core");
	const FString BlueprintEnemiesFolder = TEXT("/Game/CodexHarness/Blueprints/Enemies");
	const FString BasicMapPackagePath = TEXT("/Game/Maps/BasicMap");
	const FString BasicMapObjectPath = TEXT("/Game/Maps/BasicMap.BasicMap");
	const FString PlayerHitReactionTemplatePath = TEXT("/Niagara/DefaultAssets/Templates/Systems/RadialBurst.RadialBurst");

	const TCHAR* MoveActionAssetName = TEXT("IA_Move");
	const TCHAR* LookActionAssetName = TEXT("IA_Look");
	const TCHAR* FireActionAssetName = TEXT("IA_Fire");
	const TCHAR* RestartActionAssetName = TEXT("IA_Restart");
	const TCHAR* MappingContextAssetName = TEXT("IMC_Default");
	const TCHAR* InputConfigAssetName = TEXT("DA_DefaultInputConfig");
	const TCHAR* EffectsConfigAssetName = TEXT("DA_DefaultEffectsConfig");
	const TCHAR* PlayerHitReactionSystemAssetName = TEXT("NS_PlayerHitReaction");
	const TCHAR* PlayerHitCameraShakeBlueprintAssetName = TEXT("BP_CodexHarnessPlayerHitCameraShake");
	const TCHAR* GameInstanceBlueprintAssetName = TEXT("BP_CodexHarnessGameInstance");
	const TCHAR* GameModeBlueprintAssetName = TEXT("BP_CodexHarnessGameMode");
	const TCHAR* PlayerControllerBlueprintAssetName = TEXT("BP_CodexHarnessPlayerController");
	const TCHAR* CharacterBlueprintAssetName = TEXT("BP_CodexHarnessCharacter");
	const TCHAR* EnemyCharacterBlueprintAssetName = TEXT("BP_CodexHarnessEnemyCharacter");
	const TCHAR* HudBlueprintAssetName = TEXT("BP_CodexHarnessHUD");

	struct FGeneratedVoxModel
	{
		int32 SizeX = 0;
		int32 SizeY = 0;
		int32 SizeZ = 0;
		TArray<uint8> Palette;

		struct FVoxelEntry
		{
			uint8 X = 0;
			uint8 Y = 0;
			uint8 Z = 0;
			uint8 ColorIndex = 0;
		};

		TArray<FVoxelEntry> Voxels;
	};

	struct FHeadlessSetupArtifacts
	{
		FString PlayerVoxFilename;
		FString EnemyVoxFilename;
		TObjectPtr<UMaterial> VoxBaseMaterial = nullptr;
		TObjectPtr<UStaticMesh> PlayerVoxMesh = nullptr;
		TObjectPtr<UStaticMesh> EnemyVoxMesh = nullptr;
		TObjectPtr<UInputAction> MoveAction = nullptr;
		TObjectPtr<UInputAction> LookAction = nullptr;
		TObjectPtr<UInputAction> FireAction = nullptr;
		TObjectPtr<UInputAction> RestartAction = nullptr;
		TObjectPtr<UInputMappingContext> MappingContext = nullptr;
		TObjectPtr<UCodexHarnessInputConfigDataAsset> InputConfig = nullptr;
		TObjectPtr<UCodexHarnessEffectsConfigDataAsset> EffectsConfig = nullptr;
		TObjectPtr<UNiagaraSystem> PlayerHitReactionSystem = nullptr;
		TObjectPtr<UBlueprint> PlayerHitCameraShakeBlueprint = nullptr;
		TObjectPtr<UBlueprint> GameInstanceBlueprint = nullptr;
		TObjectPtr<UBlueprint> GameModeBlueprint = nullptr;
		TObjectPtr<UBlueprint> PlayerControllerBlueprint = nullptr;
		TObjectPtr<UBlueprint> CharacterBlueprint = nullptr;
		TObjectPtr<UBlueprint> EnemyCharacterBlueprint = nullptr;
		TObjectPtr<UBlueprint> HudBlueprint = nullptr;
		TObjectPtr<UWorld> BasicMapWorld = nullptr;
	};

	struct FChickenVoxPalette
	{
		FColor BodyPrimary;
		FColor BodyAccent;
		FColor Beak;
		FColor Comb;
		FColor Eye;
		FColor Feet;
	};

	FString MakeObjectPath(const FString& PackagePath)
	{
		return PackagePath + TEXT(".") + FPackageName::GetLongPackageAssetName(PackagePath);
	}

	FString MakeObjectPath(const FString& Folder, const FString& AssetName)
	{
		return FString::Printf(TEXT("%s/%s.%s"), *Folder, *AssetName, *AssetName);
	}

	bool SaveObjectPackage(UObject& Object, FString& OutError)
	{
		UPackage* const Package = Object.GetPackage();
		if (Package == nullptr)
		{
			OutError = FString::Printf(TEXT("Object %s has no package."), *Object.GetName());
			return false;
		}

		const FString Filename = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.SaveFlags = SAVE_NoError;
		SaveArgs.Error = GError;
		if (!UPackage::SavePackage(Package, &Object, *Filename, SaveArgs))
		{
			OutError = FString::Printf(TEXT("Failed to save package %s."), *Package->GetName());
			return false;
		}

		return true;
	}

	bool SavePackages(const TArray<UPackage*>& PackagesToSave, FString& OutError)
	{
		if (!UEditorLoadingAndSavingUtils::SavePackages(PackagesToSave, false))
		{
			OutError = TEXT("Failed to save one or more generated assets.");
			return false;
		}

		return true;
	}

	void AddPackageIfValid(TArray<UPackage*>& PackagesToSave, UObject* Object)
	{
		if (Object != nullptr && Object->GetOutermost() != nullptr)
		{
			PackagesToSave.AddUnique(Object->GetOutermost());
		}
	}

	template <typename TObjectType, typename TFactoryType>
	TObjectType* CreateAssetWithFactory(
		const FString& Folder,
		const FString& AssetName,
		UClass* AssetClass,
		TFunctionRef<void(TFactoryType&)> SetupFactory)
	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get();
		TFactoryType* const Factory = NewObject<TFactoryType>();
		SetupFactory(*Factory);
		return Cast<TObjectType>(AssetTools.CreateAsset(AssetName, Folder, AssetClass, Factory, FName(TEXT("CodexHarnessHeadlessSetup"))));
	}

	template <typename TObjectType>
	TObjectType* LoadOrCreateAsset(const FString& Folder, const FString& AssetName, TFunctionRef<TObjectType*()> CreateAsset)
	{
		const FString PackagePath = FString::Printf(TEXT("%s/%s"), *Folder, *AssetName);
		if (FPackageName::DoesPackageExist(PackagePath))
		{
			if (TObjectType* Existing = LoadObject<TObjectType>(nullptr, *MakeObjectPath(Folder, AssetName)))
			{
				return Existing;
			}
		}

		return CreateAsset();
	}

	template <typename TObjectType>
	TObjectType* LoadOrDuplicateAsset(const FString& Folder, const FString& AssetName, const FString& SourceObjectPath)
	{
		const FString PackagePath = FString::Printf(TEXT("%s/%s"), *Folder, *AssetName);
		if (FPackageName::DoesPackageExist(PackagePath))
		{
			if (TObjectType* Existing = LoadObject<TObjectType>(nullptr, *MakeObjectPath(Folder, AssetName)))
			{
				return Existing;
			}
		}

		TObjectType* const SourceAsset = LoadObject<TObjectType>(nullptr, *SourceObjectPath);
		if (SourceAsset == nullptr)
		{
			return nullptr;
		}

		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get();
		return Cast<TObjectType>(AssetTools.DuplicateAsset(AssetName, Folder, SourceAsset));
	}

	FObjectProperty* FindObjectPropertyChecked(UObject& TargetObject, const TCHAR* PropertyName, UClass* ExpectedBaseClass)
	{
		FObjectProperty* const ObjectProperty = FindFProperty<FObjectProperty>(TargetObject.GetClass(), PropertyName);
		check(ObjectProperty != nullptr);
		check(ObjectProperty->PropertyClass->IsChildOf(ExpectedBaseClass));
		return ObjectProperty;
	}

	FClassProperty* FindClassPropertyChecked(UObject& TargetObject, const TCHAR* PropertyName, UClass* ExpectedBaseClass)
	{
		FClassProperty* const ClassProperty = FindFProperty<FClassProperty>(TargetObject.GetClass(), PropertyName);
		check(ClassProperty != nullptr);
		check(ClassProperty->MetaClass->IsChildOf(ExpectedBaseClass));
		return ClassProperty;
	}

	void SetObjectProperty(UObject& TargetObject, const TCHAR* PropertyName, UObject* Value, UClass* ExpectedBaseClass)
	{
		FObjectProperty* const ObjectProperty = FindObjectPropertyChecked(TargetObject, PropertyName, ExpectedBaseClass);
		TargetObject.Modify();
		ObjectProperty->SetObjectPropertyValue_InContainer(&TargetObject, Value);
		TargetObject.MarkPackageDirty();
	}

	void SetClassProperty(UObject& TargetObject, const TCHAR* PropertyName, UClass* Value, UClass* ExpectedBaseClass)
	{
		FClassProperty* const ClassProperty = FindClassPropertyChecked(TargetObject, PropertyName, ExpectedBaseClass);
		TargetObject.Modify();
		ClassProperty->SetPropertyValue_InContainer(&TargetObject, Value);
		TargetObject.MarkPackageDirty();
	}

	bool CompileBlueprint(UBlueprint& Blueprint, FString& OutError)
	{
		FKismetEditorUtilities::CompileBlueprint(&Blueprint);
		if (Blueprint.Status == BS_Error || Blueprint.GeneratedClass == nullptr)
		{
			OutError = FString::Printf(TEXT("Failed to compile blueprint %s."), *Blueprint.GetPathName());
			return false;
		}

		return true;
	}

	UBlueprint* LoadOrCreateBlueprint(const FString& Folder, const FString& AssetName, UClass* ParentClass, FString& OutError)
	{
		const FString PackagePath = FString::Printf(TEXT("%s/%s"), *Folder, *AssetName);
		if (FPackageName::DoesPackageExist(PackagePath))
		{
			if (UBlueprint* Existing = LoadObject<UBlueprint>(nullptr, *MakeObjectPath(Folder, AssetName)))
			{
				const UClass* ExistingParent = Existing->GeneratedClass != nullptr
					? Existing->GeneratedClass->GetSuperClass()
					: nullptr;
				if (ExistingParent == nullptr)
				{
					ExistingParent = Existing->ParentClass.Get();
				}
				if (ExistingParent == nullptr || !ExistingParent->IsChildOf(ParentClass))
				{
					OutError = FString::Printf(TEXT("Blueprint %s does not inherit from %s."), *Existing->GetPathName(), *ParentClass->GetName());
					return nullptr;
				}

				if (!CompileBlueprint(*Existing, OutError))
				{
					return nullptr;
				}

				return Existing;
			}
		}

		UPackage* const Package = CreatePackage(*PackagePath);
		if (Package == nullptr)
		{
			OutError = FString::Printf(TEXT("Failed to create package for blueprint %s."), *AssetName);
			return nullptr;
		}

		Package->FullyLoad();

		UBlueprint* const Blueprint = FKismetEditorUtilities::CreateBlueprint(
			ParentClass,
			Package,
			*AssetName,
			BPTYPE_Normal,
			UBlueprint::StaticClass(),
			UBlueprintGeneratedClass::StaticClass(),
			FName(TEXT("CodexHarnessHeadlessSetup")));
		if (Blueprint == nullptr)
		{
			OutError = FString::Printf(TEXT("Failed to create blueprint %s."), *AssetName);
			return nullptr;
		}

		FAssetRegistryModule::AssetCreated(Blueprint);
		Blueprint->MarkPackageDirty();

		if (!CompileBlueprint(*Blueprint, OutError))
		{
			return nullptr;
		}

		return Blueprint;
	}

	UMaterialExpressionConstant* AddConstantExpression(UMaterial& Material, const float Value, const int32 X, const int32 Y)
	{
		UMaterialExpressionConstant* const Constant = NewObject<UMaterialExpressionConstant>(&Material);
		Constant->R = Value;
		Constant->MaterialExpressionEditorX = X;
		Constant->MaterialExpressionEditorY = Y;
		Material.GetExpressionCollection().AddExpression(Constant);
		return Constant;
	}

	UMaterial* LoadOrCreateVoxBaseMaterial(FString& OutError)
	{
		if (UMaterial* ExistingMaterial = LoadObject<UMaterial>(nullptr, *MakeObjectPath(VoxBaseMaterialPackagePath)))
		{
			return ExistingMaterial;
		}

		UPackage* const Package = CreatePackage(*VoxBaseMaterialPackagePath);
		if (Package == nullptr)
		{
			OutError = TEXT("Failed to create material package for M_VoxBase.");
			return nullptr;
		}

		UMaterialFactoryNew* const MaterialFactory = NewObject<UMaterialFactoryNew>();
		UMaterial* const Material = Cast<UMaterial>(MaterialFactory->FactoryCreateNew(
			UMaterial::StaticClass(),
			Package,
			*FString(TEXT("M_VoxBase")),
			RF_Public | RF_Standalone | RF_Transactional,
			nullptr,
			GWarn));
		if (Material == nullptr)
		{
			OutError = TEXT("Failed to create M_VoxBase material.");
			return nullptr;
		}

		Material->PreEditChange(nullptr);
		UMaterialEditorOnlyData* const MaterialEditorOnlyData = Material->GetEditorOnlyData();

		UMaterialExpressionVertexColor* const VertexColor = NewObject<UMaterialExpressionVertexColor>(Material);
		VertexColor->MaterialExpressionEditorX = -400;
		VertexColor->MaterialExpressionEditorY = -200;
		Material->GetExpressionCollection().AddExpression(VertexColor);
		MaterialEditorOnlyData->BaseColor.Connect(0, VertexColor);
		MaterialEditorOnlyData->Roughness.Connect(0, AddConstantExpression(*Material, 1.0f, -400, 0));
		MaterialEditorOnlyData->Specular.Connect(0, AddConstantExpression(*Material, 0.0f, -400, 120));
		MaterialEditorOnlyData->Metallic.Connect(0, AddConstantExpression(*Material, 0.0f, -400, 240));

		Material->PostEditChange();
		Material->MarkPackageDirty();
		Package->SetDirtyFlag(true);
		FAssetRegistryModule::AssetCreated(Material);
		Material->ForceRecompileForRendering();

		if (!SaveObjectPackage(*Material, OutError))
		{
			return nullptr;
		}

		return Material;
	}

	FGeneratedVoxModel BuildChickenVoxModel(const FChickenVoxPalette& Palette)
	{
		FGeneratedVoxModel Model;
		Model.SizeX = 12;
		Model.SizeY = 10;
		Model.SizeZ = 12;
		Model.Palette.SetNumZeroed(256 * 4);

		auto SetPaletteColor = [&Model](const int32 PaletteIndex, const FColor& Color)
		{
			const int32 Offset = (PaletteIndex - 1) * 4;
			Model.Palette[Offset + 0] = Color.R;
			Model.Palette[Offset + 1] = Color.G;
			Model.Palette[Offset + 2] = Color.B;
			Model.Palette[Offset + 3] = Color.A;
		};

		SetPaletteColor(1, Palette.BodyPrimary);
		SetPaletteColor(2, Palette.BodyAccent);
		SetPaletteColor(3, Palette.Beak);
		SetPaletteColor(4, Palette.Comb);
		SetPaletteColor(5, Palette.Eye);
		SetPaletteColor(6, Palette.Feet);

		auto AddVoxelBox = [&Model](const uint8 MinX, const uint8 MinY, const uint8 MinZ, const uint8 MaxX, const uint8 MaxY, const uint8 MaxZ, const uint8 ColorIndex)
		{
			for (uint8 Z = MinZ; Z <= MaxZ; ++Z)
			{
				for (uint8 Y = MinY; Y <= MaxY; ++Y)
				{
					for (uint8 X = MinX; X <= MaxX; ++X)
					{
						FGeneratedVoxModel::FVoxelEntry& Voxel = Model.Voxels.AddDefaulted_GetRef();
						Voxel.X = X;
						Voxel.Y = Y;
						Voxel.Z = Z;
						Voxel.ColorIndex = ColorIndex;
					}
				}
			}
		};

		auto AddVoxel = [&Model](const uint8 X, const uint8 Y, const uint8 Z, const uint8 ColorIndex)
		{
			FGeneratedVoxModel::FVoxelEntry& Voxel = Model.Voxels.AddDefaulted_GetRef();
			Voxel.X = X;
			Voxel.Y = Y;
			Voxel.Z = Z;
			Voxel.ColorIndex = ColorIndex;
		};

		AddVoxelBox(2, 2, 2, 7, 7, 6, 1);
		AddVoxelBox(3, 3, 3, 6, 6, 5, 2);
		AddVoxelBox(6, 3, 6, 9, 6, 9, 1);
		AddVoxelBox(4, 2, 3, 5, 2, 5, 2);
		AddVoxelBox(4, 7, 3, 5, 7, 5, 2);
		AddVoxelBox(1, 4, 5, 2, 5, 8, 2);
		AddVoxelBox(10, 4, 7, 11, 5, 8, 3);
		AddVoxelBox(4, 4, 0, 4, 4, 1, 6);
		AddVoxelBox(6, 5, 0, 6, 5, 1, 6);
		AddVoxelBox(3, 4, 0, 5, 4, 0, 6);
		AddVoxelBox(5, 5, 0, 7, 5, 0, 6);
		AddVoxelBox(6, 4, 10, 8, 5, 10, 4);
		AddVoxelBox(7, 4, 11, 7, 5, 11, 4);
		AddVoxel(9, 3, 8, 5);
		AddVoxel(9, 6, 8, 5);

		return Model;
	}

	FGeneratedVoxModel BuildPlayerChickenVoxModel()
	{
		return BuildChickenVoxModel({
			FColor(245, 245, 240, 255),
			FColor(216, 216, 208, 255),
			FColor(244, 184, 72, 255),
			FColor(216, 52, 44, 255),
			FColor(24, 24, 28, 255),
			FColor(236, 156, 56, 255)
		});
	}

	FGeneratedVoxModel BuildEnemyChickenVoxModel()
	{
		return BuildChickenVoxModel({
			FColor(196, 48, 40, 255),
			FColor(126, 18, 18, 255),
			FColor(250, 188, 68, 255),
			FColor(122, 8, 12, 255),
			FColor(24, 24, 28, 255),
			FColor(244, 160, 58, 255)
		});
	}

	void AppendChunk(TArray<uint8>& Buffer, const ANSICHAR* ChunkId, const TArray<uint8>& Content, const TArray<uint8>& Children = {})
	{
		Buffer.Append(reinterpret_cast<const uint8*>(ChunkId), 4);

		auto AppendUint32 = [&Buffer](const uint32 Value)
		{
			Buffer.Append(reinterpret_cast<const uint8*>(&Value), sizeof(Value));
		};

		AppendUint32(Content.Num());
		AppendUint32(Children.Num());
		Buffer.Append(Content);
		Buffer.Append(Children);
	}

	bool WriteGeneratedVoxFile(const FString& RelativeFilename, const FGeneratedVoxModel& Model, FString& OutAbsoluteFilename, FString& OutError)
	{
		OutAbsoluteFilename = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectDir(), RelativeFilename));

		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		const FString Directory = FPaths::GetPath(OutAbsoluteFilename);
		if (!PlatformFile.DirectoryExists(*Directory) && !PlatformFile.CreateDirectoryTree(*Directory))
		{
			OutError = FString::Printf(TEXT("Failed to create VOX source directory: %s"), *Directory);
			return false;
		}

		TArray<uint8> SizeContent;
		SizeContent.Append(reinterpret_cast<const uint8*>(&Model.SizeX), sizeof(int32));
		SizeContent.Append(reinterpret_cast<const uint8*>(&Model.SizeY), sizeof(int32));
		SizeContent.Append(reinterpret_cast<const uint8*>(&Model.SizeZ), sizeof(int32));

		TArray<uint8> XyziContent;
		const uint32 VoxelCount = static_cast<uint32>(Model.Voxels.Num());
		XyziContent.Append(reinterpret_cast<const uint8*>(&VoxelCount), sizeof(uint32));
		for (const FGeneratedVoxModel::FVoxelEntry& Voxel : Model.Voxels)
		{
			XyziContent.Add(Voxel.X);
			XyziContent.Add(Voxel.Y);
			XyziContent.Add(Voxel.Z);
			XyziContent.Add(Voxel.ColorIndex);
		}

		TArray<uint8> RgbaContent = Model.Palette;

		TArray<uint8> MainChildren;
		AppendChunk(MainChildren, "SIZE", SizeContent);
		AppendChunk(MainChildren, "XYZI", XyziContent);
		AppendChunk(MainChildren, "RGBA", RgbaContent);

		TArray<uint8> VoxFileBytes;
		VoxFileBytes.Append(reinterpret_cast<const uint8*>("VOX "), 4);
		const uint32 Version = 150;
		VoxFileBytes.Append(reinterpret_cast<const uint8*>(&Version), sizeof(uint32));
		AppendChunk(VoxFileBytes, "MAIN", {}, MainChildren);

		if (!FFileHelper::SaveArrayToFile(VoxFileBytes, *OutAbsoluteFilename))
		{
			OutError = FString::Printf(TEXT("Failed to write sample VOX file: %s"), *OutAbsoluteFilename);
			return false;
		}

		return true;
	}

	UStaticMesh* ImportOrUpdateVoxStaticMesh(const FString& SourceFilename, const FString& DestinationPackagePath, UMaterialInterface* Material, FString& OutError)
	{
		UAssetImportTask* const ImportTask = NewObject<UAssetImportTask>(GetTransientPackage());
		UVoxStaticMeshFactory* const VoxFactory = NewObject<UVoxStaticMeshFactory>(ImportTask);
		if (ImportTask == nullptr || VoxFactory == nullptr)
		{
			OutError = TEXT("Failed to allocate VOX import task.");
			return nullptr;
		}

		ImportTask->Filename = SourceFilename;
		ImportTask->DestinationPath = FPackageName::GetLongPackagePath(DestinationPackagePath);
		ImportTask->DestinationName = FPackageName::GetLongPackageAssetName(DestinationPackagePath);
		ImportTask->bReplaceExisting = true;
		ImportTask->bReplaceExistingSettings = true;
		ImportTask->bAutomated = true;
		ImportTask->bSave = true;
		ImportTask->bAsync = false;
		ImportTask->Factory = VoxFactory;

		TArray<UAssetImportTask*> ImportTasks;
		ImportTasks.Add(ImportTask);
		FAssetToolsModule::GetModule().Get().ImportAssetTasks(ImportTasks);

		UStaticMesh* ImportedMesh = nullptr;
		for (UObject* const ImportedObject : ImportTask->GetObjects())
		{
			ImportedMesh = Cast<UStaticMesh>(ImportedObject);
			if (ImportedMesh != nullptr)
			{
				break;
			}
		}

		if (ImportedMesh == nullptr)
		{
			ImportedMesh = LoadObject<UStaticMesh>(nullptr, *MakeObjectPath(DestinationPackagePath));
		}

		if (ImportedMesh == nullptr)
		{
			OutError = FString::Printf(TEXT("Failed to import VOX file into %s."), *DestinationPackagePath);
			return nullptr;
		}

		if (Material != nullptr)
		{
			ImportedMesh->Modify();
			if (ImportedMesh->GetStaticMaterials().IsEmpty())
			{
				ImportedMesh->GetStaticMaterials().Add(FStaticMaterial(Material, TEXT("VoxelMaterial"), TEXT("VoxelMaterial")));
			}
			else
			{
				ImportedMesh->GetStaticMaterials()[0] = FStaticMaterial(Material, TEXT("VoxelMaterial"), TEXT("VoxelMaterial"));
			}
			ImportedMesh->MarkPackageDirty();
			ImportedMesh->PostEditChange();
		}

		if (!SaveObjectPackage(*ImportedMesh, OutError))
		{
			return nullptr;
		}

		return ImportedMesh;
	}

	void AddNegate(FEnhancedActionKeyMapping& Mapping, UObject& Outer, const bool bNegateX, const bool bNegateY)
	{
		UInputModifierNegate* const Modifier = NewObject<UInputModifierNegate>(&Outer);
		Modifier->bX = bNegateX;
		Modifier->bY = bNegateY;
		Mapping.Modifiers.Add(Modifier);
	}

	void AddSwizzle(FEnhancedActionKeyMapping& Mapping, UObject& Outer)
	{
		UInputModifierSwizzleAxis* const Modifier = NewObject<UInputModifierSwizzleAxis>(&Outer);
		Modifier->Order = EInputAxisSwizzle::YXZ;
		Mapping.Modifiers.Add(Modifier);
	}

	bool ConfigureInputAssets(FHeadlessSetupArtifacts& Artifacts, FString& OutError)
	{
		Artifacts.MoveAction = LoadOrCreateAsset<UInputAction>(InputActionsFolder, MoveActionAssetName, []()
		{
			return CreateAssetWithFactory<UInputAction, UInputAction_Factory>(
				InputActionsFolder,
				MoveActionAssetName,
				UInputAction::StaticClass(),
				[](UInputAction_Factory&) {});
		});
		Artifacts.LookAction = LoadOrCreateAsset<UInputAction>(InputActionsFolder, LookActionAssetName, []()
		{
			return CreateAssetWithFactory<UInputAction, UInputAction_Factory>(
				InputActionsFolder,
				LookActionAssetName,
				UInputAction::StaticClass(),
				[](UInputAction_Factory&) {});
		});
		Artifacts.FireAction = LoadOrCreateAsset<UInputAction>(InputActionsFolder, FireActionAssetName, []()
		{
			return CreateAssetWithFactory<UInputAction, UInputAction_Factory>(
				InputActionsFolder,
				FireActionAssetName,
				UInputAction::StaticClass(),
				[](UInputAction_Factory&) {});
		});
		Artifacts.RestartAction = LoadOrCreateAsset<UInputAction>(InputActionsFolder, RestartActionAssetName, []()
		{
			return CreateAssetWithFactory<UInputAction, UInputAction_Factory>(
				InputActionsFolder,
				RestartActionAssetName,
				UInputAction::StaticClass(),
				[](UInputAction_Factory&) {});
		});

		if (Artifacts.MoveAction == nullptr || Artifacts.LookAction == nullptr || Artifacts.FireAction == nullptr || Artifacts.RestartAction == nullptr)
		{
			OutError = TEXT("Failed to create one or more input actions.");
			return false;
		}

		Artifacts.MoveAction->Modify();
		Artifacts.LookAction->Modify();
		Artifacts.FireAction->Modify();
		Artifacts.RestartAction->Modify();
		Artifacts.MoveAction->ValueType = EInputActionValueType::Axis2D;
		Artifacts.LookAction->ValueType = EInputActionValueType::Axis2D;
		Artifacts.FireAction->ValueType = EInputActionValueType::Boolean;
		Artifacts.RestartAction->ValueType = EInputActionValueType::Boolean;
		Artifacts.MoveAction->MarkPackageDirty();
		Artifacts.LookAction->MarkPackageDirty();
		Artifacts.FireAction->MarkPackageDirty();
		Artifacts.RestartAction->MarkPackageDirty();

		Artifacts.MappingContext = LoadOrCreateAsset<UInputMappingContext>(InputContextsFolder, MappingContextAssetName, []()
		{
			return CreateAssetWithFactory<UInputMappingContext, UInputMappingContext_Factory>(
				InputContextsFolder,
				MappingContextAssetName,
				UInputMappingContext::StaticClass(),
				[](UInputMappingContext_Factory&) {});
		});
		if (Artifacts.MappingContext == nullptr)
		{
			OutError = TEXT("Failed to create input mapping context.");
			return false;
		}

		Artifacts.MappingContext->Modify();
		TArray<FEnhancedActionKeyMapping>& Mappings = const_cast<TArray<FEnhancedActionKeyMapping>&>(Artifacts.MappingContext->GetMappings());
		Mappings.Reset();

		Artifacts.MappingContext->MapKey(Artifacts.MoveAction, EKeys::D);
		{
			FEnhancedActionKeyMapping& Mapping = Artifacts.MappingContext->MapKey(Artifacts.MoveAction, EKeys::A);
			AddNegate(Mapping, *Artifacts.MappingContext, true, false);
		}
		{
			FEnhancedActionKeyMapping& Mapping = Artifacts.MappingContext->MapKey(Artifacts.MoveAction, EKeys::W);
			AddSwizzle(Mapping, *Artifacts.MappingContext);
		}
		{
			FEnhancedActionKeyMapping& Mapping = Artifacts.MappingContext->MapKey(Artifacts.MoveAction, EKeys::S);
			AddSwizzle(Mapping, *Artifacts.MappingContext);
			AddNegate(Mapping, *Artifacts.MappingContext, false, true);
		}
		Artifacts.MappingContext->MapKey(Artifacts.MoveAction, EKeys::Gamepad_Left2D);
		Artifacts.MappingContext->MapKey(Artifacts.LookAction, EKeys::Gamepad_Right2D);
		Artifacts.MappingContext->MapKey(Artifacts.FireAction, EKeys::LeftMouseButton);
		Artifacts.MappingContext->MapKey(Artifacts.FireAction, EKeys::Gamepad_RightTrigger);
		Artifacts.MappingContext->MapKey(Artifacts.RestartAction, EKeys::R);
		Artifacts.MappingContext->MapKey(Artifacts.RestartAction, EKeys::Gamepad_FaceButton_Bottom);
		Artifacts.MappingContext->MarkPackageDirty();

		Artifacts.InputConfig = LoadOrCreateAsset<UCodexHarnessInputConfigDataAsset>(InputConfigsFolder, InputConfigAssetName, []()
		{
			return CreateAssetWithFactory<UCodexHarnessInputConfigDataAsset, UDataAssetFactory>(
				InputConfigsFolder,
				InputConfigAssetName,
				UCodexHarnessInputConfigDataAsset::StaticClass(),
				[](UDataAssetFactory& Factory)
				{
					Factory.DataAssetClass = UCodexHarnessInputConfigDataAsset::StaticClass();
				});
		});
		if (Artifacts.InputConfig == nullptr)
		{
			OutError = TEXT("Failed to create input config data asset.");
			return false;
		}

		Artifacts.InputConfig->Modify();
		Artifacts.InputConfig->InputMappingContext = Artifacts.MappingContext;
		Artifacts.InputConfig->MoveAction = Artifacts.MoveAction;
		Artifacts.InputConfig->LookAction = Artifacts.LookAction;
		Artifacts.InputConfig->FireAction = Artifacts.FireAction;
		Artifacts.InputConfig->RestartAction = Artifacts.RestartAction;
		Artifacts.InputConfig->MarkPackageDirty();

		return true;
	}

	bool ConfigureEffectsAssets(FHeadlessSetupArtifacts& Artifacts, FString& OutError)
	{
		Artifacts.PlayerHitReactionSystem = LoadOrDuplicateAsset<UNiagaraSystem>(
			EffectsFolder,
			PlayerHitReactionSystemAssetName,
			PlayerHitReactionTemplatePath);
		if (Artifacts.PlayerHitReactionSystem == nullptr)
		{
			OutError = TEXT("Failed to create player hit reaction Niagara system.");
			return false;
		}

		Artifacts.EffectsConfig = LoadOrCreateAsset<UCodexHarnessEffectsConfigDataAsset>(EffectsFolder, EffectsConfigAssetName, []()
		{
			return CreateAssetWithFactory<UCodexHarnessEffectsConfigDataAsset, UDataAssetFactory>(
				EffectsFolder,
				EffectsConfigAssetName,
				UCodexHarnessEffectsConfigDataAsset::StaticClass(),
				[](UDataAssetFactory& Factory)
				{
					Factory.DataAssetClass = UCodexHarnessEffectsConfigDataAsset::StaticClass();
				});
		});
		if (Artifacts.EffectsConfig == nullptr)
		{
			OutError = TEXT("Failed to create effects config data asset.");
			return false;
		}

		Artifacts.EffectsConfig->Modify();
		Artifacts.EffectsConfig->PlayerHitReactionSystem = Artifacts.PlayerHitReactionSystem;
		Artifacts.EffectsConfig->PlayerHitReactionLocationOffset = FVector(0.0f, 0.0f, 40.0f);
		Artifacts.EffectsConfig->PlayerHitReactionScale = FVector(0.35f, 0.35f, 0.35f);
		Artifacts.PlayerHitCameraShakeBlueprint = LoadOrCreateBlueprint(
			EffectsFolder,
			PlayerHitCameraShakeBlueprintAssetName,
			UCodexHarnessPlayerHitCameraShake::StaticClass(),
			OutError);
		if (Artifacts.PlayerHitCameraShakeBlueprint == nullptr || Artifacts.PlayerHitCameraShakeBlueprint->GeneratedClass == nullptr)
		{
			if (OutError.IsEmpty())
			{
				OutError = TEXT("Failed to create player hit camera shake blueprint.");
			}
			return false;
		}

		SetClassProperty(
			*Artifacts.EffectsConfig,
			TEXT("PlayerHitCameraShakeClass"),
			Artifacts.PlayerHitCameraShakeBlueprint->GeneratedClass,
			UCameraShakeBase::StaticClass());
		Artifacts.EffectsConfig->PlayerHitCameraShakeScale = 1.0f;
		Artifacts.EffectsConfig->MarkPackageDirty();

		return true;
	}

	bool ConfigureBlueprintAssets(FHeadlessSetupArtifacts& Artifacts, FString& OutError)
	{
		Artifacts.GameInstanceBlueprint = LoadOrCreateBlueprint(
			BlueprintCoreFolder,
			GameInstanceBlueprintAssetName,
			UCodexHarnessGameInstance::StaticClass(),
			OutError);
		Artifacts.GameModeBlueprint = LoadOrCreateBlueprint(
			BlueprintCoreFolder,
			GameModeBlueprintAssetName,
			ACodexHarnessGameMode::StaticClass(),
			OutError);
		Artifacts.PlayerControllerBlueprint = LoadOrCreateBlueprint(
			BlueprintCoreFolder,
			PlayerControllerBlueprintAssetName,
			ACodexHarnessPlayerController::StaticClass(),
			OutError);
		Artifacts.CharacterBlueprint = LoadOrCreateBlueprint(
			BlueprintCoreFolder,
			CharacterBlueprintAssetName,
			ACodexHarnessCharacter::StaticClass(),
			OutError);
		Artifacts.EnemyCharacterBlueprint = LoadOrCreateBlueprint(
			BlueprintEnemiesFolder,
			EnemyCharacterBlueprintAssetName,
			ACodexHarnessEnemyCharacter::StaticClass(),
			OutError);
		Artifacts.HudBlueprint = LoadOrCreateBlueprint(
			BlueprintCoreFolder,
			HudBlueprintAssetName,
			ACodexHarnessHUD::StaticClass(),
			OutError);

		if (Artifacts.GameInstanceBlueprint == nullptr
			|| Artifacts.GameModeBlueprint == nullptr
			|| Artifacts.PlayerControllerBlueprint == nullptr
			|| Artifacts.CharacterBlueprint == nullptr
			|| Artifacts.EnemyCharacterBlueprint == nullptr
			|| Artifacts.HudBlueprint == nullptr)
		{
			if (OutError.IsEmpty())
			{
				OutError = TEXT("Failed to create one or more blueprint wrappers.");
			}
			return false;
		}

		UObject* const GameInstanceDefaultObject = Artifacts.GameInstanceBlueprint->GeneratedClass != nullptr
			? Artifacts.GameInstanceBlueprint->GeneratedClass->GetDefaultObject()
			: nullptr;
		if (GameInstanceDefaultObject == nullptr)
		{
			OutError = TEXT("GameInstance blueprint default object is invalid.");
			return false;
		}

		SetObjectProperty(*GameInstanceDefaultObject, TEXT("DefaultInputConfig"), Artifacts.InputConfig, UCodexHarnessInputConfigDataAsset::StaticClass());
		SetObjectProperty(*GameInstanceDefaultObject, TEXT("DefaultEffectsConfig"), Artifacts.EffectsConfig, UCodexHarnessEffectsConfigDataAsset::StaticClass());
		FBlueprintEditorUtils::MarkBlueprintAsModified(Artifacts.GameInstanceBlueprint);
		if (!CompileBlueprint(*Artifacts.GameInstanceBlueprint, OutError))
		{
			return false;
		}

		ACodexHarnessCharacter* const CharacterDefaultObject = Artifacts.CharacterBlueprint->GeneratedClass != nullptr
			? Artifacts.CharacterBlueprint->GeneratedClass->GetDefaultObject<ACodexHarnessCharacter>()
			: nullptr;
		if (CharacterDefaultObject == nullptr)
		{
			OutError = TEXT("Character blueprint default object is invalid.");
			return false;
		}

		SetObjectProperty(*CharacterDefaultObject, TEXT("DefaultVisualMesh"), Artifacts.PlayerVoxMesh, UStaticMesh::StaticClass());
		CharacterDefaultObject->RefreshVisualMeshDefaults();
		FBlueprintEditorUtils::MarkBlueprintAsModified(Artifacts.CharacterBlueprint);
		if (!CompileBlueprint(*Artifacts.CharacterBlueprint, OutError))
		{
			return false;
		}

		ACodexHarnessEnemyCharacter* const EnemyCharacterDefaultObject = Artifacts.EnemyCharacterBlueprint->GeneratedClass != nullptr
			? Artifacts.EnemyCharacterBlueprint->GeneratedClass->GetDefaultObject<ACodexHarnessEnemyCharacter>()
			: nullptr;
		if (EnemyCharacterDefaultObject == nullptr)
		{
			OutError = TEXT("EnemyCharacter blueprint default object is invalid.");
			return false;
		}

		SetObjectProperty(*EnemyCharacterDefaultObject, TEXT("DefaultVisualMesh"), Artifacts.EnemyVoxMesh, UStaticMesh::StaticClass());
		EnemyCharacterDefaultObject->RefreshVisualMeshDefaults();
		FBlueprintEditorUtils::MarkBlueprintAsModified(Artifacts.EnemyCharacterBlueprint);
		if (!CompileBlueprint(*Artifacts.EnemyCharacterBlueprint, OutError))
		{
			return false;
		}

		UObject* const GameModeDefaultObject = Artifacts.GameModeBlueprint->GeneratedClass != nullptr
			? Artifacts.GameModeBlueprint->GeneratedClass->GetDefaultObject()
			: nullptr;
		if (GameModeDefaultObject == nullptr)
		{
			OutError = TEXT("GameMode blueprint default object is invalid.");
			return false;
		}

		SetClassProperty(*GameModeDefaultObject, TEXT("DefaultPawnClass"), Artifacts.CharacterBlueprint->GeneratedClass, APawn::StaticClass());
		SetClassProperty(*GameModeDefaultObject, TEXT("PlayerControllerClass"), Artifacts.PlayerControllerBlueprint->GeneratedClass, APlayerController::StaticClass());
		SetClassProperty(*GameModeDefaultObject, TEXT("HUDClass"), Artifacts.HudBlueprint->GeneratedClass, AHUD::StaticClass());
		SetClassProperty(*GameModeDefaultObject, TEXT("EnemyCharacterClass"), Artifacts.EnemyCharacterBlueprint->GeneratedClass, ACodexHarnessEnemyCharacter::StaticClass());
		FBlueprintEditorUtils::MarkBlueprintAsModified(Artifacts.GameModeBlueprint);
		if (!CompileBlueprint(*Artifacts.GameModeBlueprint, OutError))
		{
			return false;
		}

		if (!CompileBlueprint(*Artifacts.PlayerControllerBlueprint, OutError)
			|| !CompileBlueprint(*Artifacts.EnemyCharacterBlueprint, OutError)
			|| !CompileBlueprint(*Artifacts.HudBlueprint, OutError))
		{
			return false;
		}

		return true;
	}

	bool ApplyProjectSettingsAndMap(FHeadlessSetupArtifacts& Artifacts, FString& OutError)
	{
		UGameMapsSettings* const GameMapsSettings = GetMutableDefault<UGameMapsSettings>();
		if (GameMapsSettings == nullptr)
		{
			OutError = TEXT("GameMapsSettings could not be resolved.");
			return false;
		}

		GameMapsSettings->Modify();
		GameMapsSettings->GameInstanceClass = FSoftClassPath(Artifacts.GameInstanceBlueprint->GeneratedClass->GetPathName());
		GameMapsSettings->EditorStartupMap = FSoftObjectPath(BasicMapObjectPath);
		UGameMapsSettings::SetGameDefaultMap(BasicMapPackagePath);
		UGameMapsSettings::SetGlobalDefaultGameMode(Artifacts.GameModeBlueprint->GeneratedClass->GetPathName());
		GameMapsSettings->SaveConfig();
		GameMapsSettings->TryUpdateDefaultConfigFile();

		const FString BasicMapFilename = FPackageName::LongPackageNameToFilename(BasicMapPackagePath, FPackageName::GetMapPackageExtension());
		UWorld* const BasicMapWorld = UEditorLoadingAndSavingUtils::LoadMap(BasicMapFilename);
		if (BasicMapWorld == nullptr)
		{
			OutError = FString::Printf(TEXT("Failed to load map %s."), *BasicMapPackagePath);
			return false;
		}

		AWorldSettings* const WorldSettings = BasicMapWorld->GetWorldSettings();
		if (WorldSettings == nullptr)
		{
			OutError = TEXT("BasicMap world settings are invalid.");
			return false;
		}

		WorldSettings->Modify();
		WorldSettings->DefaultGameMode = Artifacts.GameModeBlueprint->GeneratedClass;
		BasicMapWorld->MarkPackageDirty();

		if (!UEditorLoadingAndSavingUtils::SaveMap(BasicMapWorld, BasicMapPackagePath))
		{
			OutError = FString::Printf(TEXT("Failed to save map %s."), *BasicMapPackagePath);
			return false;
		}

		Artifacts.BasicMapWorld = BasicMapWorld;
		return true;
	}

	bool ValidateArtifacts(const FHeadlessSetupArtifacts& Artifacts, FString& OutError)
	{
		if (Artifacts.InputConfig == nullptr || !Artifacts.InputConfig->HasRequiredBindings())
		{
			OutError = TEXT("InputConfig does not contain all required bindings.");
			return false;
		}

		if (Artifacts.EffectsConfig == nullptr || !Artifacts.EffectsConfig->HasRequiredAssets())
		{
			OutError = TEXT("EffectsConfig does not contain the required Niagara system and camera shake.");
			return false;
		}

		if (Artifacts.EffectsConfig->PlayerHitCameraShakeClass != Artifacts.PlayerHitCameraShakeBlueprint->GeneratedClass)
		{
			OutError = TEXT("EffectsConfig is not linked to the generated player hit camera shake blueprint.");
			return false;
		}

		const UCodexHarnessGameInstance* const GameInstanceDefaultObject = Artifacts.GameInstanceBlueprint->GeneratedClass != nullptr
			? Artifacts.GameInstanceBlueprint->GeneratedClass->GetDefaultObject<UCodexHarnessGameInstance>()
			: nullptr;
		if (GameInstanceDefaultObject == nullptr || GameInstanceDefaultObject->DefaultInputConfig != Artifacts.InputConfig)
		{
			OutError = TEXT("GameInstance blueprint is not linked to the generated input config asset.");
			return false;
		}

		if (GameInstanceDefaultObject->DefaultEffectsConfig != Artifacts.EffectsConfig)
		{
			OutError = TEXT("GameInstance blueprint is not linked to the generated effects config asset.");
			return false;
		}

		const ACodexHarnessCharacter* const CharacterDefaultObject = Artifacts.CharacterBlueprint->GeneratedClass != nullptr
			? Artifacts.CharacterBlueprint->GeneratedClass->GetDefaultObject<ACodexHarnessCharacter>()
			: nullptr;
		if (CharacterDefaultObject == nullptr || CharacterDefaultObject->GetVisualMeshComponent() == nullptr)
		{
			OutError = TEXT("Character blueprint does not expose a valid visual mesh component.");
			return false;
		}

		if (CharacterDefaultObject->GetVisualMeshComponent()->GetStaticMesh() != Artifacts.PlayerVoxMesh)
		{
			OutError = TEXT("Character blueprint visual mesh component is not linked to the generated player VOX mesh.");
			return false;
		}

		const ACodexHarnessEnemyCharacter* const EnemyCharacterDefaultObject = Artifacts.EnemyCharacterBlueprint->GeneratedClass != nullptr
			? Artifacts.EnemyCharacterBlueprint->GeneratedClass->GetDefaultObject<ACodexHarnessEnemyCharacter>()
			: nullptr;
		if (EnemyCharacterDefaultObject == nullptr || EnemyCharacterDefaultObject->GetVisualMeshComponent() == nullptr)
		{
			OutError = TEXT("EnemyCharacter blueprint does not expose a valid visual mesh component.");
			return false;
		}

		if (EnemyCharacterDefaultObject->GetVisualMeshComponent()->GetStaticMesh() != Artifacts.EnemyVoxMesh)
		{
			OutError = TEXT("EnemyCharacter blueprint visual mesh component is not linked to the generated enemy VOX mesh.");
			return false;
		}

		const AGameModeBase* const GameModeDefaultObject = Artifacts.GameModeBlueprint->GeneratedClass != nullptr
			? Artifacts.GameModeBlueprint->GeneratedClass->GetDefaultObject<AGameModeBase>()
			: nullptr;
		if (GameModeDefaultObject == nullptr
			|| GameModeDefaultObject->DefaultPawnClass != Artifacts.CharacterBlueprint->GeneratedClass
			|| GameModeDefaultObject->PlayerControllerClass != Artifacts.PlayerControllerBlueprint->GeneratedClass
			|| GameModeDefaultObject->HUDClass != Artifacts.HudBlueprint->GeneratedClass)
		{
			OutError = TEXT("GameMode blueprint class defaults are not wired to the generated blueprint wrappers.");
			return false;
		}

		const ACodexHarnessGameMode* const CodexHarnessGameModeDefaultObject = Cast<ACodexHarnessGameMode>(GameModeDefaultObject);
		if (CodexHarnessGameModeDefaultObject == nullptr || CodexHarnessGameModeDefaultObject->GetEnemyCharacterClass() != Artifacts.EnemyCharacterBlueprint->GeneratedClass)
		{
			OutError = TEXT("GameMode blueprint enemy class is not wired to the generated enemy blueprint wrapper.");
			return false;
		}

		const UGameMapsSettings* const GameMapsSettings = GetDefault<UGameMapsSettings>();
		if (GameMapsSettings == nullptr)
		{
			OutError = TEXT("GameMapsSettings could not be reloaded for validation.");
			return false;
		}

		if (GameMapsSettings->GameInstanceClass.TryLoadClass<UGameInstance>() != Artifacts.GameInstanceBlueprint->GeneratedClass)
		{
			OutError = TEXT("Project GameInstanceClass does not point to the generated GameInstance blueprint.");
			return false;
		}

		if (UGameMapsSettings::GetGlobalDefaultGameMode() != Artifacts.GameModeBlueprint->GeneratedClass->GetPathName())
		{
			OutError = TEXT("GlobalDefaultGameMode does not point to the generated GameMode blueprint.");
			return false;
		}

		if (Artifacts.BasicMapWorld == nullptr || Artifacts.BasicMapWorld->GetWorldSettings() == nullptr)
		{
			OutError = TEXT("BasicMap was not available for validation.");
			return false;
		}

		if (Artifacts.BasicMapWorld->GetWorldSettings()->DefaultGameMode != Artifacts.GameModeBlueprint->GeneratedClass)
		{
			OutError = TEXT("BasicMap world settings do not point to the generated GameMode blueprint.");
			return false;
		}

		return true;
	}

	bool WriteReport(const FHeadlessSetupArtifacts& Artifacts, FString& OutError)
	{
		const FString ReportFilename = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectDir(), ReportRelativePath));
		const FString ReportDirectory = FPaths::GetPath(ReportFilename);
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		if (!PlatformFile.DirectoryExists(*ReportDirectory) && !PlatformFile.CreateDirectoryTree(*ReportDirectory))
		{
			OutError = FString::Printf(TEXT("Failed to create report directory: %s"), *ReportDirectory);
			return false;
		}

		const FString ReportText = FString::Printf(
			TEXT("CodexHarnessHeadlessSetup succeeded.\n")
			TEXT("VOX Sources:\n")
			TEXT("  Player: %s\n")
			TEXT("  Enemy: %s\n")
			TEXT("Material: %s\n")
			TEXT("StaticMeshes:\n")
			TEXT("  Player: %s\n")
			TEXT("  Enemy: %s\n")
			TEXT("Blueprints:\n")
			TEXT("  GameInstance: %s\n")
			TEXT("  GameMode: %s\n")
			TEXT("  PlayerController: %s\n")
			TEXT("  Character: %s\n")
			TEXT("  EnemyCharacter: %s\n")
			TEXT("  HUD: %s\n")
			TEXT("Input:\n")
			TEXT("  MoveAction: %s\n")
			TEXT("  LookAction: %s\n")
			TEXT("  FireAction: %s\n")
			TEXT("  RestartAction: %s\n")
			TEXT("  MappingContext: %s\n")
			TEXT("  InputConfig: %s\n")
			TEXT("Effects:\n")
			TEXT("  PlayerHitReactionSystem: %s\n")
			TEXT("  PlayerHitCameraShake: %s\n")
			TEXT("  EffectsConfig: %s\n")
			TEXT("ProjectSettings:\n")
			TEXT("  GameInstanceClass: %s\n")
			TEXT("  GlobalDefaultGameMode: %s\n")
			TEXT("  GameDefaultMap: %s\n")
			TEXT("  EditorStartupMap: %s\n")
			TEXT("Map:\n")
			TEXT("  WorldSettings.DefaultGameMode: %s\n"),
			*Artifacts.PlayerVoxFilename,
			*Artifacts.EnemyVoxFilename,
			*VoxBaseMaterialPackagePath,
			*PlayerVoxMeshPackagePath,
			*EnemyVoxMeshPackagePath,
			*Artifacts.GameInstanceBlueprint->GetPathName(),
			*Artifacts.GameModeBlueprint->GetPathName(),
			*Artifacts.PlayerControllerBlueprint->GetPathName(),
			*Artifacts.CharacterBlueprint->GetPathName(),
			*Artifacts.EnemyCharacterBlueprint->GetPathName(),
			*Artifacts.HudBlueprint->GetPathName(),
			*Artifacts.MoveAction->GetPathName(),
			*Artifacts.LookAction->GetPathName(),
			*Artifacts.FireAction->GetPathName(),
			*Artifacts.RestartAction->GetPathName(),
			*Artifacts.MappingContext->GetPathName(),
			*Artifacts.InputConfig->GetPathName(),
			*Artifacts.PlayerHitReactionSystem->GetPathName(),
			*Artifacts.PlayerHitCameraShakeBlueprint->GetPathName(),
			*Artifacts.EffectsConfig->GetPathName(),
			*Artifacts.GameInstanceBlueprint->GeneratedClass->GetPathName(),
			*Artifacts.GameModeBlueprint->GeneratedClass->GetPathName(),
			*BasicMapPackagePath,
			*BasicMapObjectPath,
			*Artifacts.GameModeBlueprint->GeneratedClass->GetPathName());

		if (!FFileHelper::SaveStringToFile(ReportText, *ReportFilename, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
		{
			OutError = FString::Printf(TEXT("Failed to write report file: %s"), *ReportFilename);
			return false;
		}

		return true;
	}
}

UCodexHarnessHeadlessSetupCommandlet::UCodexHarnessHeadlessSetupCommandlet()
{
	LogToConsole = true;
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	HelpDescription = TEXT("Creates VOX assets, input assets, blueprint wrappers, and project defaults for CodexHarness.");
}

int32 UCodexHarnessHeadlessSetupCommandlet::Main(const FString& Params)
{
	static_cast<void>(Params);

	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
	AssetRegistry.SearchAllAssets(true);
	AssetRegistry.WaitForCompletion();

	FHeadlessSetupArtifacts Artifacts;
	FString ErrorMessage;
	if (!WriteGeneratedVoxFile(PlayerVoxSourceRelativePath, BuildPlayerChickenVoxModel(), Artifacts.PlayerVoxFilename, ErrorMessage))
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	if (!WriteGeneratedVoxFile(EnemyVoxSourceRelativePath, BuildEnemyChickenVoxModel(), Artifacts.EnemyVoxFilename, ErrorMessage))
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	Artifacts.VoxBaseMaterial = LoadOrCreateVoxBaseMaterial(ErrorMessage);
	if (Artifacts.VoxBaseMaterial == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	Artifacts.PlayerVoxMesh = ImportOrUpdateVoxStaticMesh(Artifacts.PlayerVoxFilename, PlayerVoxMeshPackagePath, Artifacts.VoxBaseMaterial, ErrorMessage);
	if (Artifacts.PlayerVoxMesh == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	Artifacts.EnemyVoxMesh = ImportOrUpdateVoxStaticMesh(Artifacts.EnemyVoxFilename, EnemyVoxMeshPackagePath, Artifacts.VoxBaseMaterial, ErrorMessage);
	if (Artifacts.EnemyVoxMesh == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	if (!ConfigureInputAssets(Artifacts, ErrorMessage))
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	if (!ConfigureEffectsAssets(Artifacts, ErrorMessage))
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	if (!ConfigureBlueprintAssets(Artifacts, ErrorMessage))
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	TArray<UPackage*> PackagesToSave;
	AddPackageIfValid(PackagesToSave, Artifacts.MoveAction);
	AddPackageIfValid(PackagesToSave, Artifacts.LookAction);
	AddPackageIfValid(PackagesToSave, Artifacts.FireAction);
	AddPackageIfValid(PackagesToSave, Artifacts.RestartAction);
	AddPackageIfValid(PackagesToSave, Artifacts.MappingContext);
	AddPackageIfValid(PackagesToSave, Artifacts.InputConfig);
	AddPackageIfValid(PackagesToSave, Artifacts.PlayerHitReactionSystem);
	AddPackageIfValid(PackagesToSave, Artifacts.PlayerHitCameraShakeBlueprint);
	AddPackageIfValid(PackagesToSave, Artifacts.EffectsConfig);
	AddPackageIfValid(PackagesToSave, Artifacts.GameInstanceBlueprint);
	AddPackageIfValid(PackagesToSave, Artifacts.GameModeBlueprint);
	AddPackageIfValid(PackagesToSave, Artifacts.PlayerControllerBlueprint);
	AddPackageIfValid(PackagesToSave, Artifacts.CharacterBlueprint);
	AddPackageIfValid(PackagesToSave, Artifacts.EnemyCharacterBlueprint);
	AddPackageIfValid(PackagesToSave, Artifacts.HudBlueprint);
	if (!SavePackages(PackagesToSave, ErrorMessage))
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	if (!ApplyProjectSettingsAndMap(Artifacts, ErrorMessage))
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	if (!ValidateArtifacts(Artifacts, ErrorMessage))
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	if (!WriteReport(Artifacts, ErrorMessage))
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	UE_LOG(LogTemp, Display, TEXT("CodexHarness headless setup completed successfully."));
	UE_LOG(LogTemp, Display, TEXT("Player VOX Source: %s"), *Artifacts.PlayerVoxFilename);
	UE_LOG(LogTemp, Display, TEXT("Enemy VOX Source: %s"), *Artifacts.EnemyVoxFilename);
	UE_LOG(LogTemp, Display, TEXT("Material: %s"), *VoxBaseMaterialPackagePath);
	UE_LOG(LogTemp, Display, TEXT("Player StaticMesh: %s"), *PlayerVoxMeshPackagePath);
	UE_LOG(LogTemp, Display, TEXT("Enemy StaticMesh: %s"), *EnemyVoxMeshPackagePath);
	UE_LOG(LogTemp, Display, TEXT("GameInstance Blueprint: %s"), *Artifacts.GameInstanceBlueprint->GetPathName());
	UE_LOG(LogTemp, Display, TEXT("GameMode Blueprint: %s"), *Artifacts.GameModeBlueprint->GetPathName());
	UE_LOG(LogTemp, Display, TEXT("PlayerController Blueprint: %s"), *Artifacts.PlayerControllerBlueprint->GetPathName());
	UE_LOG(LogTemp, Display, TEXT("Character Blueprint: %s"), *Artifacts.CharacterBlueprint->GetPathName());
	UE_LOG(LogTemp, Display, TEXT("EnemyCharacter Blueprint: %s"), *Artifacts.EnemyCharacterBlueprint->GetPathName());
	UE_LOG(LogTemp, Display, TEXT("HUD Blueprint: %s"), *Artifacts.HudBlueprint->GetPathName());
	UE_LOG(LogTemp, Display, TEXT("InputConfig: %s"), *Artifacts.InputConfig->GetPathName());
	UE_LOG(LogTemp, Display, TEXT("PlayerHitReactionSystem: %s"), *Artifacts.PlayerHitReactionSystem->GetPathName());
	UE_LOG(LogTemp, Display, TEXT("PlayerHitCameraShake: %s"), *Artifacts.PlayerHitCameraShakeBlueprint->GetPathName());
	UE_LOG(LogTemp, Display, TEXT("EffectsConfig: %s"), *Artifacts.EffectsConfig->GetPathName());
	return 0;
}
