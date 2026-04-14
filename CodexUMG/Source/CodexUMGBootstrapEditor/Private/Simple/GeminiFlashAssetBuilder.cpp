// Copyright Epic Games, Inc. All Rights Reserved.

#include "Simple/GeminiFlashAssetBuilder.h"

#include "Simple/GeminiFlashSimplePopupWidget.h"
#include "Simple/GeminiFlashSimpleSlotWidget.h"

#include "AssetToolsModule.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/TextBlock.h"
#include "Editor.h"
#include "Factories/BlueprintFactory.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Subsystems/EditorAssetSubsystem.h"
#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"

namespace
{
	static const TCHAR* const GeminiFlashPackagePath = TEXT("/Game/UI/GeminiFlash");
	static const TCHAR* const SimpleSlotWidgetName = TEXT("WBP_SimpleSlot");
	static const TCHAR* const SimplePopupWidgetName = TEXT("WBP_SimplePopup");

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

		// Root Border
		UBorder* RootBorder = Tree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border_Highlight"));
		Tree->RootWidget = RootBorder;
		EnsureWidgetGuid(WBP, RootBorder);
		RootBorder->bIsVariable = true;
		RootBorder->SetPadding(FMargin(10.0f));
		RootBorder->SetBrushColor(FLinearColor(1, 1, 1, 0.1f));

		// Value Text
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

		// UniformGridPanel
		UUniformGridPanel* Grid = Tree->ConstructWidget<UUniformGridPanel>(UUniformGridPanel::StaticClass(), TEXT("Grid_Slots"));
		RootCanvas->AddChild(Grid);
		EnsureWidgetGuid(WBP, Grid);
		Grid->bIsVariable = true;
		Grid->SetSlotPadding(FMargin(5.0f));

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Grid->Slot))
		{
			CanvasSlot->SetAnchors(FAnchors(0.5f));
			CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
			CanvasSlot->SetAutoSize(true);
		}

		// Create 8 slots in a 4x2 grid
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
}

bool FGeminiFlashAssetBuilder::RunBuild(FString& OutErrorMessage)
{
	// 1. Ensure directory
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

	// 2. Build Simple Slot
	UWidgetBlueprint* SlotWBP = CreateWidgetBlueprint(GeminiFlashPackagePath, SimpleSlotWidgetName, UGeminiFlashSimpleSlotWidget::StaticClass());
	if (!SlotWBP || !BuildSimpleSlotWidget(SlotWBP, OutErrorMessage))
	{
		OutErrorMessage = TEXT("Failed to build SimpleSlot WBP");
		return false;
	}

	// 3. Build Simple Popup
	UWidgetBlueprint* PopupWBP = CreateWidgetBlueprint(GeminiFlashPackagePath, SimplePopupWidgetName, UGeminiFlashSimplePopupWidget::StaticClass());
	if (!PopupWBP || !BuildSimplePopupWidget(PopupWBP, SlotWBP->GeneratedClass, OutErrorMessage))
	{
		OutErrorMessage = TEXT("Failed to build SimplePopup WBP");
		return false;
	}

	SaveAsset(SlotWBP);
	SaveAsset(PopupWBP);

	return true;
}
