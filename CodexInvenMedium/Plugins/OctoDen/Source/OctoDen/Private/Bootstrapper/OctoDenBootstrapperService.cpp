#include "Bootstrapper/OctoDenBootstrapperService.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Bootstrapper/OctoDenBootstrapperSettings.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "Engine/Blueprint.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "FileHelpers.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/WorldSettings.h"
#include "GameMapsSettings.h"
#include "GameProjectUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/PackageName.h"
#include "Shared/OctoDenAssetNaming.h"
#include "UObject/Package.h"

namespace
{
	const TCHAR* ManagedMapsFolder = TEXT("/Game/Maps");
	const TCHAR* BasicTemplateMapPackage = TEXT("/Engine/Maps/Templates/Template_Default");
	const TCHAR* TimeOfDayTemplateMapPackage = TEXT("/Engine/Maps/Templates/TimeOfDay_Default");

	bool SaveDirtyMapsForTransition(FText& OutFailure)
	{
		if (!FEditorFileUtils::SaveDirtyPackages(true, true, false, false, false, true))
		{
			OutFailure = NSLOCTEXT("OctoDenBootstrapper", "SaveDirtyMapsCancelled", "Map transition was canceled while saving dirty maps.");
			return false;
		}
		return true;
	}

	bool LoadMapIntoEditor(const FString& MapPackageName, FText& OutFailure)
	{
		const FString MapFilename = FPackageName::LongPackageNameToFilename(MapPackageName, FPackageName::GetMapPackageExtension());
		if (!FEditorFileUtils::LoadMap(MapFilename, false, true))
		{
			OutFailure = FText::Format(
				NSLOCTEXT("OctoDenBootstrapper", "LoadMapFailed", "Failed to load map '{0}' into the editor."),
				FText::FromString(MapPackageName));
			return false;
		}
		return true;
	}

	bool SavePackages(const TArray<UPackage*>& Packages, FText& OutFailure)
	{
		TArray<UPackage*> PackagesToSave;
		for (UPackage* Package : Packages)
		{
			if (Package != nullptr)
			{
				PackagesToSave.AddUnique(Package);
			}
		}

		if (PackagesToSave.IsEmpty())
		{
			return true;
		}

		if (FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, false, false) != FEditorFileUtils::PR_Success)
		{
			OutFailure = NSLOCTEXT("OctoDenBootstrapper", "SaveGeneratedPackagesFailed", "Failed to save one or more generated assets.");
			return false;
		}
		return true;
	}

	bool CreateManagedMapWorld(const EOctoDenManagedMapTemplate TemplateKind, UWorld*& OutWorld, FText& OutFailure)
	{
		switch (TemplateKind)
		{
		case EOctoDenManagedMapTemplate::Blank:
			OutWorld = UEditorLoadingAndSavingUtils::NewBlankMap(false);
			break;
		case EOctoDenManagedMapTemplate::TimeOfDay:
			OutWorld = UEditorLoadingAndSavingUtils::NewMapFromTemplate(TimeOfDayTemplateMapPackage, false);
			break;
		case EOctoDenManagedMapTemplate::Basic:
		default:
			OutWorld = UEditorLoadingAndSavingUtils::NewMapFromTemplate(BasicTemplateMapPackage, false);
			break;
		}

		if (OutWorld == nullptr)
		{
			OutFailure = NSLOCTEXT("OctoDenBootstrapper", "ManagedMapCreationFailed", "Unable to create the requested managed map template.");
			return false;
		}
		return true;
	}

	bool ResolveManagedMapPath(const UOctoDenBootstrapperSettings& Settings, FString& OutPackageName, FText& OutFailure)
	{
		const FString AssetName = OctoDenAssetNaming::SanitizeAssetName(Settings.ManagedMapName, TEXT("Main"));
		OutPackageName = FString(ManagedMapsFolder) / AssetName;
		if (!FPackageName::IsValidLongPackageName(OutPackageName))
		{
			OutFailure = FText::Format(
				NSLOCTEXT("OctoDenBootstrapper", "InvalidManagedMapPath", "Managed map path '{0}' is not a valid package path."),
				FText::FromString(OutPackageName));
			return false;
		}
		return true;
	}

	bool ResolveModuleInfo(const FString& ModuleName, FModuleContextInfo& OutModuleInfo)
	{
		const TArray<FModuleContextInfo>& Modules = GameProjectUtils::GetCurrentProjectModules();
		for (const FModuleContextInfo& ModuleInfo : Modules)
		{
			if (ModuleInfo.ModuleName == ModuleName)
			{
				OutModuleInfo = ModuleInfo;
				return true;
			}
		}
		return false;
	}

	UClass* FindProjectClass(const FString& ModuleName, const FString& RequestedClassName)
	{
		const FString NativePath = FString::Printf(TEXT("/Script/%s.%s"), *ModuleName, *RequestedClassName);
		return LoadObject<UClass>(nullptr, *NativePath);
	}

	bool ResolveOrCreateNativeClass(const UOctoDenBootstrapperSettings& Settings, const FString& RequestedClassName, UClass* ParentClass, UClass*& OutClass, FText& OutFailure)
	{
		OutClass = FindProjectClass(Settings.RuntimeModuleName, RequestedClassName);
		if (OutClass != nullptr)
		{
			if (!OutClass->IsChildOf(ParentClass))
			{
				OutFailure = FText::Format(
					NSLOCTEXT("OctoDenBootstrapper", "ExistingNativeClassWrongParent", "Existing class '{0}' is not derived from '{1}'."),
					FText::FromString(RequestedClassName),
					FText::FromString(ParentClass->GetName()));
				return false;
			}
			return true;
		}

		FModuleContextInfo ModuleInfo;
		if (!ResolveModuleInfo(Settings.RuntimeModuleName, ModuleInfo))
		{
			OutFailure = FText::Format(
				NSLOCTEXT("OctoDenBootstrapper", "RuntimeModuleNotFound", "Runtime module '{0}' could not be resolved from the current project."),
				FText::FromString(Settings.RuntimeModuleName));
			return false;
		}

		FString HeaderFilePath;
		FString SourceFilePath;
		GameProjectUtils::EReloadStatus ReloadStatus = GameProjectUtils::EReloadStatus::NotReloaded;
		const GameProjectUtils::EAddCodeToProjectResult Result = GameProjectUtils::AddCodeToProject(
			RequestedClassName,
			ModuleInfo.ModuleSourcePath,
			ModuleInfo,
			FNewClassInfo(ParentClass),
			TSet<FString>(),
			HeaderFilePath,
			SourceFilePath,
			OutFailure,
			ReloadStatus);

		if (Result != GameProjectUtils::EAddCodeToProjectResult::Succeeded)
		{
			return false;
		}

		OutClass = FindProjectClass(Settings.RuntimeModuleName, RequestedClassName);
		if (OutClass == nullptr)
		{
			OutFailure = FText::Format(
				NSLOCTEXT("OctoDenBootstrapper", "NativeClassNotLoadedYet", "Generated class '{0}' is not loaded yet. Wait for compilation or hot reload to finish, then try Apply again."),
				FText::FromString(RequestedClassName));
			return false;
		}
		return true;
	}

	bool ResolveOrCreateBlueprint(const FString& Folder, const FString& AssetName, UClass* ParentClass, UBlueprint*& OutBlueprint, FText& OutFailure)
	{
		const FString SafeFolder = OctoDenAssetNaming::NormalizePackageFolder(Folder, TEXT("/Game/Blueprints"));
		const FString SafeAssetName = OctoDenAssetNaming::SanitizeAssetName(AssetName, TEXT("BP_Generated"));
		const FString ObjectPath = OctoDenAssetNaming::BuildObjectPath(SafeFolder, SafeAssetName);

		OutBlueprint = LoadObject<UBlueprint>(nullptr, *ObjectPath);
		if (OutBlueprint != nullptr)
		{
			UClass* ExistingParent = OutBlueprint->GeneratedClass != nullptr ? OutBlueprint->GeneratedClass->GetSuperClass() : nullptr;
			if (ExistingParent == nullptr || !ExistingParent->IsChildOf(ParentClass))
			{
				OutFailure = FText::Format(
					NSLOCTEXT("OctoDenBootstrapper", "BlueprintWrongParent", "Blueprint '{0}' does not inherit from '{1}'."),
					FText::FromString(ObjectPath),
					FText::FromString(ParentClass->GetName()));
				return false;
			}
			return true;
		}

		UPackage* Package = CreatePackage(*OctoDenAssetNaming::BuildPackagePath(SafeFolder, SafeAssetName));
		if (Package == nullptr)
		{
			OutFailure = FText::Format(
				NSLOCTEXT("OctoDenBootstrapper", "CreateBlueprintPackageFailed", "Failed to create package for blueprint '{0}'."),
				FText::FromString(SafeAssetName));
			return false;
		}

		Package->FullyLoad();
		OutBlueprint = FKismetEditorUtilities::CreateBlueprint(ParentClass, Package, *SafeAssetName, BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), FName(TEXT("OctoDenBootstrapper")));
		if (OutBlueprint == nullptr)
		{
			OutFailure = FText::Format(
				NSLOCTEXT("OctoDenBootstrapper", "CreateBlueprintFailed", "Failed to create blueprint '{0}'."),
				FText::FromString(SafeAssetName));
			return false;
		}

		FKismetEditorUtilities::CompileBlueprint(OutBlueprint);
		FAssetRegistryModule::AssetCreated(OutBlueprint);
		OutBlueprint->MarkPackageDirty();
		Package->MarkPackageDirty();
		return true;
	}
}

void OctoDenBootstrapper::PopulateSuggestedDefaults(UOctoDenBootstrapperSettings& Settings)
{
	Settings.DeriveDefaultsFromProject();
}

bool OctoDenBootstrapper::CreateManagedMap(UOctoDenBootstrapperSettings& Settings, FText& OutFailure)
{
	FString MapPackageName;
	if (!ResolveManagedMapPath(Settings, MapPackageName, OutFailure))
	{
		return false;
	}

	if (FPackageName::DoesPackageExist(MapPackageName))
	{
		OutFailure = FText::Format(NSLOCTEXT("OctoDenBootstrapper", "ManagedMapAlreadyExists", "Map '{0}' already exists."), FText::FromString(MapPackageName));
		return false;
	}

	if (!SaveDirtyMapsForTransition(OutFailure))
	{
		return false;
	}

	UWorld* NewWorld = nullptr;
	if (!CreateManagedMapWorld(Settings.ManagedMapTemplate, NewWorld, OutFailure))
	{
		return false;
	}

	if (!UEditorLoadingAndSavingUtils::SaveMap(NewWorld, MapPackageName))
	{
		OutFailure = FText::Format(NSLOCTEXT("OctoDenBootstrapper", "SaveManagedMapFailed", "Failed to save managed map '{0}'."), FText::FromString(MapPackageName));
		return false;
	}

	const FString MapObjectPath = FString::Printf(TEXT("%s.%s"), *MapPackageName, *FPackageName::GetShortName(MapPackageName));
	Settings.TargetMap = FSoftObjectPath(MapObjectPath);
	Settings.EditorStartupMap = Settings.TargetMap;
	Settings.GameDefaultMap = Settings.TargetMap;
	Settings.LastStatus = FText::Format(NSLOCTEXT("OctoDenBootstrapper", "ManagedMapCreated", "Created managed map '{0}'."), FText::FromString(MapPackageName));
	return true;
}

bool OctoDenBootstrapper::OpenManagedMap(UOctoDenBootstrapperSettings& Settings, FText& OutFailure)
{
	FString MapPackageName;
	if (!ResolveManagedMapPath(Settings, MapPackageName, OutFailure))
	{
		return false;
	}

	if (!FPackageName::DoesPackageExist(MapPackageName))
	{
		OutFailure = FText::Format(NSLOCTEXT("OctoDenBootstrapper", "ManagedMapMissing", "Map '{0}' does not exist."), FText::FromString(MapPackageName));
		return false;
	}

	if (!SaveDirtyMapsForTransition(OutFailure))
	{
		return false;
	}

	if (!LoadMapIntoEditor(MapPackageName, OutFailure))
	{
		return false;
	}

	Settings.LastStatus = FText::Format(NSLOCTEXT("OctoDenBootstrapper", "ManagedMapOpened", "Opened managed map '{0}'."), FText::FromString(MapPackageName));
	return true;
}

bool OctoDenBootstrapper::GenerateNativeClasses(UOctoDenBootstrapperSettings& Settings, FText& OutFailure)
{
	if (Settings.RuntimeModuleName.TrimStartAndEnd().IsEmpty())
	{
		OutFailure = NSLOCTEXT("OctoDenBootstrapper", "MissingRuntimeModule", "Runtime module name is required.");
		return false;
	}

	Settings.GameInstanceClassName = OctoDenAssetNaming::ToPascalIdentifier(Settings.GameInstanceClassName, TEXT("CodexInvenMediumGameInstance"));
	Settings.GameModeClassName = OctoDenAssetNaming::ToPascalIdentifier(Settings.GameModeClassName, TEXT("CodexInvenMediumMainGameMode"));

	UClass* GameInstanceClass = nullptr;
	if (!ResolveOrCreateNativeClass(Settings, Settings.GameInstanceClassName, UGameInstance::StaticClass(), GameInstanceClass, OutFailure))
	{
		return false;
	}

	UClass* GameModeClass = nullptr;
	if (!ResolveOrCreateNativeClass(Settings, Settings.GameModeClassName, AGameModeBase::StaticClass(), GameModeClass, OutFailure))
	{
		return false;
	}

	Settings.LastStatus = FText::Format(NSLOCTEXT("OctoDenBootstrapper", "GenerateCodeSucceeded", "Native classes are available: {0}, {1}."), FText::FromString(Settings.GameInstanceClassName), FText::FromString(Settings.GameModeClassName));
	return true;
}

bool OctoDenBootstrapper::AreNativeClassesReady(const UOctoDenBootstrapperSettings& Settings)
{
	return FindProjectClass(Settings.RuntimeModuleName, Settings.GameInstanceClassName) != nullptr
		&& FindProjectClass(Settings.RuntimeModuleName, Settings.GameModeClassName) != nullptr;
}

bool OctoDenBootstrapper::CreateBlueprintsAndApply(UOctoDenBootstrapperSettings& Settings, FText& OutFailure)
{
	UClass* GameInstanceClass = FindProjectClass(Settings.RuntimeModuleName, Settings.GameInstanceClassName);
	UClass* GameModeClass = FindProjectClass(Settings.RuntimeModuleName, Settings.GameModeClassName);
	if (GameInstanceClass == nullptr || GameModeClass == nullptr)
	{
		OutFailure = NSLOCTEXT("OctoDenBootstrapper", "NativeClassesNotReady", "Generated classes are not loaded yet. Wait for compilation or hot reload, then try again.");
		return false;
	}

	UBlueprint* GameInstanceBlueprint = nullptr;
	if (!ResolveOrCreateBlueprint(Settings.GameInstanceBlueprintFolder, Settings.GameInstanceBlueprintName, GameInstanceClass, GameInstanceBlueprint, OutFailure))
	{
		return false;
	}

	UBlueprint* GameModeBlueprint = nullptr;
	if (!ResolveOrCreateBlueprint(Settings.GameModeBlueprintFolder, Settings.GameModeBlueprintName, GameModeClass, GameModeBlueprint, OutFailure))
	{
		return false;
	}

	const FString MapPackageName = Settings.TargetMap.GetLongPackageName();
	if (MapPackageName.IsEmpty())
	{
		OutFailure = NSLOCTEXT("OctoDenBootstrapper", "TargetMapMissing", "Target map must be selected before Apply.");
		return false;
	}

	if (!LoadMapIntoEditor(MapPackageName, OutFailure))
	{
		return false;
	}

	UWorld* World = GEditor != nullptr ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (World == nullptr)
	{
		OutFailure = NSLOCTEXT("OctoDenBootstrapper", "EditorWorldMissing", "Editor world could not be resolved after loading the target map.");
		return false;
	}

	AWorldSettings* WorldSettings = World->GetWorldSettings();
	if (WorldSettings == nullptr)
	{
		OutFailure = NSLOCTEXT("OctoDenBootstrapper", "WorldSettingsMissing", "Loaded map does not expose valid world settings.");
		return false;
	}

	WorldSettings->Modify();
	WorldSettings->DefaultGameMode = GameModeBlueprint->GeneratedClass;
	World->MarkPackageDirty();

	UGameMapsSettings* GameMapsSettings = GetMutableDefault<UGameMapsSettings>();
	GameMapsSettings->Modify();
	GameMapsSettings->GameInstanceClass = FSoftClassPath(GameInstanceBlueprint->GeneratedClass->GetPathName());
	if (!Settings.EditorStartupMap.GetLongPackageName().IsEmpty())
	{
		GameMapsSettings->EditorStartupMap = Settings.EditorStartupMap;
	}
	if (!Settings.GameDefaultMap.GetLongPackageName().IsEmpty())
	{
		UGameMapsSettings::SetGameDefaultMap(Settings.GameDefaultMap.GetLongPackageName());
	}
	if (Settings.bSetGlobalDefaultGameMode)
	{
		UGameMapsSettings::SetGlobalDefaultGameMode(GameModeBlueprint->GeneratedClass->GetPathName());
	}
	GameMapsSettings->SaveConfig();

	TArray<UPackage*> PackagesToSave;
	PackagesToSave.Add(GameInstanceBlueprint->GetOutermost());
	PackagesToSave.Add(GameModeBlueprint->GetOutermost());
	PackagesToSave.Add(World->GetOutermost());
	if (!SavePackages(PackagesToSave, OutFailure))
	{
		return false;
	}

	Settings.LastStatus = NSLOCTEXT("OctoDenBootstrapper", "ApplySucceeded", "Blueprint assets and project settings were applied.");
	return true;
}
