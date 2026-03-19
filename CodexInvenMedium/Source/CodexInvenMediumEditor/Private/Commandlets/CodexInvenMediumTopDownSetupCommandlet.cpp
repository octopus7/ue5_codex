#include "Commandlets/CodexInvenMediumTopDownSetupCommandlet.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "CodexInvenGameInstance.h"
#include "CodexInvenMainGameMode.h"
#include "Engine/Blueprint.h"
#include "Engine/DataAsset.h"
#include "Factories/DataAssetFactory.h"
#include "FileHelpers.h"
#include "GameMapsSettings.h"
#include "Input/CodexInvenMediumInputConfigDataAsset.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "InputEditorModule.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/PackageName.h"
#include "UObject/UnrealType.h"

namespace
{
	const FString InputRootFolder = TEXT("/Game/Input");
	const FString InputActionsFolder = TEXT("/Game/Input/Actions");
	const FString InputContextsFolder = TEXT("/Game/Input/Contexts");
	const FString InputConfigsFolder = TEXT("/Game/Input/Configs");
	const FString BlueprintCoreFolder = TEXT("/Game/Blueprints/Core");

	FString MakeObjectPath(const FString& Folder, const FString& AssetName)
	{
		return FString::Printf(TEXT("%s/%s.%s"), *Folder, *AssetName, *AssetName);
	}

	template <typename TObjectType, typename TFactoryType>
	TObjectType* CreateAssetWithFactory(const FString& Folder, const FString& AssetName, UClass* AssetClass, TFunctionRef<void(TFactoryType&)> SetupFactory)
	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get();
		TFactoryType* Factory = NewObject<TFactoryType>();
		SetupFactory(*Factory);
		return Cast<TObjectType>(AssetTools.CreateAsset(AssetName, Folder, AssetClass, Factory, FName(TEXT("CodexInvenMediumTopDownSetup"))));
	}

	template <typename TObjectType>
	TObjectType* LoadOrCreateAsset(const FString& Folder, const FString& AssetName, TFunctionRef<TObjectType*()> CreateAsset)
	{
		if (TObjectType* Existing = LoadObject<TObjectType>(nullptr, *MakeObjectPath(Folder, AssetName)))
		{
			return Existing;
		}
		return CreateAsset();
	}

	void AddNegate(FEnhancedActionKeyMapping& Mapping, UObject& Outer, const bool bNegateX, const bool bNegateY)
	{
		UInputModifierNegate* Modifier = NewObject<UInputModifierNegate>(&Outer);
		Modifier->bX = bNegateX;
		Modifier->bY = bNegateY;
		Mapping.Modifiers.Add(Modifier);
	}

	void AddSwizzle(FEnhancedActionKeyMapping& Mapping, UObject& Outer)
	{
		UInputModifierSwizzleAxis* Modifier = NewObject<UInputModifierSwizzleAxis>(&Outer);
		Modifier->Order = EInputAxisSwizzle::YXZ;
		Mapping.Modifiers.Add(Modifier);
	}

	FObjectProperty* FindObjectPropertyChecked(UObject& TargetObject, const TCHAR* PropertyName, UClass* ExpectedBaseClass)
	{
		FObjectProperty* ObjectProperty = FindFProperty<FObjectProperty>(TargetObject.GetClass(), PropertyName);
		check(ObjectProperty != nullptr);
		check(ObjectProperty->PropertyClass->IsChildOf(ExpectedBaseClass));
		return ObjectProperty;
	}

	void SetObjectProperty(UObject& TargetObject, const TCHAR* PropertyName, UObject* Value, UClass* ExpectedBaseClass)
	{
		FObjectProperty* ObjectProperty = FindObjectPropertyChecked(TargetObject, PropertyName, ExpectedBaseClass);
		TargetObject.Modify();
		ObjectProperty->SetObjectPropertyValue_InContainer(&TargetObject, Value);
		TargetObject.MarkPackageDirty();
	}

	UBlueprint* LoadOrCreateBlueprint(const FString& Folder, const FString& AssetName, UClass* ParentClass)
	{
		if (UBlueprint* Existing = LoadObject<UBlueprint>(nullptr, *MakeObjectPath(Folder, AssetName)))
		{
			return Existing;
		}

		UPackage* Package = CreatePackage(*FString::Printf(TEXT("%s/%s"), *Folder, *AssetName));
		check(Package != nullptr);
		Package->FullyLoad();

		UBlueprint* Blueprint = FKismetEditorUtilities::CreateBlueprint(
			ParentClass,
			Package,
			*AssetName,
			BPTYPE_Normal,
			UBlueprint::StaticClass(),
			UBlueprintGeneratedClass::StaticClass(),
			FName(TEXT("CodexInvenMediumTopDownSetup")));
		check(Blueprint != nullptr);

		FAssetRegistryModule::AssetCreated(Blueprint);
		FKismetEditorUtilities::CompileBlueprint(Blueprint);
		Blueprint->MarkPackageDirty();
		return Blueprint;
	}
}

UCodexInvenMediumTopDownSetupCommandlet::UCodexInvenMediumTopDownSetupCommandlet()
{
	LogToConsole = true;
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	HelpDescription = TEXT("Creates the default top-down input assets and blueprint wrappers for CodexInvenMedium.");
}

int32 UCodexInvenMediumTopDownSetupCommandlet::Main(const FString& Params)
{
		IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
		AssetRegistry.SearchAllAssets(true);
		AssetRegistry.WaitForCompletion();

		UInputAction* MoveAction = LoadOrCreateAsset<UInputAction>(InputActionsFolder, TEXT("IA_Move"), []()
		{
			return CreateAssetWithFactory<UInputAction, UInputAction_Factory>(InputActionsFolder, TEXT("IA_Move"), UInputAction::StaticClass(), [](UInputAction_Factory& Factory)
			{
			});
		});
		UInputAction* LookAction = LoadOrCreateAsset<UInputAction>(InputActionsFolder, TEXT("IA_Look"), []()
		{
			return CreateAssetWithFactory<UInputAction, UInputAction_Factory>(InputActionsFolder, TEXT("IA_Look"), UInputAction::StaticClass(), [](UInputAction_Factory& Factory)
			{
			});
		});
		UInputAction* JumpAction = LoadOrCreateAsset<UInputAction>(InputActionsFolder, TEXT("IA_Jump"), []()
		{
			return CreateAssetWithFactory<UInputAction, UInputAction_Factory>(InputActionsFolder, TEXT("IA_Jump"), UInputAction::StaticClass(), [](UInputAction_Factory& Factory)
			{
			});
		});
		UInputAction* FireAction = LoadOrCreateAsset<UInputAction>(InputActionsFolder, TEXT("IA_Fire"), []()
		{
			return CreateAssetWithFactory<UInputAction, UInputAction_Factory>(InputActionsFolder, TEXT("IA_Fire"), UInputAction::StaticClass(), [](UInputAction_Factory& Factory)
			{
			});
		});

		check(MoveAction && LookAction && JumpAction && FireAction);
		MoveAction->ValueType = EInputActionValueType::Axis2D;
		LookAction->ValueType = EInputActionValueType::Axis2D;
		JumpAction->ValueType = EInputActionValueType::Boolean;
		FireAction->ValueType = EInputActionValueType::Boolean;
		MoveAction->MarkPackageDirty();
		LookAction->MarkPackageDirty();
		JumpAction->MarkPackageDirty();
		FireAction->MarkPackageDirty();

		UInputMappingContext* MappingContext = LoadOrCreateAsset<UInputMappingContext>(InputContextsFolder, TEXT("IMC_Default"), []()
		{
			return CreateAssetWithFactory<UInputMappingContext, UInputMappingContext_Factory>(InputContextsFolder, TEXT("IMC_Default"), UInputMappingContext::StaticClass(), [](UInputMappingContext_Factory& Factory)
			{
			});
		});
		check(MappingContext != nullptr);

		TArray<FEnhancedActionKeyMapping>& Mappings = const_cast<TArray<FEnhancedActionKeyMapping>&>(MappingContext->GetMappings());
		Mappings.Reset();

		MappingContext->MapKey(MoveAction, EKeys::D);
		{
			FEnhancedActionKeyMapping& Mapping = MappingContext->MapKey(MoveAction, EKeys::A);
			AddNegate(Mapping, *MappingContext, true, false);
		}
		{
			FEnhancedActionKeyMapping& Mapping = MappingContext->MapKey(MoveAction, EKeys::W);
			AddSwizzle(Mapping, *MappingContext);
		}
		{
			FEnhancedActionKeyMapping& Mapping = MappingContext->MapKey(MoveAction, EKeys::S);
			AddSwizzle(Mapping, *MappingContext);
			AddNegate(Mapping, *MappingContext, false, true);
		}
		MappingContext->MapKey(MoveAction, EKeys::Gamepad_Left2D);

		MappingContext->MapKey(LookAction, EKeys::Gamepad_Right2D);
		MappingContext->MapKey(JumpAction, EKeys::SpaceBar);
		MappingContext->MapKey(JumpAction, EKeys::Gamepad_FaceButton_Bottom);
		MappingContext->MapKey(FireAction, EKeys::LeftMouseButton);
		MappingContext->MapKey(FireAction, EKeys::Gamepad_RightTrigger);
		MappingContext->MarkPackageDirty();

		UCodexInvenMediumInputConfigDataAsset* InputConfig = LoadOrCreateAsset<UCodexInvenMediumInputConfigDataAsset>(InputConfigsFolder, TEXT("DA_DefaultInputConfig"), []()
		{
			return CreateAssetWithFactory<UCodexInvenMediumInputConfigDataAsset, UDataAssetFactory>(
				InputConfigsFolder,
				TEXT("DA_DefaultInputConfig"),
				UCodexInvenMediumInputConfigDataAsset::StaticClass(),
				[](UDataAssetFactory& Factory)
				{
					Factory.DataAssetClass = UCodexInvenMediumInputConfigDataAsset::StaticClass();
				});
		});
		check(InputConfig != nullptr);
		InputConfig->InputMappingContext = MappingContext;
		InputConfig->MoveAction = MoveAction;
		InputConfig->LookAction = LookAction;
		InputConfig->JumpAction = JumpAction;
		InputConfig->FireAction = FireAction;
		InputConfig->MarkPackageDirty();

		UBlueprint* GameInstanceBlueprint = LoadOrCreateBlueprint(BlueprintCoreFolder, TEXT("BP_CodexInvenGameInstance"), UCodexInvenGameInstance::StaticClass());
		UBlueprint* GameModeBlueprint = LoadOrCreateBlueprint(BlueprintCoreFolder, TEXT("BP_CodexInvenMainGameMode"), ACodexInvenMainGameMode::StaticClass());
		check(GameInstanceBlueprint && GameModeBlueprint);

		UObject* GameInstanceDefaultObject = GameInstanceBlueprint->GeneratedClass != nullptr ? GameInstanceBlueprint->GeneratedClass->GetDefaultObject() : nullptr;
		check(GameInstanceDefaultObject != nullptr);
		SetObjectProperty(*GameInstanceDefaultObject, TEXT("DefaultInputConfig"), InputConfig, UDataAsset::StaticClass());
		FBlueprintEditorUtils::MarkBlueprintAsModified(GameInstanceBlueprint);
		FKismetEditorUtilities::CompileBlueprint(GameInstanceBlueprint);

		UGameMapsSettings* GameMapsSettings = GetMutableDefault<UGameMapsSettings>();
		GameMapsSettings->GameInstanceClass = FSoftClassPath(GameInstanceBlueprint->GeneratedClass->GetPathName());
		UGameMapsSettings::SetGlobalDefaultGameMode(GameModeBlueprint->GeneratedClass->GetPathName());
		GameMapsSettings->SaveConfig();
		GameMapsSettings->TryUpdateDefaultConfigFile();

		TArray<UPackage*> PackagesToSave;
		PackagesToSave.Add(MoveAction->GetPackage());
		PackagesToSave.Add(LookAction->GetPackage());
		PackagesToSave.Add(JumpAction->GetPackage());
		PackagesToSave.Add(FireAction->GetPackage());
		PackagesToSave.Add(MappingContext->GetPackage());
		PackagesToSave.Add(InputConfig->GetPackage());
		PackagesToSave.Add(GameInstanceBlueprint->GetPackage());
		PackagesToSave.Add(GameModeBlueprint->GetPackage());
		if (!UEditorLoadingAndSavingUtils::SavePackages(PackagesToSave, false))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to save one or more generated top-down setup assets."));
			return 1;
		}

		UE_LOG(LogTemp, Display, TEXT("Top-down setup completed successfully."));
		return 0;
}
