#include "InputBuilder/InputBuilderService.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Engine/Blueprint.h"
#include "Engine/DataAsset.h"
#include "Engine/GameInstance.h"
#include "EnhancedActionKeyMapping.h"
#include "Factories/DataAssetFactory.h"
#include "FileHelpers.h"
#include "GameMapsSettings.h"
#include "InputAction.h"
#include "InputEditorModule.h"
#include "InputBuilder/OctoDenInputMappingRules.h"
#include "InputMappingContext.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/PackageName.h"
#include "Shared/OctoDenAssetNaming.h"
#include "Shared/OctoDenReflectionHelpers.h"
#include "UObject/Package.h"

namespace
{
	template <typename TObjectType>
	TObjectType* LoadAssetByPath(const FString& ObjectPath)
	{
		return LoadObject<TObjectType>(nullptr, *ObjectPath);
	}

	template <typename TObjectType, typename TFactoryType>
	TObjectType* CreateAsset(const FString& Folder, const FString& AssetName, TFunctionRef<void(TFactoryType&)> SetupFactory)
	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get();
		TFactoryType* Factory = NewObject<TFactoryType>();
		SetupFactory(*Factory);
		return Cast<TObjectType>(AssetTools.CreateAsset(AssetName, Folder, TObjectType::StaticClass(), Factory, FName(TEXT("OctoDen"))));
	}

	FString GetManagedActionObjectPath(const UOctoDenInputBuilderSettings& Settings, const EOctoDenManagedInputAction ManagedAction)
	{
		return OctoDenAssetNaming::BuildObjectPath(Settings.InputActionFolder, Settings.GetCanonicalActionAssetName(ManagedAction));
	}

	UInputAction* ResolveOrCreateManagedInputAction(UOctoDenInputBuilderSettings& Settings, const EOctoDenManagedInputAction ManagedAction, bool& bOutCreated, FText& OutFailure)
	{
		const FString SafeFolder = OctoDenAssetNaming::NormalizePackageFolder(Settings.InputActionFolder, TEXT("/Game/Input/Actions"));
		const FString AssetName = Settings.GetCanonicalActionAssetName(ManagedAction);
		const FString ObjectPath = OctoDenAssetNaming::BuildObjectPath(SafeFolder, AssetName);

		if (UInputAction* Existing = LoadAssetByPath<UInputAction>(ObjectPath))
		{
			bOutCreated = false;
			return Existing;
		}

		bOutCreated = true;
		UInputAction* Created = CreateAsset<UInputAction, UInputAction_Factory>(SafeFolder, AssetName, [](UInputAction_Factory&)
		{
		});
		if (Created == nullptr)
		{
			OutFailure = FText::Format(NSLOCTEXT("OctoDenInputBuilder", "CreateInputActionFailed", "Failed to create Input Action '{0}'."), FText::FromString(AssetName));
			return nullptr;
		}

		FAssetRegistryModule::AssetCreated(Created);
		Created->MarkPackageDirty();
		return Created;
	}

	UObject* ResolveOrCreateInputConfigAsset(UOctoDenInputBuilderSettings& Settings, FText& OutFailure)
	{
		const FString SafeFolder = OctoDenAssetNaming::NormalizePackageFolder(Settings.InputConfigFolder, TEXT("/Game/Input/Configs"));
		const FString SafeAssetName = OctoDenAssetNaming::SanitizeAssetName(Settings.InputConfigAssetName, TEXT("DA_DefaultInputConfig"));
		const FString ObjectPath = OctoDenAssetNaming::BuildObjectPath(SafeFolder, SafeAssetName);
		if (UObject* Existing = LoadObject<UObject>(nullptr, *ObjectPath))
		{
			return Existing;
		}

		UClass* DataAssetClass = LoadObject<UClass>(nullptr, TEXT("/Script/CodexInvenMedium.CodexInvenMediumInputConfigDataAsset"));
		if (DataAssetClass == nullptr)
		{
			OutFailure = NSLOCTEXT("OctoDenInputBuilder", "InputConfigClassMissing", "Runtime input config class could not be loaded.");
			return nullptr;
		}

		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get();
		UDataAssetFactory* Factory = NewObject<UDataAssetFactory>();
		Factory->DataAssetClass = DataAssetClass;
		UObject* Created = AssetTools.CreateAsset(SafeAssetName, SafeFolder, DataAssetClass, Factory, FName(TEXT("OctoDen")));
		if (Created == nullptr)
		{
			OutFailure = FText::Format(NSLOCTEXT("OctoDenInputBuilder", "CreateInputConfigFailed", "Failed to create input config asset '{0}'."), FText::FromString(SafeAssetName));
			return nullptr;
		}

		FAssetRegistryModule::AssetCreated(Created);
		Created->MarkPackageDirty();
		return Created;
	}

	bool ResolveProjectGameInstance(UBlueprint*& OutBlueprint, UObject*& OutDefaultObject, FText& OutFailure)
	{
		const UGameMapsSettings* GameMapsSettings = GetDefault<UGameMapsSettings>();
		if (GameMapsSettings == nullptr)
		{
			OutFailure = NSLOCTEXT("OctoDenInputBuilder", "GameMapsSettingsUnavailable", "GameMapsSettings could not be resolved.");
			return false;
		}

		UClass* GameInstanceClass = GameMapsSettings->GameInstanceClass.TryLoadClass<UGameInstance>();
		if (GameInstanceClass == nullptr)
		{
			OutFailure = NSLOCTEXT("OctoDenInputBuilder", "GameInstanceClassMissing", "Project GameInstanceClass is not assigned or could not be loaded.");
			return false;
		}

		OutBlueprint = Cast<UBlueprint>(GameInstanceClass->ClassGeneratedBy);
		if (OutBlueprint == nullptr)
		{
			OutFailure = FText::Format(NSLOCTEXT("OctoDenInputBuilder", "GameInstanceBlueprintRequired", "GameInstanceClass '{0}' must be a blueprint child so defaults can be saved."), FText::FromString(GameInstanceClass->GetPathName()));
			return false;
		}

		OutDefaultObject = GameInstanceClass->GetDefaultObject();
		if (OutDefaultObject == nullptr)
		{
			OutFailure = NSLOCTEXT("OctoDenInputBuilder", "GameInstanceDefaultObjectMissing", "GameInstance class does not expose a valid default object.");
			return false;
		}

		return true;
	}
}

void OctoDenInputBuilder::AnalyzeInputContext(const UOctoDenInputBuilderSettings& Settings, FOctoDenInputContextAnalysis& OutAnalysis)
{
	OutAnalysis = FOctoDenInputContextAnalysis();
	OutAnalysis.bHasSelection = Settings.SelectedInputMappingContext != nullptr;
	if (Settings.SelectedInputMappingContext == nullptr)
	{
		return;
	}

	for (const FEnhancedActionKeyMapping& Mapping : Settings.SelectedInputMappingContext->GetMappings())
	{
		if (Mapping.Action == nullptr)
		{
			++OutAnalysis.NullActionMappingCount;
			continue;
		}

		for (uint8 Index = 0; Index < 4; ++Index)
		{
			const EOctoDenManagedInputAction Candidate = static_cast<EOctoDenManagedInputAction>(Index);
			if (Mapping.Action->GetPathName() == GetManagedActionObjectPath(Settings, Candidate) && Mapping.Key.IsValid())
			{
				OutAnalysis.PresentActions.AddUnique(Candidate);
			}
		}
	}

	for (uint8 Index = 0; Index < 4; ++Index)
	{
		const EOctoDenManagedInputAction Candidate = static_cast<EOctoDenManagedInputAction>(Index);
		if (!OutAnalysis.PresentActions.Contains(Candidate))
		{
			OutAnalysis.AvailableActions.Add(Candidate);
		}
	}
}

bool OctoDenInputBuilder::BuildManagedAction(UOctoDenInputBuilderSettings& Settings, FOctoDenManagedActionBuildResult& OutResult, FText& OutFailure)
{
	if (Settings.SelectedInputMappingContext == nullptr)
	{
		OutFailure = NSLOCTEXT("OctoDenInputBuilder", "InputMappingContextMissing", "Select an Input Mapping Context before building actions.");
		return false;
	}

	Settings.InputActionFolder = OctoDenAssetNaming::NormalizePackageFolder(Settings.InputActionFolder, TEXT("/Game/Input/Actions"));
	if (!FPackageName::IsValidLongPackageName(Settings.InputActionFolder))
	{
		OutFailure = NSLOCTEXT("OctoDenInputBuilder", "InvalidInputActionFolder", "Input Action folder must resolve to a valid /Game package path.");
		return false;
	}

	bool bCreatedInputAction = false;
	UInputAction* InputAction = ResolveOrCreateManagedInputAction(Settings, Settings.ManagedAction, bCreatedInputAction, OutFailure);
	if (InputAction == nullptr)
	{
		return false;
	}

	OctoDenInputMappingRules::ConfigureInputAction(*InputAction, Settings.ManagedAction);
	const FOctoDenKeyDraft Draft = Settings.ManagedAction == EOctoDenManagedInputAction::Jump ? Settings.JumpDraft : Settings.ManagedAction == EOctoDenManagedInputAction::Fire ? Settings.FireDraft : FOctoDenKeyDraft();
	if (!OctoDenInputMappingRules::ApplyMappings(*Settings.SelectedInputMappingContext, *InputAction, Settings.ManagedAction, Draft, OutResult, OutFailure))
	{
		return false;
	}

	OutResult.bCreatedInputAction = bCreatedInputAction;
	TArray<UPackage*> PackagesToSave;
	PackagesToSave.Add(InputAction->GetOutermost());
	PackagesToSave.Add(Settings.SelectedInputMappingContext->GetOutermost());
	FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, false, false);

	Settings.LastStatus = FText::Format(NSLOCTEXT("OctoDenInputBuilder", "BuildManagedActionSucceeded", "Built {0}: removed {1} stale mappings, added {2} mappings."), UOctoDenInputBuilderSettings::GetManagedActionLabel(Settings.ManagedAction), FText::AsNumber(OutResult.RemovedExistingMappings + OutResult.RemovedNullMappings), FText::AsNumber(OutResult.AddedMappings));
	return true;
}

bool OctoDenInputBuilder::CanLinkRuntimeInputConfig(const UOctoDenInputBuilderSettings& Settings, FText* OutReason)
{
	if (Settings.SelectedInputMappingContext == nullptr)
	{
		if (OutReason != nullptr)
		{
			*OutReason = NSLOCTEXT("OctoDenInputBuilder", "LinkRequiresImc", "Select an Input Mapping Context.");
		}
		return false;
	}

	FOctoDenInputContextAnalysis Analysis;
	AnalyzeInputContext(Settings, Analysis);
	if (Analysis.PresentActions.Num() != 4)
	{
		if (OutReason != nullptr)
		{
			*OutReason = NSLOCTEXT("OctoDenInputBuilder", "LinkRequiresAllActions", "All four managed actions must exist in the selected Input Mapping Context.");
		}
		return false;
	}

	const FString SafeFolder = OctoDenAssetNaming::NormalizePackageFolder(Settings.InputConfigFolder, TEXT("/Game/Input/Configs"));
	const FString SafeName = OctoDenAssetNaming::SanitizeAssetName(Settings.InputConfigAssetName, TEXT("DA_DefaultInputConfig"));
	if (!FPackageName::IsValidObjectPath(OctoDenAssetNaming::BuildObjectPath(SafeFolder, SafeName)))
	{
		if (OutReason != nullptr)
		{
			*OutReason = NSLOCTEXT("OctoDenInputBuilder", "LinkRequiresValidConfigPath", "Input config path must resolve to a valid object path.");
		}
		return false;
	}
	return true;
}

bool OctoDenInputBuilder::LinkRuntimeInputConfig(UOctoDenInputBuilderSettings& Settings, FText& OutFailure)
{
	if (!CanLinkRuntimeInputConfig(Settings, &OutFailure))
	{
		return false;
	}

	UObject* InputConfigAsset = ResolveOrCreateInputConfigAsset(Settings, OutFailure);
	if (InputConfigAsset == nullptr)
	{
		return false;
	}

	UInputAction* MoveAction = LoadAssetByPath<UInputAction>(GetManagedActionObjectPath(Settings, EOctoDenManagedInputAction::Move));
	UInputAction* LookAction = LoadAssetByPath<UInputAction>(GetManagedActionObjectPath(Settings, EOctoDenManagedInputAction::Look));
	UInputAction* JumpAction = LoadAssetByPath<UInputAction>(GetManagedActionObjectPath(Settings, EOctoDenManagedInputAction::Jump));
	UInputAction* FireAction = LoadAssetByPath<UInputAction>(GetManagedActionObjectPath(Settings, EOctoDenManagedInputAction::Fire));
	if (MoveAction == nullptr || LookAction == nullptr || JumpAction == nullptr || FireAction == nullptr)
	{
		OutFailure = NSLOCTEXT("OctoDenInputBuilder", "ManagedActionAssetsMissing", "One or more canonical Input Action assets could not be loaded.");
		return false;
	}

	if (!OctoDenReflectionHelpers::SetObjectProperty(*InputConfigAsset, TEXT("InputMappingContext"), Settings.SelectedInputMappingContext, UInputMappingContext::StaticClass(), OutFailure)
		|| !OctoDenReflectionHelpers::SetObjectProperty(*InputConfigAsset, TEXT("MoveAction"), MoveAction, UInputAction::StaticClass(), OutFailure)
		|| !OctoDenReflectionHelpers::SetObjectProperty(*InputConfigAsset, TEXT("LookAction"), LookAction, UInputAction::StaticClass(), OutFailure)
		|| !OctoDenReflectionHelpers::SetObjectProperty(*InputConfigAsset, TEXT("JumpAction"), JumpAction, UInputAction::StaticClass(), OutFailure)
		|| !OctoDenReflectionHelpers::SetObjectProperty(*InputConfigAsset, TEXT("FireAction"), FireAction, UInputAction::StaticClass(), OutFailure))
	{
		return false;
	}

	UBlueprint* GameInstanceBlueprint = nullptr;
	UObject* DefaultObject = nullptr;
	if (!ResolveProjectGameInstance(GameInstanceBlueprint, DefaultObject, OutFailure))
	{
		return false;
	}

	if (!OctoDenReflectionHelpers::SetObjectProperty(*DefaultObject, TEXT("DefaultInputConfig"), InputConfigAsset, UDataAsset::StaticClass(), OutFailure))
	{
		return false;
	}

	FBlueprintEditorUtils::MarkBlueprintAsModified(GameInstanceBlueprint);
	FKismetEditorUtilities::CompileBlueprint(GameInstanceBlueprint);
	GameInstanceBlueprint->MarkPackageDirty();

	TArray<UPackage*> PackagesToSave;
	PackagesToSave.Add(InputConfigAsset->GetOutermost());
	PackagesToSave.Add(GameInstanceBlueprint->GetOutermost());
	FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, false, false);

	Settings.LastStatus = NSLOCTEXT("OctoDenInputBuilder", "LinkSucceeded", "Runtime input config asset and GameInstance defaults were updated.");
	return true;
}
