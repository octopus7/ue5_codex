// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editor/CodexEditorAssetLibrary.h"

#if WITH_EDITOR

#include "AssetToolsModule.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "WidgetBlueprintFactory.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/PackageName.h"
#include "Styling/SlateColor.h"
#include "UObject/SavePackage.h"
#include "WidgetBlueprint.h"
#include "Blueprint/UserWidget.h"

namespace
{
	UTextBlock* MakeTextBlock(UWidgetTree* WidgetTree, const TCHAR* Name, const FString& Text, const FLinearColor& Color)
	{
		UTextBlock* TextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), Name);
		TextBlock->SetText(FText::FromString(Text));
		TextBlock->SetColorAndOpacity(FSlateColor(Color));
		return TextBlock;
	}
}

#endif

FString UCodexEditorAssetLibrary::CreateOrbitControlsWidgetBlueprint(const FString& AssetPath)
{
#if WITH_EDITOR
	const FString PackagePath = FPackageName::GetLongPackagePath(AssetPath);
	const FString AssetName = FPackageName::GetLongPackageAssetName(AssetPath);

	if (PackagePath.IsEmpty() || AssetName.IsEmpty())
	{
		return FString();
	}

	UWidgetBlueprintFactory* Factory = NewObject<UWidgetBlueprintFactory>();
	Factory->ParentClass = UUserWidget::StaticClass();

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	UWidgetBlueprint* WidgetBlueprint = Cast<UWidgetBlueprint>(
		AssetToolsModule.Get().CreateAsset(AssetName, PackagePath, UWidgetBlueprint::StaticClass(), Factory));

	if (!WidgetBlueprint || !WidgetBlueprint->WidgetTree)
	{
		return FString();
	}

	UWidgetTree* WidgetTree = WidgetBlueprint->WidgetTree;
	WidgetBlueprint->Modify();
	WidgetTree->Modify();
	WidgetTree->RootWidget = nullptr;

	UBorder* RootBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("RootBorder"));
	RootBorder->SetPadding(FMargin(18.0f, 16.0f));
	RootBorder->SetBrushColor(FLinearColor(0.05f, 0.10f, 0.14f, 0.94f));
	WidgetTree->RootWidget = RootBorder;

	UVerticalBox* ContentBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ContentBox"));
	RootBorder->SetContent(ContentBox);

	UTextBlock* TitleText = MakeTextBlock(WidgetTree, TEXT("TitleText"), TEXT("Orbit Add-ons"), FLinearColor(0.95f, 0.97f, 1.0f, 1.0f));
	if (UVerticalBoxSlot* TitleSlot = ContentBox->AddChildToVerticalBox(TitleText))
	{
		TitleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));
	}

	UTextBlock* BodyText = MakeTextBlock(
		WidgetTree,
		TEXT("BodyText"),
		TEXT("Add a ring Niagara system or a tiny satellite to the current planet."),
		FLinearColor(0.72f, 0.79f, 0.86f, 1.0f));
	if (UVerticalBoxSlot* BodySlot = ContentBox->AddChildToVerticalBox(BodyText))
	{
		BodySlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
	}

	UHorizontalBox* ButtonRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("ButtonRow"));
	if (UVerticalBoxSlot* RowSlot = ContentBox->AddChildToVerticalBox(ButtonRow))
	{
		RowSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 10.0f));
	}

	UButton* AddRingButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("AddRingButton"));
	AddRingButton->SetBackgroundColor(FLinearColor(0.24f, 0.53f, 0.76f, 1.0f));
	AddRingButton->SetContent(MakeTextBlock(WidgetTree, TEXT("AddRingLabel"), TEXT("Add Ring NS"), FLinearColor::White));
	if (UHorizontalBoxSlot* RingSlot = ButtonRow->AddChildToHorizontalBox(AddRingButton))
	{
		RingSlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
	}

	UButton* AddSatelliteButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("AddSatelliteButton"));
	AddSatelliteButton->SetBackgroundColor(FLinearColor(0.28f, 0.62f, 0.44f, 1.0f));
	AddSatelliteButton->SetContent(MakeTextBlock(WidgetTree, TEXT("AddSatelliteLabel"), TEXT("Add Satellite"), FLinearColor::White));
	ButtonRow->AddChildToHorizontalBox(AddSatelliteButton);

	UTextBlock* FooterText = MakeTextBlock(
		WidgetTree,
		TEXT("FooterText"),
		TEXT("Editor-facing WBP placeholder for the later orbit feature pass."),
		FLinearColor(0.58f, 0.66f, 0.73f, 1.0f));
	ContentBox->AddChildToVerticalBox(FooterText);

	FKismetEditorUtilities::CompileBlueprint(WidgetBlueprint);

	UPackage* Package = WidgetBlueprint->GetOutermost();
	const FString PackageFilename = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	UPackage::SavePackage(Package, WidgetBlueprint, *PackageFilename, SaveArgs);

	return AssetPath;
#else
	return FString();
#endif
}
