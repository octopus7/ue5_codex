// Copyright Epic Games, Inc. All Rights Reserved.

#include "Simple/GeminiFlashAssetBuilder.h"

#include "Simple/GeminiFlashSimplePopupWidget.h"
#include "Simple/GeminiFlashSimpleSlotWidget.h"
#include "Simple/GeminiFlashSimpleInteractableActor.h"
#include "Interaction/CodexInteractionAssetPaths.h"

#include "AssetToolsModule.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/StaticMeshComponent.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Factories/BlueprintFactory.h"
#include "FileHelpers.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Subsystems/EditorAssetSubsystem.h"
#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"
#include "Misc/PackageName.h"

namespace
{
	static const TCHAR* const GeminiFlashPackagePath = TEXT("/Game/UI/GeminiFlash");
	static const TCHAR* const SimpleSlotWidgetName = TEXT("WBP_SimpleSlot");
	static const TCHAR* const SimplePopupWidgetName = TEXT("WBP_SimplePopup");
	static const TCHAR* const SimpleInteractableName = TEXT("BP_GeminiFlashSimpleInteractable");
	static const TCHAR* const BasicMapAssetPath = TEXT("/Game/Maps/BasicMap");

	FString MakeObjectPath(const FString& PackagePath, const FString& AssetName)
	{
		return FString::Printf(TEXT("%s/%s.%s"), *PackagePath, *AssetName, *AssetName);
	}

	UWidgetBlueprint* CreateWidgetBlueprint(const FString& PackagePath, const FString& AssetName, UClass* ParentClass)
	{
		const FString ObjectPath = MakeObjectPath(PackagePath, AssetName);
		if (UWidgetBlueprint* ExistingBlueprint = LoadObject<UWidgetBlueprint>(nullptr, *ObjectPath))
		{
			return ExistingBlueprint;
		}

		UWidgetBlueprintFactory* Factory = NewObject<UWidgetBlueprintFactory>();
		Factory->ParentClass = ParentClass;
		Factory->BlueprintType = BPTYPE_Normal;
		Factory->bEditAfterNew = false;

		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
		return Cast<UWidgetBlueprint>(AssetToolsModule.Get().CreateAsset(AssetName, PackagePath, UWidgetBlueprint::StaticClass(), Factory));
	}

	UBlueprint* CreateBlueprint(const FString& PackagePath, const FString& AssetName, UClass* ParentClass)
	{
		const FString ObjectPath = MakeObjectPath(PackagePath, AssetName);
		if (UBlueprint* ExistingBlueprint = LoadObject<UBlueprint>(nullptr, *ObjectPath))
		{
			return ExistingBlueprint;
		}

		UBlueprintFactory* Factory = NewObject<UBlueprintFactory>();
		Factory->ParentClass = ParentClass;
		Factory->BlueprintType = BPTYPE_Normal;
		Factory->bEditAfterNew = false;

		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
		return Cast<UBlueprint>(AssetToolsModule.Get().CreateAsset(AssetName, PackagePath, UBlueprint::StaticClass(), Factory));
	}

	void CompileBlueprint(UBlueprint* Blueprint)
	{
		if (Blueprint != nullptr)
		{
			FKismetEditorUtilities::CompileBlueprint(Blueprint);
		}
	}

	void SaveAsset(UObject* Asset)
	{
		if (GEditor && Asset)
		{
			if (UEditorAssetSubsystem* AssetSubsystem = GEditor->GetEditorSubsystem<UEditorAssetSubsystem>())
			{
				AssetSubsystem->SaveLoadedAsset(Asset, false);
			}
		}
	}

	void EnsureWidgetGuid(UWidgetBlueprint* WidgetBlueprint, UWidget* Widget)
	{
		if (WidgetBlueprint && Widget && !WidgetBlueprint->WidgetVariableNameToGuidMap.Contains(Widget->GetFName()))
		{
			WidgetBlueprint->OnVariableAdded(Widget->GetFName());
		}
	}

	bool BuildSimpleSlotWidget(UWidgetBlueprint* WBP, FString& OutError)
	{
		UWidgetTree* Tree = WBP->WidgetTree;
		if (!Tree) return false;

		USizeBox* RootSizeBox = Tree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("SizeBox_Fixed"));
		Tree->RootWidget = RootSizeBox;
		EnsureWidgetGuid(WBP, RootSizeBox);
		RootSizeBox->SetWidthOverride(80.0f);
		RootSizeBox->SetHeightOverride(80.0f);

		UBorder* RootBorder = Tree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border_Highlight"));
		RootSizeBox->SetContent(RootBorder);
		EnsureWidgetGuid(WBP, RootBorder);
		RootBorder->bIsVariable = true;
		RootBorder->SetPadding(FMargin(10.0f));
		RootBorder->SetBrushColor(FLinearColor(1, 1, 1, 0.1f));

		UTextBlock* ValueText = Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TXT_Value"));
		RootBorder->SetContent(ValueText);
		EnsureWidgetGuid(WBP, ValueText);
		ValueText->bIsVariable = true;
		ValueText->SetJustification(ETextJustify::Center);
		
		FSlateFontInfo Font = ValueText->GetFont();
		Font.Size = 32;
		ValueText->SetFont(Font);

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(WBP);
		CompileBlueprint(WBP);
		return true;
	}

	bool BuildSimplePopupWidget(UWidgetBlueprint* WBP, UClass* SlotClass, FString& OutError)
	{
		UWidgetTree* Tree = WBP->WidgetTree;
		if (!Tree) return false;

		UCanvasPanel* RootCanvas = Tree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
		Tree->RootWidget = RootCanvas;
		EnsureWidgetGuid(WBP, RootCanvas);

		// Background Border for the whole grid
		UBorder* BackgroundBorder = Tree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border_Background"));
		RootCanvas->AddChild(BackgroundBorder);
		EnsureWidgetGuid(WBP, BackgroundBorder);
		BackgroundBorder->SetBrushColor(FLinearColor(0.01f, 0.01f, 0.01f, 0.85f)); // Clearer Dark Background
		BackgroundBorder->SetPadding(FMargin(20.0f));

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(BackgroundBorder->Slot))
		{
			CanvasSlot->SetAnchors(FAnchors(0.5f));
			CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
			CanvasSlot->SetAutoSize(true);
		}

		UUniformGridPanel* Grid = Tree->ConstructWidget<UUniformGridPanel>(UUniformGridPanel::StaticClass(), TEXT("Grid_Slots"));
		BackgroundBorder->SetContent(Grid);
		EnsureWidgetGuid(WBP, Grid);
		Grid->bIsVariable = true;
		Grid->SetSlotPadding(FMargin(8.0f));

		for (int32 i = 0; i < 8; ++i)
		{
			const FName SlotName = *FString::Printf(TEXT("Slot_%d"), i);
			UUserWidget* SlotWidget = Tree->ConstructWidget<UUserWidget>(SlotClass, SlotName);
			if (SlotWidget)
			{
				Grid->AddChild(SlotWidget);
				if (UUniformGridSlot* GridSlot = Cast<UUniformGridSlot>(SlotWidget->Slot))
				{
					GridSlot->SetColumn(i % 4);
					GridSlot->SetRow(i / 4);
				}
			}
		}

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(WBP);
		CompileBlueprint(WBP);
		return true;
	}

	bool ConfigureInteractableBlueprint(UBlueprint* Blueprint, FString& OutError)
	{
		CompileBlueprint(Blueprint);

		AGeminiFlashSimpleInteractableActor* DefaultObject = Blueprint->GeneratedClass ? Cast<AGeminiFlashSimpleInteractableActor>(Blueprint->GeneratedClass->GetDefaultObject()) : nullptr;
		if (!DefaultObject) return false;

		UStaticMesh* SignMesh = LoadObject<UStaticMesh>(nullptr, CodexInteractionAssetPaths::WoodenSignMeshObjectPath);
		if (SignMesh && DefaultObject->GetStaticMeshComponent())
		{
			DefaultObject->GetStaticMeshComponent()->SetStaticMesh(SignMesh);
		}

		DefaultObject->MarkPackageDirty();
		Blueprint->MarkPackageDirty();
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
		CompileBlueprint(Blueprint);
		return true;
	}

	bool PlaceActorInMap(UBlueprint* ActorBP, FString& OutError)
	{
		FString MapFilename;
		if (!FPackageName::TryConvertLongPackageNameToFilename(BasicMapAssetPath, MapFilename, FPackageName::GetMapPackageExtension()))
		{
			return false;
		}

		UWorld* World = UEditorLoadingAndSavingUtils::LoadMap(MapFilename);
		if (!World) return false;

		UEditorActorSubsystem* ActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
		if (!ActorSubsystem) return false;

		// Check if already placed
		for (AActor* Actor : ActorSubsystem->GetAllLevelActors())
		{
			if (Actor && Actor->GetActorLabel() == SimpleInteractableName)
			{
				return true; // Already exists
			}
		}

		// Spawn at a fixed location near PlayerStart
		FVector SpawnLoc(500.0f, 0.0f, 100.0f);
		ActorSubsystem->SpawnActorFromClass(Cast<UClass>(ActorBP->GeneratedClass), SpawnLoc, FRotator::ZeroRotator);

		World->MarkPackageDirty();
		UEditorLoadingAndSavingUtils::SaveMap(World, BasicMapAssetPath);
		return true;
	}
}

bool FGeminiFlashAssetBuilder::RunBuild(FString& OutErrorMessage)
{
	// Ensure Directory
	if (GEditor)
	{
		if (UEditorAssetSubsystem* AssetSubsystem = GEditor->GetEditorSubsystem<UEditorAssetSubsystem>())
		{
			if (!AssetSubsystem->DoesDirectoryExist(GeminiFlashPackagePath))
			{
				AssetSubsystem->MakeDirectory(GeminiFlashPackagePath);
			}
		}
	}

	// 1. Widgets
	UWidgetBlueprint* SlotWBP = CreateWidgetBlueprint(GeminiFlashPackagePath, SimpleSlotWidgetName, UGeminiFlashSimpleSlotWidget::StaticClass());
	if (!SlotWBP || !BuildSimpleSlotWidget(SlotWBP, OutErrorMessage))
	{
		OutErrorMessage = TEXT("Failed to build SimpleSlot WBP");
		return false;
	}

	UWidgetBlueprint* PopupWBP = CreateWidgetBlueprint(GeminiFlashPackagePath, SimplePopupWidgetName, UGeminiFlashSimplePopupWidget::StaticClass());
	if (!PopupWBP || !BuildSimplePopupWidget(PopupWBP, SlotWBP->GeneratedClass, OutErrorMessage))
	{
		OutErrorMessage = TEXT("Failed to build SimplePopup WBP");
		return false;
	}

	// 2. Actor
	UBlueprint* ActorBP = CreateBlueprint(GeminiFlashPackagePath, SimpleInteractableName, AGeminiFlashSimpleInteractableActor::StaticClass());
	if (!ActorBP || !ConfigureInteractableBlueprint(ActorBP, OutErrorMessage))
	{
		OutErrorMessage = TEXT("Failed to build SimpleInteractable BP");
		return false;
	}

	// 3. Level Placement
	if (!PlaceActorInMap(ActorBP, OutErrorMessage))
	{
		OutErrorMessage = TEXT("Failed to place actor in map");
		return false;
	}

	SaveAsset(SlotWBP);
	SaveAsset(PopupWBP);
	SaveAsset(ActorBP);

	return true;
}
