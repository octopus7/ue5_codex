#include "Transient/AttendanceScaffolding/CodexInvenAttendanceScaffoldCommandlet.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Blueprint/WidgetTree.h"
#include "CodexInvenAttendanceCompactWidget.h"
#include "CodexInvenAttendanceConfigDataAsset.h"
#include "CodexInvenAttendanceScrollableWidget.h"
#include "CodexInvenGameInstance.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/Blueprint.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureDefines.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/PackageName.h"
#include "Styling/SlateColor.h"
#include "TextureCompiler.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/UnrealType.h"
#include "WidgetBlueprint.h"

DEFINE_LOG_CATEGORY_STATIC(LogCodexInvenAttendanceScaffold, Log, All);

namespace
{
	const TCHAR* AttendancePanelTexturePackagePath = TEXT("/Game/UI/Attendance/T_AttendancePanelBackdrop");
	const TCHAR* AttendanceRailTexturePackagePath = TEXT("/Game/UI/Attendance/T_AttendanceRailBackdrop");
	const TCHAR* AttendanceConfigPackagePath = TEXT("/Game/Data/Attendance/DA_AttendanceConfig");
	const TCHAR* AttendanceWidget5PackagePath = TEXT("/Game/UI/Attendance/WBP_Attendance5Day");
	const TCHAR* AttendanceWidget7PackagePath = TEXT("/Game/UI/Attendance/WBP_Attendance7Day");
	const TCHAR* AttendanceWidget14PackagePath = TEXT("/Game/UI/Attendance/WBP_Attendance14Day");
	const TCHAR* GameInstanceBlueprintPackagePath = TEXT("/Game/Blueprints/Core/BP_CodexInvenGameInstance");
	const FName AttendanceCreationContext(TEXT("CodexInvenAttendanceScaffold"));
	constexpr int32 PanelTextureWidth = 768;
	constexpr int32 PanelTextureHeight = 512;
	constexpr int32 RailTextureWidth = 1024;
	constexpr int32 RailTextureHeight = 192;

	struct FAttendanceWidgetSpec
	{
		const TCHAR* PackagePath = nullptr;
		TSubclassOf<UCodexInvenAttendanceWidgetBase> ParentClass = nullptr;
		FVector2D PanelSize = FVector2D::ZeroVector;
		bool bUseScrollBox = false;
		int32 ExpectedTotalDays = 0;
		float DayEntryWidth = 136.0f;
		float DayEntryHeight = 176.0f;
		float DayEntrySpacing = 12.0f;
	};

	FString MakeObjectPath(const FString& InPackagePath)
	{
		return InPackagePath + TEXT(".") + FPackageName::GetLongPackageAssetName(InPackagePath);
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

	bool SaveBlueprintAsset(UWidgetBlueprint& InWidgetBlueprint, FString& OutError)
	{
		return SaveObjectPackage(InWidgetBlueprint, OutError);
	}

	void SetPixel(TArray64<uint8>& InOutPixels, const int32 InWidth, const int32 InHeight, const int32 InX, const int32 InY, const FColor& InColor)
	{
		if (InX < 0 || InX >= InWidth || InY < 0 || InY >= InHeight)
		{
			return;
		}

		const int64 PixelIndex = ((static_cast<int64>(InY) * InWidth) + InX) * 4;
		InOutPixels[PixelIndex + 0] = InColor.B;
		InOutPixels[PixelIndex + 1] = InColor.G;
		InOutPixels[PixelIndex + 2] = InColor.R;
		InOutPixels[PixelIndex + 3] = InColor.A;
	}

	void BuildPanelBackdropPixels(TArray64<uint8>& OutPixels)
	{
		OutPixels.Init(0, static_cast<int64>(PanelTextureWidth) * PanelTextureHeight * 4);

		const FLinearColor TopColor(0.06f, 0.10f, 0.17f, 0.99f);
		const FLinearColor BottomColor(0.02f, 0.04f, 0.07f, 0.99f);
		const FLinearColor BorderColor(0.90f, 0.78f, 0.34f, 0.98f);
		const FLinearColor HighlightColor(0.18f, 0.24f, 0.38f, 0.28f);

		for (int32 Y = 0; Y < PanelTextureHeight; ++Y)
		{
			const float NormalizedY = static_cast<float>(Y) / static_cast<float>(PanelTextureHeight - 1);
			for (int32 X = 0; X < PanelTextureWidth; ++X)
			{
				const bool bIsBorderPixel =
					X < 4 ||
					Y < 4 ||
					X >= PanelTextureWidth - 4 ||
					Y >= PanelTextureHeight - 4;

				FLinearColor PixelColor = FMath::Lerp(TopColor, BottomColor, NormalizedY);
				const float CenterGlowX = 1.0f - FMath::Abs(((static_cast<float>(X) / static_cast<float>(PanelTextureWidth - 1)) * 2.0f) - 1.0f);
				const float HeaderGlow = FMath::Clamp(1.0f - (NormalizedY * 2.2f), 0.0f, 1.0f);
				PixelColor = FMath::Lerp(PixelColor, HighlightColor, CenterGlowX * HeaderGlow * HighlightColor.A);
				PixelColor.A = TopColor.A;

				if (bIsBorderPixel)
				{
					PixelColor = BorderColor;
				}

				SetPixel(OutPixels, PanelTextureWidth, PanelTextureHeight, X, Y, PixelColor.GetClamped().ToFColorSRGB());
			}
		}
	}

	void BuildRailBackdropPixels(TArray64<uint8>& OutPixels)
	{
		OutPixels.Init(0, static_cast<int64>(RailTextureWidth) * RailTextureHeight * 4);

		const FLinearColor FillColor(0.05f, 0.08f, 0.13f, 0.95f);
		const FLinearColor HighlightColor(0.16f, 0.22f, 0.36f, 0.34f);
		const FLinearColor BorderColor(0.32f, 0.40f, 0.58f, 0.90f);

		for (int32 Y = 0; Y < RailTextureHeight; ++Y)
		{
			const float NormalizedY = static_cast<float>(Y) / static_cast<float>(RailTextureHeight - 1);
			for (int32 X = 0; X < RailTextureWidth; ++X)
			{
				const bool bIsBorderPixel =
					X < 3 ||
					Y < 3 ||
					X >= RailTextureWidth - 3 ||
					Y >= RailTextureHeight - 3;

				FLinearColor PixelColor = FillColor;
				const float Sweep = FMath::Max(0.0f, 1.0f - FMath::Abs(((static_cast<float>(X) / static_cast<float>(RailTextureWidth - 1)) * 2.0f) - 1.0f));
				const float TopGlow = FMath::Clamp(1.0f - (NormalizedY * 2.6f), 0.0f, 1.0f);
				PixelColor = FMath::Lerp(PixelColor, HighlightColor, Sweep * TopGlow * HighlightColor.A);
				PixelColor.A = FillColor.A;

				if (bIsBorderPixel)
				{
					PixelColor = BorderColor;
				}

				SetPixel(OutPixels, RailTextureWidth, RailTextureHeight, X, Y, PixelColor.GetClamped().ToFColorSRGB());
			}
		}
	}

	UTexture2D* CreateOrUpdateTextureAsset(
		const FString& InPackagePath,
		const int32 InWidth,
		const int32 InHeight,
		void (*BuildPixels)(TArray64<uint8>&),
		FString& OutError)
	{
		UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, *MakeObjectPath(InPackagePath));
		if (Texture == nullptr)
		{
			UPackage* const Package = CreatePackage(*InPackagePath);
			Texture = NewObject<UTexture2D>(Package, *FPackageName::GetLongPackageAssetName(InPackagePath), RF_Public | RF_Standalone);
			if (Texture == nullptr)
			{
				OutError = FString::Printf(TEXT("Failed to create texture asset %s."), *InPackagePath);
				return nullptr;
			}

			FAssetRegistryModule::AssetCreated(Texture);
		}

		TArray64<uint8> PixelData;
		BuildPixels(PixelData);

		Texture->MipGenSettings = TMGS_NoMipmaps;
		Texture->NeverStream = true;
		Texture->SRGB = true;
		Texture->LODGroup = TEXTUREGROUP_UI;
		Texture->CompressionSettings = TC_EditorIcon;
		Texture->Filter = TF_Bilinear;
		Texture->Source.Init(InWidth, InHeight, 1, 1, TSF_BGRA8, PixelData.GetData());
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

	UButton* CreateActionButton(
		UWidgetTree& InWidgetTree,
		const FName InButtonName,
		const FText& InButtonText,
		const FLinearColor& InBackgroundColor)
	{
		UButton* const Button = InWidgetTree.ConstructWidget<UButton>(UButton::StaticClass(), InButtonName);
		Button->SetBackgroundColor(InBackgroundColor);

		UTextBlock* const LabelText = InWidgetTree.ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *FString::Printf(TEXT("%sLabel"), *InButtonName.ToString()));
		FSlateFontInfo LabelFont = LabelText->GetFont();
		LabelFont.Size = 16;
		LabelText->SetFont(LabelFont);
		LabelText->SetText(InButtonText);
		LabelText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		Button->AddChild(LabelText);

		if (UButtonSlot* const ButtonSlot = Cast<UButtonSlot>(LabelText->Slot))
		{
			ButtonSlot->SetPadding(FMargin(18.0f, 10.0f));
			ButtonSlot->SetHorizontalAlignment(HAlign_Center);
			ButtonSlot->SetVerticalAlignment(VAlign_Center);
		}

		return Button;
	}

	void BuildAttendanceWidgetTree(
		UWidgetBlueprint& InWidgetBlueprint,
		UTexture2D& InPanelTexture,
		UTexture2D& InRailTexture,
		const FAttendanceWidgetSpec& InWidgetSpec)
	{
		UWidgetTree& WidgetTree = *InWidgetBlueprint.WidgetTree;
		RenameExistingWidgetsToTransient(WidgetTree);

		UCanvasPanel* const RootCanvasPanel = WidgetTree.ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvasPanel"));
		WidgetTree.RootWidget = RootCanvasPanel;

		UImage* const ScreenDimmerImage = WidgetTree.ConstructWidget<UImage>(UImage::StaticClass(), TEXT("ScreenDimmerImage"));
		ScreenDimmerImage->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.55f));
		if (UCanvasPanelSlot* const DimmerSlot = RootCanvasPanel->AddChildToCanvas(ScreenDimmerImage))
		{
			DimmerSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
			DimmerSlot->SetOffsets(FMargin(0.0f));
		}

		USizeBox* const PanelSizeBox = WidgetTree.ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("PanelSizeBox"));
		PanelSizeBox->SetWidthOverride(InWidgetSpec.PanelSize.X);
		PanelSizeBox->SetHeightOverride(InWidgetSpec.PanelSize.Y);
		if (UCanvasPanelSlot* const PanelSlot = RootCanvasPanel->AddChildToCanvas(PanelSizeBox))
		{
			SetCanvasSlotLayout(
				*PanelSlot,
				FVector2D::ZeroVector,
				InWidgetSpec.PanelSize,
				FAnchors(0.5f, 0.5f, 0.5f, 0.5f),
				FVector2D(0.5f, 0.5f));
		}

		UOverlay* const PanelOverlay = WidgetTree.ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("PanelOverlay"));
		PanelSizeBox->SetContent(PanelOverlay);

		UImage* const PanelBackgroundImage = WidgetTree.ConstructWidget<UImage>(UImage::StaticClass(), TEXT("PanelBackgroundImage"));
		PanelBackgroundImage->SetBrushFromTexture(&InPanelTexture, true);
		PanelOverlay->AddChildToOverlay(PanelBackgroundImage);

		UBorder* const ContentBorder = WidgetTree.ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ContentBorder"));
		ContentBorder->SetPadding(FMargin(28.0f));
		ContentBorder->SetBrushColor(FLinearColor::White);
		if (UOverlaySlot* const ContentSlot = PanelOverlay->AddChildToOverlay(ContentBorder))
		{
			ContentSlot->SetHorizontalAlignment(HAlign_Fill);
			ContentSlot->SetVerticalAlignment(VAlign_Fill);
		}

		UVerticalBox* const ContentBox = WidgetTree.ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ContentBox"));
		ContentBorder->SetContent(ContentBox);

		UTextBlock* const TitleTextBlock = WidgetTree.ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EventTitleTextBlock"));
		TitleTextBlock->bIsVariable = true;
		FSlateFontInfo TitleFont = TitleTextBlock->GetFont();
		TitleFont.Size = 28;
		TitleTextBlock->SetFont(TitleFont);
		TitleTextBlock->SetText(FText::FromString(TEXT("Attendance Event")));
		TitleTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor(0.98f, 0.94f, 0.76f, 1.0f)));
		if (UVerticalBoxSlot* const TitleSlot = ContentBox->AddChildToVerticalBox(TitleTextBlock))
		{
			TitleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));
		}

		UTextBlock* const StatusTextBlock = WidgetTree.ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StatusTextBlock"));
		StatusTextBlock->bIsVariable = true;
		FSlateFontInfo StatusFont = StatusTextBlock->GetFont();
		StatusFont.Size = 15;
		StatusTextBlock->SetFont(StatusFont);
		StatusTextBlock->SetText(FText::FromString(TEXT("Status")));
		StatusTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor(0.86f, 0.90f, 0.96f, 0.92f)));
		if (UVerticalBoxSlot* const StatusSlot = ContentBox->AddChildToVerticalBox(StatusTextBlock))
		{
			StatusSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 18.0f));
		}

		UOverlay* const RailOverlay = WidgetTree.ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("RailOverlay"));
		if (UVerticalBoxSlot* const RailSlot = ContentBox->AddChildToVerticalBox(RailOverlay))
		{
			RailSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			RailSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 20.0f));
		}

		UImage* const RailBackgroundImage = WidgetTree.ConstructWidget<UImage>(UImage::StaticClass(), TEXT("RailBackgroundImage"));
		RailBackgroundImage->SetBrushFromTexture(&InRailTexture, true);
		RailOverlay->AddChildToOverlay(RailBackgroundImage);

		UBorder* const RailContentBorder = WidgetTree.ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("RailContentBorder"));
		RailContentBorder->SetPadding(FMargin(18.0f, 16.0f));
		RailContentBorder->SetBrushColor(FLinearColor::White);
		if (UOverlaySlot* const RailContentSlot = RailOverlay->AddChildToOverlay(RailContentBorder))
		{
			RailContentSlot->SetHorizontalAlignment(HAlign_Fill);
			RailContentSlot->SetVerticalAlignment(VAlign_Fill);
		}

		if (InWidgetSpec.bUseScrollBox)
		{
			UScrollBox* const DayScrollBox = WidgetTree.ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("DayEntryPanel"));
			DayScrollBox->bIsVariable = true;
			DayScrollBox->SetOrientation(Orient_Horizontal);
			DayScrollBox->SetAllowRightClickDragScrolling(false);
			DayScrollBox->SetConsumeMouseWheel(EConsumeMouseWheel::Never);
			RailContentBorder->SetContent(DayScrollBox);
		}
		else
		{
			UHorizontalBox* const DayHorizontalBox = WidgetTree.ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("DayEntryPanel"));
			DayHorizontalBox->bIsVariable = true;
			RailContentBorder->SetContent(DayHorizontalBox);
		}

		UHorizontalBox* const FooterBox = WidgetTree.ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("FooterBox"));
		if (UVerticalBoxSlot* const FooterSlot = ContentBox->AddChildToVerticalBox(FooterBox))
		{
			FooterSlot->SetHorizontalAlignment(HAlign_Fill);
		}

		UButton* const CloseButton = CreateActionButton(
			WidgetTree,
			TEXT("CloseButton"),
			FText::FromString(TEXT("\uB098\uC911\uC5D0")),
			FLinearColor(0.18f, 0.22f, 0.30f, 0.98f));
		CloseButton->bIsVariable = true;
		if (UHorizontalBoxSlot* const CloseButtonSlot = FooterBox->AddChildToHorizontalBox(CloseButton))
		{
			CloseButtonSlot->SetPadding(FMargin(0.0f, 0.0f, 12.0f, 0.0f));
		}

		UButton* const ClaimButton = CreateActionButton(
			WidgetTree,
			TEXT("ClaimButton"),
			FText::FromString(TEXT("\uC218\uB839")),
			FLinearColor(0.70f, 0.55f, 0.14f, 0.98f));
		ClaimButton->bIsVariable = true;
		if (UHorizontalBoxSlot* const ClaimButtonSlot = FooterBox->AddChildToHorizontalBox(ClaimButton))
		{
			ClaimButtonSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			ClaimButtonSlot->SetHorizontalAlignment(HAlign_Right);
		}
	}

	bool SetClassDefaultIntProperty(UClass& InClass, const FName InPropertyName, const int32 InValue, FString& OutError)
	{
		UObject* const DefaultObject = InClass.GetDefaultObject();
		FIntProperty* const Property = FindFProperty<FIntProperty>(&InClass, InPropertyName);
		if (DefaultObject == nullptr || Property == nullptr)
		{
			OutError = FString::Printf(TEXT("Failed to find int property %s on class %s."), *InPropertyName.ToString(), *InClass.GetName());
			return false;
		}

		DefaultObject->Modify();
		*Property->ContainerPtrToValuePtr<int32>(DefaultObject) = InValue;
		return true;
	}

	bool SetClassDefaultFloatProperty(UClass& InClass, const FName InPropertyName, const float InValue, FString& OutError)
	{
		UObject* const DefaultObject = InClass.GetDefaultObject();
		FFloatProperty* const Property = FindFProperty<FFloatProperty>(&InClass, InPropertyName);
		if (DefaultObject == nullptr || Property == nullptr)
		{
			OutError = FString::Printf(TEXT("Failed to find float property %s on class %s."), *InPropertyName.ToString(), *InClass.GetName());
			return false;
		}

		DefaultObject->Modify();
		*Property->ContainerPtrToValuePtr<float>(DefaultObject) = InValue;
		return true;
	}

	bool CreateOrUpdateAttendanceWidgetBlueprint(
		const FAttendanceWidgetSpec& InWidgetSpec,
		UTexture2D& InPanelTexture,
		UTexture2D& InRailTexture,
		UWidgetBlueprint*& OutWidgetBlueprint,
		FString& OutError)
	{
		UWidgetBlueprint* WidgetBlueprint = LoadObject<UWidgetBlueprint>(nullptr, *MakeObjectPath(InWidgetSpec.PackagePath));
		if (WidgetBlueprint == nullptr)
		{
			UPackage* const Package = CreatePackage(InWidgetSpec.PackagePath);
			WidgetBlueprint = Cast<UWidgetBlueprint>(FKismetEditorUtilities::CreateBlueprint(
				*InWidgetSpec.ParentClass,
				Package,
				*FPackageName::GetLongPackageAssetName(InWidgetSpec.PackagePath),
				BPTYPE_Normal,
				UWidgetBlueprint::StaticClass(),
				UWidgetBlueprintGeneratedClass::StaticClass(),
				AttendanceCreationContext));
			if (WidgetBlueprint == nullptr)
			{
				OutError = FString::Printf(TEXT("Failed to create widget blueprint %s."), InWidgetSpec.PackagePath);
				return false;
			}

			FAssetRegistryModule::AssetCreated(WidgetBlueprint);
		}
		else if (WidgetBlueprint->ParentClass != *InWidgetSpec.ParentClass)
		{
			OutError = FString::Printf(
				TEXT("Existing widget blueprint %s uses parent %s instead of %s."),
				*WidgetBlueprint->GetName(),
				*GetNameSafe(WidgetBlueprint->ParentClass),
				*GetNameSafe(*InWidgetSpec.ParentClass));
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

		BuildAttendanceWidgetTree(*WidgetBlueprint, InPanelTexture, InRailTexture, InWidgetSpec);
		WidgetBlueprint->WidgetVariableNameToGuidMap.Reset();

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(WidgetBlueprint);
		FKismetEditorUtilities::CompileBlueprint(WidgetBlueprint);

		UClass* const GeneratedClass = WidgetBlueprint->GeneratedClass;
		if (GeneratedClass == nullptr)
		{
			OutError = FString::Printf(TEXT("Widget blueprint %s did not generate a class."), InWidgetSpec.PackagePath);
			return false;
		}

		if (!SetClassDefaultIntProperty(*GeneratedClass, TEXT("ExpectedTotalDays"), InWidgetSpec.ExpectedTotalDays, OutError) ||
			!SetClassDefaultFloatProperty(*GeneratedClass, TEXT("DayEntryWidth"), InWidgetSpec.DayEntryWidth, OutError) ||
			!SetClassDefaultFloatProperty(*GeneratedClass, TEXT("DayEntryHeight"), InWidgetSpec.DayEntryHeight, OutError) ||
			!SetClassDefaultFloatProperty(*GeneratedClass, TEXT("DayEntrySpacing"), InWidgetSpec.DayEntrySpacing, OutError))
		{
			return false;
		}

		WidgetBlueprint->MarkPackageDirty();
		if (!SaveBlueprintAsset(*WidgetBlueprint, OutError))
		{
			return false;
		}

		OutWidgetBlueprint = WidgetBlueprint;
		return true;
	}

	FCodexInvenAttendanceRewardDefinition MakeReward(const ECodexInvenPickupType InPickupType, const int32 InQuantity)
	{
		FCodexInvenAttendanceRewardDefinition Reward;
		Reward.PickupType = InPickupType;
		Reward.Quantity = InQuantity;
		return Reward;
	}

	bool CreateOrUpdateAttendanceConfigAsset(
		UWidgetBlueprint& InWidget5Day,
		UWidgetBlueprint& InWidget7Day,
		UWidgetBlueprint& InWidget14Day,
		UCodexInvenAttendanceConfigDataAsset*& OutAttendanceConfig,
		FString& OutError)
	{
		UCodexInvenAttendanceConfigDataAsset* AttendanceConfig =
			LoadObject<UCodexInvenAttendanceConfigDataAsset>(nullptr, *MakeObjectPath(AttendanceConfigPackagePath));
		if (AttendanceConfig == nullptr)
		{
			UPackage* const Package = CreatePackage(AttendanceConfigPackagePath);
			AttendanceConfig = NewObject<UCodexInvenAttendanceConfigDataAsset>(
				Package,
				*FPackageName::GetLongPackageAssetName(AttendanceConfigPackagePath),
				RF_Public | RF_Standalone);
			if (AttendanceConfig == nullptr)
			{
				OutError = TEXT("Failed to create the attendance config data asset.");
				return false;
			}

			FAssetRegistryModule::AssetCreated(AttendanceConfig);
		}

		AttendanceConfig->Modify();
		AttendanceConfig->PopupInitialDelaySeconds = 0.25f;
		AttendanceConfig->ActiveEvents.Reset();

		auto AddEvent =
			[&AttendanceConfig](
				const FName InEventId,
				const TCHAR* InTitle,
				const int32 InPriority,
				const int32 InTotalDays,
				UWidgetBlueprint& InWidgetBlueprint,
				TArray<FCodexInvenAttendanceRewardDefinition>&& InRewards)
		{
			FCodexInvenAttendanceEventDefinition& EventDefinition = AttendanceConfig->ActiveEvents.AddDefaulted_GetRef();
			EventDefinition.EventId = InEventId;
			EventDefinition.Title = FText::FromString(InTitle);
			EventDefinition.Priority = InPriority;
			EventDefinition.TotalDays = InTotalDays;
			EventDefinition.PopupWidgetClass = InWidgetBlueprint.GeneratedClass;
			EventDefinition.Rewards = MoveTemp(InRewards);
		};

		AddEvent(
			TEXT("Attendance05Day"),
			TEXT("5\uC77C \uCD08\uBC18 \uCD9C\uC11D"),
			10,
			5,
			InWidget5Day,
			{
				MakeReward(ECodexInvenPickupType::CylinderRed, 2),
				MakeReward(ECodexInvenPickupType::CylinderGreen, 2),
				MakeReward(ECodexInvenPickupType::CubeRed, 1),
				MakeReward(ECodexInvenPickupType::CylinderBlue, 3),
				MakeReward(ECodexInvenPickupType::CubeGold, 1)
			});

		AddEvent(
			TEXT("Attendance07Day"),
			TEXT("7\uC77C \uBCF4\uAE09 \uCD9C\uC11D"),
			20,
			7,
			InWidget7Day,
			{
				MakeReward(ECodexInvenPickupType::CylinderGreen, 3),
				MakeReward(ECodexInvenPickupType::CylinderBlue, 2),
				MakeReward(ECodexInvenPickupType::CubeBlue, 1),
				MakeReward(ECodexInvenPickupType::CylinderRed, 4),
				MakeReward(ECodexInvenPickupType::CubeGreen, 1),
				MakeReward(ECodexInvenPickupType::CylinderGold, 2),
				MakeReward(ECodexInvenPickupType::CubeGold, 1)
			});

		AddEvent(
			TEXT("Attendance14Day"),
			TEXT("14\uC77C \uC7A5\uAE30 \uCD9C\uC11D"),
			30,
			14,
			InWidget14Day,
			{
				MakeReward(ECodexInvenPickupType::CylinderRed, 2),
				MakeReward(ECodexInvenPickupType::CylinderGreen, 2),
				MakeReward(ECodexInvenPickupType::CylinderBlue, 2),
				MakeReward(ECodexInvenPickupType::CubeRed, 1),
				MakeReward(ECodexInvenPickupType::CylinderRed, 3),
				MakeReward(ECodexInvenPickupType::CubeGreen, 1),
				MakeReward(ECodexInvenPickupType::CylinderGold, 2),
				MakeReward(ECodexInvenPickupType::CubeBlue, 1),
				MakeReward(ECodexInvenPickupType::CylinderGreen, 4),
				MakeReward(ECodexInvenPickupType::CylinderBlue, 4),
				MakeReward(ECodexInvenPickupType::CubeRed, 1),
				MakeReward(ECodexInvenPickupType::CubeGreen, 1),
				MakeReward(ECodexInvenPickupType::CylinderGold, 3),
				MakeReward(ECodexInvenPickupType::CubeGold, 1)
			});

		AttendanceConfig->MarkPackageDirty();
		if (!SaveObjectPackage(*AttendanceConfig, OutError))
		{
			return false;
		}

		OutAttendanceConfig = AttendanceConfig;
		return true;
	}

	bool AssignAttendanceConfigToGameInstanceBlueprint(
		UCodexInvenAttendanceConfigDataAsset& InAttendanceConfig,
		FString& OutError)
	{
		UBlueprint* const GameInstanceBlueprint = LoadObject<UBlueprint>(nullptr, *MakeObjectPath(GameInstanceBlueprintPackagePath));
		if (GameInstanceBlueprint == nullptr)
		{
			OutError = TEXT("Failed to load BP_CodexInvenGameInstance.");
			return false;
		}

		FKismetEditorUtilities::CompileBlueprint(GameInstanceBlueprint);

		UCodexInvenGameInstance* const DefaultObject = GameInstanceBlueprint->GeneratedClass != nullptr
			? Cast<UCodexInvenGameInstance>(GameInstanceBlueprint->GeneratedClass->GetDefaultObject())
			: nullptr;
		if (DefaultObject == nullptr)
		{
			OutError = TEXT("BP_CodexInvenGameInstance does not have a valid generated class default object.");
			return false;
		}

		DefaultObject->Modify();
		DefaultObject->AttendanceConfig = &InAttendanceConfig;

		FBlueprintEditorUtils::MarkBlueprintAsModified(GameInstanceBlueprint);
		GameInstanceBlueprint->MarkPackageDirty();
		return SaveObjectPackage(*GameInstanceBlueprint, OutError);
	}
}

UCodexInvenAttendanceScaffoldCommandlet::UCodexInvenAttendanceScaffoldCommandlet()
{
	IsServer = false;
	IsClient = false;
	IsEditor = true;
	LogToConsole = true;
	ShowErrorCount = true;
	HelpDescription = TEXT("Create or update attendance widget blueprints, textures, and the attendance config asset.");
}

int32 UCodexInvenAttendanceScaffoldCommandlet::Main(const FString& InParams)
{
	static_cast<void>(InParams);

	FString ErrorMessage;
	UTexture2D* const PanelTexture = CreateOrUpdateTextureAsset(
		AttendancePanelTexturePackagePath,
		PanelTextureWidth,
		PanelTextureHeight,
		&BuildPanelBackdropPixels,
		ErrorMessage);
	if (PanelTexture == nullptr)
	{
		UE_LOG(LogCodexInvenAttendanceScaffold, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	UTexture2D* const RailTexture = CreateOrUpdateTextureAsset(
		AttendanceRailTexturePackagePath,
		RailTextureWidth,
		RailTextureHeight,
		&BuildRailBackdropPixels,
		ErrorMessage);
	if (RailTexture == nullptr)
	{
		UE_LOG(LogCodexInvenAttendanceScaffold, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	UWidgetBlueprint* Widget5Day = nullptr;
	UWidgetBlueprint* Widget7Day = nullptr;
	UWidgetBlueprint* Widget14Day = nullptr;

	if (!CreateOrUpdateAttendanceWidgetBlueprint(
		FAttendanceWidgetSpec{ AttendanceWidget5PackagePath, UCodexInvenAttendanceCompactWidget::StaticClass(), FVector2D(980.0f, 500.0f), false, 5, 160.0f, 176.0f, 12.0f },
		*PanelTexture,
		*RailTexture,
		Widget5Day,
		ErrorMessage) ||
		!CreateOrUpdateAttendanceWidgetBlueprint(
			FAttendanceWidgetSpec{ AttendanceWidget7PackagePath, UCodexInvenAttendanceCompactWidget::StaticClass(), FVector2D(1180.0f, 520.0f), false, 7, 136.0f, 176.0f, 10.0f },
			*PanelTexture,
			*RailTexture,
			Widget7Day,
			ErrorMessage) ||
		!CreateOrUpdateAttendanceWidgetBlueprint(
			FAttendanceWidgetSpec{ AttendanceWidget14PackagePath, UCodexInvenAttendanceScrollableWidget::StaticClass(), FVector2D(1024.0f, 540.0f), true, 14, 132.0f, 176.0f, 12.0f },
			*PanelTexture,
			*RailTexture,
			Widget14Day,
			ErrorMessage))
	{
		UE_LOG(LogCodexInvenAttendanceScaffold, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	UCodexInvenAttendanceConfigDataAsset* AttendanceConfig = nullptr;
	if (!CreateOrUpdateAttendanceConfigAsset(*Widget5Day, *Widget7Day, *Widget14Day, AttendanceConfig, ErrorMessage))
	{
		UE_LOG(LogCodexInvenAttendanceScaffold, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	if (!AssignAttendanceConfigToGameInstanceBlueprint(*AttendanceConfig, ErrorMessage))
	{
		UE_LOG(LogCodexInvenAttendanceScaffold, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	UE_LOG(
		LogCodexInvenAttendanceScaffold,
		Display,
		TEXT("Attendance assets are ready: %s, %s, %s, and %s."),
		AttendanceWidget5PackagePath,
		AttendanceWidget7PackagePath,
		AttendanceWidget14PackagePath,
		AttendanceConfigPackagePath);

	return 0;
}
