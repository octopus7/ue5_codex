#include "Commandlets/CMWBootstrapContentCommandlet.h"

#include "Assets/CMWEditorAssetUtils.h"
#include "Components/CanvasPanel.h"
#include "Engine/Blueprint.h"
#include "EngineUtils.h"
#include "Factories/MaterialFactoryNew.h"
#include "FileHelpers.h"
#include "GameFramework/WorldSettings.h"
#include "GameFramework/PlayerStart.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionComponentMask.h"
#include "Materials/MaterialExpressionVertexColor.h"
#include "Systems/Combat/CMWProjectile.h"
#include "Systems/Game/CMWGameDataAsset.h"
#include "Systems/Game/CMWGameMode.h"
#include "Systems/Input/CMWTopDownCharacter.h"
#include "Systems/Input/CMWTopDownPlayerController.h"
#include "Systems/UI/CMWMinimapWidget.h"
#include "Blueprint/WidgetTree.h"
#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"
#include "GameMapsSettings.h"
#include "Misc/PackageName.h"

namespace
{
	UInputAction* EnsureInputAction(const FString& PackagePath, EInputActionValueType ValueType)
	{
		UInputAction* InputAction = CMWEditorAssetUtils::LoadOrCreateAsset<UInputAction>(PackagePath, [ValueType](UPackage* Package, const FName& AssetName)
		{
			return NewObject<UInputAction>(Package, UInputAction::StaticClass(), AssetName, RF_Public | RF_Standalone);
		});

		if (InputAction)
		{
			InputAction->ValueType = ValueType;
			InputAction->MarkPackageDirty();
		}

		return InputAction;
	}

	void AddMappingModifier(FEnhancedActionKeyMapping& Mapping, UInputModifier* Modifier)
	{
		if (Modifier)
		{
			Mapping.Modifiers.Add(Modifier);
		}
	}

	UInputMappingContext* EnsureInputMappingContext(const FString& PackagePath, UInputAction* MoveAction, UInputAction* AttackAction, UInputAction* ToggleAction)
	{
		UInputMappingContext* MappingContext = CMWEditorAssetUtils::LoadOrCreateAsset<UInputMappingContext>(PackagePath, [](UPackage* Package, const FName& AssetName)
		{
			return NewObject<UInputMappingContext>(Package, UInputMappingContext::StaticClass(), AssetName, RF_Public | RF_Standalone);
		});

		if (!MappingContext)
		{
			return nullptr;
		}

		MappingContext->Modify();
		MappingContext->UnmapAll();

		auto ConfigureMoveMapping = [MappingContext](FEnhancedActionKeyMapping& Mapping, bool bNegate, bool bSwizzleToYAxis)
		{
			if (bSwizzleToYAxis)
			{
				UInputModifierSwizzleAxis* SwizzleModifier = NewObject<UInputModifierSwizzleAxis>(MappingContext);
				SwizzleModifier->Order = EInputAxisSwizzle::YXZ;
				AddMappingModifier(Mapping, SwizzleModifier);
			}

			if (bNegate)
			{
				AddMappingModifier(Mapping, NewObject<UInputModifierNegate>(MappingContext));
			}
		};

		if (MoveAction)
		{
			ConfigureMoveMapping(MappingContext->MapKey(MoveAction, EKeys::W), false, true);
			ConfigureMoveMapping(MappingContext->MapKey(MoveAction, EKeys::S), true, true);
			ConfigureMoveMapping(MappingContext->MapKey(MoveAction, EKeys::D), false, false);
			ConfigureMoveMapping(MappingContext->MapKey(MoveAction, EKeys::A), true, false);
		}

		if (AttackAction)
		{
			MappingContext->MapKey(AttackAction, EKeys::LeftMouseButton);
		}

		if (ToggleAction)
		{
			MappingContext->MapKey(ToggleAction, EKeys::RightMouseButton);
			MappingContext->MapKey(ToggleAction, EKeys::Q);
		}

		MappingContext->MarkPackageDirty();
		return MappingContext;
	}

	UMaterial* EnsureVoxelMaterial(const FString& PackagePath)
	{
		UMaterial* Material = CMWEditorAssetUtils::LoadOrCreateAsset<UMaterial>(PackagePath, [](UPackage* Package, const FName& AssetName)
		{
			UMaterialFactoryNew* Factory = NewObject<UMaterialFactoryNew>();
			return Cast<UMaterial>(Factory->FactoryCreateNew(UMaterial::StaticClass(), Package, AssetName, RF_Public | RF_Standalone, nullptr, GWarn));
		});

		if (!Material)
		{
			return nullptr;
		}

		Material->Modify();
		Material->TwoSided = true;
		Material->BlendMode = BLEND_Opaque;
		Material->SetShadingModel(MSM_Unlit);

		UMaterialExpressionVertexColor* VertexColorExpression = NewObject<UMaterialExpressionVertexColor>(Material);
		VertexColorExpression->MaterialExpressionEditorX = -400;
		VertexColorExpression->MaterialExpressionEditorY = 0;

		UMaterialExpressionComponentMask* ColorMaskExpression = NewObject<UMaterialExpressionComponentMask>(Material);
		ColorMaskExpression->MaterialExpressionEditorX = -220;
		ColorMaskExpression->MaterialExpressionEditorY = 0;
		ColorMaskExpression->Input.Connect(0, VertexColorExpression);
		ColorMaskExpression->R = true;
		ColorMaskExpression->G = true;
		ColorMaskExpression->B = true;
		ColorMaskExpression->A = false;

		auto* EditorOnly = Material->GetEditorOnlyData();
		EditorOnly->ExpressionCollection.Expressions.Reset();
		EditorOnly->ExpressionCollection.Expressions.Add(VertexColorExpression);
		EditorOnly->ExpressionCollection.Expressions.Add(ColorMaskExpression);
		EditorOnly->BaseColor.Connect(0, ColorMaskExpression);
		EditorOnly->EmissiveColor.Connect(0, ColorMaskExpression);

		Material->PostEditChange();
		Material->MarkPackageDirty();
		return Material;
	}

	UWidgetBlueprint* EnsureMinimapWidgetBlueprint(const FString& PackagePath)
	{
		UWidgetBlueprint* WidgetBlueprint = CMWEditorAssetUtils::LoadOrCreateAsset<UWidgetBlueprint>(PackagePath, [](UPackage* Package, const FName& AssetName)
		{
			UWidgetBlueprintFactory* Factory = NewObject<UWidgetBlueprintFactory>();
			Factory->ParentClass = UCMWMinimapWidget::StaticClass();
			return Cast<UWidgetBlueprint>(Factory->FactoryCreateNew(UWidgetBlueprint::StaticClass(), Package, AssetName, RF_Public | RF_Standalone, nullptr, GWarn, NAME_None));
		});

		if (WidgetBlueprint)
		{
			if (WidgetBlueprint->WidgetTree && WidgetBlueprint->WidgetTree->RootWidget == nullptr)
			{
				WidgetBlueprint->WidgetTree->Modify();
				WidgetBlueprint->WidgetTree->RootWidget = WidgetBlueprint->WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
			}

			FKismetEditorUtilities::CompileBlueprint(WidgetBlueprint);
			WidgetBlueprint->MarkPackageDirty();
		}

		return WidgetBlueprint;
	}

	UCMWGameDataAsset* EnsureGameDataAsset(
		const FString& PackagePath,
		UInputMappingContext* MappingContext,
		UInputAction* MoveAction,
		UInputAction* AttackAction,
		UInputAction* ToggleAction,
		UWidgetBlueprint* MinimapWidgetBlueprint,
		UMaterial* SharedVoxelMaterial)
	{
		UCMWGameDataAsset* GameDataAsset = CMWEditorAssetUtils::LoadOrCreateAsset<UCMWGameDataAsset>(PackagePath, [](UPackage* Package, const FName& AssetName)
		{
			return NewObject<UCMWGameDataAsset>(Package, UCMWGameDataAsset::StaticClass(), AssetName, RF_Public | RF_Standalone);
		});

		if (!GameDataAsset)
		{
			return nullptr;
		}

		GameDataAsset->Modify();
		GameDataAsset->PlayerMappingContext = MappingContext;
		GameDataAsset->MoveAction = MoveAction;
		GameDataAsset->AttackAction = AttackAction;
		GameDataAsset->ToggleAttackModeAction = ToggleAction;
		GameDataAsset->ProjectileClass = ACMWProjectile::StaticClass();
		GameDataAsset->SharedVoxelMaterial = SharedVoxelMaterial;
		if (MinimapWidgetBlueprint && MinimapWidgetBlueprint->GeneratedClass)
		{
			GameDataAsset->MinimapWidgetClass = MinimapWidgetBlueprint->GeneratedClass;
		}

		GameDataAsset->MarkPackageDirty();
		return GameDataAsset;
	}

	UBlueprint* EnsureBlueprintAsset(const FString& PackagePath, UClass* ParentClass)
	{
		UBlueprint* BlueprintAsset = CMWEditorAssetUtils::LoadAsset<UBlueprint>(PackagePath);
		if (!BlueprintAsset)
		{
			UPackage* Package = CMWEditorAssetUtils::FindOrCreatePackage(PackagePath);
			const FString AssetName = CMWEditorAssetUtils::GetAssetNameFromPackagePath(PackagePath);
			BlueprintAsset = FKismetEditorUtilities::CreateBlueprint(
				ParentClass,
				Package,
				*AssetName,
				BPTYPE_Normal,
				UBlueprint::StaticClass(),
				UBlueprintGeneratedClass::StaticClass(),
				FName(TEXT("CMWBootstrapContent")));
		}

		if (BlueprintAsset)
		{
			FKismetEditorUtilities::CompileBlueprint(BlueprintAsset);
			BlueprintAsset->MarkPackageDirty();
		}

		return BlueprintAsset;
	}

	UBlueprint* EnsureGameModeBlueprint(const FString& PackagePath, UBlueprint* PawnBlueprint, UBlueprint* PlayerControllerBlueprint)
	{
		UBlueprint* GameModeBlueprint = EnsureBlueprintAsset(PackagePath, ACMWGameMode::StaticClass());
		if (!GameModeBlueprint || !GameModeBlueprint->GeneratedClass)
		{
			return GameModeBlueprint;
		}

		if (AGameModeBase* GameModeDefaults = Cast<AGameModeBase>(GameModeBlueprint->GeneratedClass->GetDefaultObject()))
		{
			GameModeDefaults->Modify();
			UClass* PawnClass = PawnBlueprint && PawnBlueprint->GeneratedClass
				? static_cast<UClass*>(PawnBlueprint->GeneratedClass)
				: static_cast<UClass*>(ACMWTopDownCharacter::StaticClass());
			UClass* PlayerControllerClass = PlayerControllerBlueprint && PlayerControllerBlueprint->GeneratedClass
				? static_cast<UClass*>(PlayerControllerBlueprint->GeneratedClass)
				: static_cast<UClass*>(ACMWTopDownPlayerController::StaticClass());

			GameModeDefaults->DefaultPawnClass = PawnClass;
			GameModeDefaults->PlayerControllerClass = PlayerControllerClass;
		}

		FKismetEditorUtilities::CompileBlueprint(GameModeBlueprint);
		GameModeBlueprint->MarkPackageDirty();
		return GameModeBlueprint;
	}

	bool ConfigureProjectAndMapGameMode(UBlueprint* GameModeBlueprint, const FString& MapAssetPath, FString& OutError)
	{
		if (!GameModeBlueprint || !GameModeBlueprint->GeneratedClass)
		{
			OutError = TEXT("GameMode Blueprint or its generated class was null.");
			return false;
		}

		const FString GameModeClassPath = GameModeBlueprint->GeneratedClass->GetPathName();
		UGameMapsSettings::SetGlobalDefaultGameMode(GameModeClassPath);
		UGameMapsSettings* GameMapsSettings = GetMutableDefault<UGameMapsSettings>();
		GameMapsSettings->SaveConfig();
		GameMapsSettings->TryUpdateDefaultConfigFile();

		const FString MapFilename = FPackageName::LongPackageNameToFilename(MapAssetPath, FPackageName::GetMapPackageExtension());
		UWorld* World = UEditorLoadingAndSavingUtils::LoadMap(MapFilename);
		if (!World)
		{
			OutError = FString::Printf(TEXT("Failed to load map '%s'."), *MapAssetPath);
			return false;
		}

		AWorldSettings* WorldSettings = World->GetWorldSettings();
		if (!WorldSettings)
		{
			OutError = FString::Printf(TEXT("Map '%s' did not have valid WorldSettings."), *MapAssetPath);
			return false;
		}

		WorldSettings->Modify();
		WorldSettings->DefaultGameMode = GameModeBlueprint->GeneratedClass;

		constexpr float TargetPlayerStartZ = 240.0f;
		bool bHasPlayerStart = false;
		for (TActorIterator<APlayerStart> It(World); It; ++It)
		{
			bHasPlayerStart = true;

			const FVector CurrentLocation = It->GetActorLocation();
			if (CurrentLocation.Z < TargetPlayerStartZ)
			{
				It->Modify();
				It->SetActorLocation(FVector(CurrentLocation.X, CurrentLocation.Y, TargetPlayerStartZ), false, nullptr, ETeleportType::TeleportPhysics);
			}
		}

		if (!bHasPlayerStart)
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.OverrideLevel = World->PersistentLevel;
			if (APlayerStart* PlayerStart = World->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), FVector(0.0f, 0.0f, TargetPlayerStartZ), FRotator::ZeroRotator, SpawnParameters))
			{
				PlayerStart->SetActorLabel(TEXT("CMW_PlayerStart"));
			}
		}

		World->MarkPackageDirty();

		if (!UEditorLoadingAndSavingUtils::SaveMap(World, MapAssetPath))
		{
			OutError = FString::Printf(TEXT("Failed to save map '%s' after setting GameMode Override."), *MapAssetPath);
			return false;
		}

		UE_LOG(LogTemp, Display, TEXT("Configured project default and map override GameMode to '%s'."), *GameModeClassPath);
		return true;
	}
}

UCMWBootstrapContentCommandlet::UCMWBootstrapContentCommandlet()
{
	IsClient = false;
	IsEditor = true;
	LogToConsole = true;
}

int32 UCMWBootstrapContentCommandlet::Main(const FString& Params)
{
	UInputAction* MoveAction = EnsureInputAction(TEXT("/Game/Input/IA_Move"), EInputActionValueType::Axis2D);
	UInputAction* AttackAction = EnsureInputAction(TEXT("/Game/Input/IA_Attack"), EInputActionValueType::Boolean);
	UInputAction* ToggleAction = EnsureInputAction(TEXT("/Game/Input/IA_ToggleAttackMode"), EInputActionValueType::Boolean);
	UInputMappingContext* MappingContext = EnsureInputMappingContext(TEXT("/Game/Input/IMC_Player"), MoveAction, AttackAction, ToggleAction);
	UMaterial* SharedVoxelMaterial = EnsureVoxelMaterial(TEXT("/Game/Voxel/Materials/M_VoxelVertexColor"));
	UWidgetBlueprint* MinimapWidget = EnsureMinimapWidgetBlueprint(TEXT("/Game/UI/WBP_Minimap"));
	UBlueprint* PawnBlueprint = EnsureBlueprintAsset(TEXT("/Game/Blueprints/BP_CMWTopDownCharacter"), ACMWTopDownCharacter::StaticClass());
	UBlueprint* PlayerControllerBlueprint = EnsureBlueprintAsset(TEXT("/Game/Blueprints/BP_CMWTopDownPlayerController"), ACMWTopDownPlayerController::StaticClass());
	UBlueprint* GameModeBlueprint = EnsureGameModeBlueprint(TEXT("/Game/Blueprints/BP_CMWGameMode"), PawnBlueprint, PlayerControllerBlueprint);
	UCMWGameDataAsset* GameData = EnsureGameDataAsset(
		TEXT("/Game/Data/DA_CMWGameData"),
		MappingContext,
		MoveAction,
		AttackAction,
		ToggleAction,
		MinimapWidget,
		SharedVoxelMaterial);

	TArray<UObject*> AssetsToSave;
	AssetsToSave.Append({ MoveAction, AttackAction, ToggleAction, MappingContext, SharedVoxelMaterial, MinimapWidget, PawnBlueprint, PlayerControllerBlueprint, GameModeBlueprint, GameData });

	for (UObject* Asset : AssetsToSave)
	{
		if (!Asset)
		{
			UE_LOG(LogTemp, Error, TEXT("Bootstrap content failed because one of the assets could not be created."));
			return 1;
		}

		FString SaveError;
		if (!CMWEditorAssetUtils::SaveAsset(Asset, &SaveError))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to save asset '%s': %s"), *Asset->GetPathName(), *SaveError);
			return 1;
		}
	}

	FString GameModeConfigurationError;
	if (!ConfigureProjectAndMapGameMode(GameModeBlueprint, TEXT("/Game/Maps/BasicMap"), GameModeConfigurationError))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to configure GameMode Blueprint: %s"), *GameModeConfigurationError);
		return 1;
	}

	UE_LOG(LogTemp, Display, TEXT("Bootstrap content commandlet created/updated input, widget, GameMode Blueprint, material, and game data assets successfully."));
	return 0;
}
