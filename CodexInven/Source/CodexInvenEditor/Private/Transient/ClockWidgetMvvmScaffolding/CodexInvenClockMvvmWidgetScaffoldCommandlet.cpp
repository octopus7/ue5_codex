#include "Transient/ClockWidgetMvvmScaffolding/CodexInvenClockMvvmWidgetScaffoldCommandlet.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Blueprint/WidgetTree.h"
#include "CodexInvenClockMvvmViewModel.h"
#include "CodexInvenClockMvvmWidget.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Widget.h"
#include "Editor.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/PackageName.h"
#include "MVVMBlueprintView.h"
#include "MVVMBlueprintViewBinding.h"
#include "MVVMBlueprintViewModelContext.h"
#include "MVVMEditorSubsystem.h"
#include "MVVMPropertyPath.h"
#include "Types/MVVMBindingMode.h"
#include "Styling/SlateColor.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/UnrealType.h"
#include "WidgetBlueprint.h"
#include "WidgetBlueprint/CodexInvenWidgetBlueprintTreeUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogCodexInvenClockMvvmWidgetScaffold, Log, All);

namespace
{
	const TCHAR* ClockWidgetPackagePath = TEXT("/Game/UI/WBP_CodexClockMvvm");
	const TCHAR* ClockFaceTexturePackagePath = TEXT("/Game/UI/T_ClockFace_Analog");
	const FName ClockWidgetCreationContext(TEXT("CodexInvenClockMvvmWidgetScaffold"));
	const FName ClockViewModelName(TEXT("ClockViewModel"));
	const FVector2D ClockCanvasSize(176.0f, 176.0f);
	const FVector2D ClockCanvasCenter(ClockCanvasSize.X * 0.5f, ClockCanvasSize.Y * 0.5f);
	const FVector2D ContainerScreenOffset(-216.0f, 18.0f);
	const FLinearColor ContainerTint(0.25f, 0.14f, 0.08f, 0.88f);
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

	void BuildClockWidgetTree(UWidgetBlueprint& InWidgetBlueprint, UTexture2D& InClockFaceTexture)
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

		UImage* const ClockFaceImage = WidgetTree.ConstructWidget<UImage>(UImage::StaticClass(), TEXT("ClockFaceImage"));
		ClockFaceImage->SetBrushFromTexture(&InClockFaceTexture, true);
		if (UCanvasPanelSlot* const FaceSlot = AnalogClockCanvas->AddChildToCanvas(ClockFaceImage))
		{
			FaceSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
			FaceSlot->SetOffsets(FMargin(0.0f));
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

	bool AddFunctionBinding(
		UWidgetBlueprint& InWidgetBlueprint,
		UMVVMEditorSubsystem& InEditorSubsystem,
		const FGuid InViewModelId,
		const FName InWidgetName,
		const FName InSourcePropertyName,
		const UClass& InDestinationOwnerClass,
		const FName InDestinationFunctionName,
		FString& OutError)
	{
		FProperty* const SourceProperty = FindFProperty<FProperty>(UCodexInvenClockMvvmViewModel::StaticClass(), InSourcePropertyName);
		if (SourceProperty == nullptr)
		{
			OutError = FString::Printf(TEXT("Failed to find viewmodel property %s."), *InSourcePropertyName.ToString());
			return false;
		}

		const UFunction* const DestinationFunction = InDestinationOwnerClass.FindFunctionByName(InDestinationFunctionName);
		if (DestinationFunction == nullptr)
		{
			OutError = FString::Printf(TEXT("Failed to find widget function %s on %s."), *InDestinationFunctionName.ToString(), *InDestinationOwnerClass.GetName());
			return false;
		}

		FMVVMBlueprintPropertyPath SourcePath;
		SourcePath.SetViewModelId(InViewModelId);
		SourcePath.AppendPropertyPath(&InWidgetBlueprint, UE::MVVM::FMVVMConstFieldVariant(SourceProperty));

		FMVVMBlueprintPropertyPath DestinationPath;
		DestinationPath.SetWidgetName(InWidgetName);
		DestinationPath.AppendPropertyPath(&InWidgetBlueprint, UE::MVVM::FMVVMConstFieldVariant(DestinationFunction));

		FMVVMBlueprintViewBinding& Binding = InEditorSubsystem.AddBinding(&InWidgetBlueprint);
		InEditorSubsystem.SetSourcePathForBinding(&InWidgetBlueprint, Binding, SourcePath);
		InEditorSubsystem.SetDestinationPathForBinding(&InWidgetBlueprint, Binding, DestinationPath, false);
		InEditorSubsystem.SetBindingTypeForBinding(&InWidgetBlueprint, Binding, EMVVMBindingMode::OneWayToDestination);
		return true;
	}

	bool ConfigureMvvmView(UWidgetBlueprint& InWidgetBlueprint, FString& OutError)
	{
		if (GEditor == nullptr)
		{
			OutError = TEXT("The editor subsystem is unavailable.");
			return false;
		}

		UMVVMEditorSubsystem* const EditorSubsystem = GEditor->GetEditorSubsystem<UMVVMEditorSubsystem>();
		if (EditorSubsystem == nullptr)
		{
			OutError = TEXT("Failed to acquire the MVVM editor subsystem.");
			return false;
		}

		UMVVMBlueprintView* View = EditorSubsystem->RequestView(&InWidgetBlueprint);
		if (View == nullptr)
		{
			OutError = TEXT("Failed to create the MVVM blueprint view.");
			return false;
		}

		while (View->GetNumBindings() > 0)
		{
			const FMVVMBlueprintViewBinding* const Binding = View->GetBindingAt(0);
			if (Binding == nullptr)
			{
				break;
			}

			EditorSubsystem->RemoveBinding(&InWidgetBlueprint, *Binding);
		}

		TArray<FName> ExistingViewModelNames;
		for (const FMVVMBlueprintViewModelContext& ExistingContext : View->GetViewModels())
		{
			ExistingViewModelNames.Add(ExistingContext.GetViewModelName());
		}

		for (const FName ExistingViewModelName : ExistingViewModelNames)
		{
			EditorSubsystem->RemoveViewModel(&InWidgetBlueprint, ExistingViewModelName);
		}

		const FGuid ViewModelId = EditorSubsystem->AddViewModel(&InWidgetBlueprint, UCodexInvenClockMvvmViewModel::StaticClass());
		View = EditorSubsystem->GetView(&InWidgetBlueprint);
		if (View == nullptr)
		{
			OutError = TEXT("The MVVM blueprint view disappeared after adding the viewmodel.");
			return false;
		}

		const FMVVMBlueprintViewModelContext* const AddedViewModel = View->FindViewModel(ViewModelId);
		if (AddedViewModel == nullptr)
		{
			OutError = TEXT("Failed to resolve the added MVVM viewmodel.");
			return false;
		}

		if (AddedViewModel->GetViewModelName() != ClockViewModelName)
		{
			FText RenameError;
			if (!EditorSubsystem->RenameViewModel(&InWidgetBlueprint, AddedViewModel->GetViewModelName(), ClockViewModelName, RenameError))
			{
				OutError = FString::Printf(TEXT("Failed to rename the MVVM viewmodel: %s"), *RenameError.ToString());
				return false;
			}
		}

		View = EditorSubsystem->GetView(&InWidgetBlueprint);
		if (View == nullptr)
		{
			OutError = TEXT("Failed to reacquire the MVVM blueprint view after renaming the viewmodel.");
			return false;
		}

		const FMVVMBlueprintViewModelContext* const NamedViewModel = View->FindViewModel(ClockViewModelName);
		if (NamedViewModel == nullptr)
		{
			OutError = TEXT("Failed to find the renamed MVVM viewmodel.");
			return false;
		}

		FMVVMBlueprintViewModelContext* const MutableViewModel = View->FindViewModel(NamedViewModel->GetViewModelId());
		if (MutableViewModel == nullptr)
		{
			OutError = TEXT("Failed to get a mutable MVVM viewmodel context.");
			return false;
		}

		MutableViewModel->CreationType = EMVVMBlueprintViewModelContextCreationType::CreateInstance;
		MutableViewModel->bCreateGetterFunction = false;
		MutableViewModel->bCreateSetterFunction = false;
		MutableViewModel->bOptional = false;

		if (View->GetSettings() != nullptr)
		{
			View->GetSettings()->bInitializeSourcesOnConstruct = true;
			View->GetSettings()->bInitializeBindingsOnConstruct = true;
			View->GetSettings()->bInitializeEventsOnConstruct = true;
			View->GetSettings()->bCreateViewWithoutBindings = true;
		}

		if (!AddFunctionBinding(
			InWidgetBlueprint,
			*EditorSubsystem,
			NamedViewModel->GetViewModelId(),
			TEXT("ClockTextBlock"),
			TEXT("DigitalClockText"),
			*UTextBlock::StaticClass(),
			TEXT("SetText"),
			OutError))
		{
			return false;
		}

		if (!AddFunctionBinding(
			InWidgetBlueprint,
			*EditorSubsystem,
			NamedViewModel->GetViewModelId(),
			TEXT("HourHand"),
			TEXT("HourHandAngle"),
			*UWidget::StaticClass(),
			TEXT("SetRenderTransformAngle"),
			OutError))
		{
			return false;
		}

		if (!AddFunctionBinding(
			InWidgetBlueprint,
			*EditorSubsystem,
			NamedViewModel->GetViewModelId(),
			TEXT("MinuteHand"),
			TEXT("MinuteHandAngle"),
			*UWidget::StaticClass(),
			TEXT("SetRenderTransformAngle"),
			OutError))
		{
			return false;
		}

		if (!AddFunctionBinding(
			InWidgetBlueprint,
			*EditorSubsystem,
			NamedViewModel->GetViewModelId(),
			TEXT("SecondHand"),
			TEXT("SecondHandAngle"),
			*UWidget::StaticClass(),
			TEXT("SetRenderTransformAngle"),
			OutError))
		{
			return false;
		}

		return true;
	}

	bool CreateOrUpdateClockWidgetBlueprint(UWidgetBlueprint*& OutWidgetBlueprint, FString& OutError)
	{
		UTexture2D* const ClockFaceTexture = LoadObject<UTexture2D>(nullptr, *MakeWidgetBlueprintObjectPath(ClockFaceTexturePackagePath));
		if (ClockFaceTexture == nullptr)
		{
			OutError = TEXT("Failed to load /Game/UI/T_ClockFace_Analog. The base clock face texture must exist before scaffolding the MVVM clock.");
			return false;
		}

		UWidgetBlueprint* WidgetBlueprint = LoadObject<UWidgetBlueprint>(nullptr, *MakeWidgetBlueprintObjectPath(ClockWidgetPackagePath));
		const bool bCreatedNewBlueprint = WidgetBlueprint == nullptr;

		if (WidgetBlueprint == nullptr)
		{
			UPackage* const Package = CreatePackage(ClockWidgetPackagePath);
			WidgetBlueprint = Cast<UWidgetBlueprint>(FKismetEditorUtilities::CreateBlueprint(
				UCodexInvenClockMvvmWidget::StaticClass(),
				Package,
				*FPackageName::GetLongPackageAssetName(ClockWidgetPackagePath),
				BPTYPE_Normal,
				UWidgetBlueprint::StaticClass(),
				UWidgetBlueprintGeneratedClass::StaticClass(),
				ClockWidgetCreationContext));
			if (WidgetBlueprint == nullptr)
			{
				OutError = TEXT("Failed to create the MVVM clock widget blueprint asset.");
				return false;
			}

			FAssetRegistryModule::AssetCreated(WidgetBlueprint);
		}
		else if (WidgetBlueprint->ParentClass != UCodexInvenClockMvvmWidget::StaticClass())
		{
			OutError = FString::Printf(
				TEXT("Existing widget blueprint %s uses parent %s instead of %s."),
				*WidgetBlueprint->GetName(),
				*GetNameSafe(WidgetBlueprint->ParentClass),
				*UCodexInvenClockMvvmWidget::StaticClass()->GetName());
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

		BuildClockWidgetTree(*WidgetBlueprint, *ClockFaceTexture);
		WidgetBlueprint->WidgetVariableNameToGuidMap.Reset();

		if (!ConfigureMvvmView(*WidgetBlueprint, OutError))
		{
			return false;
		}

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
				LogCodexInvenClockMvvmWidgetScaffold,
				Display,
				TEXT("MVVM clock widget tree%s:\n%s"),
				bCreatedNewBlueprint ? TEXT(" created") : TEXT(" updated"),
				*FCodexInvenWidgetBlueprintTreeUtils::FormatWidgetTree(TreeRoot));
		}
		else
		{
			UE_LOG(LogCodexInvenClockMvvmWidgetScaffold, Warning, TEXT("%s"), *TreeError);
		}

		return true;
	}
}

UCodexInvenClockMvvmWidgetScaffoldCommandlet::UCodexInvenClockMvvmWidgetScaffoldCommandlet()
{
	IsServer = false;
	IsClient = false;
	IsEditor = true;
	LogToConsole = true;
	ShowErrorCount = true;
	HelpDescription = TEXT("Create or update /Game/UI/WBP_CodexClockMvvm as a BP child of UCodexInvenClockMvvmWidget.");
}

int32 UCodexInvenClockMvvmWidgetScaffoldCommandlet::Main(const FString& InParams)
{
	static_cast<void>(InParams);

	UWidgetBlueprint* WidgetBlueprint = nullptr;
	FString ErrorMessage;
	if (!CreateOrUpdateClockWidgetBlueprint(WidgetBlueprint, ErrorMessage))
	{
		UE_LOG(LogCodexInvenClockMvvmWidgetScaffold, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	UE_LOG(
		LogCodexInvenClockMvvmWidgetScaffold,
		Display,
		TEXT("MVVM clock widget blueprint is ready at %s."),
		ClockWidgetPackagePath);

	return 0;
}
