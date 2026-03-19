#include "Transient/ClockWidgetScaffolding/CodexInvenClockWidgetScaffoldCommandlet.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Blueprint/WidgetTree.h"
#include "CodexInvenClockWidget.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Fonts/SlateFontInfo.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/PackageName.h"
#include "Styling/SlateColor.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "WidgetBlueprint.h"
#include "WidgetBlueprint/CodexInvenWidgetBlueprintTreeUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogCodexInvenClockWidgetScaffold, Log, All);

namespace
{
	const TCHAR* ClockWidgetPackagePath = TEXT("/Game/UI/WBP_CodexClock");
	const FName ClockWidgetCreationContext(TEXT("CodexInvenClockWidgetScaffold"));
	const FVector2D ClockCanvasSize(176.0f, 176.0f);
	const FVector2D ClockCanvasCenter(ClockCanvasSize.X * 0.5f, ClockCanvasSize.Y * 0.5f);
	const FVector2D ContainerScreenOffset(0.0f, 18.0f);
	const FLinearColor ContainerTint(0.02f, 0.04f, 0.08f, 0.86f);
	const FLinearColor FaceTint(0.05f, 0.08f, 0.12f, 0.65f);
	const FLinearColor MinorTickTint(0.62f, 0.68f, 0.78f, 0.70f);
	const FLinearColor MajorTickTint(0.92f, 0.95f, 1.0f, 0.98f);
	const FLinearColor HourHandTint(0.92f, 0.95f, 1.0f, 1.0f);
	const FLinearColor MinuteHandTint(0.76f, 0.86f, 1.0f, 1.0f);
	const FLinearColor SecondHandTint(1.0f, 0.40f, 0.34f, 1.0f);
	const FLinearColor CenterPinTint(0.93f, 0.96f, 1.0f, 1.0f);

	FString MakeWidgetBlueprintObjectPath(const FString& InPackagePath)
	{
		return InPackagePath + TEXT(".") + FPackageName::GetLongPackageAssetName(InPackagePath);
	}

	bool SaveBlueprintAsset(UWidgetBlueprint& InWidgetBlueprint, FString& OutError)
	{
		UPackage* const Package = InWidgetBlueprint.GetPackage();
		if (Package == nullptr)
		{
			OutError = FString::Printf(TEXT("Widget blueprint %s has no package."), *InWidgetBlueprint.GetName());
			return false;
		}

		const FString Filename = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.SaveFlags = SAVE_NoError;
		SaveArgs.Error = GError;
		if (!UPackage::SavePackage(Package, &InWidgetBlueprint, *Filename, SaveArgs))
		{
			OutError = FString::Printf(TEXT("Failed to save widget blueprint package %s."), *Package->GetName());
			return false;
		}

		return true;
	}

	void SetCanvasSlotLayout(
		UCanvasPanelSlot& InSlot,
		const FVector2D& InPosition,
		const FVector2D& InSize,
		const FAnchors& InAnchors,
		const FVector2D& InAlignment,
		const bool bInAutoSize = false)
	{
		InSlot.SetAnchors(InAnchors);
		InSlot.SetAlignment(InAlignment);
		InSlot.SetPosition(InPosition);
		InSlot.SetAutoSize(bInAutoSize);
		if (!bInAutoSize)
		{
			InSlot.SetSize(InSize);
		}
	}

	void RenameExistingWidgetsToTransient(UWidgetTree& InWidgetTree)
	{
		TArray<UWidget*> ExistingWidgets;
		InWidgetTree.GetAllWidgets(ExistingWidgets);
		for (UWidget* ExistingWidget : ExistingWidgets)
		{
			if (ExistingWidget != nullptr)
			{
				ExistingWidget->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors | REN_NonTransactional | REN_DoNotDirty);
			}
		}

		InWidgetTree.NamedSlotBindings.Empty();
		InWidgetTree.RootWidget = nullptr;
	}

	UBorder* AddClockHand(
		UWidgetTree& InWidgetTree,
		UCanvasPanel& InCanvas,
		const TCHAR* InWidgetName,
		const FVector2D& InSize,
		const FLinearColor& InTint)
	{
		UBorder* const Hand = InWidgetTree.ConstructWidget<UBorder>(UBorder::StaticClass(), FName(InWidgetName));
		Hand->SetBrushColor(InTint);
		Hand->SetRenderTransformPivot(FVector2D(0.5f, 1.0f));

		if (UCanvasPanelSlot* const HandSlot = InCanvas.AddChildToCanvas(Hand))
		{
			SetCanvasSlotLayout(
				*HandSlot,
				ClockCanvasCenter,
				InSize,
				FAnchors(0.0f, 0.0f, 0.0f, 0.0f),
				FVector2D(0.5f, 1.0f));
		}

		return Hand;
	}

	void AddClockTick(UWidgetTree& InWidgetTree, UCanvasPanel& InCanvas, const int32 InTickIndex)
	{
		const bool bIsMajorTick = (InTickIndex % 3) == 0;
		const float AngleDegrees = static_cast<float>(InTickIndex) * 30.0f;
		const float AngleRadians = FMath::DegreesToRadians(AngleDegrees);
		const FVector2D Direction(FMath::Sin(AngleRadians), -FMath::Cos(AngleRadians));
		const float TickRadius = bIsMajorTick ? 72.0f : 70.0f;
		const FVector2D TickSize = bIsMajorTick ? FVector2D(4.0f, 18.0f) : FVector2D(3.0f, 12.0f);
		const FVector2D TickPosition = ClockCanvasCenter + (Direction * TickRadius);
		const FName TickName(*FString::Printf(TEXT("TickMark%02d"), InTickIndex));

		UBorder* const TickWidget = InWidgetTree.ConstructWidget<UBorder>(UBorder::StaticClass(), TickName);
		TickWidget->SetBrushColor(bIsMajorTick ? MajorTickTint : MinorTickTint);
		TickWidget->SetRenderTransformPivot(FVector2D(0.5f, 1.0f));
		TickWidget->SetRenderTransformAngle(AngleDegrees);

		if (UCanvasPanelSlot* const TickSlot = InCanvas.AddChildToCanvas(TickWidget))
		{
			SetCanvasSlotLayout(
				*TickSlot,
				TickPosition,
				TickSize,
				FAnchors(0.0f, 0.0f, 0.0f, 0.0f),
				FVector2D(0.5f, 1.0f));
		}
	}

	void BuildClockWidgetTree(UWidgetBlueprint& InWidgetBlueprint)
	{
		UWidgetTree& WidgetTree = *InWidgetBlueprint.WidgetTree;
		RenameExistingWidgetsToTransient(WidgetTree);

		UCanvasPanel* const RootCanvasPanel = WidgetTree.ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvasPanel"));
		WidgetTree.RootWidget = RootCanvasPanel;

		UBorder* const ContainerBorder = WidgetTree.ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ClockContainerBorder"));
		ContainerBorder->SetBrushColor(ContainerTint);
		ContainerBorder->SetPadding(FMargin(16.0f, 14.0f));
		if (UCanvasPanelSlot* const ContainerSlot = RootCanvasPanel->AddChildToCanvas(ContainerBorder))
		{
			SetCanvasSlotLayout(
				*ContainerSlot,
				ContainerScreenOffset,
				FVector2D::ZeroVector,
				FAnchors(0.5f, 0.0f, 0.5f, 0.0f),
				FVector2D(0.5f, 0.0f),
				true);
		}

		UVerticalBox* const ClockContentBox = WidgetTree.ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ClockContentBox"));
		ContainerBorder->SetContent(ClockContentBox);

		UTextBlock* const ClockTextBlock = WidgetTree.ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ClockTextBlock"));
		FSlateFontInfo DigitalClockFont = ClockTextBlock->GetFont();
		DigitalClockFont.Size = 18;
		ClockTextBlock->SetFont(DigitalClockFont);
		ClockTextBlock->SetText(FText::FromString(TEXT("0000-00-00 00:00:00")));
		ClockTextBlock->SetJustification(ETextJustify::Center);
		ClockTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		ClockTextBlock->SetShadowColorAndOpacity(FLinearColor::Black);
		ClockTextBlock->SetShadowOffset(FVector2D(1.0f, 1.0f));
		if (UVerticalBoxSlot* const DigitalClockSlot = ClockContentBox->AddChildToVerticalBox(ClockTextBlock))
		{
			DigitalClockSlot->SetHorizontalAlignment(HAlign_Center);
			DigitalClockSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
		}

		USizeBox* const AnalogClockSizeBox = WidgetTree.ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("AnalogClockSizeBox"));
		AnalogClockSizeBox->SetWidthOverride(ClockCanvasSize.X);
		AnalogClockSizeBox->SetHeightOverride(ClockCanvasSize.Y);
		if (UVerticalBoxSlot* const AnalogClockBoxSlot = ClockContentBox->AddChildToVerticalBox(AnalogClockSizeBox))
		{
			AnalogClockBoxSlot->SetHorizontalAlignment(HAlign_Center);
		}

		UCanvasPanel* const AnalogClockCanvas = WidgetTree.ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("AnalogClockCanvas"));
		AnalogClockSizeBox->SetContent(AnalogClockCanvas);

		UBorder* const ClockFaceBorder = WidgetTree.ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ClockFaceBorder"));
		ClockFaceBorder->SetBrushColor(FaceTint);
		if (UCanvasPanelSlot* const FaceSlot = AnalogClockCanvas->AddChildToCanvas(ClockFaceBorder))
		{
			FaceSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
			FaceSlot->SetOffsets(FMargin(0.0f));
		}

		for (int32 TickIndex = 0; TickIndex < 12; ++TickIndex)
		{
			AddClockTick(WidgetTree, *AnalogClockCanvas, TickIndex);
		}

		AddClockHand(WidgetTree, *AnalogClockCanvas, TEXT("HourHand"), FVector2D(6.0f, 44.0f), HourHandTint);
		AddClockHand(WidgetTree, *AnalogClockCanvas, TEXT("MinuteHand"), FVector2D(4.0f, 62.0f), MinuteHandTint);
		AddClockHand(WidgetTree, *AnalogClockCanvas, TEXT("SecondHand"), FVector2D(2.0f, 70.0f), SecondHandTint);

		UBorder* const CenterPin = WidgetTree.ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ClockCenterPin"));
		CenterPin->SetBrushColor(CenterPinTint);
		if (UCanvasPanelSlot* const CenterPinSlot = AnalogClockCanvas->AddChildToCanvas(CenterPin))
		{
			SetCanvasSlotLayout(
				*CenterPinSlot,
				ClockCanvasCenter,
				FVector2D(10.0f, 10.0f),
				FAnchors(0.0f, 0.0f, 0.0f, 0.0f),
				FVector2D(0.5f, 0.5f));
		}
	}

	bool CreateOrUpdateClockWidgetBlueprint(UWidgetBlueprint*& OutWidgetBlueprint, FString& OutError)
	{
		UWidgetBlueprint* WidgetBlueprint = LoadObject<UWidgetBlueprint>(nullptr, *MakeWidgetBlueprintObjectPath(ClockWidgetPackagePath));
		bool bCreatedNewBlueprint = false;

		if (WidgetBlueprint == nullptr)
		{
			UPackage* const Package = CreatePackage(ClockWidgetPackagePath);
			WidgetBlueprint = Cast<UWidgetBlueprint>(FKismetEditorUtilities::CreateBlueprint(
				UCodexInvenClockWidget::StaticClass(),
				Package,
				*FPackageName::GetLongPackageAssetName(ClockWidgetPackagePath),
				BPTYPE_Normal,
				UWidgetBlueprint::StaticClass(),
				UWidgetBlueprintGeneratedClass::StaticClass(),
				ClockWidgetCreationContext));
			if (WidgetBlueprint == nullptr)
			{
				OutError = TEXT("Failed to create the clock widget blueprint asset.");
				return false;
			}

			FAssetRegistryModule::AssetCreated(WidgetBlueprint);
			bCreatedNewBlueprint = true;
		}
		else if (WidgetBlueprint->ParentClass != UCodexInvenClockWidget::StaticClass())
		{
			OutError = FString::Printf(
				TEXT("Existing widget blueprint %s uses parent %s instead of %s."),
				*WidgetBlueprint->GetName(),
				*GetNameSafe(WidgetBlueprint->ParentClass),
				*UCodexInvenClockWidget::StaticClass()->GetName());
			return false;
		}

		if (WidgetBlueprint->WidgetTree == nullptr)
		{
			WidgetBlueprint->WidgetTree = NewObject<UWidgetTree>(WidgetBlueprint, TEXT("WidgetTree"), RF_Transactional);
		}

		WidgetBlueprint->SetFlags(RF_Transactional);
		WidgetBlueprint->Modify();
		WidgetBlueprint->WidgetTree->SetFlags(RF_Transactional);
		WidgetBlueprint->WidgetTree->Modify();

		BuildClockWidgetTree(*WidgetBlueprint);

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(WidgetBlueprint);
		FKismetEditorUtilities::CompileBlueprint(WidgetBlueprint);
		WidgetBlueprint->MarkPackageDirty();

		if (!SaveBlueprintAsset(*WidgetBlueprint, OutError))
		{
			return false;
		}

		OutWidgetBlueprint = WidgetBlueprint;

		FString TreeError;
		FCodexInvenWidgetBlueprintTreeNode TreeRoot;
		if (FCodexInvenWidgetBlueprintTreeUtils::BuildWidgetTreeDescription(*WidgetBlueprint, TreeRoot, TreeError))
		{
			UE_LOG(
				LogCodexInvenClockWidgetScaffold,
				Display,
				TEXT("Clock widget tree%s:\n%s"),
				bCreatedNewBlueprint ? TEXT(" created") : TEXT(" updated"),
				*FCodexInvenWidgetBlueprintTreeUtils::FormatWidgetTree(TreeRoot));
		}
		else
		{
			UE_LOG(LogCodexInvenClockWidgetScaffold, Warning, TEXT("%s"), *TreeError);
		}

		return true;
	}
}

UCodexInvenClockWidgetScaffoldCommandlet::UCodexInvenClockWidgetScaffoldCommandlet()
{
	IsServer = false;
	IsClient = false;
	IsEditor = true;
	LogToConsole = true;
	ShowErrorCount = true;
	HelpDescription = TEXT("Create or update /Game/UI/WBP_CodexClock as a BP child of UCodexInvenClockWidget.");
}

int32 UCodexInvenClockWidgetScaffoldCommandlet::Main(const FString& InParams)
{
	static_cast<void>(InParams);

	UWidgetBlueprint* WidgetBlueprint = nullptr;
	FString ErrorMessage;
	if (!CreateOrUpdateClockWidgetBlueprint(WidgetBlueprint, ErrorMessage))
	{
		UE_LOG(LogCodexInvenClockWidgetScaffold, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	UE_LOG(
		LogCodexInvenClockWidgetScaffold,
		Display,
		TEXT("Clock widget blueprint is ready at %s."),
		ClockWidgetPackagePath);

	return 0;
}
