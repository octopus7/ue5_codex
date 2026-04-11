// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CodexInteractionAssetBuilder.h"

#include "CodexTopDownInputConfigDataAsset.h"
#include "Interaction/CodexInteractableActor.h"
#include "Interaction/CodexInteractionAssetPaths.h"
#include "Interaction/CodexInteractionMessagePopupWidget.h"
#include "Interaction/CodexInteractionScrollMessagePopupWidget.h"
#include "Interaction/CodexInteractionComponent.h"
#include "Interaction/CodexInteractionIndicatorWidget.h"
#include "Interaction/CodexPopupInteractableActor.h"
#include "Interaction/CodexScrollMessagePopupInteractableActor.h"
#include "Interaction/CodexInteractionTypes.h"
#include "AssetToolsModule.h"
#include "Blueprint/WidgetTree.h"
#include "Components/BackgroundBlur.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "Factories/BlueprintFactory.h"
#include "Factories/Texture2dFactoryNew.h"
#include "FileHelpers.h"
#include "GameFramework/PlayerStart.h"
#include "HAL/PlatformProcess.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "InputEditorModule.h"
#include "InputMappingContext.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateTypes.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Subsystems/EditorAssetSubsystem.h"
#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"

namespace
{
	using namespace CodexInteractionAssetPaths;

	static const TCHAR* const BasicMapAssetPath = TEXT("/Game/Maps/BasicMap");
	static const TCHAR* const InteractionPlacementLabelApple = TEXT("InteractionTest_Apple");
	static const TCHAR* const InteractionPlacementLabelStrawberry = TEXT("InteractionTest_Strawberry");
	static const TCHAR* const InteractionPlacementLabelWoodenSign = TEXT("InteractionTest_WoodenSignPopup");
	static const TCHAR* const InteractionPlacementLabelWoodenSignScroll = TEXT("InteractionTest_WoodenSignScrollPopup");
	static const FName InteractionPlacementTag = TEXT("CodexInteractionTestPlacement");

	FString EscapePowerShellSingleQuotedString(const FString& Value)
	{
		FString Escaped(Value);
		Escaped.ReplaceInline(TEXT("'"), TEXT("''"));
		return Escaped;
	}

	bool IsCurrentProjectEditorRunning()
	{
		const FString ProjectFilePath = FPaths::ConvertRelativePathToFull(FPaths::GetProjectFilePath());
		const FString EscapedProjectFilePath = EscapePowerShellSingleQuotedString(ProjectFilePath);
		const FString PowerShellScript = FString::Printf(
			TEXT("$p = Get-CimInstance Win32_Process -Filter \"Name = 'UnrealEditor.exe'\" | Where-Object { $_.CommandLine -and $_.CommandLine -match [regex]::Escape('%s') } | Select-Object -First 1; if ($p) { Write-Output 'RUNNING' }"),
			*EscapedProjectFilePath);

		FString StdOut;
		FString StdErr;
		int32 ReturnCode = 0;
		FPlatformProcess::ExecProcess(
			TEXT("powershell.exe"),
			*FString::Printf(TEXT("-NoProfile -NonInteractive -ExecutionPolicy Bypass -Command \"%s\""), *PowerShellScript.ReplaceCharWithEscapedChar()),
			&ReturnCode,
			&StdOut,
			&StdErr);

		return ReturnCode == 0 && StdOut.Contains(TEXT("RUNNING"));
	}

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

	UWidgetBlueprint* CreateWidgetBlueprint(const FString& PackagePath, const FString& AssetName, UClass* ParentClass)
	{
		const FString ObjectPath = MakeObjectPath(PackagePath, AssetName);
		if (UWidgetBlueprint* ExistingBlueprint = LoadAsset<UWidgetBlueprint>(ObjectPath))
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

	UEditorActorSubsystem* GetEditorActorSubsystem()
	{
		return GEditor ? GEditor->GetEditorSubsystem<UEditorActorSubsystem>() : nullptr;
	}

	void ConfigureInteractAction(UInputAction& InteractAction)
	{
		InteractAction.ValueType = EInputActionValueType::Boolean;
		InteractAction.Modifiers.Reset();
		InteractAction.Triggers.Reset();
		InteractAction.MarkPackageDirty();
	}

	void ConfigurePopupCloseAction(UInputAction& PopupCloseAction)
	{
		PopupCloseAction.ValueType = EInputActionValueType::Boolean;
		PopupCloseAction.Modifiers.Reset();
		PopupCloseAction.Triggers.Reset();
		PopupCloseAction.MarkPackageDirty();
	}

	void ConfigureInteractMappings(UInputMappingContext& MappingContext, UInputAction& InteractAction)
	{
		MappingContext.UnmapAllKeysFromAction(&InteractAction);

		FEnhancedActionKeyMapping& FKeyMapping = MappingContext.MapKey(&InteractAction, EKeys::F);
		FKeyMapping.Modifiers.Reset();
		MappingContext.MarkPackageDirty();
	}

	void ConfigurePopupCloseMappings(UInputMappingContext& MappingContext, UInputAction& PopupCloseAction)
	{
		MappingContext.UnmapAllKeysFromAction(&PopupCloseAction);

		FEnhancedActionKeyMapping& SpaceKeyMapping = MappingContext.MapKey(&PopupCloseAction, EKeys::SpaceBar);
		SpaceKeyMapping.Modifiers.Reset();
		MappingContext.MarkPackageDirty();
	}

	void ConfigureInputConfig(
		UCodexTopDownInputConfigDataAsset& InputConfig,
		UInputMappingContext& MappingContext,
		UInputAction& InteractAction,
		UInputAction& PopupCloseAction)
	{
		if (InputConfig.DefaultMappingContext == nullptr)
		{
			InputConfig.DefaultMappingContext = &MappingContext;
		}

		InputConfig.InteractAction = &InteractAction;
		InputConfig.PopupCloseAction = &PopupCloseAction;
		InputConfig.MarkPackageDirty();
	}

	TArray64<uint8> BuildTexturePixels(const int32 Size, const float MinRadius, const float MaxRadius)
	{
		TArray64<uint8> Pixels;
		Pixels.SetNumZeroed(Size * Size * 4);

		const float Center = (static_cast<float>(Size) - 1.0f) * 0.5f;
		for (int32 Y = 0; Y < Size; ++Y)
		{
			for (int32 X = 0; X < Size; ++X)
			{
				const float DeltaX = static_cast<float>(X) - Center;
				const float DeltaY = static_cast<float>(Y) - Center;
				const float Distance = FMath::Sqrt((DeltaX * DeltaX) + (DeltaY * DeltaY));
				const bool bIsOpaque = Distance >= MinRadius && Distance <= MaxRadius;
				const int64 PixelIndex = static_cast<int64>((Y * Size) + X) * 4;
				Pixels[PixelIndex + 0] = 255;
				Pixels[PixelIndex + 1] = 255;
				Pixels[PixelIndex + 2] = 255;
				Pixels[PixelIndex + 3] = bIsOpaque ? 255 : 0;
			}
		}

		return Pixels;
	}

	void SetTexturePixel(TArray64<uint8>& Pixels, const int32 Size, const int32 X, const int32 Y, const FColor& Color)
	{
		if (X < 0 || X >= Size || Y < 0 || Y >= Size)
		{
			return;
		}

		const int64 PixelIndex = static_cast<int64>((Y * Size) + X) * 4;
		Pixels[PixelIndex + 0] = Color.B;
		Pixels[PixelIndex + 1] = Color.G;
		Pixels[PixelIndex + 2] = Color.R;
		Pixels[PixelIndex + 3] = Color.A;
	}

	TArray64<uint8> BuildSmileTexturePixels(const int32 Size)
	{
		TArray64<uint8> Pixels;
		Pixels.SetNumZeroed(Size * Size * 4);

		const float Center = (static_cast<float>(Size) - 1.0f) * 0.5f;
		const float FaceRadius = static_cast<float>(Size) * 0.34f;
		const float OutlineRadius = FaceRadius + 1.5f;
		const FColor OutlineColor(110, 86, 24, 255);
		const FColor FaceColor(255, 221, 72, 255);
		const FColor FeatureColor(78, 54, 12, 255);

		for (int32 Y = 0; Y < Size; ++Y)
		{
			for (int32 X = 0; X < Size; ++X)
			{
				const float DeltaX = static_cast<float>(X) - Center;
				const float DeltaY = static_cast<float>(Y) - Center;
				const float Distance = FMath::Sqrt((DeltaX * DeltaX) + (DeltaY * DeltaY));
				if (Distance <= FaceRadius)
				{
					SetTexturePixel(Pixels, Size, X, Y, FaceColor);
				}
				else if (Distance <= OutlineRadius)
				{
					SetTexturePixel(Pixels, Size, X, Y, OutlineColor);
				}
			}
		}

		auto DrawFilledCircle = [&Pixels, Size](const FVector2f CenterPoint, const float Radius, const FColor& Color)
		{
			const int32 MinX = FMath::FloorToInt(CenterPoint.X - Radius);
			const int32 MaxX = FMath::CeilToInt(CenterPoint.X + Radius);
			const int32 MinY = FMath::FloorToInt(CenterPoint.Y - Radius);
			const int32 MaxY = FMath::CeilToInt(CenterPoint.Y + Radius);

			for (int32 Y = MinY; Y <= MaxY; ++Y)
			{
				for (int32 X = MinX; X <= MaxX; ++X)
				{
					const float DeltaX = static_cast<float>(X) - CenterPoint.X;
					const float DeltaY = static_cast<float>(Y) - CenterPoint.Y;
					if ((DeltaX * DeltaX) + (DeltaY * DeltaY) <= Radius * Radius)
					{
						SetTexturePixel(Pixels, Size, X, Y, Color);
					}
				}
			}
		};

		DrawFilledCircle(FVector2f(Center - 9.0f, Center - 7.0f), 3.2f, FeatureColor);
		DrawFilledCircle(FVector2f(Center + 9.0f, Center - 7.0f), 3.2f, FeatureColor);

		for (int32 Y = 0; Y < Size; ++Y)
		{
			for (int32 X = 0; X < Size; ++X)
			{
				const float DeltaX = static_cast<float>(X) - Center;
				const float DeltaY = static_cast<float>(Y) - (Center + 3.0f);
				const float Distance = FMath::Sqrt((DeltaX * DeltaX) + (DeltaY * DeltaY));
				const bool bWithinMouthArc = DeltaY >= 2.0f && DeltaY <= 11.0f && Distance >= 11.0f && Distance <= 13.0f;
				if (bWithinMouthArc)
				{
					SetTexturePixel(Pixels, Size, X, Y, FeatureColor);
				}
			}
		}

		return Pixels;
	}

	void ConfigureTexture(UTexture2D& Texture, TConstArrayView64<uint8> PixelData, const int32 Size)
	{
		Texture.Source.Init(Size, Size, 1, 1, TSF_BGRA8, PixelData.GetData());
		Texture.CompressionSettings = TC_EditorIcon;
		Texture.MipGenSettings = TMGS_NoMipmaps;
		Texture.NeverStream = true;
		Texture.LODGroup = TEXTUREGROUP_UI;
		Texture.Filter = TF_Bilinear;
		Texture.AddressX = TA_Clamp;
		Texture.AddressY = TA_Clamp;
		Texture.SRGB = true;
		Texture.MarkPackageDirty();
		Texture.PostEditChange();
		Texture.UpdateResource();
	}

	void EnsureWidgetGuid(UWidgetBlueprint& WidgetBlueprint, UWidget& Widget)
	{
		if (!WidgetBlueprint.WidgetVariableNameToGuidMap.Contains(Widget.GetFName()))
		{
			WidgetBlueprint.OnVariableAdded(Widget.GetFName());
		}
	}

	template <typename WidgetType>
	WidgetType* FindWidget(UWidgetBlueprint& WidgetBlueprint, const FName WidgetName)
	{
		return WidgetBlueprint.WidgetTree ? Cast<WidgetType>(WidgetBlueprint.WidgetTree->FindWidget(WidgetName)) : nullptr;
	}

	template <typename WidgetType>
	WidgetType* EnsurePanelChild(UWidgetBlueprint& WidgetBlueprint, UPanelWidget& ParentWidget, const FName WidgetName, const bool bIsVariable)
	{
		WidgetType* Widget = FindWidget<WidgetType>(WidgetBlueprint, WidgetName);
		if (Widget == nullptr && WidgetBlueprint.WidgetTree != nullptr)
		{
			Widget = WidgetBlueprint.WidgetTree->ConstructWidget<WidgetType>(WidgetType::StaticClass(), WidgetName);
			if (Widget == nullptr)
			{
				return nullptr;
			}

			ParentWidget.AddChild(Widget);
		}

		if (Widget != nullptr)
		{
			EnsureWidgetGuid(WidgetBlueprint, *Widget);
			Widget->bIsVariable = bIsVariable;
		}

		return Widget;
	}

	void ConfigureCanvasSlot(UWidget& Widget, const FVector2D& Position, const bool bAutoSize, const FVector2D& Alignment)
	{
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget.Slot))
		{
			CanvasSlot->SetPosition(Position);
			CanvasSlot->SetAutoSize(bAutoSize);
			CanvasSlot->SetAlignment(Alignment);
		}
	}

	void ConfigureOverlaySlot(UWidget& Widget)
	{
		if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(Widget.Slot))
		{
			OverlaySlot->SetHorizontalAlignment(HAlign_Center);
			OverlaySlot->SetVerticalAlignment(VAlign_Center);
		}
	}

	void ConfigureCanvasAnchors(UWidget& Widget, const FAnchors& Anchors, const FMargin& Offsets, const FVector2D& Alignment)
	{
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget.Slot))
		{
			CanvasSlot->SetAnchors(Anchors);
			CanvasSlot->SetOffsets(Offsets);
			CanvasSlot->SetAlignment(Alignment);
		}
	}

	void ConfigureHorizontalBoxSlot(
		UWidget& Widget,
		const FMargin& Padding,
		const ESlateSizeRule::Type SizeRule,
		const EHorizontalAlignment HorizontalAlignment,
		const EVerticalAlignment VerticalAlignment)
	{
		if (UHorizontalBoxSlot* HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(Widget.Slot))
		{
			HorizontalBoxSlot->SetPadding(Padding);
			HorizontalBoxSlot->SetHorizontalAlignment(HorizontalAlignment);
			HorizontalBoxSlot->SetVerticalAlignment(VerticalAlignment);
			HorizontalBoxSlot->SetSize(FSlateChildSize(SizeRule));
		}
	}

	void ConfigureVerticalBoxSlot(
		UWidget& Widget,
		const FMargin& Padding,
		const ESlateSizeRule::Type SizeRule,
		const EHorizontalAlignment HorizontalAlignment,
		const EVerticalAlignment VerticalAlignment)
	{
		if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(Widget.Slot))
		{
			VerticalBoxSlot->SetPadding(Padding);
			VerticalBoxSlot->SetHorizontalAlignment(HorizontalAlignment);
			VerticalBoxSlot->SetVerticalAlignment(VerticalAlignment);
			VerticalBoxSlot->SetSize(FSlateChildSize(SizeRule));
		}
	}

	FSlateBrush MakeRoundedBrush(const FLinearColor& FillColor, const float Radius, const FLinearColor& OutlineColor = FLinearColor::Transparent, const float OutlineWidth = 0.0f)
	{
		FSlateBrush Brush;
		Brush.DrawAs = ESlateBrushDrawType::RoundedBox;
		Brush.TintColor = FSlateColor(FillColor);
		Brush.OutlineSettings = FSlateBrushOutlineSettings(Radius, FSlateColor(OutlineColor), OutlineWidth);
		return Brush;
	}

	UTextBlock* EnsureButtonLabel(UWidgetBlueprint& WidgetBlueprint, UButton& Button, const FName WidgetName, const FText& LabelText)
	{
		UTextBlock* Label = FindWidget<UTextBlock>(WidgetBlueprint, WidgetName);
		if (Label == nullptr && WidgetBlueprint.WidgetTree != nullptr)
		{
			Label = WidgetBlueprint.WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), WidgetName);
		}

		if (Label == nullptr)
		{
			return nullptr;
		}

		EnsureWidgetGuid(WidgetBlueprint, *Label);
		Label->bIsVariable = false;
		Label->SetText(LabelText);
		Label->SetJustification(ETextJustify::Center);
		Label->SetColorAndOpacity(FSlateColor(FLinearColor::White));

		if (Button.GetContent() != Label)
		{
			Button.SetContent(Label);
		}

		return Label;
	}

	void ConfigureButtonStyle(UButton& Button, const FLinearColor& BaseColor)
	{
		FButtonStyle Style = Button.GetStyle();
		Style.SetNormal(MakeRoundedBrush(BaseColor, 12.0f, FLinearColor(1.0f, 1.0f, 1.0f, 0.15f), 1.0f));
		Style.SetHovered(MakeRoundedBrush(BaseColor * 1.12f, 12.0f, FLinearColor(1.0f, 1.0f, 1.0f, 0.22f), 1.0f));
		Style.SetPressed(MakeRoundedBrush(BaseColor * 0.88f, 12.0f, FLinearColor(1.0f, 1.0f, 1.0f, 0.22f), 1.0f));
		Style.SetDisabled(MakeRoundedBrush(BaseColor * 0.5f, 12.0f, FLinearColor(1.0f, 1.0f, 1.0f, 0.12f), 1.0f));
		Style.SetNormalPadding(FMargin(14.0f, 8.0f));
		Style.SetPressedPadding(FMargin(14.0f, 8.0f, 14.0f, 6.0f));
		Button.SetStyle(Style);
		Button.SetBackgroundColor(FLinearColor::White);
		Button.SetColorAndOpacity(FLinearColor::White);
	}

	bool ConfigureInteractionWidgetBlueprint(UWidgetBlueprint& WidgetBlueprint, UTexture2D& FilledCircleTexture, UTexture2D& OuterRingTexture, FString& OutError)
	{
		if (WidgetBlueprint.WidgetTree == nullptr)
		{
			OutError = TEXT("Interaction widget blueprint is missing a WidgetTree.");
			return false;
		}

		UCanvasPanel* RootCanvas = Cast<UCanvasPanel>(WidgetBlueprint.WidgetTree->RootWidget);
		if (RootCanvas == nullptr)
		{
			RootCanvas = WidgetBlueprint.WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
			WidgetBlueprint.WidgetTree->RootWidget = RootCanvas;
		}

		if (RootCanvas == nullptr)
		{
			OutError = TEXT("Failed to create RootCanvas for the interaction widget blueprint.");
			return false;
		}

		EnsureWidgetGuid(WidgetBlueprint, *RootCanvas);
		RootCanvas->bIsVariable = false;

		UOverlay* MarkerGroup = EnsurePanelChild<UOverlay>(WidgetBlueprint, *RootCanvas, TEXT("MarkerGroup"), false);
		UImage* FilledCircle = EnsurePanelChild<UImage>(WidgetBlueprint, *MarkerGroup, TEXT("IMG_FilledCircle"), true);
		UImage* OuterRing = EnsurePanelChild<UImage>(WidgetBlueprint, *MarkerGroup, TEXT("IMG_OuterRing"), true);
		UBorder* PromptBackground = EnsurePanelChild<UBorder>(WidgetBlueprint, *RootCanvas, TEXT("BOR_PromptBackground"), true);

		UTextBlock* PromptText = FindWidget<UTextBlock>(WidgetBlueprint, TEXT("TXT_Prompt"));
		if (PromptText == nullptr)
		{
			PromptText = WidgetBlueprint.WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TXT_Prompt"));
		}

		if (!MarkerGroup || !FilledCircle || !OuterRing || !PromptBackground || !PromptText)
		{
			OutError = TEXT("Failed to construct one or more widgets for WBP_InteractionIndicator.");
			return false;
		}

		EnsureWidgetGuid(WidgetBlueprint, *PromptText);
		PromptText->bIsVariable = true;

		if (PromptBackground->GetContent() != PromptText)
		{
			PromptBackground->SetContent(PromptText);
		}

		ConfigureCanvasSlot(*MarkerGroup, FVector2D(0.0f, 8.0f), true, FVector2D::ZeroVector);
		ConfigureCanvasSlot(*PromptBackground, FVector2D(56.0f, 32.0f), true, FVector2D(0.0f, 0.5f));
		ConfigureOverlaySlot(*FilledCircle);
		ConfigureOverlaySlot(*OuterRing);

		FilledCircle->SetBrushFromTexture(&FilledCircleTexture, false);
		{
			FSlateBrush Brush = FilledCircle->GetBrush();
			Brush.ImageSize = FVector2D(48.0f, 48.0f);
			FilledCircle->SetBrush(Brush);
		}
		FilledCircle->SetRenderOpacity(0.0f);

		OuterRing->SetBrushFromTexture(&OuterRingTexture, false);
		{
			FSlateBrush Brush = OuterRing->GetBrush();
			Brush.ImageSize = FVector2D(48.0f, 48.0f);
			OuterRing->SetBrush(Brush);
		}
		OuterRing->SetRenderOpacity(0.0f);

		PromptBackground->SetPadding(FMargin(10.0f, 4.0f));
		PromptBackground->SetBrushColor(FLinearColor::White);
		PromptBackground->SetRenderOpacity(0.0f);

		PromptText->SetText(FText::FromString(TEXT("먹기")));
		PromptText->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
		PromptText->SetRenderOpacity(0.0f);

		WidgetBlueprint.Modify();
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(&WidgetBlueprint);
		CompileBlueprint(&WidgetBlueprint);
		WidgetBlueprint.MarkPackageDirty();
		return true;
	}

	bool ConfigureMessagePopupWidgetBlueprint(UWidgetBlueprint& WidgetBlueprint, FString& OutError)
	{
		if (WidgetBlueprint.WidgetTree == nullptr)
		{
			OutError = TEXT("Message popup widget blueprint is missing a WidgetTree.");
			return false;
		}

		UCanvasPanel* RootCanvas = Cast<UCanvasPanel>(WidgetBlueprint.WidgetTree->RootWidget);
		if (RootCanvas == nullptr)
		{
			RootCanvas = WidgetBlueprint.WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
			WidgetBlueprint.WidgetTree->RootWidget = RootCanvas;
		}

		if (RootCanvas == nullptr)
		{
			OutError = TEXT("Failed to create RootCanvas for WBP_InteractionMessagePopup.");
			return false;
		}

		EnsureWidgetGuid(WidgetBlueprint, *RootCanvas);
		RootCanvas->bIsVariable = false;

		UOverlay* ScreenRoot = EnsurePanelChild<UOverlay>(WidgetBlueprint, *RootCanvas, TEXT("Overlay_ScreenRoot"), false);
		USizeBox* PopupFrame = EnsurePanelChild<USizeBox>(WidgetBlueprint, *ScreenRoot, TEXT("SizeBox_PopupFrame"), false);
		if (!ScreenRoot || !PopupFrame)
		{
			OutError = TEXT("Failed to create screen root widgets for WBP_InteractionMessagePopup.");
			return false;
		}

		UBackgroundBlur* BackgroundBlurPanel = FindWidget<UBackgroundBlur>(WidgetBlueprint, TEXT("BackgroundBlur_Panel"));
		if (BackgroundBlurPanel == nullptr)
		{
			BackgroundBlurPanel = WidgetBlueprint.WidgetTree->ConstructWidget<UBackgroundBlur>(UBackgroundBlur::StaticClass(), TEXT("BackgroundBlur_Panel"));
		}

		UBorder* SkyTintPanel = FindWidget<UBorder>(WidgetBlueprint, TEXT("Border_SkyTintPanel"));
		if (SkyTintPanel == nullptr)
		{
			SkyTintPanel = WidgetBlueprint.WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border_SkyTintPanel"));
		}

		UVerticalBox* ContentColumn = FindWidget<UVerticalBox>(WidgetBlueprint, TEXT("VerticalBox_Content"));
		if (ContentColumn == nullptr)
		{
			ContentColumn = WidgetBlueprint.WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VerticalBox_Content"));
		}

		UHorizontalBox* TitleBar = FindWidget<UHorizontalBox>(WidgetBlueprint, TEXT("HorizontalBox_TitleBar"));
		if (TitleBar == nullptr)
		{
			TitleBar = WidgetBlueprint.WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HorizontalBox_TitleBar"));
		}

		UTextBlock* TitleText = FindWidget<UTextBlock>(WidgetBlueprint, TEXT("TXT_Title"));
		if (TitleText == nullptr)
		{
			TitleText = WidgetBlueprint.WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TXT_Title"));
		}

		UButton* CloseButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_Close"));
		if (CloseButton == nullptr)
		{
			CloseButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_Close"));
		}

		UTextBlock* MessageText = FindWidget<UTextBlock>(WidgetBlueprint, TEXT("TXT_Message"));
		if (MessageText == nullptr)
		{
			MessageText = WidgetBlueprint.WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TXT_Message"));
		}

		UHorizontalBox* ActionButtons = FindWidget<UHorizontalBox>(WidgetBlueprint, TEXT("HB_ActionButtons"));
		if (ActionButtons == nullptr)
		{
			ActionButtons = WidgetBlueprint.WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HB_ActionButtons"));
		}

		UButton* OkButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_Ok"));
		if (OkButton == nullptr)
		{
			OkButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_Ok"));
		}

		UButton* YesButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_Yes"));
		if (YesButton == nullptr)
		{
			YesButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_Yes"));
		}

		UButton* NoButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_No"));
		if (NoButton == nullptr)
		{
			NoButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_No"));
		}

		if (!BackgroundBlurPanel || !SkyTintPanel || !ContentColumn || !TitleBar || !TitleText || !CloseButton || !MessageText || !ActionButtons || !OkButton || !YesButton || !NoButton)
		{
			OutError = TEXT("Failed to construct one or more popup widgets for WBP_InteractionMessagePopup.");
			return false;
		}

		EnsureWidgetGuid(WidgetBlueprint, *BackgroundBlurPanel);
		EnsureWidgetGuid(WidgetBlueprint, *SkyTintPanel);
		EnsureWidgetGuid(WidgetBlueprint, *ContentColumn);
		EnsureWidgetGuid(WidgetBlueprint, *TitleBar);
		EnsureWidgetGuid(WidgetBlueprint, *TitleText);
		EnsureWidgetGuid(WidgetBlueprint, *CloseButton);
		EnsureWidgetGuid(WidgetBlueprint, *MessageText);
		EnsureWidgetGuid(WidgetBlueprint, *ActionButtons);
		EnsureWidgetGuid(WidgetBlueprint, *OkButton);
		EnsureWidgetGuid(WidgetBlueprint, *YesButton);
		EnsureWidgetGuid(WidgetBlueprint, *NoButton);

		BackgroundBlurPanel->bIsVariable = false;
		SkyTintPanel->bIsVariable = false;
		ContentColumn->bIsVariable = false;
		TitleBar->bIsVariable = false;
		TitleText->bIsVariable = true;
		CloseButton->bIsVariable = true;
		MessageText->bIsVariable = true;
		ActionButtons->bIsVariable = true;
		OkButton->bIsVariable = true;
		YesButton->bIsVariable = true;
		NoButton->bIsVariable = true;

		if (PopupFrame->GetContent() != BackgroundBlurPanel)
		{
			PopupFrame->SetContent(BackgroundBlurPanel);
		}

		if (BackgroundBlurPanel->GetContent() != SkyTintPanel)
		{
			BackgroundBlurPanel->SetContent(SkyTintPanel);
		}

		if (SkyTintPanel->GetContent() != ContentColumn)
		{
			SkyTintPanel->SetContent(ContentColumn);
		}

		if (TitleBar->GetChildrenCount() != 2 || TitleBar->GetChildAt(0) != TitleText || TitleBar->GetChildAt(1) != CloseButton)
		{
			TitleBar->ClearChildren();
			TitleBar->AddChildToHorizontalBox(TitleText);
			TitleBar->AddChildToHorizontalBox(CloseButton);
		}

		if (ActionButtons->GetChildrenCount() != 3
			|| ActionButtons->GetChildAt(0) != OkButton
			|| ActionButtons->GetChildAt(1) != YesButton
			|| ActionButtons->GetChildAt(2) != NoButton)
		{
			ActionButtons->ClearChildren();
			ActionButtons->AddChildToHorizontalBox(OkButton);
			ActionButtons->AddChildToHorizontalBox(YesButton);
			ActionButtons->AddChildToHorizontalBox(NoButton);
		}

		if (ContentColumn->GetChildrenCount() != 3
			|| ContentColumn->GetChildAt(0) != TitleBar
			|| ContentColumn->GetChildAt(1) != MessageText
			|| ContentColumn->GetChildAt(2) != ActionButtons)
		{
			ContentColumn->ClearChildren();
			ContentColumn->AddChildToVerticalBox(TitleBar);
			ContentColumn->AddChildToVerticalBox(MessageText);
			ContentColumn->AddChildToVerticalBox(ActionButtons);
		}

		ConfigureCanvasAnchors(*ScreenRoot, FAnchors(0.0f, 0.0f, 1.0f, 1.0f), FMargin(0.0f), FVector2D::ZeroVector);
		ConfigureOverlaySlot(*PopupFrame);

		PopupFrame->SetWidthOverride(520.0f);
		PopupFrame->SetMinDesiredHeight(240.0f);

		BackgroundBlurPanel->SetBlurStrength(18.0f);
		BackgroundBlurPanel->SetOverrideAutoRadiusCalculation(true);
		BackgroundBlurPanel->SetBlurRadius(12);
		BackgroundBlurPanel->SetApplyAlphaToBlur(true);
		BackgroundBlurPanel->SetCornerRadius(FVector4(20.0f, 20.0f, 20.0f, 20.0f));

		SkyTintPanel->SetPadding(FMargin(24.0f, 20.0f));
		SkyTintPanel->SetBrush(MakeRoundedBrush(FLinearColor(0.49f, 0.72f, 0.95f, 0.18f), 20.0f, FLinearColor(1.0f, 1.0f, 1.0f, 0.08f), 1.0f));

		TitleText->SetText(FText::FromString(TEXT("Notice")));
		TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));

		CloseButton->SetToolTipText(FText::FromString(TEXT("Close")));
		ConfigureButtonStyle(*CloseButton, FLinearColor(0.22f, 0.42f, 0.60f, 0.96f));
		EnsureButtonLabel(WidgetBlueprint, *CloseButton, TEXT("TXT_CloseLabel"), FText::FromString(TEXT("X")));

		MessageText->SetText(FText::FromString(TEXT("This sign has no message.")));
		MessageText->SetAutoWrapText(true);
		MessageText->SetJustification(ETextJustify::Left);
		MessageText->SetColorAndOpacity(FSlateColor(FLinearColor::White));

		ConfigureButtonStyle(*OkButton, FLinearColor(0.24f, 0.52f, 0.74f, 0.96f));
		ConfigureButtonStyle(*YesButton, FLinearColor(0.24f, 0.52f, 0.74f, 0.96f));
		ConfigureButtonStyle(*NoButton, FLinearColor(0.15f, 0.27f, 0.40f, 0.96f));
		EnsureButtonLabel(WidgetBlueprint, *OkButton, TEXT("TXT_OkLabel"), FText::FromString(TEXT("OK")));
		EnsureButtonLabel(WidgetBlueprint, *YesButton, TEXT("TXT_YesLabel"), FText::FromString(TEXT("Yes")));
		EnsureButtonLabel(WidgetBlueprint, *NoButton, TEXT("TXT_NoLabel"), FText::FromString(TEXT("No")));

		ConfigureVerticalBoxSlot(*TitleBar, FMargin(0.0f, 0.0f, 0.0f, 18.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*MessageText, FMargin(0.0f, 0.0f, 0.0f, 20.0f), ESlateSizeRule::Fill, HAlign_Fill, VAlign_Fill);
		ConfigureVerticalBoxSlot(*ActionButtons, FMargin(0.0f), ESlateSizeRule::Automatic, HAlign_Right, VAlign_Bottom);

		ConfigureHorizontalBoxSlot(*TitleText, FMargin(0.0f, 0.0f, 12.0f, 0.0f), ESlateSizeRule::Fill, HAlign_Left, VAlign_Center);
		ConfigureHorizontalBoxSlot(*CloseButton, FMargin(0.0f), ESlateSizeRule::Automatic, HAlign_Right, VAlign_Center);
		ConfigureHorizontalBoxSlot(*OkButton, FMargin(0.0f, 0.0f, 10.0f, 0.0f), ESlateSizeRule::Automatic, HAlign_Right, VAlign_Center);
		ConfigureHorizontalBoxSlot(*YesButton, FMargin(0.0f, 0.0f, 10.0f, 0.0f), ESlateSizeRule::Automatic, HAlign_Right, VAlign_Center);
		ConfigureHorizontalBoxSlot(*NoButton, FMargin(0.0f), ESlateSizeRule::Automatic, HAlign_Right, VAlign_Center);

		WidgetBlueprint.Modify();
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(&WidgetBlueprint);
		CompileBlueprint(&WidgetBlueprint);
		WidgetBlueprint.MarkPackageDirty();
		return true;
	}

	bool ConfigureScrollMessagePopupWidgetBlueprint(UWidgetBlueprint& WidgetBlueprint, UTexture2D& SmileIconTexture, FString& OutError)
	{
		if (WidgetBlueprint.WidgetTree == nullptr)
		{
			OutError = TEXT("Scroll message popup widget blueprint is missing a WidgetTree.");
			return false;
		}

		UCanvasPanel* RootCanvas = Cast<UCanvasPanel>(WidgetBlueprint.WidgetTree->RootWidget);
		if (RootCanvas == nullptr)
		{
			RootCanvas = WidgetBlueprint.WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
			WidgetBlueprint.WidgetTree->RootWidget = RootCanvas;
		}

		if (RootCanvas == nullptr)
		{
			OutError = TEXT("Failed to create RootCanvas for WBP_InteractionScrollMessagePopup.");
			return false;
		}

		EnsureWidgetGuid(WidgetBlueprint, *RootCanvas);
		RootCanvas->bIsVariable = false;

		UOverlay* ScreenRoot = EnsurePanelChild<UOverlay>(WidgetBlueprint, *RootCanvas, TEXT("Overlay_ScreenRoot"), false);
		USizeBox* PopupFrame = EnsurePanelChild<USizeBox>(WidgetBlueprint, *ScreenRoot, TEXT("SizeBox_PopupFrame"), false);
		if (!ScreenRoot || !PopupFrame)
		{
			OutError = TEXT("Failed to create screen root widgets for WBP_InteractionScrollMessagePopup.");
			return false;
		}

		UBackgroundBlur* BackgroundBlurPanel = FindWidget<UBackgroundBlur>(WidgetBlueprint, TEXT("BackgroundBlur_Panel"));
		if (BackgroundBlurPanel == nullptr)
		{
			BackgroundBlurPanel = WidgetBlueprint.WidgetTree->ConstructWidget<UBackgroundBlur>(UBackgroundBlur::StaticClass(), TEXT("BackgroundBlur_Panel"));
		}

		UBorder* YellowTintPanel = FindWidget<UBorder>(WidgetBlueprint, TEXT("Border_YellowTintPanel"));
		if (YellowTintPanel == nullptr)
		{
			YellowTintPanel = WidgetBlueprint.WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border_YellowTintPanel"));
		}

		UVerticalBox* ContentColumn = FindWidget<UVerticalBox>(WidgetBlueprint, TEXT("VerticalBox_Content"));
		if (ContentColumn == nullptr)
		{
			ContentColumn = WidgetBlueprint.WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VerticalBox_Content"));
		}

		UHorizontalBox* TitleBar = FindWidget<UHorizontalBox>(WidgetBlueprint, TEXT("HorizontalBox_TitleBar"));
		if (TitleBar == nullptr)
		{
			TitleBar = WidgetBlueprint.WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HorizontalBox_TitleBar"));
		}

		UTextBlock* TitleText = FindWidget<UTextBlock>(WidgetBlueprint, TEXT("TXT_Title"));
		if (TitleText == nullptr)
		{
			TitleText = WidgetBlueprint.WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TXT_Title"));
		}

		UImage* SmileIcon = FindWidget<UImage>(WidgetBlueprint, TEXT("Image_SmileIcon"));
		if (SmileIcon == nullptr)
		{
			SmileIcon = WidgetBlueprint.WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Image_SmileIcon"));
		}

		UScrollBox* MessageScrollBox = FindWidget<UScrollBox>(WidgetBlueprint, TEXT("ScrollBox_Message"));
		if (MessageScrollBox == nullptr)
		{
			MessageScrollBox = WidgetBlueprint.WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("ScrollBox_Message"));
		}

		UTextBlock* MessageText = FindWidget<UTextBlock>(WidgetBlueprint, TEXT("TXT_Message"));
		if (MessageText == nullptr)
		{
			MessageText = WidgetBlueprint.WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TXT_Message"));
		}

		UHorizontalBox* ButtonRow = FindWidget<UHorizontalBox>(WidgetBlueprint, TEXT("HorizontalBox_ButtonRow"));
		if (ButtonRow == nullptr)
		{
			ButtonRow = WidgetBlueprint.WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HorizontalBox_ButtonRow"));
		}

		UButton* OkButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_Ok"));
		if (OkButton == nullptr)
		{
			OkButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_Ok"));
		}

		if (!BackgroundBlurPanel || !YellowTintPanel || !ContentColumn || !TitleBar || !TitleText || !SmileIcon || !MessageScrollBox || !MessageText || !ButtonRow || !OkButton)
		{
			OutError = TEXT("Failed to construct one or more popup widgets for WBP_InteractionScrollMessagePopup.");
			return false;
		}

		EnsureWidgetGuid(WidgetBlueprint, *BackgroundBlurPanel);
		EnsureWidgetGuid(WidgetBlueprint, *YellowTintPanel);
		EnsureWidgetGuid(WidgetBlueprint, *ContentColumn);
		EnsureWidgetGuid(WidgetBlueprint, *TitleBar);
		EnsureWidgetGuid(WidgetBlueprint, *TitleText);
		EnsureWidgetGuid(WidgetBlueprint, *SmileIcon);
		EnsureWidgetGuid(WidgetBlueprint, *MessageScrollBox);
		EnsureWidgetGuid(WidgetBlueprint, *MessageText);
		EnsureWidgetGuid(WidgetBlueprint, *ButtonRow);
		EnsureWidgetGuid(WidgetBlueprint, *OkButton);

		BackgroundBlurPanel->bIsVariable = false;
		YellowTintPanel->bIsVariable = false;
		ContentColumn->bIsVariable = false;
		TitleBar->bIsVariable = false;
		TitleText->bIsVariable = true;
		SmileIcon->bIsVariable = true;
		MessageScrollBox->bIsVariable = true;
		MessageText->bIsVariable = true;
		ButtonRow->bIsVariable = false;
		OkButton->bIsVariable = true;

		if (PopupFrame->GetContent() != BackgroundBlurPanel)
		{
			PopupFrame->SetContent(BackgroundBlurPanel);
		}

		if (BackgroundBlurPanel->GetContent() != YellowTintPanel)
		{
			BackgroundBlurPanel->SetContent(YellowTintPanel);
		}

		if (YellowTintPanel->GetContent() != ContentColumn)
		{
			YellowTintPanel->SetContent(ContentColumn);
		}

		if (TitleBar->GetChildrenCount() != 1 || TitleBar->GetChildAt(0) != TitleText)
		{
			TitleBar->ClearChildren();
			TitleBar->AddChildToHorizontalBox(TitleText);
		}

		if (MessageScrollBox->GetChildrenCount() != 1 || MessageScrollBox->GetChildAt(0) != MessageText)
		{
			MessageScrollBox->ClearChildren();
			MessageScrollBox->AddChild(MessageText);
		}

		if (ButtonRow->GetChildrenCount() != 1 || ButtonRow->GetChildAt(0) != OkButton)
		{
			ButtonRow->ClearChildren();
			ButtonRow->AddChildToHorizontalBox(OkButton);
		}

		if (ContentColumn->GetChildrenCount() != 4
			|| ContentColumn->GetChildAt(0) != TitleBar
			|| ContentColumn->GetChildAt(1) != SmileIcon
			|| ContentColumn->GetChildAt(2) != MessageScrollBox
			|| ContentColumn->GetChildAt(3) != ButtonRow)
		{
			ContentColumn->ClearChildren();
			ContentColumn->AddChildToVerticalBox(TitleBar);
			ContentColumn->AddChildToVerticalBox(SmileIcon);
			ContentColumn->AddChildToVerticalBox(MessageScrollBox);
			ContentColumn->AddChildToVerticalBox(ButtonRow);
		}

		ConfigureCanvasAnchors(*ScreenRoot, FAnchors(0.0f, 0.0f, 1.0f, 1.0f), FMargin(0.0f), FVector2D::ZeroVector);
		ConfigureOverlaySlot(*PopupFrame);

		PopupFrame->SetWidthOverride(580.0f);
		PopupFrame->SetMinDesiredHeight(400.0f);

		BackgroundBlurPanel->SetBlurStrength(18.0f);
		BackgroundBlurPanel->SetOverrideAutoRadiusCalculation(true);
		BackgroundBlurPanel->SetBlurRadius(12);
		BackgroundBlurPanel->SetApplyAlphaToBlur(true);
		BackgroundBlurPanel->SetCornerRadius(FVector4(20.0f, 20.0f, 20.0f, 20.0f));

		YellowTintPanel->SetPadding(FMargin(24.0f, 20.0f));
		YellowTintPanel->SetBrush(MakeRoundedBrush(FLinearColor(0.98f, 0.82f, 0.28f, 0.15f), 20.0f, FLinearColor(1.0f, 1.0f, 1.0f, 0.08f), 1.0f));

		TitleText->SetText(FText::FromString(TEXT("읽기")));
		TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));

		SmileIcon->SetBrushFromTexture(&SmileIconTexture, false);
		{
			FSlateBrush Brush = SmileIcon->GetBrush();
			Brush.ImageSize = FVector2D(64.0f, 64.0f);
			SmileIcon->SetBrush(Brush);
		}
		SmileIcon->SetColorAndOpacity(FLinearColor::White);

		MessageScrollBox->SetAlwaysShowScrollbar(false);
		MessageScrollBox->SetAnimateWheelScrolling(true);
		MessageScrollBox->SetWheelScrollMultiplier(1.0f);
		MessageScrollBox->SetConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible);

		MessageText->SetText(FText::FromString(TEXT("첫 줄입니다.\n\n이 팝업은 긴 문장을 스크롤해서 읽는 형태를 테스트하기 위한 예시입니다.\n\n같은 패널 구조를 유지하면서도 본문은 내부 스크롤 영역 안에서만 움직여야 합니다.\n\n사용자는 읽기 상호작용으로 이 팝업을 열고, 내용을 확인한 뒤 OK 버튼으로 닫습니다.\n\n이 문단은 스크롤 동작을 확인하기 위해 일부러 길게 작성되었습니다.\n\n추가 문장 A.\n추가 문장 B.\n추가 문장 C.\n추가 문장 D.")));
		MessageText->SetAutoWrapText(true);
		MessageText->SetJustification(ETextJustify::Left);
		MessageText->SetColorAndOpacity(FSlateColor(FLinearColor::White));

		ConfigureButtonStyle(*OkButton, FLinearColor(0.70f, 0.50f, 0.10f, 0.96f));
		EnsureButtonLabel(WidgetBlueprint, *OkButton, TEXT("TXT_OkLabel"), FText::FromString(TEXT("OK")));

		ConfigureVerticalBoxSlot(*TitleBar, FMargin(0.0f, 0.0f, 0.0f, 14.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*SmileIcon, FMargin(0.0f, 0.0f, 0.0f, 12.0f), ESlateSizeRule::Automatic, HAlign_Center, VAlign_Center);
		ConfigureVerticalBoxSlot(*MessageScrollBox, FMargin(0.0f, 0.0f, 0.0f, 18.0f), ESlateSizeRule::Fill, HAlign_Fill, VAlign_Fill);
		ConfigureVerticalBoxSlot(*ButtonRow, FMargin(0.0f), ESlateSizeRule::Automatic, HAlign_Right, VAlign_Bottom);

		ConfigureHorizontalBoxSlot(*TitleText, FMargin(0.0f), ESlateSizeRule::Fill, HAlign_Left, VAlign_Center);
		ConfigureHorizontalBoxSlot(*OkButton, FMargin(0.0f), ESlateSizeRule::Automatic, HAlign_Right, VAlign_Center);

		if (UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(MessageText->Slot))
		{
			ScrollBoxSlot->SetPadding(FMargin(0.0f, 4.0f, 4.0f, 0.0f));
			ScrollBoxSlot->SetHorizontalAlignment(HAlign_Fill);
			ScrollBoxSlot->SetVerticalAlignment(VAlign_Top);
		}

		WidgetBlueprint.Modify();
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(&WidgetBlueprint);
		CompileBlueprint(&WidgetBlueprint);
		WidgetBlueprint.MarkPackageDirty();
		return true;
	}

	bool ConfigureInteractableBlueprint(UBlueprint& Blueprint, UStaticMesh& StaticMesh, const FText& PromptText, FString& OutError)
	{
		CompileBlueprint(&Blueprint);

		ACodexInteractableActor* DefaultObject = Blueprint.GeneratedClass ? Cast<ACodexInteractableActor>(Blueprint.GeneratedClass->GetDefaultObject()) : nullptr;
		if (DefaultObject == nullptr)
		{
			OutError = FString::Printf(TEXT("Failed to access default object for %s."), *Blueprint.GetName());
			return false;
		}

		UStaticMeshComponent* StaticMeshComponent = DefaultObject->GetStaticMeshComponent();
		UCodexInteractionComponent* InteractionComponent = DefaultObject->GetInteractionComponent();
		if (StaticMeshComponent == nullptr || InteractionComponent == nullptr)
		{
			OutError = FString::Printf(TEXT("Blueprint %s is missing required native components."), *Blueprint.GetName());
			return false;
		}

		StaticMeshComponent->SetStaticMesh(&StaticMesh);
		InteractionComponent->SetInteractionType(ECodexInteractionType::Eat);
		InteractionComponent->SetPromptText(PromptText);
		InteractionComponent->SetVisibleDistance(300.0f);
		InteractionComponent->SetInteractableDistance(120.0f);
		DefaultObject->MarkPackageDirty();
		Blueprint.MarkPackageDirty();
		FBlueprintEditorUtils::MarkBlueprintAsModified(&Blueprint);
		CompileBlueprint(&Blueprint);
		return true;
	}

	bool ConfigurePopupInteractableBlueprint(
		UBlueprint& Blueprint,
		UStaticMesh& StaticMesh,
		const FText& PromptText,
		const FText& PopupTitle,
		const FText& PopupMessage,
		const ECodexPopupButtonLayout ButtonLayout,
		FString& OutError)
	{
		CompileBlueprint(&Blueprint);

		ACodexPopupInteractableActor* DefaultObject = Blueprint.GeneratedClass ? Cast<ACodexPopupInteractableActor>(Blueprint.GeneratedClass->GetDefaultObject()) : nullptr;
		if (DefaultObject == nullptr)
		{
			OutError = FString::Printf(TEXT("Failed to access popup interactable default object for %s."), *Blueprint.GetName());
			return false;
		}

		UStaticMeshComponent* StaticMeshComponent = DefaultObject->GetStaticMeshComponent();
		UCodexInteractionComponent* InteractionComponent = DefaultObject->GetInteractionComponent();
		if (StaticMeshComponent == nullptr || InteractionComponent == nullptr)
		{
			OutError = FString::Printf(TEXT("Popup blueprint %s is missing required native components."), *Blueprint.GetName());
			return false;
		}

		StaticMeshComponent->SetStaticMesh(&StaticMesh);
		InteractionComponent->SetInteractionType(ECodexInteractionType::Use);
		InteractionComponent->SetPromptText(PromptText);
		InteractionComponent->SetVisibleDistance(320.0f);
		InteractionComponent->SetInteractableDistance(140.0f);
		DefaultObject->SetPopupTitle(PopupTitle);
		DefaultObject->SetPopupMessage(PopupMessage);
		DefaultObject->SetPopupButtonLayout(ButtonLayout);
		DefaultObject->MarkPackageDirty();
		Blueprint.MarkPackageDirty();
		FBlueprintEditorUtils::MarkBlueprintAsModified(&Blueprint);
		CompileBlueprint(&Blueprint);
		return true;
	}

	APlayerStart* FindPlayerStart(UEditorActorSubsystem& ActorSubsystem)
	{
		for (AActor* Actor : ActorSubsystem.GetAllLevelActors())
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				return PlayerStart;
			}
		}

		return nullptr;
	}

	AActor* FindPlacedActor(UEditorActorSubsystem& ActorSubsystem, const FString& ActorLabel)
	{
		for (AActor* Actor : ActorSubsystem.GetAllLevelActors())
		{
			if (!IsValid(Actor))
			{
				continue;
			}

			if (Actor->GetActorLabel() == ActorLabel || (Actor->Tags.Contains(InteractionPlacementTag) && Actor->GetActorLabel().Contains(ActorLabel)))
			{
				return Actor;
			}
		}

		return nullptr;
	}

	FVector ResolvePlacementOrigin(UWorld& World, const APlayerStart* PlayerStart)
	{
		FVector PlacementOrigin = PlayerStart ? PlayerStart->GetActorLocation() : FVector::ZeroVector;

		const FVector TraceStart = PlacementOrigin + FVector(0.0f, 0.0f, 500.0f);
		const FVector TraceEnd = PlacementOrigin - FVector(0.0f, 0.0f, 2000.0f);
		FHitResult HitResult;
		FCollisionQueryParams QueryParams(TEXT("CodexInteractionPlacementTrace"), false, PlayerStart);
		if (World.LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams))
		{
			PlacementOrigin = HitResult.ImpactPoint;
		}
		else if (PlayerStart != nullptr)
		{
			PlacementOrigin.Z = FMath::Max(0.0f, PlacementOrigin.Z - 64.0f);
		}

		PlacementOrigin.Z += 20.0f;
		return PlacementOrigin;
	}

	FVector ResolvePlacementForward(const APlayerStart* PlayerStart)
	{
		FVector Forward = PlayerStart ? PlayerStart->GetActorForwardVector() : FVector::ForwardVector;
		Forward.Z = 0.0f;
		if (!Forward.Normalize())
		{
			return FVector::ForwardVector;
		}

		return Forward;
	}

	AActor* SpawnOrUpdatePlacedActor(
		UEditorActorSubsystem& ActorSubsystem,
		UClass* ActorClass,
		const FString& ActorLabel,
		const FVector& ActorLocation,
		const FRotator& ActorRotation,
		FString& OutError)
	{
		if (ActorClass == nullptr)
		{
			OutError = FString::Printf(TEXT("Placement class is missing for %s."), *ActorLabel);
			return nullptr;
		}

		AActor* Actor = FindPlacedActor(ActorSubsystem, ActorLabel);
		if (Actor != nullptr && !Actor->IsA(ActorClass))
		{
			ActorSubsystem.DestroyActor(Actor);
			Actor = nullptr;
		}

		if (Actor == nullptr)
		{
			Actor = ActorSubsystem.SpawnActorFromClass(ActorClass, ActorLocation, ActorRotation, false);
			if (Actor == nullptr)
			{
				OutError = FString::Printf(TEXT("Failed to place %s into BasicMap."), *ActorLabel);
				return nullptr;
			}
		}

		Actor->Modify();
		Actor->Tags.AddUnique(InteractionPlacementTag);
		Actor->SetActorLocationAndRotation(ActorLocation, ActorRotation, false, nullptr, ETeleportType::TeleportPhysics);
		Actor->SetActorHiddenInGame(false);
		Actor->SetActorLabel(ActorLabel);
		Actor->MarkPackageDirty();
		return Actor;
	}

	bool PlaceInteractionTestActorsInMap(
		UBlueprint& AppleBlueprint,
		UBlueprint& StrawberryBlueprint,
		UBlueprint& WoodenSignBlueprint,
		UBlueprint& WoodenSignScrollBlueprint,
		FString& OutError)
	{
		FString MapFilename;
		if (!FPackageName::TryConvertLongPackageNameToFilename(BasicMapAssetPath, MapFilename, FPackageName::GetMapPackageExtension()))
		{
			OutError = TEXT("Failed to resolve BasicMap filename.");
			return false;
		}

		UWorld* World = UEditorLoadingAndSavingUtils::LoadMap(MapFilename);
		if (World == nullptr)
		{
			OutError = TEXT("Failed to load /Game/Maps/BasicMap for interaction test placement.");
			return false;
		}

		UEditorActorSubsystem* ActorSubsystem = GetEditorActorSubsystem();
		if (ActorSubsystem == nullptr)
		{
			OutError = TEXT("Editor actor subsystem is unavailable.");
			return false;
		}

		APlayerStart* PlayerStart = FindPlayerStart(*ActorSubsystem);
		const FVector PlacementOrigin = ResolvePlacementOrigin(*World, PlayerStart);
		const FVector Forward = ResolvePlacementForward(PlayerStart);
		FVector Right = FVector::CrossProduct(FVector::UpVector, Forward);
		if (!Right.Normalize())
		{
			Right = FVector::RightVector;
		}

		const FVector AppleLocation = PlacementOrigin + (Forward * 180.0f) - (Right * 48.0f);
		const FVector StrawberryLocation = PlacementOrigin + (Forward * 240.0f) + (Right * 48.0f);
		const FVector WoodenSignLocation = PlacementOrigin + (Forward * 320.0f) - (Right * 72.0f);
		const FVector WoodenSignScrollLocation = PlacementOrigin + (Forward * 360.0f) + (Right * 72.0f);
		const FRotator PlacementRotation = FRotator::ZeroRotator;

		if (SpawnOrUpdatePlacedActor(*ActorSubsystem, AppleBlueprint.GeneratedClass, InteractionPlacementLabelApple, AppleLocation, PlacementRotation, OutError) == nullptr)
		{
			return false;
		}

		if (SpawnOrUpdatePlacedActor(*ActorSubsystem, StrawberryBlueprint.GeneratedClass, InteractionPlacementLabelStrawberry, StrawberryLocation, PlacementRotation, OutError) == nullptr)
		{
			return false;
		}

		if (SpawnOrUpdatePlacedActor(*ActorSubsystem, WoodenSignBlueprint.GeneratedClass, InteractionPlacementLabelWoodenSign, WoodenSignLocation, PlacementRotation, OutError) == nullptr)
		{
			return false;
		}

		if (SpawnOrUpdatePlacedActor(*ActorSubsystem, WoodenSignScrollBlueprint.GeneratedClass, InteractionPlacementLabelWoodenSignScroll, WoodenSignScrollLocation, PlacementRotation, OutError) == nullptr)
		{
			return false;
		}

		World->MarkPackageDirty();
		if (!UEditorLoadingAndSavingUtils::SaveMap(World, BasicMapAssetPath))
		{
			OutError = TEXT("Failed to save /Game/Maps/BasicMap after placing interaction test actors.");
			return false;
		}

		return true;
	}
}

bool FCodexInteractionAssetBuilder::RunBuild(FString& OutError)
{
	if (GEditor == nullptr)
	{
		OutError = TEXT("Editor asset subsystem is unavailable.");
		return false;
	}

	if (IsCurrentProjectEditorRunning())
	{
		OutError = TEXT("An Unreal Editor session for this project is running. Stop the editor and rerun the interaction asset build.");
		return false;
	}

	EnsureDirectory(UIPath);
	EnsureDirectory(InputActionsPath);
	EnsureDirectory(InputContextsPath);
	EnsureDirectory(InputDataPath);
	EnsureDirectory(BlueprintsPath);

	UInputAction_Factory* InputActionFactory = NewObject<UInputAction_Factory>();
	InputActionFactory->InputActionClass = UInputAction::StaticClass();
	InputActionFactory->bEditAfterNew = false;

	UInputAction* InteractAction = CreateAsset<UInputAction>(InputActionsPath, InteractActionName, InputActionFactory);
	UInputAction* PopupCloseAction = CreateAsset<UInputAction>(InputActionsPath, PopupCloseActionName, InputActionFactory);

	UInputMappingContext* MappingContext = LoadAsset<UInputMappingContext>(MappingContextObjectPath);
	UCodexTopDownInputConfigDataAsset* InputConfig = LoadAsset<UCodexTopDownInputConfigDataAsset>(InputConfigObjectPath);

	if (InteractAction == nullptr || PopupCloseAction == nullptr || MappingContext == nullptr || InputConfig == nullptr)
	{
		OutError = TEXT("Top-down input assets are missing. Run the base bootstrap before building interaction assets.");
		return false;
	}

	ConfigureInteractAction(*InteractAction);
	ConfigurePopupCloseAction(*PopupCloseAction);
	ConfigureInteractMappings(*MappingContext, *InteractAction);
	ConfigurePopupCloseMappings(*MappingContext, *PopupCloseAction);
	ConfigureInputConfig(*InputConfig, *MappingContext, *InteractAction, *PopupCloseAction);

	UTexture2DFactoryNew* TextureFactory = NewObject<UTexture2DFactoryNew>();
	TextureFactory->bEditAfterNew = false;

	UTexture2D* FilledCircleTexture = CreateAsset<UTexture2D>(UIPath, FilledCircleTextureName, TextureFactory);
	UTexture2D* OuterRingTexture = CreateAsset<UTexture2D>(UIPath, OuterRingTextureName, TextureFactory);
	UTexture2D* SmileIconTexture = CreateAsset<UTexture2D>(UIPath, SmileIconTextureName, TextureFactory);
	if (FilledCircleTexture == nullptr || OuterRingTexture == nullptr || SmileIconTexture == nullptr)
	{
		OutError = TEXT("Failed to create interaction texture assets.");
		return false;
	}

	const TArray64<uint8> FilledCirclePixels = BuildTexturePixels(64, 0.0f, 16.0f);
	const TArray64<uint8> OuterRingPixels = BuildTexturePixels(64, 20.0f, 24.0f);
	const TArray64<uint8> SmileIconPixels = BuildSmileTexturePixels(128);
	ConfigureTexture(*FilledCircleTexture, FilledCirclePixels, 64);
	ConfigureTexture(*OuterRingTexture, OuterRingPixels, 64);
	ConfigureTexture(*SmileIconTexture, SmileIconPixels, 128);

	UWidgetBlueprint* IndicatorWidget = CreateWidgetBlueprint(UIPath, IndicatorWidgetName, UCodexInteractionIndicatorWidget::StaticClass());
	if (IndicatorWidget == nullptr)
	{
		OutError = TEXT("Failed to create WBP_InteractionIndicator.");
		return false;
	}

	if (!ConfigureInteractionWidgetBlueprint(*IndicatorWidget, *FilledCircleTexture, *OuterRingTexture, OutError))
	{
		return false;
	}

	UWidgetBlueprint* MessagePopupWidget = CreateWidgetBlueprint(UIPath, MessagePopupWidgetName, UCodexInteractionMessagePopupWidget::StaticClass());
	if (MessagePopupWidget == nullptr)
	{
		OutError = TEXT("Failed to create WBP_InteractionMessagePopup.");
		return false;
	}

	if (!ConfigureMessagePopupWidgetBlueprint(*MessagePopupWidget, OutError))
	{
		return false;
	}

	UWidgetBlueprint* ScrollMessagePopupWidget = CreateWidgetBlueprint(UIPath, ScrollMessagePopupWidgetName, UCodexInteractionScrollMessagePopupWidget::StaticClass());
	if (ScrollMessagePopupWidget == nullptr)
	{
		OutError = TEXT("Failed to create WBP_InteractionScrollMessagePopup.");
		return false;
	}

	if (!ConfigureScrollMessagePopupWidgetBlueprint(*ScrollMessagePopupWidget, *SmileIconTexture, OutError))
	{
		return false;
	}

	UStaticMesh* AppleMesh = LoadAsset<UStaticMesh>(AppleMeshObjectPath);
	UStaticMesh* StrawberryMesh = LoadAsset<UStaticMesh>(StrawberryMeshObjectPath);
	UStaticMesh* WoodenSignMesh = LoadAsset<UStaticMesh>(WoodenSignMeshObjectPath);
	if (AppleMesh == nullptr || StrawberryMesh == nullptr || WoodenSignMesh == nullptr)
	{
		OutError = TEXT("Required meshes were not found for the interaction test blueprints.");
		return false;
	}

	UBlueprint* AppleBlueprint = CreateBlueprint(BlueprintsPath, InteractableAppleName, ACodexInteractableActor::StaticClass());
	UBlueprint* StrawberryBlueprint = CreateBlueprint(BlueprintsPath, InteractableStrawberryName, ACodexInteractableActor::StaticClass());
	UBlueprint* WoodenSignBlueprint = CreateBlueprint(BlueprintsPath, InteractableWoodenSignPopupName, ACodexPopupInteractableActor::StaticClass());
	UBlueprint* WoodenSignScrollBlueprint = CreateBlueprint(BlueprintsPath, InteractableWoodenSignScrollPopupName, ACodexScrollMessagePopupInteractableActor::StaticClass());
	if (AppleBlueprint == nullptr || StrawberryBlueprint == nullptr || WoodenSignBlueprint == nullptr || WoodenSignScrollBlueprint == nullptr)
	{
		OutError = TEXT("Failed to create one or more interactable test blueprints.");
		return false;
	}

#if 0

	const FText EatPrompt = FText::FromString(TEXT("먹기"));
	const FText MessagePopupPrompt = FText::FromString(TEXT("보기"));
	const FText MessagePopupTitle = FText::FromString(TEXT("안내"));
	const FText MessagePopupBody = FText::FromString(TEXT("여기는 상호작용 메시지 팝업 예시입니다."));
	const FText ScrollPopupPrompt = FText::FromString(TEXT("읽기"));
	const FText ScrollPopupTitle = FText::FromString(TEXT("게시판"));
	const FText ScrollPopupBody = FText::FromString(TEXT("코덱스 마을 안내문입니다.\n\n이 팝업은 긴 텍스트를 스크롤해서 읽는 형태를 검증하기 위한 예시입니다.\n패널 외형은 기존 메시지 팝업과 거의 동일하지만, 밝은 하늘색 오버레이 대신 더 옅은 노란색 계열의 틴트를 사용합니다.\n\n사용자는 내용을 충분히 읽은 다음 OK 버튼으로만 닫을 수 있어야 합니다.\n스페이스 키는 이 팝업의 닫기 입력으로 처리되지 않아야 하며, 결과는 상호작용 서브시스템을 통해 통합 전달됩니다.\n\n문구가 길어져도 레이아웃이 깨지지 않고 자연스럽게 스크롤되는지 확인합니다."));

	if (!ConfigureInteractableBlueprint(*AppleBlueprint, *AppleMesh, FText::FromString(TEXT("먹기")), OutError))
	{
		return false;
	}

	if (!ConfigureInteractableBlueprint(*StrawberryBlueprint, *StrawberryMesh, FText::FromString(TEXT("먹기")), OutError))
	{
		return false;
	}

	if (!ConfigurePopupInteractableBlueprint(
		*WoodenSignBlueprint,
		*WoodenSignMesh,
		FText::FromString(TEXT("보기")),
		FText::FromString(TEXT("안내")),
		FText::FromString(TEXT("여기서는 상호작용 팝업이 열립니다.")),
		ECodexPopupButtonLayout::Ok,
		OutError))
	{
		return false;
	}

#endif

	const FText EatPromptText = FText::FromString(TEXT("\uBA39\uAE30"));
	const FText MessagePopupPromptText = FText::FromString(TEXT("\uBCF4\uAE30"));
	const FText MessagePopupTitleText = FText::FromString(TEXT("\uC548\uB0B4"));
	const FText MessagePopupBodyText = FText::FromString(TEXT("This is a sample interaction message popup."));
	const FText ScrollPopupPromptText = FText::FromString(TEXT("\uC77D\uAE30"));
	const FText ScrollPopupTitleText = FText::FromString(TEXT("\uAC8C\uC2DC\uD310"));
	const FText ScrollPopupBodyText = FText::FromString(TEXT("Codex village notice.\n\nThis popup verifies a long scrolling message layout. The panel follows the existing popup style, but the soft tint is yellow instead of sky blue.\n\nThe popup should only close through the OK button. Space key close handling must be ignored for this popup, and the result should still flow through the interaction subsystem.\n\nMake sure long text keeps the layout stable and scrolls naturally."));

	if (!ConfigureInteractableBlueprint(*AppleBlueprint, *AppleMesh, EatPromptText, OutError))
	{
		return false;
	}

	if (!ConfigureInteractableBlueprint(*StrawberryBlueprint, *StrawberryMesh, EatPromptText, OutError))
	{
		return false;
	}

	if (!ConfigurePopupInteractableBlueprint(
		*WoodenSignBlueprint,
		*WoodenSignMesh,
		MessagePopupPromptText,
		MessagePopupTitleText,
		MessagePopupBodyText,
		ECodexPopupButtonLayout::Ok,
		OutError))
	{
		return false;
	}

	if (!ConfigurePopupInteractableBlueprint(
		*WoodenSignScrollBlueprint,
		*WoodenSignMesh,
		ScrollPopupPromptText,
		ScrollPopupTitleText,
		ScrollPopupBodyText,
		ECodexPopupButtonLayout::Ok,
		OutError))
	{
		return false;
	}

	SaveAssets(
		{
			InteractAction,
			PopupCloseAction,
			MappingContext,
			InputConfig,
			FilledCircleTexture,
			OuterRingTexture,
			SmileIconTexture,
			IndicatorWidget,
			MessagePopupWidget,
			ScrollMessagePopupWidget,
			AppleBlueprint,
			StrawberryBlueprint,
			WoodenSignBlueprint,
			WoodenSignScrollBlueprint
		});

	if (!PlaceInteractionTestActorsInMap(*AppleBlueprint, *StrawberryBlueprint, *WoodenSignBlueprint, *WoodenSignScrollBlueprint, OutError))
	{
		return false;
	}

	return true;
}
