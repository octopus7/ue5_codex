#include "OctoDenModule.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "Engine/Blueprint.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "FileHelpers.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/Notifications/NotificationManager.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/WorldSettings.h"
#include "GameMapsSettings.h"
#include "GameProjectUtils.h"
#include "IDetailsView.h"
#include "InputAction.h"
#include "InputEditorModule.h"
#include "InputMappingContext.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/App.h"
#include "Misc/MessageDialog.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "ModuleDescriptor.h"
#include "Modules/ModuleManager.h"
#include "OctoDenBootstrapperSettings.h"
#include "OctoDenBootstrapperSettingsCustomization.h"
#include "OctoDenInputBuilderSettings.h"
#include "PropertyEditorModule.h"
#include "Styling/AppStyle.h"
#include "ToolMenus.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/StrongObjectPtr.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

DEFINE_LOG_CATEGORY_STATIC(LogOctoDen, Log, All);

#define LOCTEXT_NAMESPACE "OctoDen"

namespace OctoDen
{
	const FName BootstrapperTabId(TEXT("OctoDen.Bootstrapper"));
	const FName InputBuilderTabId(TEXT("OctoDen.InputBuilder"));
	const TCHAR* LevelEditorMainMenu = TEXT("LevelEditor.MainMenu");
	const TCHAR* OctoDenMainMenu = TEXT("LevelEditor.MainMenu.OctoDen");
	const float FooterButtonHeight = 56.0f;

	FString TrimTrailingSlash(FString InValue)
	{
		while (InValue.EndsWith(TEXT("/")))
		{
			InValue.LeftChopInline(1, EAllowShrinking::No);
		}

		return InValue;
	}

	FString NormalizeAssetFolder(FString InFolder)
	{
		InFolder.TrimStartAndEndInline();
		InFolder.ReplaceInline(TEXT("\\"), TEXT("/"));
		return TrimTrailingSlash(MoveTemp(InFolder));
	}

	FString SanitizeAssetName(FString InValue, const FString& FallbackName)
	{
		InValue.TrimStartAndEndInline();

		FString Result;
		bool bLastCharacterWasSeparator = false;

		for (const TCHAR Character : InValue)
		{
			if (FChar::IsAlnum(Character) || Character == TEXT('_'))
			{
				Result.AppendChar(Character);
				bLastCharacterWasSeparator = false;
			}
			else if ((FChar::IsWhitespace(Character) || Character == TEXT('-')) && !bLastCharacterWasSeparator && !Result.IsEmpty())
			{
				Result.AppendChar(TEXT('_'));
				bLastCharacterWasSeparator = true;
			}
		}

		while (Result.EndsWith(TEXT("_")))
		{
			Result.LeftChopInline(1, EAllowShrinking::No);
		}

		if (Result.IsEmpty())
		{
			Result = FallbackName;
		}

		if (!Result.IsEmpty() && FChar::IsDigit(Result[0]))
		{
			Result = TEXT("Generated") + Result;
		}

		return Result;
	}

	FString ToPascalCase(const FString& InValue)
	{
		FString Result;
		bool bUpperNext = true;

		for (const TCHAR Character : InValue)
		{
			if (FChar::IsAlnum(Character))
			{
				Result.AppendChar(bUpperNext ? FChar::ToUpper(Character) : Character);
				bUpperNext = false;
			}
			else
			{
				bUpperNext = true;
			}
		}

		if (Result.IsEmpty())
		{
			Result = TEXT("Generated");
		}

		if (FChar::IsDigit(Result[0]))
		{
			Result = TEXT("Generated") + Result;
		}

		return Result;
	}

	FString NormalizeAssetPrefix(FString InPrefix)
	{
		InPrefix.TrimStartAndEndInline();
		InPrefix.ReplaceInline(TEXT(" "), TEXT("_"));
		InPrefix.ReplaceInline(TEXT("-"), TEXT("_"));

		FString Result;
		for (const TCHAR Character : InPrefix)
		{
			if (FChar::IsAlnum(Character) || Character == TEXT('_'))
			{
				Result.AppendChar(Character);
			}
		}

		if (!Result.IsEmpty() && !Result.EndsWith(TEXT("_")))
		{
			Result.Append(TEXT("_"));
		}

		return Result;
	}

	FString MakePrefixedAssetName(const FString& InValue, const FString& InPrefix, const FString& FallbackStem)
	{
		const FString NormalizedPrefix = NormalizeAssetPrefix(InPrefix);
		FString WorkingValue = InValue.TrimStartAndEnd();

		if (!NormalizedPrefix.IsEmpty() && WorkingValue.StartsWith(NormalizedPrefix, ESearchCase::IgnoreCase))
		{
			WorkingValue.RightChopInline(NormalizedPrefix.Len(), EAllowShrinking::No);
		}

		return NormalizedPrefix + ToPascalCase(WorkingValue.IsEmpty() ? FallbackStem : WorkingValue);
	}

	FString MakePackagePath(const FString& Folder, const FString& AssetName)
	{
		return NormalizeAssetFolder(Folder) / AssetName;
	}

	FString MakeObjectPath(const FString& Folder, const FString& AssetName)
	{
		const FString PackagePath = MakePackagePath(Folder, AssetName);
		return PackagePath + TEXT(".") + AssetName;
	}

	bool IsValidAssetFolder(const FString& Folder)
	{
		const FString NormalizedFolder = NormalizeAssetFolder(Folder);
		return !NormalizedFolder.IsEmpty() && FPackageName::IsValidLongPackageName(NormalizedFolder);
	}

	bool SavePackages(const TArray<UPackage*>& InPackages, FText& OutFailReason)
	{
		TArray<UPackage*> PackagesToSave;
		for (UPackage* Package : InPackages)
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

		const FEditorFileUtils::EPromptReturnCode Result = FEditorFileUtils::PromptForCheckoutAndSave(
			PackagesToSave,
			/*bCheckDirty=*/false,
			/*bPromptToSave=*/false);

		if (Result != FEditorFileUtils::PR_Success)
		{
			OutFailReason = LOCTEXT("SavePackagesFailed", "Failed to save one or more generated assets or maps.");
			return false;
		}

		return true;
	}

	FText GetBootstrapperHelpText()
	{
		return LOCTEXT(
			"BootstrapperHelpText",
			"Workflow\n"
			"1. Create or open a managed map under /Game/Maps.\n"
			"2. Confirm the runtime module and the generated class names.\n"
			"3. Click Generate Code.\n"
			"4. Wait for compilation or hot reload to finish.\n"
			"5. Click Create Blueprints & Apply to update the selected map and project defaults.");
	}
}

namespace OctoDenBootstrapper
{
	const TCHAR* ManagedMapFolder = TEXT("/Game/Maps");
	const TCHAR* BasicTemplateMapPackage = TEXT("/Engine/Maps/Templates/Template_Default");
	const TCHAR* TimeOfDayTemplateMapPackage = TEXT("/Engine/Maps/Templates/TimeOfDay_Default");

	FString StripLevelPrefix(const FString& InMapName)
	{
		if (InMapName.StartsWith(TEXT("L_")))
		{
			return InMapName.RightChop(2);
		}

		if (InMapName.StartsWith(TEXT("LV_")))
		{
			return InMapName.RightChop(3);
		}

		return InMapName;
	}

	bool IsTemporaryMapName(const FString& InMapPackageName, const FString& InMapAssetName)
	{
		if (InMapPackageName.StartsWith(TEXT("/Temp/")) || InMapPackageName.StartsWith(TEXT("/Engine/Transient")))
		{
			return true;
		}

		return InMapAssetName.StartsWith(TEXT("Untitled"), ESearchCase::IgnoreCase);
	}

	FString MakeDefaultGameModeStem(const FString& ProjectName, const FString& MapPackageName)
	{
		const FString RawMapAssetName = MapPackageName.IsEmpty() ? FString() : FPackageName::GetShortName(MapPackageName);
		const bool bUseProjectNameOnly = RawMapAssetName.IsEmpty() || IsTemporaryMapName(MapPackageName, RawMapAssetName);
		if (bUseProjectNameOnly)
		{
			return ProjectName;
		}

		const FString MapBaseName = OctoDen::ToPascalCase(StripLevelPrefix(RawMapAssetName));
		if (MapBaseName.IsEmpty() || MapBaseName.Equals(ProjectName, ESearchCase::IgnoreCase))
		{
			return ProjectName;
		}

		return ProjectName + MapBaseName;
	}

	FString GetMapPackageName(const TSoftObjectPtr<UWorld>& MapReference)
	{
		return MapReference.ToSoftObjectPath().GetLongPackageName();
	}

	bool DoesTargetMapExist(const UOctoDenBootstrapperSettings& InSettings)
	{
		const FString TargetMapPackageName = GetMapPackageName(InSettings.TargetMap);
		return !TargetMapPackageName.IsEmpty() && FPackageName::DoesPackageExist(TargetMapPackageName);
	}

	FString GetCurrentEditorMapPackageName()
	{
		if (GEditor == nullptr)
		{
			return FString();
		}

		if (UWorld* World = GEditor->GetEditorWorldContext().World())
		{
			return World->GetOutermost()->GetName();
		}

		return FString();
	}

	FSoftObjectPath MakeMapSoftPath(const FString& MapPackageName)
	{
		if (MapPackageName.IsEmpty())
		{
			return FSoftObjectPath();
		}

		const FString MapAssetName = FPackageName::GetShortName(MapPackageName);
		return FSoftObjectPath(MapPackageName + TEXT(".") + MapAssetName);
	}

	bool ResolveRuntimeModule(const FString& PreferredModuleName, FModuleContextInfo& OutModuleInfo)
	{
		const TArray<FModuleContextInfo>& Modules = GameProjectUtils::GetCurrentProjectModules();
		const FString ProjectName = FApp::GetProjectName();
		const FModuleContextInfo* FirstRuntimeModule = nullptr;

		for (const FModuleContextInfo& ModuleInfo : Modules)
		{
			if (ModuleInfo.ModuleType != EHostType::Runtime)
			{
				continue;
			}

			if (FirstRuntimeModule == nullptr)
			{
				FirstRuntimeModule = &ModuleInfo;
			}

			if (!PreferredModuleName.IsEmpty() && ModuleInfo.ModuleName == PreferredModuleName)
			{
				OutModuleInfo = ModuleInfo;
				return true;
			}
		}

		if (!PreferredModuleName.IsEmpty())
		{
			return false;
		}

		for (const FModuleContextInfo& ModuleInfo : Modules)
		{
			if (ModuleInfo.ModuleType == EHostType::Runtime && ModuleInfo.ModuleName == ProjectName)
			{
				OutModuleInfo = ModuleInfo;
				return true;
			}
		}

		if (FirstRuntimeModule != nullptr)
		{
			OutModuleInfo = *FirstRuntimeModule;
			return true;
		}

		if (!ProjectName.IsEmpty())
		{
			OutModuleInfo.ModuleName = ProjectName;
			OutModuleInfo.ModuleType = EHostType::Runtime;
			OutModuleInfo.ModuleSourcePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("Source") / ProjectName / TEXT(""));
			return true;
		}

		return false;
	}

	UClass* FindProjectClass(const FString& ModuleName, const FString& ClassName)
	{
		const FString ClassPath = FString::Printf(TEXT("/Script/%s.%s"), *ModuleName, *ClassName);
		return LoadObject<UClass>(nullptr, *ClassPath);
	}

	bool TryLoadNativeClass(const FString& RequestedClassName, const UClass* ParentClass, const FModuleContextInfo& ModuleInfo, UClass*& OutClass, FText& OutFailReason)
	{
		OutClass = FindProjectClass(ModuleInfo.ModuleName, OctoDen::ToPascalCase(RequestedClassName));
		if (OutClass == nullptr)
		{
			OutFailReason = FText::Format(
				LOCTEXT("NativeClassNotReady", "Native class '{0}' is not loaded yet. Finish compiling or reloading module '{1}', then click 'Create Blueprints & Apply' again."),
				FText::FromString(RequestedClassName),
				FText::FromString(ModuleInfo.ModuleName));
			return false;
		}

		if (!OutClass->IsChildOf(ParentClass))
		{
			OutFailReason = FText::Format(
				LOCTEXT("NativeClassLoadedWrongParent", "Native class '{0}' is not derived from '{1}'."),
				FText::FromString(RequestedClassName),
				FText::FromString(ParentClass->GetName()));
			return false;
		}

		return true;
	}

	bool EnsureNativeClassExists(const FString& RequestedClassName, const UClass* ParentClass, const FModuleContextInfo& ModuleInfo, UClass*& OutClass, FText& OutFailReason)
	{
		OutClass = FindProjectClass(ModuleInfo.ModuleName, RequestedClassName);
		if (OutClass != nullptr)
		{
			if (!OutClass->IsChildOf(ParentClass))
			{
				OutFailReason = FText::Format(
					LOCTEXT("NativeClassWrongParent", "Existing class '{0}' is not derived from '{1}'."),
					FText::FromString(RequestedClassName),
					FText::FromString(ParentClass->GetName()));
				return false;
			}

			return true;
		}

		FString HeaderFilePath;
		FString CppFilePath;
		GameProjectUtils::EReloadStatus ReloadStatus = GameProjectUtils::EReloadStatus::NotReloaded;
		const GameProjectUtils::EAddCodeToProjectResult Result = GameProjectUtils::AddCodeToProject(
			RequestedClassName,
			ModuleInfo.ModuleSourcePath,
			ModuleInfo,
			FNewClassInfo(ParentClass),
			TSet<FString>(),
			HeaderFilePath,
			CppFilePath,
			OutFailReason,
			ReloadStatus);

		if (Result != GameProjectUtils::EAddCodeToProjectResult::Succeeded)
		{
			return false;
		}

		return true;
	}

	bool LoadExistingBlueprint(const FString& Folder, const FString& AssetName, UBlueprint*& OutBlueprint)
	{
		OutBlueprint = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), nullptr, *OctoDen::MakeObjectPath(Folder, AssetName)));
		return OutBlueprint != nullptr;
	}

	bool CreateOrReuseBlueprint(const FString& Folder, const FString& AssetName, UClass* ParentClass, UBlueprint*& OutBlueprint, FText& OutFailReason)
	{
		if (LoadExistingBlueprint(Folder, AssetName, OutBlueprint))
		{
			FKismetEditorUtilities::CompileBlueprint(OutBlueprint);

			if (OutBlueprint->GeneratedClass == nullptr || !OutBlueprint->GeneratedClass->IsChildOf(ParentClass))
			{
				OutFailReason = FText::Format(
					LOCTEXT("BlueprintWrongParent", "Existing blueprint '{0}' does not inherit from '{1}'."),
					FText::FromString(AssetName),
					FText::FromString(ParentClass->GetName()));
				return false;
			}

			return true;
		}

		const FString PackagePath = OctoDen::MakePackagePath(Folder, AssetName);
		if (!FPackageName::IsValidLongPackageName(PackagePath))
		{
			OutFailReason = FText::Format(
				LOCTEXT("InvalidBlueprintPackagePath", "Blueprint package path '{0}' is not valid."),
				FText::FromString(PackagePath));
			return false;
		}

		UPackage* Package = CreatePackage(*PackagePath);
		if (Package == nullptr)
		{
			OutFailReason = FText::Format(
				LOCTEXT("CreatePackageFailed", "Failed to create package '{0}'."),
				FText::FromString(PackagePath));
			return false;
		}

		Package->FullyLoad();

		OutBlueprint = FKismetEditorUtilities::CreateBlueprint(
			ParentClass,
			Package,
			*AssetName,
			BPTYPE_Normal,
			UBlueprint::StaticClass(),
			UBlueprintGeneratedClass::StaticClass(),
			FName(TEXT("OctoDenBootstrapper")));

		if (OutBlueprint == nullptr)
		{
			OutFailReason = FText::Format(
				LOCTEXT("CreateBlueprintFailed", "Failed to create blueprint '{0}'."),
				FText::FromString(AssetName));
			return false;
		}

		FKismetEditorUtilities::CompileBlueprint(OutBlueprint);
		FAssetRegistryModule::AssetCreated(OutBlueprint);
		OutBlueprint->MarkPackageDirty();
		Package->MarkPackageDirty();
		return true;
	}

	bool LoadMapIntoEditor(const FString& MapPackageName, FText& OutFailReason)
	{
		const FString MapFilename = FPackageName::LongPackageNameToFilename(MapPackageName, FPackageName::GetMapPackageExtension());
		if (!FEditorFileUtils::LoadMap(MapFilename, /*LoadAsTemplate=*/false, /*bShowProgress=*/true))
		{
			OutFailReason = FText::Format(
				LOCTEXT("LoadMapFailed", "Failed to load map '{0}'."),
				FText::FromString(MapPackageName));
			return false;
		}

		return true;
	}

	bool TryGetManagedMapPackageName(const UOctoDenBootstrapperSettings& InSettings, FString& OutMapPackageName, FString& OutManagedMapAssetName, FText& OutFailReason)
	{
		OutManagedMapAssetName = OctoDen::SanitizeAssetName(InSettings.ManagedMapName, TEXT("BasicMap"));
		OutMapPackageName = OctoDen::MakePackagePath(ManagedMapFolder, OutManagedMapAssetName);

		if (!FPackageName::IsValidLongPackageName(OutMapPackageName))
		{
			OutFailReason = FText::Format(
				LOCTEXT("InvalidManagedMapName", "Managed map '{0}' results in an invalid package path."),
				FText::FromString(InSettings.ManagedMapName));
			return false;
		}

		return true;
	}

	bool CreateManagedMapWorld(const EOctoDenManagedMapTemplate InTemplate, UWorld*& OutWorld, FText& OutFailReason)
	{
		switch (InTemplate)
		{
		case EOctoDenManagedMapTemplate::Blank:
			OutWorld = UEditorLoadingAndSavingUtils::NewBlankMap(/*bSaveExistingMap=*/false);
			if (OutWorld == nullptr)
			{
				OutFailReason = LOCTEXT("CreateBlankManagedMapFailed", "Failed to create a new Blank map.");
				return false;
			}
			return true;

		case EOctoDenManagedMapTemplate::TimeOfDay:
			OutWorld = UEditorLoadingAndSavingUtils::NewMapFromTemplate(TimeOfDayTemplateMapPackage, /*bSaveExistingMap=*/false);
			if (OutWorld == nullptr)
			{
				OutFailReason = FText::Format(
					LOCTEXT("CreateTimeOfDayManagedMapFailed", "Failed to create a new TimeOfDay map from template '{0}'."),
					FText::FromString(TimeOfDayTemplateMapPackage));
				return false;
			}
			return true;

		case EOctoDenManagedMapTemplate::Basic:
		default:
			OutWorld = UEditorLoadingAndSavingUtils::NewMapFromTemplate(BasicTemplateMapPackage, /*bSaveExistingMap=*/false);
			if (OutWorld == nullptr)
			{
				OutFailReason = FText::Format(
					LOCTEXT("CreateBasicManagedMapFailed", "Failed to create a new Basic map from template '{0}'."),
					FText::FromString(BasicTemplateMapPackage));
				return false;
			}
			return true;
		}
	}

	void ApplySuggestedGameModeNames(UOctoDenBootstrapperSettings& InSettings, const FString& MapPackageName)
	{
		const FString ProjectName = OctoDen::ToPascalCase(FApp::GetProjectName());
		const FString GameModeStem = MakeDefaultGameModeStem(ProjectName, MapPackageName);
		InSettings.GameModeClassName = GameModeStem + TEXT("GameMode");
		InSettings.GameModeBlueprintName = TEXT("BP_") + GameModeStem + TEXT("GameMode");
	}

	void ApplyManagedMapSelection(UOctoDenBootstrapperSettings& InSettings, const FString& MapPackageName)
	{
		const FSoftObjectPath MapPath = MakeMapSoftPath(MapPackageName);
		const TSoftObjectPtr<UWorld> MapReference(MapPath);

		InSettings.ManagedMapName = FPackageName::GetShortName(MapPackageName);
		InSettings.TargetMap = MapReference;
		InSettings.EditorStartupMap = MapReference;
		InSettings.GameDefaultMap = MapReference;
		ApplySuggestedGameModeNames(InSettings, MapPackageName);
	}

	bool SaveDirtyMapsForTransition(FText& OutFailReason)
	{
		if (!FEditorFileUtils::SaveDirtyPackages(
			/*bPromptUserToSave=*/true,
			/*bSaveMapPackages=*/true,
			/*bSaveContentPackages=*/false,
			/*bFastSave=*/false,
			/*bNotifyNoPackagesSaved=*/false,
			/*bCanBeDeclined=*/true))
		{
			OutFailReason = LOCTEXT("SaveDirtyMapsCancelled", "Create or Open was canceled while saving dirty maps.");
			return false;
		}

		return true;
	}

	bool ValidateCodeSettings(const UOctoDenBootstrapperSettings& InSettings, FText& OutFailReason)
	{
		if (InSettings.TargetRuntimeModule.TrimStartAndEnd().IsEmpty())
		{
			OutFailReason = LOCTEXT("MissingRuntimeModule", "Target runtime module is required.");
			return false;
		}

		if (InSettings.GameInstanceClassName.TrimStartAndEnd().IsEmpty() || InSettings.GameModeClassName.TrimStartAndEnd().IsEmpty())
		{
			OutFailReason = LOCTEXT("MissingNativeClassNames", "GameInstance and GameMode class names are required.");
			return false;
		}

		return true;
	}

	bool ValidateBlueprintSettings(const UOctoDenBootstrapperSettings& InSettings, FText& OutFailReason)
	{
		if (!ValidateCodeSettings(InSettings, OutFailReason))
		{
			return false;
		}

		if (GetMapPackageName(InSettings.TargetMap).IsEmpty())
		{
			OutFailReason = LOCTEXT("MissingTargetMap", "Target map is required.");
			return false;
		}

		if (GetMapPackageName(InSettings.EditorStartupMap).IsEmpty())
		{
			OutFailReason = LOCTEXT("MissingEditorStartupMap", "Editor Startup Map is required.");
			return false;
		}

		if (GetMapPackageName(InSettings.GameDefaultMap).IsEmpty())
		{
			OutFailReason = LOCTEXT("MissingGameDefaultMap", "Game Default Map is required.");
			return false;
		}

		if (InSettings.GameInstanceBlueprintName.TrimStartAndEnd().IsEmpty() || InSettings.GameModeBlueprintName.TrimStartAndEnd().IsEmpty())
		{
			OutFailReason = LOCTEXT("MissingBlueprintNames", "GameInstance and GameMode blueprint names are required.");
			return false;
		}

		if (!OctoDen::IsValidAssetFolder(InSettings.GameInstanceBlueprintFolder) || !OctoDen::IsValidAssetFolder(InSettings.GameModeBlueprintFolder))
		{
			OutFailReason = LOCTEXT("InvalidBlueprintFolder", "Blueprint folder paths must be valid long package names such as /Game/Blueprints/Core.");
			return false;
		}

		return true;
	}

	bool AreNativeClassesReady(const UOctoDenBootstrapperSettings& InSettings)
	{
		if (!DoesTargetMapExist(InSettings))
		{
			return false;
		}

		FText UnusedFailReason;
		if (!ValidateCodeSettings(InSettings, UnusedFailReason))
		{
			return false;
		}

		FModuleContextInfo RuntimeModule;
		if (!ResolveRuntimeModule(InSettings.TargetRuntimeModule.TrimStartAndEnd(), RuntimeModule))
		{
			return false;
		}

		UClass* NativeGameInstanceClass = nullptr;
		if (!TryLoadNativeClass(
			InSettings.GameInstanceClassName,
			UGameInstance::StaticClass(),
			RuntimeModule,
			NativeGameInstanceClass,
			UnusedFailReason))
		{
			return false;
		}

		UClass* NativeGameModeClass = nullptr;
		return TryLoadNativeClass(
			InSettings.GameModeClassName,
			AGameModeBase::StaticClass(),
			RuntimeModule,
			NativeGameModeClass,
			UnusedFailReason);
	}

	void PopulateDefaults(UOctoDenBootstrapperSettings& InSettings)
	{
		const FString ProjectName = OctoDen::ToPascalCase(FApp::GetProjectName());
		const FString DefaultManagedMapPackageName = OctoDen::MakePackagePath(ManagedMapFolder, TEXT("BasicMap"));
		const FString CurrentMapPackageName = GetCurrentEditorMapPackageName();
		const FString CurrentMapAssetName = CurrentMapPackageName.IsEmpty() ? FString() : FPackageName::GetShortName(CurrentMapPackageName);
		const bool bHasPersistentCurrentMap = !CurrentMapPackageName.IsEmpty() && !IsTemporaryMapName(CurrentMapPackageName, CurrentMapAssetName);
		FModuleContextInfo RuntimeModule;
		const bool bHasRuntimeModule = ResolveRuntimeModule(TEXT(""), RuntimeModule);

		InSettings.TargetRuntimeModule = bHasRuntimeModule ? RuntimeModule.ModuleName : FApp::GetProjectName();
		InSettings.ManagedMapName = TEXT("BasicMap");
		InSettings.GameInstanceClassName = ProjectName + TEXT("GameInstance");
		InSettings.GameInstanceBlueprintName = TEXT("BP_") + ProjectName + TEXT("GameInstance");

		if (FPackageName::DoesPackageExist(DefaultManagedMapPackageName))
		{
			ApplyManagedMapSelection(InSettings, DefaultManagedMapPackageName);
			return;
		}

		if (bHasPersistentCurrentMap)
		{
			ApplyManagedMapSelection(InSettings, CurrentMapPackageName);
			return;
		}

		InSettings.TargetMap.Reset();
		InSettings.EditorStartupMap.Reset();
		InSettings.GameDefaultMap.Reset();
		ApplySuggestedGameModeNames(InSettings, DefaultManagedMapPackageName);
	}
}

namespace OctoDenInputBuilder
{
	struct FBuildSummary
	{
		int32 CreatedInputActions = 0;
		int32 SkippedInputActions = 0;
		int32 FailedInputActions = 0;
		int32 AddedMappings = 0;
		bool bCreatedInputMappingContext = false;
		bool bSkippedInputMappingContext = false;
		TArray<FString> DetailLines;
		TArray<UPackage*> PackagesToSave;
	};

	void AddDetail(FBuildSummary& InSummary, const FString& InLine)
	{
		InSummary.DetailLines.Add(InLine);
	}

	bool HasAnyNamedRows(const UOctoDenInputBuilderSettings& InSettings)
	{
		for (const FOctoDenInputActionRow& Row : InSettings.Actions)
		{
			if (!Row.ActionName.TrimStartAndEnd().IsEmpty())
			{
				return true;
			}
		}

		return false;
	}

	FString BuildSummaryText(const FBuildSummary& InSummary)
	{
		FString Result = FString::Printf(
			TEXT("Input Builder completed.\nIMC Created: %s\nIMC Skipped: %s\nCreated IAs: %d\nSkipped IAs: %d\nFailed Rows: %d\nMappings Added: %d"),
			InSummary.bCreatedInputMappingContext ? TEXT("Yes") : TEXT("No"),
			InSummary.bSkippedInputMappingContext ? TEXT("Yes") : TEXT("No"),
			InSummary.CreatedInputActions,
			InSummary.SkippedInputActions,
			InSummary.FailedInputActions,
			InSummary.AddedMappings);

		if (!InSummary.DetailLines.IsEmpty())
		{
			Result.Append(TEXT("\n\nDetails\n"));
			for (const FString& Line : InSummary.DetailLines)
			{
				Result.Append(TEXT("- "));
				Result.Append(Line);
				Result.Append(TEXT("\n"));
			}
		}

		return Result;
	}

	bool CreateInputActionAsset(const FString& InFolder, const FString& InAssetName, const EInputActionValueType InValueType, UInputAction*& OutInputAction, FText& OutFailReason)
	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get();
		UInputAction_Factory* Factory = NewObject<UInputAction_Factory>();
		UObject* CreatedAsset = AssetTools.CreateAsset(InAssetName, InFolder, UInputAction::StaticClass(), Factory, FName(TEXT("OctoDenInputBuilder")));
		OutInputAction = Cast<UInputAction>(CreatedAsset);
		if (OutInputAction == nullptr)
		{
			OutFailReason = FText::Format(
				LOCTEXT("CreateInputActionFailed", "Failed to create Input Action '{0}'."),
				FText::FromString(InAssetName));
			return false;
		}

		OutInputAction->Modify();
		OutInputAction->ValueType = InValueType;
		OutInputAction->MarkPackageDirty();
		return true;
	}

	bool CreateInputMappingContextAsset(const FString& InFolder, const FString& InAssetName, UInputMappingContext*& OutInputMappingContext, FText& OutFailReason)
	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get();
		UInputMappingContext_Factory* Factory = NewObject<UInputMappingContext_Factory>();
		UObject* CreatedAsset = AssetTools.CreateAsset(InAssetName, InFolder, UInputMappingContext::StaticClass(), Factory, FName(TEXT("OctoDenInputBuilder")));
		OutInputMappingContext = Cast<UInputMappingContext>(CreatedAsset);
		if (OutInputMappingContext == nullptr)
		{
			OutFailReason = FText::Format(
				LOCTEXT("CreateInputMappingContextFailed", "Failed to create Input Mapping Context '{0}'."),
				FText::FromString(InAssetName));
			return false;
		}

		OutInputMappingContext->Modify();
		OutInputMappingContext->MarkPackageDirty();
		return true;
	}
}

void FOctoDenModule::StartupModule()
{
	ObjectPropertyChangedHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddLambda([this](UObject* Object, FPropertyChangedEvent&)
	{
		if (Object == BootstrapperSettings.Get())
		{
			RefreshBootstrapperDetails();
		}
		else if (Object == InputBuilderSettings.Get())
		{
			RefreshInputBuilderDetails();
		}
	});

	ReloadCompleteHandle = FCoreUObjectDelegates::ReloadCompleteDelegate.AddLambda([this](EReloadCompleteReason)
	{
		if (BootstrapperTab.IsValid())
		{
			RefreshBootstrapperDetails();
		}
	});

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	PropertyEditorModule.RegisterCustomClassLayout(
		UOctoDenBootstrapperSettings::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FOctoDenBootstrapperSettingsCustomization::MakeInstance));
	PropertyEditorModule.NotifyCustomizationModuleChanged();

	RegisterTabSpawners();
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FOctoDenModule::RegisterMenus));
}

void FOctoDenModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);

	if (ObjectPropertyChangedHandle.IsValid())
	{
		FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(ObjectPropertyChangedHandle);
		ObjectPropertyChangedHandle.Reset();
	}

	if (ReloadCompleteHandle.IsValid())
	{
		FCoreUObjectDelegates::ReloadCompleteDelegate.Remove(ReloadCompleteHandle);
		ReloadCompleteHandle.Reset();
	}

	if (FModuleManager::Get().IsModuleLoaded(TEXT("PropertyEditor")))
	{
		FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
		PropertyEditorModule.UnregisterCustomClassLayout(UOctoDenBootstrapperSettings::StaticClass()->GetFName());
		PropertyEditorModule.NotifyCustomizationModuleChanged();
	}

	if (FGlobalTabmanager::Get()->HasTabSpawner(OctoDen::BootstrapperTabId))
	{
		FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(OctoDen::BootstrapperTabId);
	}

	if (FGlobalTabmanager::Get()->HasTabSpawner(OctoDen::InputBuilderTabId))
	{
		FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(OctoDen::InputBuilderTabId);
	}

	BootstrapperTab.Reset();
	BootstrapperDetailsView.Reset();
	BootstrapperSettings.Reset();
	InputBuilderTab.Reset();
	InputBuilderDetailsView.Reset();
	InputBuilderSettings.Reset();
	InputBuilderResultTextBlock.Reset();
	bCanCreateBlueprintsAndApply = false;
}

bool FOctoDenModule::ShouldShowBootstrapperCodeGenerationUI(const UOctoDenBootstrapperSettings* InSettings)
{
	return InSettings != nullptr && OctoDenBootstrapper::DoesTargetMapExist(*InSettings);
}

void FOctoDenModule::RegisterTabSpawners()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(OctoDen::BootstrapperTabId, FOnSpawnTab::CreateRaw(this, &FOctoDenModule::SpawnBootstrapperTab))
		.SetDisplayName(LOCTEXT("BootstrapperTabTitle", "Bootstrapper"))
		.SetTooltipText(LOCTEXT("BootstrapperTabTooltip", "Open the OctoDen Bootstrapper tab."))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(OctoDen::InputBuilderTabId, FOnSpawnTab::CreateRaw(this, &FOctoDenModule::SpawnInputBuilderTab))
		.SetDisplayName(LOCTEXT("InputBuilderTabTitle", "Input Builder"))
		.SetTooltipText(LOCTEXT("InputBuilderTabTooltip", "Open the OctoDen Input Builder tab."))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FOctoDenModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	UToolMenus* ToolMenus = UToolMenus::Get();
	const bool bWarnIfAlreadyRegistered = false;

	if (!ToolMenus->IsMenuRegistered(OctoDen::OctoDenMainMenu))
	{
		ToolMenus->RegisterMenu(OctoDen::OctoDenMainMenu, NAME_None, EMultiBoxType::Menu, bWarnIfAlreadyRegistered);
	}

	UToolMenu* MenuBar = ToolMenus->ExtendMenu(OctoDen::LevelEditorMainMenu);
	FToolMenuSection& MenuBarSection = MenuBar->FindOrAddSection(NAME_None);
	FToolMenuEntry& OctoDenEntry = MenuBarSection.AddSubMenu(
		TEXT("OctoDen"),
		LOCTEXT("OctoDenMenuLabel", "OctoDen"),
		LOCTEXT("OctoDenMenuTooltip", "Open OctoDen developer assistance tools."),
		FNewToolMenuChoice());
	OctoDenEntry.InsertPosition = FToolMenuInsert(TEXT("Tools"), EToolMenuInsertType::After);

	UToolMenu* OctoDenMenu = ToolMenus->ExtendMenu(OctoDen::OctoDenMainMenu);
	FToolMenuSection& Section = OctoDenMenu->FindOrAddSection(TEXT("OctoDenTools"), LOCTEXT("OctoDenToolsSection", "OctoDen"));

	Section.AddMenuEntry(
		TEXT("OpenOctoDenBootstrapper"),
		LOCTEXT("OpenOctoDenBootstrapperLabel", "Bootstrapper"),
		LOCTEXT("OpenOctoDenBootstrapperTooltip", "Open the dockable Bootstrapper tab."),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FOctoDenModule::OpenBootstrapperTab)));

	Section.AddMenuEntry(
		TEXT("OpenOctoDenInputBuilder"),
		LOCTEXT("OpenOctoDenInputBuilderLabel", "Input Builder"),
		LOCTEXT("OpenOctoDenInputBuilderTooltip", "Open the dockable Input Builder tab."),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FOctoDenModule::OpenInputBuilderTab)));
}

void FOctoDenModule::OpenBootstrapperTab()
{
	FGlobalTabmanager::Get()->TryInvokeTab(OctoDen::BootstrapperTabId);
}

void FOctoDenModule::OpenInputBuilderTab()
{
	FGlobalTabmanager::Get()->TryInvokeTab(OctoDen::InputBuilderTabId);
}

void FOctoDenModule::RefreshBootstrapperDetails()
{
	bCanCreateBlueprintsAndApply = BootstrapperSettings.IsValid() && OctoDenBootstrapper::AreNativeClassesReady(*BootstrapperSettings.Get());

	if (BootstrapperDetailsView.IsValid())
	{
		BootstrapperDetailsView->ForceRefresh();
	}

	if (BootstrapperTab.IsValid())
	{
		BootstrapperTab->Invalidate(EInvalidateWidgetReason::Layout);
	}
}

void FOctoDenModule::RefreshInputBuilderDetails()
{
	if (InputBuilderDetailsView.IsValid())
	{
		InputBuilderDetailsView->ForceRefresh();
	}
}

void FOctoDenModule::UpdateInputBuilderResult(const FText& InResultText)
{
	if (InputBuilderResultTextBlock.IsValid())
	{
		InputBuilderResultTextBlock->SetText(InResultText);
	}
}

void FOctoDenModule::ResetInputBuilderSettings()
{
	InputBuilderSettings = TStrongObjectPtr<UOctoDenInputBuilderSettings>(NewObject<UOctoDenInputBuilderSettings>());
	InputBuilderSettings->Actions.AddDefaulted();

	if (InputBuilderDetailsView.IsValid())
	{
		InputBuilderDetailsView->SetObject(InputBuilderSettings.Get());
	}

	UpdateInputBuilderResult(LOCTEXT("InputBuilderDefaultResult", "Configure the target IMC and action rows, then click Build Assets."));
}

void FOctoDenModule::ShowNotification(const FText& InText, const bool bWasSuccessful) const
{
	FNotificationInfo Info(InText);
	Info.ExpireDuration = 5.0f;
	Info.bUseLargeFont = false;

	if (TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info))
	{
		Notification->SetCompletionState(bWasSuccessful ? SNotificationItem::CS_Success : SNotificationItem::CS_Fail);
	}
}

TSharedRef<SDockTab> FOctoDenModule::SpawnBootstrapperTab(const FSpawnTabArgs& SpawnTabArgs)
{
	BootstrapperSettings = TStrongObjectPtr<UOctoDenBootstrapperSettings>(NewObject<UOctoDenBootstrapperSettings>());
	OctoDenBootstrapper::PopulateDefaults(*BootstrapperSettings.Get());
	RefreshBootstrapperDetails();

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));

	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.bHideSelectionTip = true;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

	const TSharedRef<IDetailsView> DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetObject(BootstrapperSettings.Get());
	BootstrapperDetailsView = DetailsView;

	const TSharedRef<SDockTab> Tab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SBorder)
			.Padding(12.0f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				[
					DetailsView
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 12.0f, 0.0f, 0.0f)
				[
					SNew(SExpandableArea)
					.AreaTitle(LOCTEXT("BootstrapperHelpAreaTitle", "Help"))
					.InitiallyCollapsed(true)
					.BodyContent()
					[
						SNew(SBorder)
						.Padding(10.0f)
						[
							SNew(STextBlock)
							.AutoWrapText(true)
							.Text(OctoDen::GetBootstrapperHelpText())
						]
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 12.0f, 0.0f, 0.0f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 0.0f, 0.0f, 8.0f)
					[
						SNew(STextBlock)
						.Visibility_Lambda([this]()
						{
							return ShouldShowBootstrapperCodeGenerationUI(BootstrapperSettings.Get()) && !bCanCreateBlueprintsAndApply ? EVisibility::Visible : EVisibility::Collapsed;
						})
						.Text(LOCTEXT("CreateBlueprintsAndApplyHint", "GameMode and GameInstance classes must be loaded."))
						.ColorAndOpacity(FSlateColor(FLinearColor(0.65f, 0.65f, 0.65f, 1.0f)))
						.AutoWrapText(true)
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						[
							SNew(SHorizontalBox)
							.Visibility_Lambda([this]()
							{
								return ShouldShowBootstrapperCodeGenerationUI(BootstrapperSettings.Get()) ? EVisibility::Visible : EVisibility::Collapsed;
							})
							+ SHorizontalBox::Slot()
							.FillWidth(1.0f)
							.Padding(0.0f, 0.0f, 8.0f, 0.0f)
							[
								SNew(SBox)
								.HeightOverride(OctoDen::FooterButtonHeight)
								[
									SNew(SButton)
									.Text(LOCTEXT("GenerateCodeButton", "Generate Code"))
									.OnClicked_Lambda([this]()
									{
										GenerateNativeCode(BootstrapperSettings.Get());
										return FReply::Handled();
									})
								]
							]
							+ SHorizontalBox::Slot()
							.FillWidth(1.0f)
							.Padding(0.0f, 0.0f, 8.0f, 0.0f)
							[
								SNew(SBox)
								.HeightOverride(OctoDen::FooterButtonHeight)
								[
									SNew(SButton)
									.IsEnabled_Lambda([this]()
									{
										return bCanCreateBlueprintsAndApply;
									})
									.ToolTipText(TAttribute<FText>::CreateLambda([this]()
									{
										return bCanCreateBlueprintsAndApply
											? FText::GetEmpty()
											: LOCTEXT("CreateBlueprintsAndApplyTooltip", "GameMode and GameInstance classes must be loaded.");
									}))
									.Text(LOCTEXT("CreateBlueprintsAndApplyButton", "Create Blueprints && Apply"))
									.OnClicked_Lambda([this]()
									{
										CreateBlueprintsAndApply(BootstrapperSettings.Get());
										return FReply::Handled();
									})
								]
							]
						]
					]
				]
			]
		];

	Tab->SetOnTabClosed(SDockTab::FOnTabClosedCallback::CreateRaw(this, &FOctoDenModule::HandleBootstrapperTabClosed));
	BootstrapperTab = Tab;
	return Tab;
}

TSharedRef<SDockTab> FOctoDenModule::SpawnInputBuilderTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));

	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.bHideSelectionTip = true;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

	const TSharedRef<IDetailsView> DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	InputBuilderDetailsView = DetailsView;
	ResetInputBuilderSettings();

	const TSharedRef<SDockTab> Tab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SBorder)
			.Padding(12.0f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				[
					DetailsView
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 12.0f, 0.0f, 8.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					.Padding(0.0f, 0.0f, 8.0f, 0.0f)
					[
						SNew(SBox)
						.HeightOverride(OctoDen::FooterButtonHeight)
						[
							SNew(SButton)
							.Text(LOCTEXT("BuildInputAssetsButton", "Build Assets"))
							.OnClicked_Lambda([this]()
							{
								BuildInputAssets(InputBuilderSettings.Get());
								return FReply::Handled();
							})
						]
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SBox)
						.WidthOverride(120.0f)
						.HeightOverride(OctoDen::FooterButtonHeight)
						[
							SNew(SButton)
							.Text(LOCTEXT("ResetInputBuilderButton", "Reset"))
							.OnClicked_Lambda([this]()
							{
								ResetInputBuilderSettings();
								return FReply::Handled();
							})
						]
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBorder)
					.Padding(10.0f)
					[
						SNew(SScrollBox)
						+ SScrollBox::Slot()
						[
							SAssignNew(InputBuilderResultTextBlock, STextBlock)
							.AutoWrapText(true)
							.Text(LOCTEXT("InputBuilderInitialText", "Configure the target IMC and action rows, then click Build Assets."))
						]
					]
				]
			]
		];

	Tab->SetOnTabClosed(SDockTab::FOnTabClosedCallback::CreateRaw(this, &FOctoDenModule::HandleInputBuilderTabClosed));
	InputBuilderTab = Tab;
	return Tab;
}

void FOctoDenModule::HandleBootstrapperTabClosed(TSharedRef<SDockTab> InTab)
{
	BootstrapperTab.Reset();
	BootstrapperDetailsView.Reset();
	BootstrapperSettings.Reset();
	bCanCreateBlueprintsAndApply = false;
}

void FOctoDenModule::HandleInputBuilderTabClosed(TSharedRef<SDockTab> InTab)
{
	InputBuilderTab.Reset();
	InputBuilderDetailsView.Reset();
	InputBuilderSettings.Reset();
	InputBuilderResultTextBlock.Reset();
}

bool FOctoDenModule::CreateManagedMap(UOctoDenBootstrapperSettings* InSettings)
{
	if (InSettings == nullptr)
	{
		return false;
	}

	FText FailReason;
	FString MapPackageName;
	FString ManagedMapAssetName;
	if (!OctoDenBootstrapper::TryGetManagedMapPackageName(*InSettings, MapPackageName, ManagedMapAssetName, FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	if (FPackageName::DoesPackageExist(MapPackageName))
	{
		FMessageDialog::Open(
			EAppMsgType::Ok,
			FText::Format(
				LOCTEXT("ManagedMapAlreadyExists", "Managed map '{0}' already exists. Use Open instead."),
				FText::FromString(MapPackageName)));
		return false;
	}

	if (!OctoDenBootstrapper::SaveDirtyMapsForTransition(FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	UWorld* NewWorld = nullptr;
	if (!OctoDenBootstrapper::CreateManagedMapWorld(InSettings->ManagedMapTemplate, NewWorld, FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	if (!UEditorLoadingAndSavingUtils::SaveMap(NewWorld, MapPackageName))
	{
		FMessageDialog::Open(
			EAppMsgType::Ok,
			FText::Format(
				LOCTEXT("SaveManagedMapFailed", "Failed to save managed map '{0}'."),
				FText::FromString(MapPackageName)));
		return false;
	}

	if (!OctoDenBootstrapper::LoadMapIntoEditor(MapPackageName, FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	InSettings->ManagedMapName = ManagedMapAssetName;
	OctoDenBootstrapper::ApplyManagedMapSelection(*InSettings, MapPackageName);
	RefreshBootstrapperDetails();
	return true;
}

bool FOctoDenModule::OpenManagedMap(UOctoDenBootstrapperSettings* InSettings)
{
	if (InSettings == nullptr)
	{
		return false;
	}

	FText FailReason;
	FString MapPackageName;
	FString ManagedMapAssetName;
	if (!OctoDenBootstrapper::TryGetManagedMapPackageName(*InSettings, MapPackageName, ManagedMapAssetName, FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	if (!FPackageName::DoesPackageExist(MapPackageName))
	{
		FMessageDialog::Open(
			EAppMsgType::Ok,
			FText::Format(
				LOCTEXT("ManagedMapMissing", "Managed map '{0}' does not exist yet. Use Create first."),
				FText::FromString(MapPackageName)));
		return false;
	}

	if (!OctoDenBootstrapper::SaveDirtyMapsForTransition(FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	if (!OctoDenBootstrapper::LoadMapIntoEditor(MapPackageName, FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	InSettings->ManagedMapName = ManagedMapAssetName;
	OctoDenBootstrapper::ApplyManagedMapSelection(*InSettings, MapPackageName);
	RefreshBootstrapperDetails();
	return true;
}

bool FOctoDenModule::GenerateNativeCode(UOctoDenBootstrapperSettings* InSettings)
{
	if (InSettings == nullptr)
	{
		return false;
	}

	FText FailReason;
	if (!OctoDenBootstrapper::ValidateCodeSettings(*InSettings, FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	FModuleContextInfo RuntimeModule;
	if (!OctoDenBootstrapper::ResolveRuntimeModule(InSettings->TargetRuntimeModule.TrimStartAndEnd(), RuntimeModule))
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("RuntimeModuleNotFound", "Could not resolve a runtime module for this project."));
		return false;
	}

	InSettings->TargetRuntimeModule = RuntimeModule.ModuleName;
	InSettings->GameInstanceClassName = OctoDen::ToPascalCase(InSettings->GameInstanceClassName);
	InSettings->GameModeClassName = OctoDen::ToPascalCase(InSettings->GameModeClassName);
	RefreshBootstrapperDetails();

	UClass* NativeGameInstanceClass = nullptr;
	if (!OctoDenBootstrapper::EnsureNativeClassExists(
		InSettings->GameInstanceClassName,
		UGameInstance::StaticClass(),
		RuntimeModule,
		NativeGameInstanceClass,
		FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	UClass* NativeGameModeClass = nullptr;
	if (!OctoDenBootstrapper::EnsureNativeClassExists(
		InSettings->GameModeClassName,
		AGameModeBase::StaticClass(),
		RuntimeModule,
		NativeGameModeClass,
		FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	RefreshBootstrapperDetails();

	FMessageDialog::Open(
		EAppMsgType::Ok,
		LOCTEXT("NativeCodeCompleted", "Native GameInstance and GameMode classes are ready or were generated. If the editor is still compiling or reloading, wait for that to finish. The 'Create Blueprints & Apply' button will be enabled when the native classes are loaded."));
	return true;
}

bool FOctoDenModule::CreateBlueprintsAndApply(UOctoDenBootstrapperSettings* InSettings)
{
	if (InSettings == nullptr)
	{
		return false;
	}

	FText FailReason;
	if (!OctoDenBootstrapper::ValidateBlueprintSettings(*InSettings, FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	FModuleContextInfo RuntimeModule;
	if (!OctoDenBootstrapper::ResolveRuntimeModule(InSettings->TargetRuntimeModule.TrimStartAndEnd(), RuntimeModule))
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("RuntimeModuleNotFoundForBlueprint", "Could not resolve a runtime module for this project."));
		return false;
	}

	InSettings->TargetRuntimeModule = RuntimeModule.ModuleName;
	InSettings->GameInstanceClassName = OctoDen::ToPascalCase(InSettings->GameInstanceClassName);
	InSettings->GameModeClassName = OctoDen::ToPascalCase(InSettings->GameModeClassName);
	RefreshBootstrapperDetails();

	UClass* NativeGameInstanceClass = nullptr;
	if (!OctoDenBootstrapper::TryLoadNativeClass(
		InSettings->GameInstanceClassName,
		UGameInstance::StaticClass(),
		RuntimeModule,
		NativeGameInstanceClass,
		FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	UClass* NativeGameModeClass = nullptr;
	if (!OctoDenBootstrapper::TryLoadNativeClass(
		InSettings->GameModeClassName,
		AGameModeBase::StaticClass(),
		RuntimeModule,
		NativeGameModeClass,
		FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	UBlueprint* GameInstanceBlueprint = nullptr;
	if (!OctoDenBootstrapper::CreateOrReuseBlueprint(
		InSettings->GameInstanceBlueprintFolder,
		InSettings->GameInstanceBlueprintName.TrimStartAndEnd(),
		NativeGameInstanceClass,
		GameInstanceBlueprint,
		FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	UBlueprint* GameModeBlueprint = nullptr;
	if (!OctoDenBootstrapper::CreateOrReuseBlueprint(
		InSettings->GameModeBlueprintFolder,
		InSettings->GameModeBlueprintName.TrimStartAndEnd(),
		NativeGameModeClass,
		GameModeBlueprint,
		FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	const FString TargetMapPackageName = OctoDenBootstrapper::GetMapPackageName(InSettings->TargetMap);
	if (!OctoDenBootstrapper::LoadMapIntoEditor(TargetMapPackageName, FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	UWorld* EditorWorld = GEditor != nullptr ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (EditorWorld == nullptr || EditorWorld->GetOutermost()->GetName() != TargetMapPackageName)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("LoadedWorldMismatch", "The target map was loaded, but the editor world could not be resolved."));
		return false;
	}

	AWorldSettings* WorldSettings = EditorWorld->GetWorldSettings();
	if (WorldSettings == nullptr || GameModeBlueprint->GeneratedClass == nullptr || GameInstanceBlueprint->GeneratedClass == nullptr)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("GeneratedClassUnavailable", "Generated blueprint classes are not available."));
		return false;
	}

	WorldSettings->Modify();
	WorldSettings->DefaultGameMode = GameModeBlueprint->GeneratedClass;
	EditorWorld->MarkPackageDirty();

	UGameMapsSettings* GameMapsSettings = GetMutableDefault<UGameMapsSettings>();
	GameMapsSettings->Modify();
	GameMapsSettings->GameInstanceClass = FSoftClassPath(GameInstanceBlueprint->GeneratedClass->GetPathName());
	GameMapsSettings->EditorStartupMap = InSettings->EditorStartupMap.ToSoftObjectPath();
	UGameMapsSettings::SetGameDefaultMap(OctoDenBootstrapper::GetMapPackageName(InSettings->GameDefaultMap));

	if (InSettings->bSetAsGlobalDefaultGameMode)
	{
		UGameMapsSettings::SetGlobalDefaultGameMode(GameModeBlueprint->GeneratedClass->GetPathName());
	}

	GameMapsSettings->SaveConfig();
	GameMapsSettings->TryUpdateDefaultConfigFile();

	TArray<UPackage*> PackagesToSave;
	PackagesToSave.Add(GameInstanceBlueprint->GetOutermost());
	PackagesToSave.Add(GameModeBlueprint->GetOutermost());
	PackagesToSave.Add(EditorWorld->GetOutermost());

	if (!OctoDen::SavePackages(PackagesToSave, FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	FMessageDialog::Open(
		EAppMsgType::Ok,
		LOCTEXT("BlueprintApplyCompleted", "Blueprint assets were created or reused, and the selected map plus project default maps were updated."));
	return true;
}

bool FOctoDenModule::BuildInputAssets(UOctoDenInputBuilderSettings* InSettings)
{
	if (InSettings == nullptr)
	{
		return false;
	}

	if (!OctoDen::IsValidAssetFolder(InSettings->InputMappingContextFolder))
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("InvalidInputMappingContextFolder", "The Input Mapping Context folder must be a valid package path such as /Game/Input/Contexts."));
		return false;
	}

	if (!OctoDen::IsValidAssetFolder(InSettings->InputActionFolder))
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("InvalidInputActionFolder", "The Input Action folder must be a valid package path such as /Game/Input/Actions."));
		return false;
	}

	if (!OctoDenInputBuilder::HasAnyNamedRows(*InSettings))
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("MissingInputBuilderRows", "Add at least one action row before building input assets."));
		return false;
	}

	const FString InputMappingContextName = OctoDen::MakePrefixedAssetName(
		InSettings->InputMappingContextName,
		InSettings->InputMappingContextPrefix,
		TEXT("Default"));
	const FString InputMappingContextPackagePath = OctoDen::MakePackagePath(InSettings->InputMappingContextFolder, InputMappingContextName);

	if (!FPackageName::IsValidLongPackageName(InputMappingContextPackagePath))
	{
		FMessageDialog::Open(
			EAppMsgType::Ok,
			FText::Format(
				LOCTEXT("InvalidInputMappingContextPackagePath", "The generated IMC package path '{0}' is not valid."),
				FText::FromString(InputMappingContextPackagePath)));
		return false;
	}

	OctoDenInputBuilder::FBuildSummary Summary;
	UInputMappingContext* CreatedInputMappingContext = nullptr;
	const bool bInputMappingContextExists = FPackageName::DoesPackageExist(InputMappingContextPackagePath);

	if (bInputMappingContextExists)
	{
		Summary.bSkippedInputMappingContext = true;
		OctoDenInputBuilder::AddDetail(
			Summary,
			FString::Printf(TEXT("Skipped IMC '%s' because it already exists."), *InputMappingContextPackagePath));
	}
	else
	{
		FText FailReason;
		if (!OctoDenInputBuilder::CreateInputMappingContextAsset(
			OctoDen::NormalizeAssetFolder(InSettings->InputMappingContextFolder),
			InputMappingContextName,
			CreatedInputMappingContext,
			FailReason))
		{
			UpdateInputBuilderResult(FailReason);
			FMessageDialog::Open(EAppMsgType::Ok, FailReason);
			return false;
		}

		Summary.bCreatedInputMappingContext = true;
		Summary.PackagesToSave.Add(CreatedInputMappingContext->GetOutermost());
		OctoDenInputBuilder::AddDetail(
			Summary,
			FString::Printf(TEXT("Created IMC '%s'."), *InputMappingContextPackagePath));
	}

	TSet<FString> RequestedActionPackagePaths;
	for (int32 RowIndex = 0; RowIndex < InSettings->Actions.Num(); ++RowIndex)
	{
		const FOctoDenInputActionRow& Row = InSettings->Actions[RowIndex];
		const FString TrimmedActionName = Row.ActionName.TrimStartAndEnd();
		if (TrimmedActionName.IsEmpty())
		{
			++Summary.SkippedInputActions;
			OctoDenInputBuilder::AddDetail(
				Summary,
				FString::Printf(TEXT("Skipped row %d because Action Name is empty."), RowIndex + 1));
			continue;
		}

		const FString InputActionName = OctoDen::MakePrefixedAssetName(TrimmedActionName, InSettings->InputActionPrefix, TEXT("Action"));
		const FString InputActionPackagePath = OctoDen::MakePackagePath(InSettings->InputActionFolder, InputActionName);
		if (!FPackageName::IsValidLongPackageName(InputActionPackagePath))
		{
			++Summary.FailedInputActions;
			OctoDenInputBuilder::AddDetail(
				Summary,
				FString::Printf(TEXT("Failed row %d because package path '%s' is invalid."), RowIndex + 1, *InputActionPackagePath));
			continue;
		}

		if (RequestedActionPackagePaths.Contains(InputActionPackagePath))
		{
			++Summary.SkippedInputActions;
			OctoDenInputBuilder::AddDetail(
				Summary,
				FString::Printf(TEXT("Skipped duplicate requested action '%s'."), *InputActionPackagePath));
			continue;
		}

		RequestedActionPackagePaths.Add(InputActionPackagePath);

		if (FPackageName::DoesPackageExist(InputActionPackagePath))
		{
			++Summary.SkippedInputActions;
			OctoDenInputBuilder::AddDetail(
				Summary,
				FString::Printf(TEXT("Skipped IA '%s' because it already exists."), *InputActionPackagePath));
			continue;
		}

		UInputAction* CreatedInputAction = nullptr;
		FText FailReason;
		if (!OctoDenInputBuilder::CreateInputActionAsset(
			OctoDen::NormalizeAssetFolder(InSettings->InputActionFolder),
			InputActionName,
			Row.ValueType,
			CreatedInputAction,
			FailReason))
		{
			++Summary.FailedInputActions;
			OctoDenInputBuilder::AddDetail(
				Summary,
				FString::Printf(TEXT("Failed to create IA '%s': %s"), *InputActionPackagePath, *FailReason.ToString()));
			continue;
		}

		++Summary.CreatedInputActions;
		Summary.PackagesToSave.Add(CreatedInputAction->GetOutermost());
		OctoDenInputBuilder::AddDetail(
			Summary,
			FString::Printf(TEXT("Created IA '%s'."), *InputActionPackagePath));

		if (CreatedInputMappingContext == nullptr)
		{
			continue;
		}

		auto AddMappingIfValid = [&Summary, CreatedInputAction, CreatedInputMappingContext](const FKey& InKey)
		{
			if (!InKey.IsValid())
			{
				return;
			}

			CreatedInputMappingContext->Modify();
			CreatedInputMappingContext->MapKey(CreatedInputAction, InKey);
			CreatedInputMappingContext->MarkPackageDirty();
			++Summary.AddedMappings;
		};

		AddMappingIfValid(Row.PrimaryKey);
		AddMappingIfValid(Row.SecondaryKey);
		AddMappingIfValid(Row.GamepadKey);
	}

	FText SaveFailReason;
	if (!OctoDen::SavePackages(Summary.PackagesToSave, SaveFailReason))
	{
		UpdateInputBuilderResult(SaveFailReason);
		FMessageDialog::Open(EAppMsgType::Ok, SaveFailReason);
		return false;
	}

	const FText SummaryText = FText::FromString(OctoDenInputBuilder::BuildSummaryText(Summary));
	UpdateInputBuilderResult(SummaryText);
	ShowNotification(
		FText::Format(
			LOCTEXT("InputBuilderNotification", "Input Builder finished. Created {0} IA(s), skipped {1}, failed {2}."),
			FText::AsNumber(Summary.CreatedInputActions),
			FText::AsNumber(Summary.SkippedInputActions),
			FText::AsNumber(Summary.FailedInputActions)),
		Summary.FailedInputActions == 0);

	return Summary.FailedInputActions == 0;
}

IMPLEMENT_MODULE(FOctoDenModule, OctoDen)

#undef LOCTEXT_NAMESPACE
