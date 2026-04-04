#include "Commandlets/CMWBootstrapContentCommandlet.h"

#include "Assets/CMWEditorAssetUtils.h"
#include "Components/CanvasPanel.h"
#include "Factories/MaterialFactoryNew.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionVertexColor.h"
#include "Systems/Combat/CMWProjectile.h"
#include "Systems/Game/CMWGameDataAsset.h"
#include "Systems/UI/CMWMinimapWidget.h"
#include "Blueprint/WidgetTree.h"
#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"

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

		UMaterialExpressionVertexColor* VertexColorExpression = NewObject<UMaterialExpressionVertexColor>(Material);
		VertexColorExpression->MaterialExpressionEditorX = -400;
		VertexColorExpression->MaterialExpressionEditorY = 0;

		auto* EditorOnly = Material->GetEditorOnlyData();
		EditorOnly->ExpressionCollection.Expressions.Reset();
		EditorOnly->ExpressionCollection.Expressions.Add(VertexColorExpression);
		EditorOnly->BaseColor.Connect(0, VertexColorExpression);
		EditorOnly->EmissiveColor.Connect(0, VertexColorExpression);

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
	UCMWGameDataAsset* GameData = EnsureGameDataAsset(
		TEXT("/Game/Data/DA_CMWGameData"),
		MappingContext,
		MoveAction,
		AttackAction,
		ToggleAction,
		MinimapWidget,
		SharedVoxelMaterial);

	TArray<UObject*> AssetsToSave;
	AssetsToSave.Append({ MoveAction, AttackAction, ToggleAction, MappingContext, SharedVoxelMaterial, MinimapWidget, GameData });

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

	UE_LOG(LogTemp, Display, TEXT("Bootstrap content commandlet created/updated input, widget, material, and game data assets successfully."));
	return 0;
}
