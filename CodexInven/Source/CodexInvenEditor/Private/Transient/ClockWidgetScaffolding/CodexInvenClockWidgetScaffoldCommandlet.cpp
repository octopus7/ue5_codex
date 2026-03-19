#include "Transient/ClockWidgetScaffolding/CodexInvenClockWidgetScaffoldCommandlet.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Blueprint/WidgetTree.h"
#include "CodexInvenClockWidget.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/Texture2D.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/PackageName.h"
#include "Styling/SlateColor.h"
#include "TextureCompiler.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "WidgetBlueprint.h"
#include "WidgetBlueprint/CodexInvenWidgetBlueprintTreeUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogCodexInvenClockWidgetScaffold, Log, All);

namespace
{
	const TCHAR* ClockWidgetPackagePath = TEXT("/Game/UI/WBP_CodexClock");
	const TCHAR* ClockFaceTexturePackagePath = TEXT("/Game/UI/T_ClockFace_Analog");
	const FName ClockWidgetCreationContext(TEXT("CodexInvenClockWidgetScaffold"));
	const FVector2D ClockCanvasSize(176.0f, 176.0f);
	const FVector2D ClockCanvasCenter(ClockCanvasSize.X * 0.5f, ClockCanvasSize.Y * 0.5f);
	constexpr int32 ClockFaceTextureSize = 256;
	const FVector2D ContainerScreenOffset(0.0f, 18.0f);
	const FLinearColor ContainerTint(0.02f, 0.04f, 0.08f, 0.86f);
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

	FString GetAssetNameFromPackagePath(const FString& InPackagePath)
	{
		return FPackageName::GetLongPackageAssetName(InPackagePath);
	}

	bool SaveObjectPackage(UObject& InObject, FString& OutError)
	{
		UPackage* const Package = InObject.GetPackage();
		if (Package == nullptr)
		{
			OutError = FString::Printf(TEXT("Object %s has no package."), *InObject.GetName());
			return false;
		}

		const FString Filename = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.SaveFlags = SAVE_NoError;
		SaveArgs.Error = GError;
		if (!UPackage::SavePackage(Package, &InObject, *Filename, SaveArgs))
		{
			OutError = FString::Printf(TEXT("Failed to save package %s."), *Package->GetName());
			return false;
		}

		return true;
	}

	void SetPixel(TArray64<uint8>& InOutPixels, const int32 InX, const int32 InY, const FColor& InColor)
	{
		if (InX < 0 || InX >= ClockFaceTextureSize || InY < 0 || InY >= ClockFaceTextureSize)
		{
			return;
		}

		const int64 PixelIndex = ((static_cast<int64>(InY) * ClockFaceTextureSize) + InX) * 4;
		InOutPixels[PixelIndex + 0] = InColor.B;
		InOutPixels[PixelIndex + 1] = InColor.G;
		InOutPixels[PixelIndex + 2] = InColor.R;
		InOutPixels[PixelIndex + 3] = InColor.A;
	}

	void FillCircle(TArray64<uint8>& InOutPixels, const FVector2D& InCenter, const float InRadius, const FColor& InColor)
	{
		const int32 MinX = FMath::FloorToInt(InCenter.X - InRadius);
		const int32 MaxX = FMath::CeilToInt(InCenter.X + InRadius);
		const int32 MinY = FMath::FloorToInt(InCenter.Y - InRadius);
		const int32 MaxY = FMath::CeilToInt(InCenter.Y + InRadius);
		const float RadiusSquared = InRadius * InRadius;

		for (int32 Y = MinY; Y <= MaxY; ++Y)
		{
			for (int32 X = MinX; X <= MaxX; ++X)
			{
				const FVector2D Delta(static_cast<float>(X) + 0.5f - InCenter.X, static_cast<float>(Y) + 0.5f - InCenter.Y);
				if (Delta.SizeSquared() <= RadiusSquared)
				{
					SetPixel(InOutPixels, X, Y, InColor);
				}
			}
		}
	}

	void FillRing(
		TArray64<uint8>& InOutPixels,
		const FVector2D& InCenter,
		const float InInnerRadius,
		const float InOuterRadius,
		const FColor& InColor)
	{
		const int32 MinX = FMath::FloorToInt(InCenter.X - InOuterRadius);
		const int32 MaxX = FMath::CeilToInt(InCenter.X + InOuterRadius);
		const int32 MinY = FMath::FloorToInt(InCenter.Y - InOuterRadius);
		const int32 MaxY = FMath::CeilToInt(InCenter.Y + InOuterRadius);
		const float InnerRadiusSquared = InInnerRadius * InInnerRadius;
		const float OuterRadiusSquared = InOuterRadius * InOuterRadius;

		for (int32 Y = MinY; Y <= MaxY; ++Y)
		{
			for (int32 X = MinX; X <= MaxX; ++X)
			{
				const FVector2D Delta(static_cast<float>(X) + 0.5f - InCenter.X, static_cast<float>(Y) + 0.5f - InCenter.Y);
				const float DistanceSquared = Delta.SizeSquared();
				if (DistanceSquared >= InnerRadiusSquared && DistanceSquared <= OuterRadiusSquared)
				{
					SetPixel(InOutPixels, X, Y, InColor);
				}
			}
		}
	}

	void DrawLine(
		TArray64<uint8>& InOutPixels,
		const FVector2D& InStart,
		const FVector2D& InEnd,
		const float InThickness,
		const FColor& InColor)
	{
		const FVector2D Segment = InEnd - InStart;
		const float SegmentLengthSquared = Segment.SizeSquared();
		if (SegmentLengthSquared <= KINDA_SMALL_NUMBER)
		{
			FillCircle(InOutPixels, InStart, InThickness, InColor);
			return;
		}

		const float HalfThickness = InThickness * 0.5f;
		const int32 MinX = FMath::FloorToInt(FMath::Min(InStart.X, InEnd.X) - HalfThickness - 1.0f);
		const int32 MaxX = FMath::CeilToInt(FMath::Max(InStart.X, InEnd.X) + HalfThickness + 1.0f);
		const int32 MinY = FMath::FloorToInt(FMath::Min(InStart.Y, InEnd.Y) - HalfThickness - 1.0f);
		const int32 MaxY = FMath::CeilToInt(FMath::Max(InStart.Y, InEnd.Y) + HalfThickness + 1.0f);
		const float ThresholdSquared = HalfThickness * HalfThickness;

		for (int32 Y = MinY; Y <= MaxY; ++Y)
		{
			for (int32 X = MinX; X <= MaxX; ++X)
			{
				const FVector2D Sample(static_cast<float>(X) + 0.5f, static_cast<float>(Y) + 0.5f);
				const float Projection = FMath::Clamp(FVector2D::DotProduct(Sample - InStart, Segment) / SegmentLengthSquared, 0.0f, 1.0f);
				const FVector2D ClosestPoint = InStart + (Segment * Projection);
				if ((Sample - ClosestPoint).SizeSquared() <= ThresholdSquared)
				{
					SetPixel(InOutPixels, X, Y, InColor);
				}
			}
		}
	}

	void BuildClockFacePixels(TArray64<uint8>& OutPixels)
	{
		OutPixels.Init(0, static_cast<int64>(ClockFaceTextureSize) * ClockFaceTextureSize * 4);

		const FVector2D Center(ClockFaceTextureSize * 0.5f, ClockFaceTextureSize * 0.5f);
		const FColor OuterRing = FLinearColor(0.92f, 0.96f, 1.0f, 0.98f).ToFColorSRGB();
		const FColor MidRing = FLinearColor(0.18f, 0.24f, 0.31f, 0.96f).ToFColorSRGB();
		const FColor FaceFill = FLinearColor(0.03f, 0.06f, 0.10f, 0.86f).ToFColorSRGB();
		const FColor InnerGlow = FLinearColor(0.08f, 0.12f, 0.18f, 0.72f).ToFColorSRGB();
		const FColor MinorTick = FLinearColor(0.70f, 0.78f, 0.88f, 0.72f).ToFColorSRGB();
		const FColor MajorTick = FLinearColor(0.96f, 0.98f, 1.0f, 1.0f).ToFColorSRGB();

		FillCircle(OutPixels, Center, 120.0f, FaceFill);
		FillCircle(OutPixels, Center, 108.0f, InnerGlow);
		FillRing(OutPixels, Center, 112.0f, 120.0f, OuterRing);
		FillRing(OutPixels, Center, 101.0f, 106.0f, MidRing);

		for (int32 TickIndex = 0; TickIndex < 12; ++TickIndex)
		{
			const bool bIsMajorTick = (TickIndex % 3) == 0;
			const float AngleRadians = FMath::DegreesToRadians(static_cast<float>(TickIndex) * 30.0f);
			const FVector2D Direction(FMath::Sin(AngleRadians), -FMath::Cos(AngleRadians));
			const FVector2D Start = Center + (Direction * (bIsMajorTick ? 83.0f : 90.0f));
			const FVector2D End = Center + (Direction * 108.0f);
			DrawLine(OutPixels, Start, End, bIsMajorTick ? 7.0f : 4.0f, bIsMajorTick ? MajorTick : MinorTick);
		}
	}

	UTexture2D* CreateOrUpdateClockFaceTexture(FString& OutError)
	{
		UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, *MakeWidgetBlueprintObjectPath(ClockFaceTexturePackagePath));
		if (Texture == nullptr)
		{
			UPackage* const Package = CreatePackage(ClockFaceTexturePackagePath);
			Texture = NewObject<UTexture2D>(Package, *GetAssetNameFromPackagePath(ClockFaceTexturePackagePath), RF_Public | RF_Standalone);
			if (Texture == nullptr)
			{
				OutError = TEXT("Failed to create the analog clock face texture asset.");
				return nullptr;
			}

			FAssetRegistryModule::AssetCreated(Texture);
		}

		TArray64<uint8> PixelData;
		BuildClockFacePixels(PixelData);

		Texture->MipGenSettings = TMGS_NoMipmaps;
		Texture->NeverStream = true;
		Texture->SRGB = true;
		Texture->LODGroup = TEXTUREGROUP_UI;
		Texture->CompressionSettings = TC_EditorIcon;
		Texture->Filter = TF_Bilinear;
		Texture->Source.Init(ClockFaceTextureSize, ClockFaceTextureSize, 1, 1, TSF_BGRA8, PixelData.GetData());
		Texture->UpdateResource();
		Texture->PostEditChange();
		FTextureCompilingManager::Get().FinishCompilation({ Texture });
		Texture->MarkPackageDirty();

		if (!SaveObjectPackage(*Texture, OutError))
		{
			return nullptr;
		}

		return Texture;
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

	bool CreateOrUpdateClockWidgetBlueprint(UWidgetBlueprint*& OutWidgetBlueprint, FString& OutError)
	{
		UTexture2D* const ClockFaceTexture = CreateOrUpdateClockFaceTexture(OutError);
		if (ClockFaceTexture == nullptr)
		{
			return false;
		}

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

		BuildClockWidgetTree(*WidgetBlueprint, *ClockFaceTexture);
		WidgetBlueprint->WidgetVariableNameToGuidMap.Reset();

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
