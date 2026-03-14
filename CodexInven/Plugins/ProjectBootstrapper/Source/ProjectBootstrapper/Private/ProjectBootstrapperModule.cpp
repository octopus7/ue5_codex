#include "ProjectBootstrapperModule.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "Engine/Blueprint.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "FileHelpers.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/WorldSettings.h"
#include "GameMapsSettings.h"
#include "GameProjectUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/App.h"
#include "Misc/MessageDialog.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "ModuleDescriptor.h"
#include "Modules/ModuleManager.h"
#include "ProjectBootstrapperDialogSettings.h"
#include "ProjectBootstrapperDialogSettingsCustomization.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "ToolMenus.h"
#include "UObject/StrongObjectPtr.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"

DEFINE_LOG_CATEGORY_STATIC(LogProjectBootstrapper, Log, All);

#define LOCTEXT_NAMESPACE "ProjectBootstrapper"

namespace ProjectBootstrapper
{
	const TCHAR* MenuName = TEXT("LevelEditor.MainMenu.Tools");
	const TCHAR* ManagedMapFolder = TEXT("/Game/Maps");

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
		InFolder = TrimTrailingSlash(MoveTemp(InFolder));
		return InFolder;
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
			Result = TEXT("Map") + Result;
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

		const FString MapBaseName = ToPascalCase(StripLevelPrefix(RawMapAssetName));
		if (MapBaseName.IsEmpty() || MapBaseName.Equals(ProjectName, ESearchCase::IgnoreCase))
		{
			return ProjectName;
		}

		return ProjectName + MapBaseName;
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

	FString GetMapPackageName(const TSoftObjectPtr<UWorld>& MapReference)
	{
		return MapReference.ToSoftObjectPath().GetLongPackageName();
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

	bool IsValidAssetFolder(const FString& Folder)
	{
		const FString NormalizedFolder = NormalizeAssetFolder(Folder);
		return !NormalizedFolder.IsEmpty() && FPackageName::IsValidLongPackageName(NormalizedFolder);
	}

	UClass* FindProjectClass(const FString& ModuleName, const FString& ClassName)
	{
		const FString ClassPath = FString::Printf(TEXT("/Script/%s.%s"), *ModuleName, *ClassName);
		return LoadObject<UClass>(nullptr, *ClassPath);
	}

	bool TryLoadNativeClass(const FString& RequestedClassName, const UClass* ParentClass, const FModuleContextInfo& ModuleInfo, UClass*& OutClass, FText& OutFailReason)
	{
		OutClass = FindProjectClass(ModuleInfo.ModuleName, ToPascalCase(RequestedClassName));
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

		OutClass = FindProjectClass(ModuleInfo.ModuleName, RequestedClassName);
		if (OutClass != nullptr)
		{
			return true;
		}

		// AddCodeToProject may return before Live Coding or module reload finishes.
		// The second stage explicitly waits for the class to become loadable.
		return true;
	}

	bool LoadExistingBlueprint(const FString& Folder, const FString& AssetName, UBlueprint*& OutBlueprint)
	{
		OutBlueprint = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), nullptr, *MakeObjectPath(Folder, AssetName)));
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

		const FString PackagePath = MakePackagePath(Folder, AssetName);
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
			FName(TEXT("ProjectBootstrapper")));

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

	bool TryGetManagedMapPackageName(const UProjectBootstrapperDialogSettings& InSettings, FString& OutMapPackageName, FString& OutManagedMapAssetName, FText& OutFailReason)
	{
		OutManagedMapAssetName = SanitizeAssetName(InSettings.ManagedMapName, TEXT("BasicMap"));
		OutMapPackageName = MakePackagePath(ManagedMapFolder, OutManagedMapAssetName);

		if (!FPackageName::IsValidLongPackageName(OutMapPackageName))
		{
			OutFailReason = FText::Format(
				LOCTEXT("InvalidManagedMapName", "Managed map '{0}' results in an invalid package path."),
				FText::FromString(InSettings.ManagedMapName));
			return false;
		}

		return true;
	}

	void ApplySuggestedGameModeNames(UProjectBootstrapperDialogSettings& InSettings, const FString& MapPackageName)
	{
		const FString ProjectName = ToPascalCase(FApp::GetProjectName());
		const FString GameModeStem = MakeDefaultGameModeStem(ProjectName, MapPackageName);
		InSettings.GameModeClassName = GameModeStem + TEXT("GameMode");
		InSettings.GameModeBlueprintName = TEXT("BP_") + GameModeStem + TEXT("GameMode");
	}

	void ApplyManagedMapSelection(UProjectBootstrapperDialogSettings& InSettings, const FString& MapPackageName)
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

	bool ValidateCodeSettings(const UProjectBootstrapperDialogSettings& InSettings, FText& OutFailReason)
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

	bool ValidateBlueprintSettings(const UProjectBootstrapperDialogSettings& InSettings, FText& OutFailReason)
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

		if (!IsValidAssetFolder(InSettings.GameInstanceBlueprintFolder) || !IsValidAssetFolder(InSettings.GameModeBlueprintFolder))
		{
			OutFailReason = LOCTEXT("InvalidBlueprintFolder", "Blueprint folder paths must be valid long package names such as /Game/Blueprints/Core.");
			return false;
		}

		return true;
	}

	void PopulateDefaults(UProjectBootstrapperDialogSettings& InSettings)
	{
		const FString ProjectName = ToPascalCase(FApp::GetProjectName());
		const FString DefaultManagedMapPackageName = MakePackagePath(ManagedMapFolder, TEXT("BasicMap"));
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

void FProjectBootstrapperModule::StartupModule()
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	PropertyEditorModule.RegisterCustomClassLayout(
		UProjectBootstrapperDialogSettings::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FProjectBootstrapperDialogSettingsCustomization::MakeInstance));
	PropertyEditorModule.NotifyCustomizationModuleChanged();

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FProjectBootstrapperModule::RegisterMenus));
}

void FProjectBootstrapperModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);

	if (FModuleManager::Get().IsModuleLoaded(TEXT("PropertyEditor")))
	{
		FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
		PropertyEditorModule.UnregisterCustomClassLayout(UProjectBootstrapperDialogSettings::StaticClass()->GetFName());
		PropertyEditorModule.NotifyCustomizationModuleChanged();
	}

	DialogWindow.Reset();
	DialogDetailsView.Reset();
	DialogSettings.Reset();
}

void FProjectBootstrapperModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu(ProjectBootstrapper::MenuName);
	FToolMenuSection& Section = Menu->FindOrAddSection(TEXT("ProjectBootstrapper"));

	Section.AddMenuEntry(
		TEXT("OpenProjectBootstrapper"),
		LOCTEXT("OpenProjectBootstrapperLabel", "Project Bootstrapper"),
		LOCTEXT("OpenProjectBootstrapperTooltip", "Generate project-native GameInstance and GameMode classes, create matching blueprints, and assign project map defaults."),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FProjectBootstrapperModule::OpenBootstrapperWindow)));
}

void FProjectBootstrapperModule::OpenBootstrapperWindow()
{
	if (DialogWindow.IsValid())
	{
		DialogWindow->BringToFront();
		return;
	}

	DialogSettings = TStrongObjectPtr<UProjectBootstrapperDialogSettings>(NewObject<UProjectBootstrapperDialogSettings>());
	ProjectBootstrapper::PopulateDefaults(*DialogSettings.Get());

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));

	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.bHideSelectionTip = true;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

	const TSharedRef<IDetailsView> DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetObject(DialogSettings.Get());
	DialogDetailsView = DetailsView;

	TSharedRef<SWindow> Window = SNew(SWindow)
		.Title(LOCTEXT("ProjectBootstrapperWindowTitle", "Project Bootstrapper"))
		.ClientSize(FVector2D(760.0f, 700.0f))
		.SupportsMinimize(false)
		.SupportsMaximize(false)
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
					SNew(SUniformGridPanel)
					.SlotPadding(FMargin(8.0f, 0.0f))
					+ SUniformGridPanel::Slot(0, 0)
					[
						SNew(SButton)
						.Text(LOCTEXT("GenerateCodeButton", "Generate Code"))
						.OnClicked_Lambda([this]()
						{
							GenerateNativeCode(DialogSettings.Get());
							return FReply::Handled();
						})
					]
					+ SUniformGridPanel::Slot(1, 0)
					[
						SNew(SButton)
						.Text(LOCTEXT("CreateBlueprintsAndApplyButton", "Create Blueprints && Apply"))
						.OnClicked_Lambda([this]()
						{
							CreateBlueprintsAndApply(DialogSettings.Get());
							return FReply::Handled();
						})
					]
					+ SUniformGridPanel::Slot(2, 0)
					[
						SNew(SButton)
						.Text(LOCTEXT("CloseBootstrapButton", "Close"))
						.OnClicked_Lambda([this]()
						{
							DialogWindow->RequestDestroyWindow();
							return FReply::Handled();
						})
					]
				]
			]
		];

	Window->SetOnWindowClosed(FOnWindowClosed::CreateRaw(this, &FProjectBootstrapperModule::HandleDialogWindowClosed));
	DialogWindow = Window;

	FSlateApplication::Get().AddWindow(Window);
}

void FProjectBootstrapperModule::HandleDialogWindowClosed(const TSharedRef<SWindow>& Window)
{
	DialogWindow.Reset();
	DialogDetailsView.Reset();
	DialogSettings.Reset();
}

bool FProjectBootstrapperModule::CreateManagedMap(UProjectBootstrapperDialogSettings* InDialogSettings)
{
	if (InDialogSettings == nullptr)
	{
		return false;
	}

	FText FailReason;
	FString MapPackageName;
	FString ManagedMapAssetName;
	if (!ProjectBootstrapper::TryGetManagedMapPackageName(*InDialogSettings, MapPackageName, ManagedMapAssetName, FailReason))
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

	if (!ProjectBootstrapper::SaveDirtyMapsForTransition(FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	UWorld* NewWorld = UEditorLoadingAndSavingUtils::NewBlankMap(/*bSaveExistingMap=*/false);
	if (NewWorld == nullptr)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("CreateManagedMapFailed", "Failed to create a new blank map."));
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

	if (!ProjectBootstrapper::LoadMapIntoEditor(MapPackageName, FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	InDialogSettings->ManagedMapName = ManagedMapAssetName;
	ProjectBootstrapper::ApplyManagedMapSelection(*InDialogSettings, MapPackageName);
	RefreshDialogDetails();
	return true;
}

bool FProjectBootstrapperModule::OpenManagedMap(UProjectBootstrapperDialogSettings* InDialogSettings)
{
	if (InDialogSettings == nullptr)
	{
		return false;
	}

	FText FailReason;
	FString MapPackageName;
	FString ManagedMapAssetName;
	if (!ProjectBootstrapper::TryGetManagedMapPackageName(*InDialogSettings, MapPackageName, ManagedMapAssetName, FailReason))
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

	if (!ProjectBootstrapper::SaveDirtyMapsForTransition(FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	if (!ProjectBootstrapper::LoadMapIntoEditor(MapPackageName, FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	InDialogSettings->ManagedMapName = ManagedMapAssetName;
	ProjectBootstrapper::ApplyManagedMapSelection(*InDialogSettings, MapPackageName);
	RefreshDialogDetails();
	return true;
}

bool FProjectBootstrapperModule::GenerateNativeCode(UProjectBootstrapperDialogSettings* InDialogSettings)
{
	if (InDialogSettings == nullptr)
	{
		return false;
	}

	FText FailReason;
	if (!ProjectBootstrapper::ValidateCodeSettings(*InDialogSettings, FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	FModuleContextInfo RuntimeModule;
	if (!ProjectBootstrapper::ResolveRuntimeModule(InDialogSettings->TargetRuntimeModule.TrimStartAndEnd(), RuntimeModule))
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("RuntimeModuleNotFound", "Could not resolve a runtime module for this project."));
		return false;
	}

	InDialogSettings->TargetRuntimeModule = RuntimeModule.ModuleName;
	InDialogSettings->GameInstanceClassName = ProjectBootstrapper::ToPascalCase(InDialogSettings->GameInstanceClassName);
	InDialogSettings->GameModeClassName = ProjectBootstrapper::ToPascalCase(InDialogSettings->GameModeClassName);
	RefreshDialogDetails();

	UClass* NativeGameInstanceClass = nullptr;
	if (!ProjectBootstrapper::EnsureNativeClassExists(
		InDialogSettings->GameInstanceClassName,
		UGameInstance::StaticClass(),
		RuntimeModule,
		NativeGameInstanceClass,
		FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	UClass* NativeGameModeClass = nullptr;
	if (!ProjectBootstrapper::EnsureNativeClassExists(
		InDialogSettings->GameModeClassName,
		AGameModeBase::StaticClass(),
		RuntimeModule,
		NativeGameModeClass,
		FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	FMessageDialog::Open(
		EAppMsgType::Ok,
		LOCTEXT("NativeCodeCompleted", "Native GameInstance and GameMode classes are ready or were generated. If the editor is still compiling or reloading, wait for that to finish and then click 'Create Blueprints & Apply'."));
	return true;
}

bool FProjectBootstrapperModule::CreateBlueprintsAndApply(UProjectBootstrapperDialogSettings* InDialogSettings)
{
	if (InDialogSettings == nullptr)
	{
		return false;
	}

	FText FailReason;
	if (!ProjectBootstrapper::ValidateBlueprintSettings(*InDialogSettings, FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	FModuleContextInfo RuntimeModule;
	if (!ProjectBootstrapper::ResolveRuntimeModule(InDialogSettings->TargetRuntimeModule.TrimStartAndEnd(), RuntimeModule))
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("RuntimeModuleNotFound", "Could not resolve a runtime module for this project."));
		return false;
	}

	InDialogSettings->TargetRuntimeModule = RuntimeModule.ModuleName;
	InDialogSettings->GameInstanceClassName = ProjectBootstrapper::ToPascalCase(InDialogSettings->GameInstanceClassName);
	InDialogSettings->GameModeClassName = ProjectBootstrapper::ToPascalCase(InDialogSettings->GameModeClassName);
	RefreshDialogDetails();

	UClass* NativeGameInstanceClass = nullptr;
	if (!ProjectBootstrapper::TryLoadNativeClass(
		InDialogSettings->GameInstanceClassName,
		UGameInstance::StaticClass(),
		RuntimeModule,
		NativeGameInstanceClass,
		FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	UClass* NativeGameModeClass = nullptr;
	if (!ProjectBootstrapper::TryLoadNativeClass(
		InDialogSettings->GameModeClassName,
		AGameModeBase::StaticClass(),
		RuntimeModule,
		NativeGameModeClass,
		FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	UBlueprint* GameInstanceBlueprint = nullptr;
	if (!ProjectBootstrapper::CreateOrReuseBlueprint(
		InDialogSettings->GameInstanceBlueprintFolder,
		InDialogSettings->GameInstanceBlueprintName.TrimStartAndEnd(),
		NativeGameInstanceClass,
		GameInstanceBlueprint,
		FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	UBlueprint* GameModeBlueprint = nullptr;
	if (!ProjectBootstrapper::CreateOrReuseBlueprint(
		InDialogSettings->GameModeBlueprintFolder,
		InDialogSettings->GameModeBlueprintName.TrimStartAndEnd(),
		NativeGameModeClass,
		GameModeBlueprint,
		FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	const FString TargetMapPackageName = ProjectBootstrapper::GetMapPackageName(InDialogSettings->TargetMap);
	if (!ProjectBootstrapper::LoadMapIntoEditor(TargetMapPackageName, FailReason))
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
	GameMapsSettings->EditorStartupMap = InDialogSettings->EditorStartupMap.ToSoftObjectPath();
	UGameMapsSettings::SetGameDefaultMap(ProjectBootstrapper::GetMapPackageName(InDialogSettings->GameDefaultMap));

	if (InDialogSettings->bSetAsGlobalDefaultGameMode)
	{
		UGameMapsSettings::SetGlobalDefaultGameMode(GameModeBlueprint->GeneratedClass->GetPathName());
	}

	GameMapsSettings->SaveConfig();
	GameMapsSettings->TryUpdateDefaultConfigFile();

	TArray<UPackage*> PackagesToSave;
	PackagesToSave.Add(GameInstanceBlueprint->GetOutermost());
	PackagesToSave.Add(GameModeBlueprint->GetOutermost());
	PackagesToSave.Add(EditorWorld->GetOutermost());

	if (!ProjectBootstrapper::SavePackages(PackagesToSave, FailReason))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FailReason);
		return false;
	}

	FMessageDialog::Open(
		EAppMsgType::Ok,
		LOCTEXT("BlueprintApplyCompleted", "Blueprint assets were created or reused, and the selected map plus project default maps were updated."));
	return true;
}

void FProjectBootstrapperModule::RefreshDialogDetails() const
{
	if (DialogDetailsView.IsValid())
	{
		DialogDetailsView->ForceRefresh();
	}
}

IMPLEMENT_MODULE(FProjectBootstrapperModule, ProjectBootstrapper)

#undef LOCTEXT_NAMESPACE
