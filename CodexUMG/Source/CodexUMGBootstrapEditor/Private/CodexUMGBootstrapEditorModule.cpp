#include "CodexUMGBootstrapEditorModule.h"

#include "CodexGameInstance.h"
#include "CodexProjectileActor.h"
#include "CodexProjectileConfigDataAsset.h"
#include "CodexTopDownCharacter.h"
#include "CodexTopDownGameMode.h"
#include "CodexTopDownInputConfigDataAsset.h"
#include "CodexTopDownPlayerController.h"
#include "AssetToolsModule.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Factories/BlueprintFactory.h"
#include "Factories/DataAssetFactory.h"
#include "FileHelpers.h"
#include "GameMapsSettings.h"
#include "GameFramework/GameModeBase.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputEditorModule.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "InputCoreTypes.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Modules/ModuleManager.h"
#include "Subsystems/EditorAssetSubsystem.h"
#include "ToolMenus.h"
#include "UObject/UnrealType.h"

namespace CodexUMGBootstrap
{
	const FString BlueprintsCorePath = TEXT("/Game/Blueprints/Core");
	const FString BlueprintsGameModePath = TEXT("/Game/Blueprints/GameMode");
	const FString BlueprintsPlayerPath = TEXT("/Game/Blueprints/Player");
	const FString BlueprintsProjectilePath = TEXT("/Game/Blueprints/Projectile");
	const FString DataInputPath = TEXT("/Game/Data/Input");
	const FString DataProjectilePath = TEXT("/Game/Data/Projectile");
	const FString InputActionsPath = TEXT("/Game/Input/Actions");
	const FString InputContextsPath = TEXT("/Game/Input/Contexts");

	const TCHAR* MoveActionName = TEXT("IA_Move");
	const TCHAR* FireActionName = TEXT("IA_Fire");
	const TCHAR* MappingContextName = TEXT("IMC_TopDown");
	const TCHAR* InputConfigName = TEXT("DA_TopDownInputConfig");
	const TCHAR* ProjectileConfigName = TEXT("DA_PlayerProjectileConfig");
	const TCHAR* GameInstanceBlueprintName = TEXT("BP_GI_CodexUMG");
	const TCHAR* GameModeBlueprintName = TEXT("BP_GM_TopDown");
	const TCHAR* PlayerControllerBlueprintName = TEXT("BP_PC_TopDown");
	const TCHAR* CharacterBlueprintName = TEXT("BP_Character_TopDown");
	const TCHAR* ProjectileBlueprintName = TEXT("BP_Projectile_PlayerBasic");

	template <typename AssetType>
	AssetType* LoadAsset(const FString& AssetPath)
	{
		return LoadObject<AssetType>(nullptr, *AssetPath);
	}

	FString MakeObjectPath(const FString& PackagePath, const FString& AssetName)
	{
		return FString::Printf(TEXT("%s/%s.%s"), *PackagePath, *AssetName, *AssetName);
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

	bool DoesAssetExist(const FString& PackagePath, const FString& AssetName)
	{
		return LoadAsset<UObject>(MakeObjectPath(PackagePath, AssetName)) != nullptr;
	}

	template <typename AssetType>
	AssetType* CreateAsset(const FString& PackagePath, const FString& AssetName, UFactory* Factory)
	{
		const FString ObjectPath = MakeObjectPath(PackagePath, AssetName);
		if (AssetType* ExistingAsset = LoadAsset<AssetType>(ObjectPath))
		{
			return ExistingAsset;
		}

		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
		return Cast<AssetType>(AssetToolsModule.Get().CreateAsset(AssetName, PackagePath, AssetType::StaticClass(), Factory));
	}

	UBlueprint* CreateBlueprint(const FString& PackagePath, const FString& AssetName, UClass* ParentClass)
	{
		const FString ObjectPath = MakeObjectPath(PackagePath, AssetName);
		if (UBlueprint* ExistingBlueprint = LoadAsset<UBlueprint>(ObjectPath))
		{
			return ExistingBlueprint;
		}

		UBlueprintFactory* Factory = NewObject<UBlueprintFactory>();
		Factory->ParentClass = ParentClass;
		Factory->bSkipClassPicker = true;
		Factory->bEditAfterNew = false;

		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
		return Cast<UBlueprint>(AssetToolsModule.Get().CreateAsset(AssetName, PackagePath, UBlueprint::StaticClass(), Factory));
	}

	UInputModifierNegate* CreateNegateModifier(UObject* Outer, bool bNegateX, bool bNegateY)
	{
		UInputModifierNegate* Modifier = NewObject<UInputModifierNegate>(Outer, NAME_None, RF_Transactional);
		Modifier->bX = bNegateX;
		Modifier->bY = bNegateY;
		Modifier->bZ = false;
		return Modifier;
	}

	UInputModifierSwizzleAxis* CreateSwizzleModifier(UObject* Outer, EInputAxisSwizzle Order)
	{
		UInputModifierSwizzleAxis* Modifier = NewObject<UInputModifierSwizzleAxis>(Outer, NAME_None, RF_Transactional);
		Modifier->Order = Order;
		return Modifier;
	}

	bool SetObjectPropertyByName(UObject& TargetObject, const FName PropertyName, UObject* Value)
	{
		if (FObjectProperty* Property = FindFProperty<FObjectProperty>(TargetObject.GetClass(), PropertyName))
		{
			Property->SetObjectPropertyValue_InContainer(&TargetObject, Value);
			TargetObject.MarkPackageDirty();
			return true;
		}

		return false;
	}

	bool SetClassPropertyByName(UObject& TargetObject, const FName PropertyName, UClass* Value)
	{
		if (FClassProperty* Property = FindFProperty<FClassProperty>(TargetObject.GetClass(), PropertyName))
		{
			Property->SetPropertyValue_InContainer(&TargetObject, Value);
			TargetObject.MarkPackageDirty();
			return true;
		}

		return false;
	}

	bool SetIntPropertyByName(UObject& TargetObject, const FName PropertyName, int32 Value)
	{
		if (FIntProperty* Property = FindFProperty<FIntProperty>(TargetObject.GetClass(), PropertyName))
		{
			Property->SetPropertyValue_InContainer(&TargetObject, Value);
			TargetObject.MarkPackageDirty();
			return true;
		}

		return false;
	}

	void ConfigureMoveAction(UInputAction& MoveAction)
	{
		MoveAction.ValueType = EInputActionValueType::Axis2D;
		MoveAction.AccumulationBehavior = EInputActionAccumulationBehavior::Cumulative;
		MoveAction.Modifiers.Reset();
		MoveAction.Triggers.Reset();
		MoveAction.MarkPackageDirty();
	}

	void ConfigureFireAction(UInputAction& FireAction)
	{
		FireAction.ValueType = EInputActionValueType::Boolean;
		FireAction.Modifiers.Reset();
		FireAction.Triggers.Reset();
		FireAction.MarkPackageDirty();
	}

	void ConfigureMoveMappings(UInputMappingContext& MappingContext, UInputAction& MoveAction)
	{
		MappingContext.UnmapAllKeysFromAction(&MoveAction);

		FEnhancedActionKeyMapping& DMapping = MappingContext.MapKey(&MoveAction, EKeys::D);
		DMapping.Modifiers.Reset();

		FEnhancedActionKeyMapping& AMapping = MappingContext.MapKey(&MoveAction, EKeys::A);
		AMapping.Modifiers.Reset();
		AMapping.Modifiers.Add(CreateNegateModifier(&MappingContext, true, false));

		FEnhancedActionKeyMapping& WMapping = MappingContext.MapKey(&MoveAction, EKeys::W);
		WMapping.Modifiers.Reset();
		WMapping.Modifiers.Add(CreateSwizzleModifier(&MappingContext, EInputAxisSwizzle::YXZ));

		FEnhancedActionKeyMapping& SMapping = MappingContext.MapKey(&MoveAction, EKeys::S);
		SMapping.Modifiers.Reset();
		SMapping.Modifiers.Add(CreateSwizzleModifier(&MappingContext, EInputAxisSwizzle::YXZ));
		SMapping.Modifiers.Add(CreateNegateModifier(&MappingContext, false, true));

		MappingContext.MarkPackageDirty();
	}

	void ConfigureFireMappings(UInputMappingContext& MappingContext, UInputAction& FireAction)
	{
		MappingContext.UnmapAllKeysFromAction(&FireAction);

		FEnhancedActionKeyMapping& SpaceBarMapping = MappingContext.MapKey(&FireAction, EKeys::SpaceBar);
		SpaceBarMapping.Modifiers.Reset();

		MappingContext.MarkPackageDirty();
	}

	void ConfigureInputConfig(UCodexTopDownInputConfigDataAsset& InputConfig, UInputMappingContext& MappingContext, UInputAction& MoveAction, UInputAction& FireAction)
	{
		SetObjectPropertyByName(InputConfig, TEXT("DefaultMappingContext"), &MappingContext);
		SetObjectPropertyByName(InputConfig, TEXT("MoveAction"), &MoveAction);
		SetObjectPropertyByName(InputConfig, TEXT("FireAction"), &FireAction);
		SetIntPropertyByName(InputConfig, TEXT("MappingPriority"), 0);
	}

	void CompileBlueprint(UBlueprint* Blueprint)
	{
		if (Blueprint != nullptr)
		{
			FKismetEditorUtilities::CompileBlueprint(Blueprint);
		}
	}

	void ConfigureProjectileConfig(UCodexProjectileConfigDataAsset& ProjectileConfig, UBlueprint& ProjectileBlueprint)
	{
		CompileBlueprint(&ProjectileBlueprint);

		if (ProjectileBlueprint.GeneratedClass != nullptr)
		{
			SetClassPropertyByName(ProjectileConfig, TEXT("ProjectileClass"), ProjectileBlueprint.GeneratedClass);
		}
	}

	void ConfigureGameInstanceBlueprint(UBlueprint& GameInstanceBlueprint, UCodexTopDownInputConfigDataAsset& InputConfig, UCodexProjectileConfigDataAsset& ProjectileConfig)
	{
		CompileBlueprint(&GameInstanceBlueprint);

		if (UCodexGameInstance* DefaultObject = Cast<UCodexGameInstance>(GameInstanceBlueprint.GeneratedClass->GetDefaultObject()))
		{
			SetObjectPropertyByName(*DefaultObject, TEXT("TopDownInputConfig"), &InputConfig);
			SetObjectPropertyByName(*DefaultObject, TEXT("PlayerProjectileConfig"), &ProjectileConfig);
			FKismetEditorUtilities::CompileBlueprint(&GameInstanceBlueprint);
			GameInstanceBlueprint.MarkPackageDirty();
		}
	}

	void ConfigureGameModeBlueprint(UBlueprint& GameModeBlueprint, UBlueprint& PlayerControllerBlueprint, UBlueprint& CharacterBlueprint)
	{
		CompileBlueprint(&PlayerControllerBlueprint);
		CompileBlueprint(&CharacterBlueprint);
		CompileBlueprint(&GameModeBlueprint);

		if (AGameModeBase* DefaultObject = Cast<AGameModeBase>(GameModeBlueprint.GeneratedClass->GetDefaultObject()))
		{
			DefaultObject->PlayerControllerClass = PlayerControllerBlueprint.GeneratedClass;
			DefaultObject->DefaultPawnClass = CharacterBlueprint.GeneratedClass;
			GameModeBlueprint.MarkPackageDirty();
			FKismetEditorUtilities::CompileBlueprint(&GameModeBlueprint);
		}
	}

	void SaveAssets(const TArray<UObject*>& Assets)
	{
		if (GEditor == nullptr)
		{
			return;
		}

		if (UEditorAssetSubsystem* AssetSubsystem = GEditor->GetEditorSubsystem<UEditorAssetSubsystem>())
		{
			AssetSubsystem->SaveLoadedAssets(Assets, false);
		}
	}

	void ConfigureProjectDefaults(UBlueprint& GameInstanceBlueprint, UBlueprint& GameModeBlueprint)
	{
		CompileBlueprint(&GameInstanceBlueprint);
		CompileBlueprint(&GameModeBlueprint);

		UGameMapsSettings* Settings = GetMutableDefault<UGameMapsSettings>();
		Settings->GameInstanceClass = FSoftClassPath(GameInstanceBlueprint.GeneratedClass);
		UGameMapsSettings::SetGlobalDefaultGameMode(GameModeBlueprint.GeneratedClass->GetPathName());
		Settings->TryUpdateDefaultConfigFile();
	}

	bool NeedsBootstrap()
	{
		return !DoesAssetExist(InputActionsPath, MoveActionName)
			|| !DoesAssetExist(InputActionsPath, FireActionName)
			|| !DoesAssetExist(InputContextsPath, MappingContextName)
			|| !DoesAssetExist(DataInputPath, InputConfigName)
			|| !DoesAssetExist(DataProjectilePath, ProjectileConfigName)
			|| !DoesAssetExist(BlueprintsCorePath, GameInstanceBlueprintName)
			|| !DoesAssetExist(BlueprintsGameModePath, GameModeBlueprintName)
			|| !DoesAssetExist(BlueprintsPlayerPath, PlayerControllerBlueprintName)
			|| !DoesAssetExist(BlueprintsPlayerPath, CharacterBlueprintName)
			|| !DoesAssetExist(BlueprintsProjectilePath, ProjectileBlueprintName);
	}
}

void FCodexUMGBootstrapEditorModule::StartupModule()
{
	if (IsRunningCommandlet())
	{
		return;
	}

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FCodexUMGBootstrapEditorModule::RegisterMenus));

	if (CodexUMGBootstrap::NeedsBootstrap())
	{
		RunBootstrap();
	}
}

void FCodexUMGBootstrapEditorModule::ShutdownModule()
{
	if (UToolMenus::TryGet() != nullptr)
	{
		UToolMenus::UnRegisterStartupCallback(this);
		UToolMenus::UnregisterOwner(this);
	}
}

void FCodexUMGBootstrapEditorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
	FToolMenuSection& Section = Menu->AddSection("CodexUMGBootstrap", FText::FromString(TEXT("CodexUMG")));

	Section.AddMenuEntry(
		"CodexUMG.RunTopDownBootstrap",
		FText::FromString(TEXT("Bootstrap TopDown Gameplay Assets")),
		FText::FromString(TEXT("Create the top-down input, projectile data assets, and BP shells used by CodexUMG.")),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FCodexUMGBootstrapEditorModule::RunBootstrap)));
}

void FCodexUMGBootstrapEditorModule::RunBootstrap()
{
	using namespace CodexUMGBootstrap;

	EnsureDirectory(BlueprintsCorePath);
	EnsureDirectory(BlueprintsGameModePath);
	EnsureDirectory(BlueprintsPlayerPath);
	EnsureDirectory(BlueprintsProjectilePath);
	EnsureDirectory(DataInputPath);
	EnsureDirectory(DataProjectilePath);
	EnsureDirectory(InputActionsPath);
	EnsureDirectory(InputContextsPath);

	UInputAction_Factory* InputActionFactory = NewObject<UInputAction_Factory>();
	InputActionFactory->InputActionClass = UInputAction::StaticClass();
	InputActionFactory->bEditAfterNew = false;
	UInputAction* MoveAction = CreateAsset<UInputAction>(InputActionsPath, MoveActionName, InputActionFactory);
	UInputAction* FireAction = CreateAsset<UInputAction>(InputActionsPath, FireActionName, InputActionFactory);

	UInputMappingContext_Factory* InputMappingFactory = NewObject<UInputMappingContext_Factory>();
	InputMappingFactory->InputMappingContextClass = UInputMappingContext::StaticClass();
	InputMappingFactory->bEditAfterNew = false;
	UInputMappingContext* MappingContext = CreateAsset<UInputMappingContext>(InputContextsPath, MappingContextName, InputMappingFactory);

	UDataAssetFactory* DataAssetFactory = NewObject<UDataAssetFactory>();
	DataAssetFactory->DataAssetClass = UCodexTopDownInputConfigDataAsset::StaticClass();
	DataAssetFactory->bEditAfterNew = false;
	UCodexTopDownInputConfigDataAsset* InputConfig = CreateAsset<UCodexTopDownInputConfigDataAsset>(DataInputPath, InputConfigName, DataAssetFactory);
	DataAssetFactory->DataAssetClass = UCodexProjectileConfigDataAsset::StaticClass();
	UCodexProjectileConfigDataAsset* ProjectileConfig = CreateAsset<UCodexProjectileConfigDataAsset>(DataProjectilePath, ProjectileConfigName, DataAssetFactory);

	UBlueprint* GameInstanceBlueprint = CreateBlueprint(BlueprintsCorePath, GameInstanceBlueprintName, UCodexGameInstance::StaticClass());
	UBlueprint* GameModeBlueprint = CreateBlueprint(BlueprintsGameModePath, GameModeBlueprintName, ACodexTopDownGameMode::StaticClass());
	UBlueprint* PlayerControllerBlueprint = CreateBlueprint(BlueprintsPlayerPath, PlayerControllerBlueprintName, ACodexTopDownPlayerController::StaticClass());
	UBlueprint* CharacterBlueprint = CreateBlueprint(BlueprintsPlayerPath, CharacterBlueprintName, ACodexTopDownCharacter::StaticClass());
	UBlueprint* ProjectileBlueprint = CreateBlueprint(BlueprintsProjectilePath, ProjectileBlueprintName, ACodexProjectileActor::StaticClass());

	if (MoveAction == nullptr || FireAction == nullptr || MappingContext == nullptr || InputConfig == nullptr || ProjectileConfig == nullptr ||
		GameInstanceBlueprint == nullptr || GameModeBlueprint == nullptr ||
		PlayerControllerBlueprint == nullptr || CharacterBlueprint == nullptr || ProjectileBlueprint == nullptr)
	{
		return;
	}

	ConfigureMoveAction(*MoveAction);
	ConfigureFireAction(*FireAction);
	ConfigureMoveMappings(*MappingContext, *MoveAction);
	ConfigureFireMappings(*MappingContext, *FireAction);
	ConfigureInputConfig(*InputConfig, *MappingContext, *MoveAction, *FireAction);
	ConfigureProjectileConfig(*ProjectileConfig, *ProjectileBlueprint);
	ConfigureGameInstanceBlueprint(*GameInstanceBlueprint, *InputConfig, *ProjectileConfig);
	ConfigureGameModeBlueprint(*GameModeBlueprint, *PlayerControllerBlueprint, *CharacterBlueprint);
	ConfigureProjectDefaults(*GameInstanceBlueprint, *GameModeBlueprint);

	SaveAssets(
		{
			MoveAction,
			FireAction,
			MappingContext,
			InputConfig,
			ProjectileConfig,
			GameInstanceBlueprint,
			GameModeBlueprint,
			PlayerControllerBlueprint,
			CharacterBlueprint,
			ProjectileBlueprint
		});
}

IMPLEMENT_MODULE(FCodexUMGBootstrapEditorModule, CodexUMGBootstrapEditor)
