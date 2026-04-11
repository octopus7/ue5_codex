// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CodexInteractionAssetBuilder.h"

#include "CodexTopDownInputConfigDataAsset.h"
#include "Interaction/CodexDualTileTransferPopupInteractableActor.h"
#include "Interaction/CodexInteractableActor.h"
#include "Interaction/CodexInteractionAssetPaths.h"
#include "Interaction/CodexInteractionDualTileTransferPopupWidget.h"
#include "Interaction/CodexInteractionDualTileTransferTileEntryWidget.h"
#include "Interaction/CodexInteractionMessagePopupWidget.h"
#include "Interaction/CodexInteractionScrollMessagePopupWidget.h"
#include "Interaction/CodexInteractionUIPlaygroundPayload.h"
#include "Interaction/CodexInteractionComponent.h"
#include "Interaction/CodexInteractionIndicatorWidget.h"
#include "Interaction/CodexPopupInteractableActor.h"
#include "Interaction/CodexScrollMessagePopupInteractableActor.h"
#include "Interaction/CodexInteractionTypes.h"
#include "AssetToolsModule.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/BackgroundBlur.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/CheckBox.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/ListView.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ProgressBar.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/Slider.h"
#include "Components/SizeBox.h"
#include "Components/SpinBox.h"
#include "Components/TextBlock.h"
#include "Components/TileView.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Components/WrapBox.h"
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
#include "Fonts/SlateFontInfo.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateTypes.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Subsystems/EditorAssetSubsystem.h"
#include "UObject/MetaData.h"
#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"
#include "UObject/UnrealType.h"

namespace
{
	using namespace CodexInteractionAssetPaths;

	static const TCHAR* const BasicMapAssetPath = TEXT("/Game/Maps/BasicMap");
	static const TCHAR* const InteractionPlacementLabelApple = TEXT("InteractionTest_Apple");
	static const TCHAR* const InteractionPlacementLabelStrawberry = TEXT("InteractionTest_Strawberry");
	static const TCHAR* const InteractionPlacementLabelWoodenSign = TEXT("InteractionTest_WoodenSignPopup");
	static const TCHAR* const InteractionPlacementLabelWoodenSignScroll = TEXT("InteractionTest_WoodenSignScrollPopup");
	static const TCHAR* const InteractionPlacementLabelWoodenSignDualTileTransfer = TEXT("InteractionTest_WoodenSignDualTileTransferPopup");
	static const TCHAR* const InteractionPlacementLabelWoodenSignUIPlayground = TEXT("InteractionTest_WoodenSignUIPlaygroundPopup");
	static const FName InteractionPlacementTag = TEXT("CodexInteractionTestPlacement");
	static const TCHAR* const GeneratedAssetVersionKey = TEXT("CodexInteractionAssetBuilderVersion");
	static const TCHAR* const FilledCircleTextureVersion = TEXT("filled_circle_texture_v1");
	static const TCHAR* const OuterRingTextureVersion = TEXT("outer_ring_texture_v1");
	static const TCHAR* const SmileIconTextureVersion = TEXT("smile_icon_texture_v1");
	static const TCHAR* const TileRoundedGradientTextureVersion = TEXT("tile_rounded_gradient_texture_v1");
	static const TCHAR* const IndicatorWidgetVersion = TEXT("indicator_widget_v1");
	static const TCHAR* const MessagePopupWidgetVersion = TEXT("message_popup_widget_v1");
	static const TCHAR* const ScrollMessagePopupWidgetVersion = TEXT("scroll_message_popup_widget_v1");
	static const TCHAR* const DualTileTransferTileEntryWidgetVersion = TEXT("dual_tile_transfer_tile_entry_widget_v1");
	static const TCHAR* const DualTileTransferPopupWidgetVersion = TEXT("dual_tile_transfer_popup_widget_v2");
	static const TCHAR* const UIPlaygroundListEntryWidgetVersion = TEXT("ui_playground_list_entry_widget_v2");
	static const TCHAR* const UIPlaygroundTileEntryWidgetVersion = TEXT("ui_playground_tile_entry_widget_v2");
	static const TCHAR* const UIPlaygroundPopupWidgetVersion = TEXT("ui_playground_popup_widget_v2");
	static const TCHAR* const AppleInteractableBlueprintVersion = TEXT("apple_interactable_blueprint_v1");
	static const TCHAR* const StrawberryInteractableBlueprintVersion = TEXT("strawberry_interactable_blueprint_v1");
	static const TCHAR* const WoodenSignPopupBlueprintVersion = TEXT("wooden_sign_popup_blueprint_v1");
	static const TCHAR* const WoodenSignScrollPopupBlueprintVersion = TEXT("wooden_sign_scroll_popup_blueprint_v1");
	static const TCHAR* const WoodenSignDualTileTransferPopupBlueprintVersion = TEXT("wooden_sign_dual_tile_transfer_popup_blueprint_v2");
	static const TCHAR* const WoodenSignUIPlaygroundPopupBlueprintVersion = TEXT("wooden_sign_ui_playground_popup_blueprint_v2");

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

	template <typename ClassType>
	UClass* LoadNativeClass(const TCHAR* ClassName)
	{
		return LoadClass<ClassType>(nullptr, *FString::Printf(TEXT("/Script/CodexUMG.%s"), ClassName));
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
			TArray<UObject*> DirtyAssets;
			for (UObject* Asset : Assets)
			{
				if (!IsValid(Asset))
				{
					continue;
				}

				UPackage* Package = Asset->GetOutermost();
				if (Package != nullptr && Package->IsDirty())
				{
					DirtyAssets.Add(Asset);
				}
			}

			if (DirtyAssets.Num() > 0)
			{
				AssetSubsystem->SaveLoadedAssets(DirtyAssets, false);
			}
		}
	}

	bool HasGeneratedAssetVersion(const UObject& Object, const TCHAR* ExpectedVersion)
	{
		UPackage* Package = Object.GetOutermost();
		if (Package == nullptr)
		{
			return false;
		}

		FMetaData& MetaData = Package->GetMetaData();
		return MetaData.GetValue(&Object, GeneratedAssetVersionKey).Equals(ExpectedVersion, ESearchCase::CaseSensitive);
	}

	void SetGeneratedAssetVersion(UObject& Object, const TCHAR* NewVersion)
	{
		UPackage* Package = Object.GetOutermost();
		if (Package == nullptr)
		{
			return;
		}

		FMetaData& MetaData = Package->GetMetaData();
		if (MetaData.GetValue(&Object, GeneratedAssetVersionKey).Equals(NewVersion, ESearchCase::CaseSensitive))
		{
			return;
		}

		MetaData.SetValue(&Object, GeneratedAssetVersionKey, NewVersion);
		Object.MarkPackageDirty();
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

	TArray64<uint8> BuildRoundedGradientTexturePixels(const int32 Size, const float CornerRadius)
	{
		TArray64<uint8> Pixels;
		Pixels.SetNumZeroed(Size * Size * 4);

		const FVector2f HalfExtent(static_cast<float>(Size) * 0.5f - 2.0f, static_cast<float>(Size) * 0.5f - 2.0f);
		const FVector2f InnerHalfExtent = HalfExtent - FVector2f(CornerRadius, CornerRadius);
		const float Center = (static_cast<float>(Size) - 1.0f) * 0.5f;

		for (int32 Y = 0; Y < Size; ++Y)
		{
			const float VerticalAlpha = static_cast<float>(Y) / FMath::Max(1.0f, static_cast<float>(Size - 1));
			const float Brightness = FMath::Lerp(242.0f, 214.0f, VerticalAlpha);
			const float Highlight = FMath::Lerp(10.0f, 0.0f, FMath::Clamp(1.0f - VerticalAlpha * 1.8f, 0.0f, 1.0f));

			for (int32 X = 0; X < Size; ++X)
			{
				const FVector2f Point(FMath::Abs(static_cast<float>(X) - Center), FMath::Abs(static_cast<float>(Y) - Center));
				const FVector2f Delta = Point - InnerHalfExtent;
				const FVector2f ClampedDelta(FMath::Max(Delta.X, 0.0f), FMath::Max(Delta.Y, 0.0f));
				const float OutsideDistance = ClampedDelta.Size();
				const float InsideDistance = FMath::Min(FMath::Max(Delta.X, Delta.Y), 0.0f);
				const float SignedDistance = OutsideDistance + InsideDistance - CornerRadius;
				const float Alpha = FMath::Clamp(1.0f - SignedDistance, 0.0f, 1.0f);

				const int64 PixelIndex = static_cast<int64>((Y * Size) + X) * 4;
				const uint8 Gray = static_cast<uint8>(FMath::Clamp(Brightness + Highlight, 0.0f, 255.0f));
				Pixels[PixelIndex + 0] = Gray;
				Pixels[PixelIndex + 1] = Gray;
				Pixels[PixelIndex + 2] = Gray;
				Pixels[PixelIndex + 3] = static_cast<uint8>(Alpha * 255.0f);
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

	void ConfigureOverlaySlotFill(UWidget& Widget, const FMargin& Padding = FMargin(0.0f))
	{
		if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(Widget.Slot))
		{
			OverlaySlot->SetPadding(Padding);
			OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
			OverlaySlot->SetVerticalAlignment(VAlign_Fill);
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

	void SetWidgetClassProperty(UObject& Object, const TCHAR* PropertyName, UClass* WidgetClass)
	{
		if (FClassProperty* ClassProperty = FindFProperty<FClassProperty>(Object.GetClass(), PropertyName))
		{
			ClassProperty->SetPropertyValue_InContainer(&Object, WidgetClass);
			Object.MarkPackageDirty();
		}
	}

	void ConfigureButtonStyle(UButton& Button, const FLinearColor& BaseColor)
	{
		FButtonStyle Style = Button.GetStyle();
		const FMargin SharedPadding(14.0f, 8.0f);
		Style.SetNormal(MakeRoundedBrush(BaseColor, 12.0f, FLinearColor(1.0f, 1.0f, 1.0f, 0.15f), 1.0f));
		Style.SetHovered(MakeRoundedBrush(BaseColor * 1.12f, 12.0f, FLinearColor(1.0f, 1.0f, 1.0f, 0.22f), 1.0f));
		Style.SetPressed(MakeRoundedBrush(BaseColor * 0.88f, 12.0f, FLinearColor(1.0f, 1.0f, 1.0f, 0.22f), 1.0f));
		Style.SetDisabled(MakeRoundedBrush(BaseColor * 0.5f, 12.0f, FLinearColor(1.0f, 1.0f, 1.0f, 0.12f), 1.0f));
		Style.SetNormalPadding(SharedPadding);
		Style.SetPressedPadding(SharedPadding);
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

	bool ConfigureDualTileTransferTileEntryWidgetBlueprint(
		UWidgetBlueprint& WidgetBlueprint,
		UTexture2D& TileGradientTexture,
		FString& OutError)
	{
		if (WidgetBlueprint.WidgetTree == nullptr)
		{
			OutError = TEXT("Dual tile transfer tile entry widget blueprint is missing a WidgetTree.");
			return false;
		}

		UBorder* TileRoot = Cast<UBorder>(WidgetBlueprint.WidgetTree->RootWidget);
		if (TileRoot == nullptr)
		{
			TileRoot = WidgetBlueprint.WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border_TileRoot"));
			WidgetBlueprint.WidgetTree->RootWidget = TileRoot;
		}

		if (TileRoot == nullptr)
		{
			OutError = TEXT("Failed to create Border_TileRoot for WBP_InteractionDualTileTransferTileEntry.");
			return false;
		}

		EnsureWidgetGuid(WidgetBlueprint, *TileRoot);
		TileRoot->bIsVariable = false;

		UOverlay* TileContent = FindWidget<UOverlay>(WidgetBlueprint, TEXT("Overlay_TileContent"));
		if (TileContent == nullptr)
		{
			TileContent = WidgetBlueprint.WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("Overlay_TileContent"));
		}

		UImage* TileBackground = FindWidget<UImage>(WidgetBlueprint, TEXT("IMG_TileBackground"));
		if (TileBackground == nullptr)
		{
			TileBackground = WidgetBlueprint.WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("IMG_TileBackground"));
		}

		UBorder* SelectedOutline = FindWidget<UBorder>(WidgetBlueprint, TEXT("Border_SelectedOutline"));
		if (SelectedOutline == nullptr)
		{
			SelectedOutline = WidgetBlueprint.WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border_SelectedOutline"));
		}

		UTextBlock* NumberText = FindWidget<UTextBlock>(WidgetBlueprint, TEXT("TXT_Number"));
		if (NumberText == nullptr)
		{
			NumberText = WidgetBlueprint.WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TXT_Number"));
		}

		if (!TileContent || !TileBackground || !SelectedOutline || !NumberText)
		{
			OutError = TEXT("Failed to construct one or more widgets for WBP_InteractionDualTileTransferTileEntry.");
			return false;
		}

		EnsureWidgetGuid(WidgetBlueprint, *TileContent);
		EnsureWidgetGuid(WidgetBlueprint, *TileBackground);
		EnsureWidgetGuid(WidgetBlueprint, *SelectedOutline);
		EnsureWidgetGuid(WidgetBlueprint, *NumberText);

		TileContent->bIsVariable = false;
		TileBackground->bIsVariable = true;
		SelectedOutline->bIsVariable = true;
		NumberText->bIsVariable = true;

		if (TileRoot->GetContent() != TileContent)
		{
			TileRoot->SetContent(TileContent);
		}

		if (TileContent->GetChildrenCount() != 3
			|| TileContent->GetChildAt(0) != TileBackground
			|| TileContent->GetChildAt(1) != SelectedOutline
			|| TileContent->GetChildAt(2) != NumberText)
		{
			TileContent->ClearChildren();
			TileContent->AddChild(TileBackground);
			TileContent->AddChild(SelectedOutline);
			TileContent->AddChild(NumberText);
		}

		TileRoot->SetPadding(FMargin(4.0f));
		TileRoot->SetBrushColor(FLinearColor::Transparent);

		TileBackground->SetBrushFromTexture(&TileGradientTexture, false);
		{
			FSlateBrush Brush = TileBackground->GetBrush();
			Brush.ImageSize = FVector2D(76.0f, 76.0f);
			TileBackground->SetBrush(Brush);
		}
		TileBackground->SetColorAndOpacity(FLinearColor::White);

		SelectedOutline->SetPadding(FMargin(0.0f));
		SelectedOutline->SetBrush(MakeRoundedBrush(FLinearColor::Transparent, 14.0f, FLinearColor(1.0f, 1.0f, 1.0f, 0.92f), 2.0f));
		SelectedOutline->SetVisibility(ESlateVisibility::Collapsed);

		NumberText->SetText(FText::AsNumber(11));
		NumberText->SetJustification(ETextJustify::Center);
		NumberText->SetColorAndOpacity(FSlateColor(FLinearColor(0.18f, 0.20f, 0.26f, 1.0f)));
		{
			FSlateFontInfo Font = NumberText->GetFont();
			Font.Size = 22;
			NumberText->SetFont(Font);
		}

		ConfigureOverlaySlotFill(*TileBackground);
		ConfigureOverlaySlotFill(*SelectedOutline);
		ConfigureOverlaySlot(*NumberText);

		WidgetBlueprint.Modify();
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(&WidgetBlueprint);
		CompileBlueprint(&WidgetBlueprint);
		WidgetBlueprint.MarkPackageDirty();
		return true;
	}

	bool ConfigureDualTileTransferPopupWidgetBlueprint(
		UWidgetBlueprint& WidgetBlueprint,
		UClass* TileEntryWidgetClass,
		FString& OutError)
	{
		if (WidgetBlueprint.WidgetTree == nullptr)
		{
			OutError = TEXT("Dual tile transfer popup widget blueprint is missing a WidgetTree.");
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
			OutError = TEXT("Failed to create RootCanvas for WBP_InteractionDualTileTransferPopup.");
			return false;
		}

		EnsureWidgetGuid(WidgetBlueprint, *RootCanvas);
		RootCanvas->bIsVariable = false;

		UOverlay* ScreenRoot = EnsurePanelChild<UOverlay>(WidgetBlueprint, *RootCanvas, TEXT("Overlay_ScreenRoot"), false);
		USizeBox* PopupFrame = EnsurePanelChild<USizeBox>(WidgetBlueprint, *ScreenRoot, TEXT("SizeBox_PopupFrame"), false);
		if (!ScreenRoot || !PopupFrame)
		{
			OutError = TEXT("Failed to create screen root widgets for WBP_InteractionDualTileTransferPopup.");
			return false;
		}

		UBackgroundBlur* BackgroundBlurPanel = FindWidget<UBackgroundBlur>(WidgetBlueprint, TEXT("BackgroundBlur_Panel"));
		if (BackgroundBlurPanel == nullptr)
		{
			BackgroundBlurPanel = WidgetBlueprint.WidgetTree->ConstructWidget<UBackgroundBlur>(UBackgroundBlur::StaticClass(), TEXT("BackgroundBlur_Panel"));
		}

		UBorder* TintPanel = FindWidget<UBorder>(WidgetBlueprint, TEXT("Border_TintPanel"));
		if (TintPanel == nullptr)
		{
			TintPanel = WidgetBlueprint.WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border_TintPanel"));
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

		UHorizontalBox* BodyRow = FindWidget<UHorizontalBox>(WidgetBlueprint, TEXT("HorizontalBox_Body"));
		if (BodyRow == nullptr)
		{
			BodyRow = WidgetBlueprint.WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HorizontalBox_Body"));
		}

		UVerticalBox* LeftPanel = FindWidget<UVerticalBox>(WidgetBlueprint, TEXT("VerticalBox_LeftPanel"));
		if (LeftPanel == nullptr)
		{
			LeftPanel = WidgetBlueprint.WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VerticalBox_LeftPanel"));
		}

		UHorizontalBox* LeftHeader = FindWidget<UHorizontalBox>(WidgetBlueprint, TEXT("HorizontalBox_LeftHeader"));
		if (LeftHeader == nullptr)
		{
			LeftHeader = WidgetBlueprint.WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HorizontalBox_LeftHeader"));
		}

		if (UTextBlock* LeftTitle = FindWidget<UTextBlock>(WidgetBlueprint, TEXT("TXT_LeftTitle")))
		{
			LeftTitle->RemoveFromParent();
			WidgetBlueprint.WidgetTree->RemoveWidget(LeftTitle);
			WidgetBlueprint.OnVariableRemoved(LeftTitle->GetFName());
		}

		UButton* LeftAddButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_LeftAdd"));
		if (LeftAddButton == nullptr)
		{
			LeftAddButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_LeftAdd"));
		}

		UButton* LeftRemoveButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_LeftRemove"));
		if (LeftRemoveButton == nullptr)
		{
			LeftRemoveButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_LeftRemove"));
		}

		UTileView* LeftTileView = FindWidget<UTileView>(WidgetBlueprint, TEXT("TileView_Left"));
		if (LeftTileView == nullptr)
		{
			LeftTileView = WidgetBlueprint.WidgetTree->ConstructWidget<UTileView>(UTileView::StaticClass(), TEXT("TileView_Left"));
		}

		UVerticalBox* RightPanel = FindWidget<UVerticalBox>(WidgetBlueprint, TEXT("VerticalBox_RightPanel"));
		if (RightPanel == nullptr)
		{
			RightPanel = WidgetBlueprint.WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VerticalBox_RightPanel"));
		}

		UHorizontalBox* RightHeader = FindWidget<UHorizontalBox>(WidgetBlueprint, TEXT("HorizontalBox_RightHeader"));
		if (RightHeader == nullptr)
		{
			RightHeader = WidgetBlueprint.WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HorizontalBox_RightHeader"));
		}

		if (UTextBlock* RightTitle = FindWidget<UTextBlock>(WidgetBlueprint, TEXT("TXT_RightTitle")))
		{
			RightTitle->RemoveFromParent();
			WidgetBlueprint.WidgetTree->RemoveWidget(RightTitle);
			WidgetBlueprint.OnVariableRemoved(RightTitle->GetFName());
		}

		UButton* RightAddButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_RightAdd"));
		if (RightAddButton == nullptr)
		{
			RightAddButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_RightAdd"));
		}

		UButton* RightRemoveButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_RightRemove"));
		if (RightRemoveButton == nullptr)
		{
			RightRemoveButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_RightRemove"));
		}

		UTileView* RightTileView = FindWidget<UTileView>(WidgetBlueprint, TEXT("TileView_Right"));
		if (RightTileView == nullptr)
		{
			RightTileView = WidgetBlueprint.WidgetTree->ConstructWidget<UTileView>(UTileView::StaticClass(), TEXT("TileView_Right"));
		}

		if (!BackgroundBlurPanel || !TintPanel || !ContentColumn || !TitleBar || !TitleText || !CloseButton || !BodyRow
			|| !LeftPanel || !LeftHeader || !LeftAddButton || !LeftRemoveButton || !LeftTileView
			|| !RightPanel || !RightHeader || !RightAddButton || !RightRemoveButton || !RightTileView)
		{
			OutError = TEXT("Failed to construct one or more popup widgets for WBP_InteractionDualTileTransferPopup.");
			return false;
		}

		EnsureWidgetGuid(WidgetBlueprint, *BackgroundBlurPanel);
		EnsureWidgetGuid(WidgetBlueprint, *TintPanel);
		EnsureWidgetGuid(WidgetBlueprint, *ContentColumn);
		EnsureWidgetGuid(WidgetBlueprint, *TitleBar);
		EnsureWidgetGuid(WidgetBlueprint, *TitleText);
		EnsureWidgetGuid(WidgetBlueprint, *CloseButton);
		EnsureWidgetGuid(WidgetBlueprint, *BodyRow);
		EnsureWidgetGuid(WidgetBlueprint, *LeftPanel);
		EnsureWidgetGuid(WidgetBlueprint, *LeftHeader);
		EnsureWidgetGuid(WidgetBlueprint, *LeftAddButton);
		EnsureWidgetGuid(WidgetBlueprint, *LeftRemoveButton);
		EnsureWidgetGuid(WidgetBlueprint, *LeftTileView);
		EnsureWidgetGuid(WidgetBlueprint, *RightPanel);
		EnsureWidgetGuid(WidgetBlueprint, *RightHeader);
		EnsureWidgetGuid(WidgetBlueprint, *RightAddButton);
		EnsureWidgetGuid(WidgetBlueprint, *RightRemoveButton);
		EnsureWidgetGuid(WidgetBlueprint, *RightTileView);

		BackgroundBlurPanel->bIsVariable = false;
		TintPanel->bIsVariable = false;
		ContentColumn->bIsVariable = false;
		TitleBar->bIsVariable = false;
		TitleText->bIsVariable = true;
		CloseButton->bIsVariable = true;
		BodyRow->bIsVariable = false;
		LeftPanel->bIsVariable = false;
		LeftHeader->bIsVariable = false;
		LeftAddButton->bIsVariable = true;
		LeftRemoveButton->bIsVariable = true;
		LeftTileView->bIsVariable = true;
		RightPanel->bIsVariable = false;
		RightHeader->bIsVariable = false;
		RightAddButton->bIsVariable = true;
		RightRemoveButton->bIsVariable = true;
		RightTileView->bIsVariable = true;

		if (PopupFrame->GetContent() != BackgroundBlurPanel)
		{
			PopupFrame->SetContent(BackgroundBlurPanel);
		}

		if (BackgroundBlurPanel->GetContent() != TintPanel)
		{
			BackgroundBlurPanel->SetContent(TintPanel);
		}

		if (TintPanel->GetContent() != ContentColumn)
		{
			TintPanel->SetContent(ContentColumn);
		}

		if (TitleBar->GetChildrenCount() != 2 || TitleBar->GetChildAt(0) != TitleText || TitleBar->GetChildAt(1) != CloseButton)
		{
			TitleBar->ClearChildren();
			TitleBar->AddChildToHorizontalBox(TitleText);
			TitleBar->AddChildToHorizontalBox(CloseButton);
		}

		if (LeftHeader->GetChildrenCount() != 2
			|| LeftHeader->GetChildAt(0) != LeftAddButton
			|| LeftHeader->GetChildAt(1) != LeftRemoveButton)
		{
			LeftHeader->ClearChildren();
			LeftHeader->AddChildToHorizontalBox(LeftAddButton);
			LeftHeader->AddChildToHorizontalBox(LeftRemoveButton);
		}

		if (LeftPanel->GetChildrenCount() != 2 || LeftPanel->GetChildAt(0) != LeftHeader || LeftPanel->GetChildAt(1) != LeftTileView)
		{
			LeftPanel->ClearChildren();
			LeftPanel->AddChildToVerticalBox(LeftHeader);
			LeftPanel->AddChildToVerticalBox(LeftTileView);
		}

		if (RightHeader->GetChildrenCount() != 2
			|| RightHeader->GetChildAt(0) != RightAddButton
			|| RightHeader->GetChildAt(1) != RightRemoveButton)
		{
			RightHeader->ClearChildren();
			RightHeader->AddChildToHorizontalBox(RightAddButton);
			RightHeader->AddChildToHorizontalBox(RightRemoveButton);
		}

		if (RightPanel->GetChildrenCount() != 2 || RightPanel->GetChildAt(0) != RightHeader || RightPanel->GetChildAt(1) != RightTileView)
		{
			RightPanel->ClearChildren();
			RightPanel->AddChildToVerticalBox(RightHeader);
			RightPanel->AddChildToVerticalBox(RightTileView);
		}

		if (BodyRow->GetChildrenCount() != 2 || BodyRow->GetChildAt(0) != LeftPanel || BodyRow->GetChildAt(1) != RightPanel)
		{
			BodyRow->ClearChildren();
			BodyRow->AddChildToHorizontalBox(LeftPanel);
			BodyRow->AddChildToHorizontalBox(RightPanel);
		}

		if (ContentColumn->GetChildrenCount() != 2 || ContentColumn->GetChildAt(0) != TitleBar || ContentColumn->GetChildAt(1) != BodyRow)
		{
			ContentColumn->ClearChildren();
			ContentColumn->AddChildToVerticalBox(TitleBar);
			ContentColumn->AddChildToVerticalBox(BodyRow);
		}

		ConfigureCanvasAnchors(*ScreenRoot, FAnchors(0.0f, 0.0f, 1.0f, 1.0f), FMargin(0.0f), FVector2D::ZeroVector);
		ConfigureOverlaySlot(*PopupFrame);

		PopupFrame->SetWidthOverride(960.0f);
		PopupFrame->SetHeightOverride(920.0f);

		BackgroundBlurPanel->SetBlurStrength(18.0f);
		BackgroundBlurPanel->SetOverrideAutoRadiusCalculation(true);
		BackgroundBlurPanel->SetBlurRadius(12);
		BackgroundBlurPanel->SetApplyAlphaToBlur(true);
		BackgroundBlurPanel->SetCornerRadius(FVector4(20.0f, 20.0f, 20.0f, 20.0f));

		TintPanel->SetPadding(FMargin(24.0f, 22.0f));
		TintPanel->SetBrush(MakeRoundedBrush(FLinearColor(0.98f, 0.80f, 0.44f, 0.15f), 20.0f, FLinearColor(1.0f, 1.0f, 1.0f, 0.08f), 1.0f));

		TitleText->SetText(FText::FromString(TEXT("Transfer Numbers")));
		TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		{
			FSlateFontInfo Font = TitleText->GetFont();
			Font.Size = 20;
			TitleText->SetFont(Font);
		}

		CloseButton->SetToolTipText(FText::FromString(TEXT("Close")));
		ConfigureButtonStyle(*CloseButton, FLinearColor(0.68f, 0.36f, 0.22f, 0.96f));
		EnsureButtonLabel(WidgetBlueprint, *CloseButton, TEXT("TXT_CloseLabel"), FText::FromString(TEXT("Close")));

		ConfigureButtonStyle(*LeftAddButton, FLinearColor(0.39f, 0.66f, 0.74f, 0.96f));
		ConfigureButtonStyle(*LeftRemoveButton, FLinearColor(0.27f, 0.38f, 0.55f, 0.96f));
		ConfigureButtonStyle(*RightAddButton, FLinearColor(0.42f, 0.68f, 0.58f, 0.96f));
		ConfigureButtonStyle(*RightRemoveButton, FLinearColor(0.33f, 0.43f, 0.58f, 0.96f));
		EnsureButtonLabel(WidgetBlueprint, *LeftAddButton, TEXT("TXT_LeftAddLabel"), FText::FromString(TEXT("+ Add")));
		EnsureButtonLabel(WidgetBlueprint, *LeftRemoveButton, TEXT("TXT_LeftRemoveLabel"), FText::FromString(TEXT("- Remove")));
		EnsureButtonLabel(WidgetBlueprint, *RightAddButton, TEXT("TXT_RightAddLabel"), FText::FromString(TEXT("+ Add")));
		EnsureButtonLabel(WidgetBlueprint, *RightRemoveButton, TEXT("TXT_RightRemoveLabel"), FText::FromString(TEXT("- Remove")));

		LeftTileView->SetEntryWidth(76.0f);
		LeftTileView->SetEntryHeight(76.0f);
		LeftTileView->SetHorizontalEntrySpacing(8.0f);
		LeftTileView->SetVerticalEntrySpacing(8.0f);
		LeftTileView->SetSelectionMode(ESelectionMode::Single);
		SetWidgetClassProperty(*LeftTileView, TEXT("EntryWidgetClass"), TileEntryWidgetClass);

		RightTileView->SetEntryWidth(76.0f);
		RightTileView->SetEntryHeight(76.0f);
		RightTileView->SetHorizontalEntrySpacing(8.0f);
		RightTileView->SetVerticalEntrySpacing(8.0f);
		RightTileView->SetSelectionMode(ESelectionMode::Single);
		SetWidgetClassProperty(*RightTileView, TEXT("EntryWidgetClass"), TileEntryWidgetClass);

		ConfigureVerticalBoxSlot(*TitleBar, FMargin(0.0f, 0.0f, 0.0f, 18.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*BodyRow, FMargin(0.0f), ESlateSizeRule::Fill, HAlign_Fill, VAlign_Fill);

		ConfigureHorizontalBoxSlot(*TitleText, FMargin(0.0f, 0.0f, 12.0f, 0.0f), ESlateSizeRule::Fill, HAlign_Left, VAlign_Center);
		ConfigureHorizontalBoxSlot(*CloseButton, FMargin(0.0f), ESlateSizeRule::Automatic, HAlign_Right, VAlign_Center);
		ConfigureHorizontalBoxSlot(*LeftPanel, FMargin(0.0f, 0.0f, 10.0f, 0.0f), ESlateSizeRule::Fill, HAlign_Fill, VAlign_Fill);
		ConfigureHorizontalBoxSlot(*RightPanel, FMargin(10.0f, 0.0f, 0.0f, 0.0f), ESlateSizeRule::Fill, HAlign_Fill, VAlign_Fill);

		ConfigureVerticalBoxSlot(*LeftHeader, FMargin(0.0f, 0.0f, 0.0f, 10.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*LeftTileView, FMargin(0.0f), ESlateSizeRule::Fill, HAlign_Fill, VAlign_Fill);
		ConfigureVerticalBoxSlot(*RightHeader, FMargin(0.0f, 0.0f, 0.0f, 10.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*RightTileView, FMargin(0.0f), ESlateSizeRule::Fill, HAlign_Fill, VAlign_Fill);

		ConfigureHorizontalBoxSlot(*LeftAddButton, FMargin(0.0f, 0.0f, 8.0f, 0.0f), ESlateSizeRule::Automatic, HAlign_Right, VAlign_Center);
		ConfigureHorizontalBoxSlot(*LeftRemoveButton, FMargin(0.0f), ESlateSizeRule::Automatic, HAlign_Right, VAlign_Center);
		ConfigureHorizontalBoxSlot(*RightAddButton, FMargin(0.0f, 0.0f, 8.0f, 0.0f), ESlateSizeRule::Automatic, HAlign_Right, VAlign_Center);
		ConfigureHorizontalBoxSlot(*RightRemoveButton, FMargin(0.0f), ESlateSizeRule::Automatic, HAlign_Right, VAlign_Center);

		WidgetBlueprint.Modify();
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(&WidgetBlueprint);
		CompileBlueprint(&WidgetBlueprint);
		WidgetBlueprint.MarkPackageDirty();
		return true;
	}

	bool ConfigureUIPlaygroundListEntryWidgetBlueprint(UWidgetBlueprint& WidgetBlueprint, FString& OutError)
	{
		if (WidgetBlueprint.WidgetTree == nullptr)
		{
			OutError = TEXT("UI playground list entry widget blueprint is missing a WidgetTree.");
			return false;
		}

		UBorder* RootBorder = Cast<UBorder>(WidgetBlueprint.WidgetTree->RootWidget);
		if (RootBorder == nullptr)
		{
			RootBorder = WidgetBlueprint.WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border_Root"));
			WidgetBlueprint.WidgetTree->RootWidget = RootBorder;
		}

		if (RootBorder == nullptr)
		{
			OutError = TEXT("Failed to create Border_Root for WBP_InteractionUIPlaygroundListEntry.");
			return false;
		}

		EnsureWidgetGuid(WidgetBlueprint, *RootBorder);
		RootBorder->bIsVariable = true;

		UOverlay* ListContent = FindWidget<UOverlay>(WidgetBlueprint, TEXT("Overlay_ListContent"));
		if (ListContent == nullptr)
		{
			ListContent = WidgetBlueprint.WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("Overlay_ListContent"));
		}

		UBorder* SelectedOutline = FindWidget<UBorder>(WidgetBlueprint, TEXT("Border_Selection"));
		if (SelectedOutline == nullptr)
		{
			SelectedOutline = WidgetBlueprint.WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border_Selection"));
		}

		UVerticalBox* ContentColumn = FindWidget<UVerticalBox>(WidgetBlueprint, TEXT("VerticalBox_Content"));
		if (ContentColumn == nullptr)
		{
			ContentColumn = WidgetBlueprint.WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VerticalBox_Content"));
		}

		UTextBlock* LabelText = FindWidget<UTextBlock>(WidgetBlueprint, TEXT("TXT_Label"));
		if (LabelText == nullptr)
		{
			LabelText = WidgetBlueprint.WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TXT_Label"));
		}

		UTextBlock* DescriptionText = FindWidget<UTextBlock>(WidgetBlueprint, TEXT("TXT_Description"));
		if (DescriptionText == nullptr)
		{
			DescriptionText = WidgetBlueprint.WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TXT_Description"));
		}

		if (!ListContent || !SelectedOutline || !ContentColumn || !LabelText || !DescriptionText)
		{
			OutError = TEXT("Failed to construct one or more widgets for WBP_InteractionUIPlaygroundListEntry.");
			return false;
		}

		EnsureWidgetGuid(WidgetBlueprint, *ListContent);
		EnsureWidgetGuid(WidgetBlueprint, *SelectedOutline);
		EnsureWidgetGuid(WidgetBlueprint, *ContentColumn);
		EnsureWidgetGuid(WidgetBlueprint, *LabelText);
		EnsureWidgetGuid(WidgetBlueprint, *DescriptionText);

		ListContent->bIsVariable = true;
		SelectedOutline->bIsVariable = true;
		ContentColumn->bIsVariable = true;
		LabelText->bIsVariable = true;
		DescriptionText->bIsVariable = true;

		if (RootBorder->GetContent() != ListContent)
		{
			RootBorder->SetContent(ListContent);
		}

		if (ListContent->GetChildrenCount() != 2 || ListContent->GetChildAt(0) != SelectedOutline || ListContent->GetChildAt(1) != ContentColumn)
		{
			ListContent->ClearChildren();
			ListContent->AddChild(SelectedOutline);
			ListContent->AddChild(ContentColumn);
		}

		if (ContentColumn->GetChildrenCount() != 2
			|| ContentColumn->GetChildAt(0) != LabelText
			|| ContentColumn->GetChildAt(1) != DescriptionText)
		{
			ContentColumn->ClearChildren();
			ContentColumn->AddChildToVerticalBox(LabelText);
			ContentColumn->AddChildToVerticalBox(DescriptionText);
		}

		ConfigureOverlaySlotFill(*SelectedOutline);
		ConfigureOverlaySlotFill(*ContentColumn, FMargin(12.0f, 10.0f));
		ConfigureVerticalBoxSlot(*LabelText, FMargin(0.0f, 0.0f, 0.0f, 4.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*DescriptionText, FMargin(0.0f, 0.0f, 0.0f, 4.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);

		RootBorder->SetPadding(FMargin(4.0f));
		RootBorder->SetBrush(MakeRoundedBrush(FLinearColor(0.16f, 0.22f, 0.28f, 0.92f), 12.0f, FLinearColor(1.0f, 1.0f, 1.0f, 0.10f), 1.0f));

		SelectedOutline->SetPadding(FMargin(0.0f));
		SelectedOutline->SetBrush(MakeRoundedBrush(FLinearColor::Transparent, 12.0f, FLinearColor(0.86f, 0.95f, 1.0f, 0.90f), 2.0f));
		SelectedOutline->SetVisibility(ESlateVisibility::Collapsed);

		LabelText->SetText(FText::FromString(TEXT("Label")));
		LabelText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		{
			FSlateFontInfo Font = LabelText->GetFont();
			Font.Size = 18;
			LabelText->SetFont(Font);
		}

		DescriptionText->SetText(FText::FromString(TEXT("Description")));
		DescriptionText->SetAutoWrapText(true);
		DescriptionText->SetJustification(ETextJustify::Left);
		DescriptionText->SetColorAndOpacity(FSlateColor(FLinearColor(0.88f, 0.91f, 0.95f, 1.0f)));
		{
			FSlateFontInfo Font = DescriptionText->GetFont();
			Font.Size = 13;
			DescriptionText->SetFont(Font);
		}

		WidgetBlueprint.Modify();
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(&WidgetBlueprint);
		CompileBlueprint(&WidgetBlueprint);
		WidgetBlueprint.MarkPackageDirty();
		return true;
	}

	bool ConfigureUIPlaygroundTileEntryWidgetBlueprint(UWidgetBlueprint& WidgetBlueprint, UTexture2D& TileGradientTexture, FString& OutError)
	{
		if (WidgetBlueprint.WidgetTree == nullptr)
		{
			OutError = TEXT("UI playground tile entry widget blueprint is missing a WidgetTree.");
			return false;
		}

		UBorder* TileRoot = Cast<UBorder>(WidgetBlueprint.WidgetTree->RootWidget);
		if (TileRoot == nullptr)
		{
			TileRoot = WidgetBlueprint.WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border_TileRoot"));
			WidgetBlueprint.WidgetTree->RootWidget = TileRoot;
		}

		if (TileRoot == nullptr)
		{
			OutError = TEXT("Failed to create Border_TileRoot for WBP_InteractionUIPlaygroundTileEntry.");
			return false;
		}

		EnsureWidgetGuid(WidgetBlueprint, *TileRoot);
		TileRoot->bIsVariable = true;

		UOverlay* TileContent = FindWidget<UOverlay>(WidgetBlueprint, TEXT("Overlay_TileContent"));
		if (TileContent == nullptr)
		{
			TileContent = WidgetBlueprint.WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("Overlay_TileContent"));
		}

		UImage* TileBackground = FindWidget<UImage>(WidgetBlueprint, TEXT("IMG_TileBackground"));
		if (TileBackground == nullptr)
		{
			TileBackground = WidgetBlueprint.WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("IMG_TileBackground"));
		}

		UBorder* SelectedOutline = FindWidget<UBorder>(WidgetBlueprint, TEXT("Border_Outline"));
		if (SelectedOutline == nullptr)
		{
			SelectedOutline = WidgetBlueprint.WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border_Outline"));
		}

		UTextBlock* LabelText = FindWidget<UTextBlock>(WidgetBlueprint, TEXT("TXT_Label"));
		if (LabelText == nullptr)
		{
			LabelText = WidgetBlueprint.WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TXT_Label"));
		}

		if (!TileContent || !TileBackground || !SelectedOutline || !LabelText)
		{
			OutError = TEXT("Failed to construct one or more widgets for WBP_InteractionUIPlaygroundTileEntry.");
			return false;
		}

		EnsureWidgetGuid(WidgetBlueprint, *TileContent);
		EnsureWidgetGuid(WidgetBlueprint, *TileBackground);
		EnsureWidgetGuid(WidgetBlueprint, *SelectedOutline);
		EnsureWidgetGuid(WidgetBlueprint, *LabelText);

		TileContent->bIsVariable = true;
		TileBackground->bIsVariable = true;
		SelectedOutline->bIsVariable = true;
		LabelText->bIsVariable = true;

		if (TileRoot->GetContent() != TileContent)
		{
			TileRoot->SetContent(TileContent);
		}

		if (TileContent->GetChildrenCount() != 3
			|| TileContent->GetChildAt(0) != TileBackground
			|| TileContent->GetChildAt(1) != SelectedOutline
			|| TileContent->GetChildAt(2) != LabelText)
		{
			TileContent->ClearChildren();
			TileContent->AddChild(TileBackground);
			TileContent->AddChild(SelectedOutline);
			TileContent->AddChild(LabelText);
		}

		TileRoot->SetPadding(FMargin(4.0f));
		TileRoot->SetBrush(MakeRoundedBrush(FLinearColor::Transparent, 14.0f, FLinearColor(1.0f, 1.0f, 1.0f, 0.10f), 1.0f));

		TileBackground->SetBrushFromTexture(&TileGradientTexture, false);
		{
			FSlateBrush Brush = TileBackground->GetBrush();
			Brush.ImageSize = FVector2D(76.0f, 76.0f);
			TileBackground->SetBrush(Brush);
		}
		TileBackground->SetColorAndOpacity(FLinearColor::White);

		SelectedOutline->SetPadding(FMargin(0.0f));
		SelectedOutline->SetBrush(MakeRoundedBrush(FLinearColor::Transparent, 14.0f, FLinearColor(1.0f, 1.0f, 1.0f, 0.92f), 2.0f));
		SelectedOutline->SetVisibility(ESlateVisibility::Collapsed);

		LabelText->SetText(FText::FromString(TEXT("11")));
		LabelText->SetJustification(ETextJustify::Center);
		LabelText->SetColorAndOpacity(FSlateColor(FLinearColor(0.18f, 0.20f, 0.26f, 1.0f)));
		{
			FSlateFontInfo Font = LabelText->GetFont();
			Font.Size = 22;
			LabelText->SetFont(Font);
		}

		ConfigureOverlaySlotFill(*TileBackground);
		ConfigureOverlaySlotFill(*SelectedOutline);
		ConfigureOverlaySlot(*LabelText);

		WidgetBlueprint.Modify();
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(&WidgetBlueprint);
		CompileBlueprint(&WidgetBlueprint);
		WidgetBlueprint.MarkPackageDirty();
		return true;
	}

	bool ConfigureUIPlaygroundBasicPageWidget(UWidgetBlueprint& WidgetBlueprint, UTexture2D& FilledCircleTexture, UTexture2D& OuterRingTexture, UTexture2D& SmileIconTexture, FString& OutError)
	{
		UVerticalBox* BasicContent = FindWidget<UVerticalBox>(WidgetBlueprint, TEXT("VerticalBox_PageBasicContent"));
		UTextBlock* BasicHeadline = FindWidget<UTextBlock>(WidgetBlueprint, TEXT("TXT_BasicHeadline"));
		if (BasicHeadline == nullptr)
		{
			BasicHeadline = WidgetBlueprint.WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TXT_BasicHeadline"));
		}

		UTextBlock* BasicBody = FindWidget<UTextBlock>(WidgetBlueprint, TEXT("TXT_BasicDescription"));
		if (BasicBody == nullptr)
		{
			BasicBody = WidgetBlueprint.WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TXT_BasicDescription"));
		}

		UHorizontalBox* BasicImages = FindWidget<UHorizontalBox>(WidgetBlueprint, TEXT("HorizontalBox_ImagePreview"));
		if (BasicImages == nullptr)
		{
			BasicImages = WidgetBlueprint.WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HorizontalBox_ImagePreview"));
		}

		UImage* BasicFilledCircle = FindWidget<UImage>(WidgetBlueprint, TEXT("IMG_FilledCircle"));
		if (BasicFilledCircle == nullptr)
		{
			BasicFilledCircle = WidgetBlueprint.WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("IMG_FilledCircle"));
		}

		UImage* BasicOuterRing = FindWidget<UImage>(WidgetBlueprint, TEXT("IMG_OuterRing"));
		if (BasicOuterRing == nullptr)
		{
			BasicOuterRing = WidgetBlueprint.WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("IMG_OuterRing"));
		}

		UImage* BasicSmileIcon = FindWidget<UImage>(WidgetBlueprint, TEXT("IMG_SmileIcon"));
		if (BasicSmileIcon == nullptr)
		{
			BasicSmileIcon = WidgetBlueprint.WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("IMG_SmileIcon"));
		}

		UWrapBox* BasicButtons = FindWidget<UWrapBox>(WidgetBlueprint, TEXT("WrapBox_ButtonSamples"));
		if (BasicButtons == nullptr)
		{
			BasicButtons = WidgetBlueprint.WidgetTree->ConstructWidget<UWrapBox>(UWrapBox::StaticClass(), TEXT("WrapBox_ButtonSamples"));
		}

		UButton* BasicPrimaryButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_Primary"));
		if (BasicPrimaryButton == nullptr)
		{
			BasicPrimaryButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_Primary"));
		}

		UButton* BasicSecondaryButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_Secondary"));
		if (BasicSecondaryButton == nullptr)
		{
			BasicSecondaryButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_Secondary"));
		}

		UButton* BasicDisabledButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_DisabledSample"));
		if (BasicDisabledButton == nullptr)
		{
			BasicDisabledButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_DisabledSample"));
		}

		UButton* BasicPingButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_StatusPing"));
		if (BasicPingButton == nullptr)
		{
			BasicPingButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_StatusPing"));
		}

		UProgressBar* BasicProgressBar = FindWidget<UProgressBar>(WidgetBlueprint, TEXT("ProgressBar_Sample"));
		if (BasicProgressBar == nullptr)
		{
			BasicProgressBar = WidgetBlueprint.WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("ProgressBar_Sample"));
		}

		if (!BasicContent || !BasicHeadline || !BasicBody || !BasicImages || !BasicFilledCircle || !BasicOuterRing || !BasicSmileIcon
			|| !BasicButtons || !BasicPrimaryButton || !BasicSecondaryButton || !BasicDisabledButton || !BasicPingButton || !BasicProgressBar)
		{
			OutError = TEXT("Failed to construct basic page widgets for WBP_InteractionUIPlaygroundPopup.");
			return false;
		}

		EnsureWidgetGuid(WidgetBlueprint, *BasicContent);
		EnsureWidgetGuid(WidgetBlueprint, *BasicHeadline);
		EnsureWidgetGuid(WidgetBlueprint, *BasicBody);
		EnsureWidgetGuid(WidgetBlueprint, *BasicImages);
		EnsureWidgetGuid(WidgetBlueprint, *BasicFilledCircle);
		EnsureWidgetGuid(WidgetBlueprint, *BasicOuterRing);
		EnsureWidgetGuid(WidgetBlueprint, *BasicSmileIcon);
		EnsureWidgetGuid(WidgetBlueprint, *BasicButtons);
		EnsureWidgetGuid(WidgetBlueprint, *BasicPrimaryButton);
		EnsureWidgetGuid(WidgetBlueprint, *BasicSecondaryButton);
		EnsureWidgetGuid(WidgetBlueprint, *BasicDisabledButton);
		EnsureWidgetGuid(WidgetBlueprint, *BasicPingButton);
		EnsureWidgetGuid(WidgetBlueprint, *BasicProgressBar);

		BasicContent->bIsVariable = true;
		BasicHeadline->bIsVariable = true;
		BasicBody->bIsVariable = true;
		BasicImages->bIsVariable = true;
		BasicFilledCircle->bIsVariable = true;
		BasicOuterRing->bIsVariable = true;
		BasicSmileIcon->bIsVariable = true;
		BasicButtons->bIsVariable = true;
		BasicPrimaryButton->bIsVariable = true;
		BasicSecondaryButton->bIsVariable = true;
		BasicDisabledButton->bIsVariable = true;
		BasicPingButton->bIsVariable = true;
		BasicProgressBar->bIsVariable = true;

		BasicContent->ClearChildren();
		BasicContent->AddChildToVerticalBox(BasicHeadline);
		BasicContent->AddChildToVerticalBox(BasicBody);
		BasicContent->AddChildToVerticalBox(BasicImages);
		BasicContent->AddChildToVerticalBox(BasicButtons);
		BasicContent->AddChildToVerticalBox(BasicProgressBar);

		BasicImages->ClearChildren();
		BasicImages->AddChildToHorizontalBox(BasicFilledCircle);
		BasicImages->AddChildToHorizontalBox(BasicOuterRing);
		BasicImages->AddChildToHorizontalBox(BasicSmileIcon);

		BasicButtons->ClearChildren();
		BasicButtons->AddChildToWrapBox(BasicPrimaryButton);
		BasicButtons->AddChildToWrapBox(BasicSecondaryButton);
		BasicButtons->AddChildToWrapBox(BasicDisabledButton);
		BasicButtons->AddChildToWrapBox(BasicPingButton);

		BasicHeadline->SetText(FText::FromString(TEXT("Basic Controls")));
		BasicBody->SetText(FText::FromString(TEXT("Start here: click Primary, Secondary, or Ping. The disabled button should stay inert while the status bar and progress bar react.")));
		BasicBody->SetAutoWrapText(true);
		BasicFilledCircle->SetBrushFromTexture(&FilledCircleTexture, false);
		BasicOuterRing->SetBrushFromTexture(&OuterRingTexture, false);
		BasicSmileIcon->SetBrushFromTexture(&SmileIconTexture, false);
		BasicProgressBar->SetPercent(0.42f);

		ConfigureButtonStyle(*BasicPrimaryButton, FLinearColor(0.30f, 0.56f, 0.78f, 0.96f));
		ConfigureButtonStyle(*BasicSecondaryButton, FLinearColor(0.24f, 0.38f, 0.56f, 0.96f));
		ConfigureButtonStyle(*BasicDisabledButton, FLinearColor(0.18f, 0.23f, 0.31f, 0.96f));
		ConfigureButtonStyle(*BasicPingButton, FLinearColor(0.36f, 0.68f, 0.58f, 0.96f));
		BasicDisabledButton->SetIsEnabled(false);
		EnsureButtonLabel(WidgetBlueprint, *BasicPrimaryButton, TEXT("TXT_PrimaryLabel"), FText::FromString(TEXT("Primary")));
		EnsureButtonLabel(WidgetBlueprint, *BasicSecondaryButton, TEXT("TXT_SecondaryLabel"), FText::FromString(TEXT("Secondary")));
		EnsureButtonLabel(WidgetBlueprint, *BasicDisabledButton, TEXT("TXT_DisabledSampleLabel"), FText::FromString(TEXT("Disabled")));
		EnsureButtonLabel(WidgetBlueprint, *BasicPingButton, TEXT("TXT_StatusPingLabel"), FText::FromString(TEXT("Ping")));

		ConfigureVerticalBoxSlot(*BasicHeadline, FMargin(0.0f, 0.0f, 0.0f, 10.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*BasicBody, FMargin(0.0f, 0.0f, 0.0f, 18.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*BasicImages, FMargin(0.0f, 0.0f, 0.0f, 22.0f), ESlateSizeRule::Automatic, HAlign_Left, VAlign_Center);
		ConfigureVerticalBoxSlot(*BasicButtons, FMargin(0.0f, 0.0f, 0.0f, 18.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*BasicProgressBar, FMargin(0.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureHorizontalBoxSlot(*BasicFilledCircle, FMargin(0.0f, 0.0f, 14.0f, 0.0f), ESlateSizeRule::Automatic, HAlign_Left, VAlign_Center);
		ConfigureHorizontalBoxSlot(*BasicOuterRing, FMargin(0.0f, 0.0f, 14.0f, 0.0f), ESlateSizeRule::Automatic, HAlign_Left, VAlign_Center);
		ConfigureHorizontalBoxSlot(*BasicSmileIcon, FMargin(0.0f), ESlateSizeRule::Automatic, HAlign_Left, VAlign_Center);

		return true;
	}

	bool ConfigureUIPlaygroundInputPageWidget(UWidgetBlueprint& WidgetBlueprint, FString& OutError)
	{
		UVerticalBox* InputContent = FindWidget<UVerticalBox>(WidgetBlueprint, TEXT("VerticalBox_PageInputContent"));
		UTextBlock* InputSummary = FindWidget<UTextBlock>(WidgetBlueprint, TEXT("TXT_InputSummary"));
		if (InputSummary == nullptr)
		{
			InputSummary = WidgetBlueprint.WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TXT_InputSummary"));
		}

		UEditableTextBox* NameTextBox = FindWidget<UEditableTextBox>(WidgetBlueprint, TEXT("ETB_InputText"));
		if (NameTextBox == nullptr)
		{
			NameTextBox = WidgetBlueprint.WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass(), TEXT("ETB_InputText"));
		}

		UMultiLineEditableTextBox* NotesTextBox = FindWidget<UMultiLineEditableTextBox>(WidgetBlueprint, TEXT("MultiLineEditableTextBox_Notes"));
		if (NotesTextBox == nullptr)
		{
			NotesTextBox = WidgetBlueprint.WidgetTree->ConstructWidget<UMultiLineEditableTextBox>(UMultiLineEditableTextBox::StaticClass(), TEXT("MultiLineEditableTextBox_Notes"));
		}

		UCheckBox* EnableOptionCheckBox = FindWidget<UCheckBox>(WidgetBlueprint, TEXT("CHK_Toggle"));
		if (EnableOptionCheckBox == nullptr)
		{
			EnableOptionCheckBox = WidgetBlueprint.WidgetTree->ConstructWidget<UCheckBox>(UCheckBox::StaticClass(), TEXT("CHK_Toggle"));
		}

		USlider* ValueSlider = FindWidget<USlider>(WidgetBlueprint, TEXT("SLD_Value"));
		if (ValueSlider == nullptr)
		{
			ValueSlider = WidgetBlueprint.WidgetTree->ConstructWidget<USlider>(USlider::StaticClass(), TEXT("SLD_Value"));
		}

		USpinBox* CountSpinBox = FindWidget<USpinBox>(WidgetBlueprint, TEXT("SPN_Value"));
		if (CountSpinBox == nullptr)
		{
			CountSpinBox = WidgetBlueprint.WidgetTree->ConstructWidget<USpinBox>(USpinBox::StaticClass(), TEXT("SPN_Value"));
		}

		UComboBoxString* ModeComboBox = FindWidget<UComboBoxString>(WidgetBlueprint, TEXT("CB_Preset"));
		if (ModeComboBox == nullptr)
		{
			ModeComboBox = WidgetBlueprint.WidgetTree->ConstructWidget<UComboBoxString>(UComboBoxString::StaticClass(), TEXT("CB_Preset"));
		}

		if (!InputContent || !InputSummary || !NameTextBox || !NotesTextBox || !EnableOptionCheckBox || !ValueSlider || !CountSpinBox || !ModeComboBox)
		{
			OutError = TEXT("Failed to construct input page widgets for WBP_InteractionUIPlaygroundPopup.");
			return false;
		}

		EnsureWidgetGuid(WidgetBlueprint, *InputContent);
		EnsureWidgetGuid(WidgetBlueprint, *InputSummary);
		EnsureWidgetGuid(WidgetBlueprint, *NameTextBox);
		EnsureWidgetGuid(WidgetBlueprint, *NotesTextBox);
		EnsureWidgetGuid(WidgetBlueprint, *EnableOptionCheckBox);
		EnsureWidgetGuid(WidgetBlueprint, *ValueSlider);
		EnsureWidgetGuid(WidgetBlueprint, *CountSpinBox);
		EnsureWidgetGuid(WidgetBlueprint, *ModeComboBox);

		InputContent->bIsVariable = true;
		InputSummary->bIsVariable = true;
		NameTextBox->bIsVariable = true;
		NotesTextBox->bIsVariable = true;
		EnableOptionCheckBox->bIsVariable = true;
		ValueSlider->bIsVariable = true;
		CountSpinBox->bIsVariable = true;
		ModeComboBox->bIsVariable = true;

		InputContent->ClearChildren();
		InputContent->AddChildToVerticalBox(InputSummary);
		InputContent->AddChildToVerticalBox(NameTextBox);
		InputContent->AddChildToVerticalBox(NotesTextBox);
		InputContent->AddChildToVerticalBox(EnableOptionCheckBox);
		InputContent->AddChildToVerticalBox(ValueSlider);
		InputContent->AddChildToVerticalBox(CountSpinBox);
		InputContent->AddChildToVerticalBox(ModeComboBox);

		InputSummary->SetText(FText::FromString(TEXT("Commit the name field, type notes, toggle the checkbox, move the slider, adjust the spin box, and pick a preset. Each change should update the status bar.")));
		InputSummary->SetAutoWrapText(true);

		NameTextBox->SetText(FText::FromString(TEXT("Codex")));
		NameTextBox->SetHintText(FText::FromString(TEXT("Enter a name")));

		NotesTextBox->SetText(FText::FromString(TEXT("Sample notes for multiline entry.")));
		NotesTextBox->SetHintText(FText::FromString(TEXT("Enter notes")));

		EnableOptionCheckBox->SetIsChecked(true);
		ValueSlider->SetMinValue(0.0f);
		ValueSlider->SetMaxValue(1.0f);
		ValueSlider->SetValue(0.65f);
		ValueSlider->SetStepSize(0.05f);
		CountSpinBox->SetMinValue(0.0f);
		CountSpinBox->SetMaxValue(10.0f);
		CountSpinBox->SetValue(3.0f);
		CountSpinBox->SetDelta(1.0f);

		ModeComboBox->ClearOptions();
		ModeComboBox->AddOption(TEXT("Mode A"));
		ModeComboBox->AddOption(TEXT("Mode B"));
		ModeComboBox->AddOption(TEXT("Mode C"));
		ModeComboBox->SetSelectedOption(TEXT("Mode B"));

		ConfigureVerticalBoxSlot(*InputSummary, FMargin(0.0f, 0.0f, 0.0f, 16.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*NameTextBox, FMargin(0.0f, 0.0f, 0.0f, 12.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*NotesTextBox, FMargin(0.0f, 0.0f, 0.0f, 16.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*EnableOptionCheckBox, FMargin(0.0f, 0.0f, 0.0f, 16.0f), ESlateSizeRule::Automatic, HAlign_Left, VAlign_Center);
		ConfigureVerticalBoxSlot(*ValueSlider, FMargin(0.0f, 0.0f, 0.0f, 14.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*CountSpinBox, FMargin(0.0f, 0.0f, 0.0f, 14.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*ModeComboBox, FMargin(0.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);

		return true;
	}

	bool ConfigureUIPlaygroundCollectionPageWidget(
		UWidgetBlueprint& WidgetBlueprint,
		UClass* ListEntryWidgetClass,
		UClass* TileEntryWidgetClass,
		FString& OutError)
	{
		(void)TileEntryWidgetClass;

		UVerticalBox* CollectionContent = FindWidget<UVerticalBox>(WidgetBlueprint, TEXT("VerticalBox_PageCollectionContent"));
		UTextBlock* CollectionSummary = FindWidget<UTextBlock>(WidgetBlueprint, TEXT("TXT_CollectionSummary"));
		if (CollectionSummary == nullptr)
		{
			CollectionSummary = WidgetBlueprint.WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TXT_CollectionSummary"));
		}

		UHorizontalBox* CollectionActions = FindWidget<UHorizontalBox>(WidgetBlueprint, TEXT("HorizontalBox_CollectionActions"));
		if (CollectionActions == nullptr)
		{
			CollectionActions = WidgetBlueprint.WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HorizontalBox_CollectionActions"));
		}

		UButton* ListAddButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_ListAdd"));
		if (ListAddButton == nullptr)
		{
			ListAddButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_ListAdd"));
		}

		UButton* ListRemoveButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_ListRemove"));
		if (ListRemoveButton == nullptr)
		{
			ListRemoveButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_ListRemove"));
		}

		UButton* ListToggleSelectionButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_ListToggleSelection"));
		if (ListToggleSelectionButton == nullptr)
		{
			ListToggleSelectionButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_ListToggleSelection"));
		}

		USizeBox* CollectionListViewport = FindWidget<USizeBox>(WidgetBlueprint, TEXT("SizeBox_CollectionListViewport"));
		if (CollectionListViewport == nullptr)
		{
			CollectionListViewport = WidgetBlueprint.WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("SizeBox_CollectionListViewport"));
		}

		UListView* ListViewItems = FindWidget<UListView>(WidgetBlueprint, TEXT("ListView_Items"));
		if (ListViewItems == nullptr)
		{
			ListViewItems = WidgetBlueprint.WidgetTree->ConstructWidget<UListView>(UListView::StaticClass(), TEXT("ListView_Items"));
		}

		if (!CollectionContent || !CollectionSummary || !CollectionActions || !ListAddButton || !ListRemoveButton || !ListToggleSelectionButton
			|| !CollectionListViewport || !ListViewItems)
		{
			OutError = TEXT("Failed to construct collection page widgets for WBP_InteractionUIPlaygroundPopup.");
			return false;
		}

		EnsureWidgetGuid(WidgetBlueprint, *CollectionContent);
		EnsureWidgetGuid(WidgetBlueprint, *CollectionSummary);
		EnsureWidgetGuid(WidgetBlueprint, *CollectionActions);
		EnsureWidgetGuid(WidgetBlueprint, *ListAddButton);
		EnsureWidgetGuid(WidgetBlueprint, *ListRemoveButton);
		EnsureWidgetGuid(WidgetBlueprint, *ListToggleSelectionButton);
		EnsureWidgetGuid(WidgetBlueprint, *CollectionListViewport);
		EnsureWidgetGuid(WidgetBlueprint, *ListViewItems);

		CollectionContent->bIsVariable = true;
		CollectionSummary->bIsVariable = true;
		CollectionActions->bIsVariable = true;
		ListAddButton->bIsVariable = true;
		ListRemoveButton->bIsVariable = true;
		ListToggleSelectionButton->bIsVariable = true;
		CollectionListViewport->bIsVariable = true;
		ListViewItems->bIsVariable = true;

		CollectionContent->ClearChildren();
		CollectionContent->AddChildToVerticalBox(CollectionSummary);
		CollectionContent->AddChildToVerticalBox(CollectionActions);
		CollectionContent->AddChildToVerticalBox(CollectionListViewport);

		CollectionActions->ClearChildren();
		CollectionActions->AddChildToHorizontalBox(ListAddButton);
		CollectionActions->AddChildToHorizontalBox(ListRemoveButton);
		CollectionActions->AddChildToHorizontalBox(ListToggleSelectionButton);

		if (CollectionListViewport->GetContent() != ListViewItems)
		{
			CollectionListViewport->SetContent(ListViewItems);
		}

		CollectionSummary->SetText(FText::FromString(TEXT("Click a row to select it. Add creates rows, Remove deletes the selected row, and Select / Clear toggles the highlight. Keep adding rows until the viewport scrolls.")));
		CollectionSummary->SetAutoWrapText(true);

		ConfigureButtonStyle(*ListAddButton, FLinearColor(0.36f, 0.58f, 0.76f, 0.96f));
		ConfigureButtonStyle(*ListRemoveButton, FLinearColor(0.38f, 0.34f, 0.42f, 0.96f));
		ConfigureButtonStyle(*ListToggleSelectionButton, FLinearColor(0.46f, 0.64f, 0.48f, 0.96f));
		EnsureButtonLabel(WidgetBlueprint, *ListAddButton, TEXT("TXT_ListAddLabel"), FText::FromString(TEXT("Add")));
		EnsureButtonLabel(WidgetBlueprint, *ListRemoveButton, TEXT("TXT_ListRemoveLabel"), FText::FromString(TEXT("Remove")));
		EnsureButtonLabel(WidgetBlueprint, *ListToggleSelectionButton, TEXT("TXT_ListToggleSelectionLabel"), FText::FromString(TEXT("Select / Clear")));

		CollectionListViewport->SetHeightOverride(360.0f);
		ConfigureVerticalBoxSlot(*CollectionSummary, FMargin(0.0f, 0.0f, 0.0f, 16.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*CollectionActions, FMargin(0.0f, 0.0f, 0.0f, 16.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*CollectionListViewport, FMargin(0.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Fill);
		ConfigureHorizontalBoxSlot(*ListAddButton, FMargin(0.0f, 0.0f, 10.0f, 0.0f), ESlateSizeRule::Automatic, HAlign_Left, VAlign_Center);
		ConfigureHorizontalBoxSlot(*ListRemoveButton, FMargin(0.0f, 0.0f, 10.0f, 0.0f), ESlateSizeRule::Automatic, HAlign_Left, VAlign_Center);
		ConfigureHorizontalBoxSlot(*ListToggleSelectionButton, FMargin(0.0f), ESlateSizeRule::Automatic, HAlign_Left, VAlign_Center);

		ListViewItems->SetSelectionMode(ESelectionMode::Single);
		SetWidgetClassProperty(*ListViewItems, TEXT("EntryWidgetClass"), ListEntryWidgetClass);

		return true;
	}

	bool ConfigureUIPlaygroundAdvancedPageWidget(UWidgetBlueprint& WidgetBlueprint, UClass* TileEntryWidgetClass, FString& OutError)
	{
		UVerticalBox* AdvancedContent = FindWidget<UVerticalBox>(WidgetBlueprint, TEXT("VerticalBox_PageAdvancedContent"));
		UTextBlock* FocusStatus = FindWidget<UTextBlock>(WidgetBlueprint, TEXT("TXT_FocusStatus"));
		if (FocusStatus == nullptr)
		{
			FocusStatus = WidgetBlueprint.WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TXT_FocusStatus"));
		}

		UTileView* TileViewSlots = FindWidget<UTileView>(WidgetBlueprint, TEXT("TileView_Slots"));
		if (TileViewSlots == nullptr)
		{
			TileViewSlots = WidgetBlueprint.WidgetTree->ConstructWidget<UTileView>(UTileView::StaticClass(), TEXT("TileView_Slots"));
		}

		UTextBlock* DragStatus = FindWidget<UTextBlock>(WidgetBlueprint, TEXT("TXT_DragStatus"));
		if (DragStatus == nullptr)
		{
			DragStatus = WidgetBlueprint.WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TXT_DragStatus"));
		}

		if (!AdvancedContent || !FocusStatus || !TileViewSlots || !DragStatus)
		{
			OutError = TEXT("Failed to construct advanced page widgets for WBP_InteractionUIPlaygroundPopup.");
			return false;
		}

		EnsureWidgetGuid(WidgetBlueprint, *AdvancedContent);
		EnsureWidgetGuid(WidgetBlueprint, *FocusStatus);
		EnsureWidgetGuid(WidgetBlueprint, *TileViewSlots);
		EnsureWidgetGuid(WidgetBlueprint, *DragStatus);

		AdvancedContent->bIsVariable = true;
		FocusStatus->bIsVariable = true;
		TileViewSlots->bIsVariable = true;
		DragStatus->bIsVariable = true;

		AdvancedContent->ClearChildren();
		AdvancedContent->AddChildToVerticalBox(FocusStatus);
		AdvancedContent->AddChildToVerticalBox(TileViewSlots);
		AdvancedContent->AddChildToVerticalBox(DragStatus);

		FocusStatus->SetText(FText::FromString(TEXT("Click a tile to confirm focus and selection. Then drag a filled tile onto another slot in the same panel.")));
		FocusStatus->SetAutoWrapText(true);
		DragStatus->SetText(FText::FromString(TEXT("Filled and empty slots should both react. The status bar reports when a drag starts and when a drop succeeds.")));
		DragStatus->SetAutoWrapText(true);

		ConfigureVerticalBoxSlot(*FocusStatus, FMargin(0.0f, 0.0f, 0.0f, 16.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*TileViewSlots, FMargin(0.0f, 0.0f, 0.0f, 16.0f), ESlateSizeRule::Automatic, HAlign_Left, VAlign_Center);
		ConfigureVerticalBoxSlot(*DragStatus, FMargin(0.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);

		TileViewSlots->SetEntryWidth(104.0f);
		TileViewSlots->SetEntryHeight(68.0f);
		TileViewSlots->SetSelectionMode(ESelectionMode::Single);
		TileViewSlots->SetHorizontalEntrySpacing(10.0f);
		TileViewSlots->SetVerticalEntrySpacing(10.0f);
		SetWidgetClassProperty(*TileViewSlots, TEXT("EntryWidgetClass"), TileEntryWidgetClass);

		ConfigureVerticalBoxSlot(*FocusStatus, FMargin(0.0f, 0.0f, 0.0f, 12.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*TileViewSlots, FMargin(0.0f, 0.0f, 0.0f, 12.0f), ESlateSizeRule::Fill, HAlign_Fill, VAlign_Fill);
		ConfigureVerticalBoxSlot(*DragStatus, FMargin(0.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);

		return true;
	}

	bool ConfigureUIPlaygroundPopupWidgetBlueprint(
		UWidgetBlueprint& WidgetBlueprint,
		UClass* ListEntryWidgetClass,
		UClass* TileEntryWidgetClass,
		UTexture2D& FilledCircleTexture,
		UTexture2D& OuterRingTexture,
		UTexture2D& SmileIconTexture,
		FString& OutError)
	{
		if (WidgetBlueprint.WidgetTree == nullptr)
		{
			OutError = TEXT("UI playground popup widget blueprint is missing a WidgetTree.");
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
			OutError = TEXT("Failed to create RootCanvas for WBP_InteractionUIPlaygroundPopup.");
			return false;
		}

		EnsureWidgetGuid(WidgetBlueprint, *RootCanvas);
		RootCanvas->bIsVariable = false;

		UOverlay* ScreenRoot = EnsurePanelChild<UOverlay>(WidgetBlueprint, *RootCanvas, TEXT("Overlay_ScreenRoot"), false);
		USizeBox* PopupFrame = EnsurePanelChild<USizeBox>(WidgetBlueprint, *ScreenRoot, TEXT("SizeBox_PopupFrame"), false);
		if (!ScreenRoot || !PopupFrame)
		{
			OutError = TEXT("Failed to create screen root widgets for WBP_InteractionUIPlaygroundPopup.");
			return false;
		}

		UBackgroundBlur* BackgroundBlurPanel = FindWidget<UBackgroundBlur>(WidgetBlueprint, TEXT("BackgroundBlur_Panel"));
		if (BackgroundBlurPanel == nullptr)
		{
			BackgroundBlurPanel = WidgetBlueprint.WidgetTree->ConstructWidget<UBackgroundBlur>(UBackgroundBlur::StaticClass(), TEXT("BackgroundBlur_Panel"));
		}

		UBorder* TintPanel = FindWidget<UBorder>(WidgetBlueprint, TEXT("Border_TintPanel"));
		if (TintPanel == nullptr)
		{
			TintPanel = WidgetBlueprint.WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border_TintPanel"));
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

		UHorizontalBox* TitleActions = FindWidget<UHorizontalBox>(WidgetBlueprint, TEXT("HorizontalBox_TitleActions"));
		if (TitleActions == nullptr)
		{
			TitleActions = WidgetBlueprint.WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HorizontalBox_TitleActions"));
		}

		UButton* ResetButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_Reset"));
		if (ResetButton == nullptr)
		{
			ResetButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_Reset"));
		}

		UButton* SubmitButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_Submit"));
		if (SubmitButton == nullptr)
		{
			SubmitButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_Submit"));
		}

		UButton* CloseButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_Close"));
		if (CloseButton == nullptr)
		{
			CloseButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_Close"));
		}

		UHorizontalBox* BodyRow = FindWidget<UHorizontalBox>(WidgetBlueprint, TEXT("HorizontalBox_Body"));
		if (BodyRow == nullptr)
		{
			BodyRow = WidgetBlueprint.WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HorizontalBox_Body"));
		}

		UBorder* LeftNav = FindWidget<UBorder>(WidgetBlueprint, TEXT("Border_LeftNav"));
		if (LeftNav == nullptr)
		{
			LeftNav = WidgetBlueprint.WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border_LeftNav"));
		}

		UVerticalBox* NavButtons = FindWidget<UVerticalBox>(WidgetBlueprint, TEXT("VerticalBox_NavButtons"));
		if (NavButtons == nullptr)
		{
			NavButtons = WidgetBlueprint.WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VerticalBox_NavButtons"));
		}

		UButton* TabBasicButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_TabBasic"));
		if (TabBasicButton == nullptr)
		{
			TabBasicButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_TabBasic"));
		}

		UButton* TabInputButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_TabInput"));
		if (TabInputButton == nullptr)
		{
			TabInputButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_TabInput"));
		}

		UButton* TabCollectionButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_TabCollection"));
		if (TabCollectionButton == nullptr)
		{
			TabCollectionButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_TabCollection"));
		}

		UButton* TabAdvancedButton = FindWidget<UButton>(WidgetBlueprint, TEXT("BTN_TabAdvanced"));
		if (TabAdvancedButton == nullptr)
		{
			TabAdvancedButton = WidgetBlueprint.WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("BTN_TabAdvanced"));
		}

		UBorder* MainContent = FindWidget<UBorder>(WidgetBlueprint, TEXT("Border_MainContent"));
		if (MainContent == nullptr)
		{
			MainContent = WidgetBlueprint.WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border_MainContent"));
		}

		UWidgetSwitcher* PageSwitcher = FindWidget<UWidgetSwitcher>(WidgetBlueprint, TEXT("Switcher_Sections"));
		if (PageSwitcher == nullptr)
		{
			PageSwitcher = WidgetBlueprint.WidgetTree->ConstructWidget<UWidgetSwitcher>(UWidgetSwitcher::StaticClass(), TEXT("Switcher_Sections"));
		}

		UScrollBox* PageBasic = FindWidget<UScrollBox>(WidgetBlueprint, TEXT("ScrollBox_PageBasic"));
		if (PageBasic == nullptr)
		{
			PageBasic = WidgetBlueprint.WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("ScrollBox_PageBasic"));
		}

		UScrollBox* PageInput = FindWidget<UScrollBox>(WidgetBlueprint, TEXT("ScrollBox_PageInput"));
		if (PageInput == nullptr)
		{
			PageInput = WidgetBlueprint.WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("ScrollBox_PageInput"));
		}

		UScrollBox* PageCollection = FindWidget<UScrollBox>(WidgetBlueprint, TEXT("ScrollBox_PageCollection"));
		if (PageCollection == nullptr)
		{
			PageCollection = WidgetBlueprint.WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("ScrollBox_PageCollection"));
		}

		UScrollBox* PageAdvanced = FindWidget<UScrollBox>(WidgetBlueprint, TEXT("ScrollBox_PageAdvanced"));
		if (PageAdvanced == nullptr)
		{
			PageAdvanced = WidgetBlueprint.WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("ScrollBox_PageAdvanced"));
		}

		if (!BackgroundBlurPanel || !TintPanel || !ContentColumn || !TitleBar || !TitleText || !TitleActions || !ResetButton || !SubmitButton || !CloseButton
			|| !BodyRow || !LeftNav || !NavButtons || !TabBasicButton || !TabInputButton || !TabCollectionButton || !TabAdvancedButton
			|| !MainContent || !PageSwitcher || !PageBasic || !PageInput || !PageCollection || !PageAdvanced)
		{
			OutError = TEXT("Failed to construct one or more popup widgets for WBP_InteractionUIPlaygroundPopup.");
			return false;
		}

		EnsureWidgetGuid(WidgetBlueprint, *BackgroundBlurPanel);
		EnsureWidgetGuid(WidgetBlueprint, *TintPanel);
		EnsureWidgetGuid(WidgetBlueprint, *ContentColumn);
		EnsureWidgetGuid(WidgetBlueprint, *TitleBar);
		EnsureWidgetGuid(WidgetBlueprint, *TitleText);
		EnsureWidgetGuid(WidgetBlueprint, *TitleActions);
		EnsureWidgetGuid(WidgetBlueprint, *ResetButton);
		EnsureWidgetGuid(WidgetBlueprint, *SubmitButton);
		EnsureWidgetGuid(WidgetBlueprint, *CloseButton);
		EnsureWidgetGuid(WidgetBlueprint, *BodyRow);
		EnsureWidgetGuid(WidgetBlueprint, *LeftNav);
		EnsureWidgetGuid(WidgetBlueprint, *NavButtons);
		EnsureWidgetGuid(WidgetBlueprint, *TabBasicButton);
		EnsureWidgetGuid(WidgetBlueprint, *TabInputButton);
		EnsureWidgetGuid(WidgetBlueprint, *TabCollectionButton);
		EnsureWidgetGuid(WidgetBlueprint, *TabAdvancedButton);
		EnsureWidgetGuid(WidgetBlueprint, *MainContent);
		EnsureWidgetGuid(WidgetBlueprint, *PageSwitcher);
		EnsureWidgetGuid(WidgetBlueprint, *PageBasic);
		EnsureWidgetGuid(WidgetBlueprint, *PageInput);
		EnsureWidgetGuid(WidgetBlueprint, *PageCollection);
		EnsureWidgetGuid(WidgetBlueprint, *PageAdvanced);

		BackgroundBlurPanel->bIsVariable = true;
		TintPanel->bIsVariable = true;
		ContentColumn->bIsVariable = true;
		TitleBar->bIsVariable = true;
		TitleText->bIsVariable = true;
		TitleActions->bIsVariable = true;
		ResetButton->bIsVariable = true;
		SubmitButton->bIsVariable = true;
		CloseButton->bIsVariable = true;
		BodyRow->bIsVariable = true;
		LeftNav->bIsVariable = true;
		NavButtons->bIsVariable = true;
		TabBasicButton->bIsVariable = true;
		TabInputButton->bIsVariable = true;
		TabCollectionButton->bIsVariable = true;
		TabAdvancedButton->bIsVariable = true;
		MainContent->bIsVariable = true;
		PageSwitcher->bIsVariable = true;
		PageBasic->bIsVariable = true;
		PageInput->bIsVariable = true;
		PageCollection->bIsVariable = true;
		PageAdvanced->bIsVariable = true;

		if (ScreenRoot->GetChildrenCount() != 1 || ScreenRoot->GetChildAt(0) != PopupFrame)
		{
			ScreenRoot->ClearChildren();
			ScreenRoot->AddChild(PopupFrame);
		}

		if (PopupFrame->GetContent() != BackgroundBlurPanel)
		{
			PopupFrame->SetContent(BackgroundBlurPanel);
		}

		if (BackgroundBlurPanel->GetContent() != TintPanel)
		{
			BackgroundBlurPanel->SetContent(TintPanel);
		}

		if (TintPanel->GetContent() != ContentColumn)
		{
			TintPanel->SetContent(ContentColumn);
		}

		UBorder* StatusBar = FindWidget<UBorder>(WidgetBlueprint, TEXT("Border_StatusBar"));
		if (StatusBar == nullptr)
		{
			StatusBar = WidgetBlueprint.WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border_StatusBar"));
			EnsureWidgetGuid(WidgetBlueprint, *StatusBar);
		}
		StatusBar->bIsVariable = true;
		UTextBlock* StatusText = FindWidget<UTextBlock>(WidgetBlueprint, TEXT("TXT_Status"));
		if (StatusText == nullptr)
		{
			StatusText = WidgetBlueprint.WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TXT_Status"));
			EnsureWidgetGuid(WidgetBlueprint, *StatusText);
		}
		StatusText->bIsVariable = true;
		StatusText->SetText(FText::FromString(TEXT("Ready")));
		StatusText->SetColorAndOpacity(FSlateColor(FLinearColor(0.84f, 0.91f, 0.96f, 1.0f)));
		ConfigureVerticalBoxSlot(*StatusText, FMargin(0.0f), ESlateSizeRule::Automatic, HAlign_Left, VAlign_Center);

		if (ContentColumn->GetChildrenCount() != 3
			|| ContentColumn->GetChildAt(0) != TitleBar
			|| ContentColumn->GetChildAt(1) != BodyRow
			|| ContentColumn->GetChildAt(2) != StatusBar)
		{
			ContentColumn->ClearChildren();
			ContentColumn->AddChildToVerticalBox(TitleBar);
			ContentColumn->AddChildToVerticalBox(BodyRow);
			ContentColumn->AddChildToVerticalBox(StatusBar);
		}

		if (TitleBar->GetChildrenCount() != 2 || TitleBar->GetChildAt(0) != TitleText || TitleBar->GetChildAt(1) != TitleActions)
		{
			TitleBar->ClearChildren();
			TitleBar->AddChildToHorizontalBox(TitleText);
			TitleBar->AddChildToHorizontalBox(TitleActions);
		}

		if (TitleActions->GetChildrenCount() != 3
			|| TitleActions->GetChildAt(0) != ResetButton
			|| TitleActions->GetChildAt(1) != SubmitButton
			|| TitleActions->GetChildAt(2) != CloseButton)
		{
			TitleActions->ClearChildren();
			TitleActions->AddChildToHorizontalBox(ResetButton);
			TitleActions->AddChildToHorizontalBox(SubmitButton);
			TitleActions->AddChildToHorizontalBox(CloseButton);
		}

		if (BodyRow->GetChildrenCount() != 2 || BodyRow->GetChildAt(0) != LeftNav || BodyRow->GetChildAt(1) != MainContent)
		{
			BodyRow->ClearChildren();
			BodyRow->AddChildToHorizontalBox(LeftNav);
			BodyRow->AddChildToHorizontalBox(MainContent);
		}

		if (LeftNav->GetContent() != NavButtons)
		{
			LeftNav->SetContent(NavButtons);
		}

		if (NavButtons->GetChildrenCount() != 4)
		{
			NavButtons->ClearChildren();
			NavButtons->AddChildToVerticalBox(TabBasicButton);
			NavButtons->AddChildToVerticalBox(TabInputButton);
			NavButtons->AddChildToVerticalBox(TabCollectionButton);
			NavButtons->AddChildToVerticalBox(TabAdvancedButton);
		}

		if (MainContent->GetContent() != PageSwitcher)
		{
			MainContent->SetContent(PageSwitcher);
		}

		if (PageSwitcher->GetChildrenCount() != 4)
		{
			PageSwitcher->ClearChildren();
			PageSwitcher->AddChild(PageBasic);
			PageSwitcher->AddChild(PageInput);
			PageSwitcher->AddChild(PageCollection);
			PageSwitcher->AddChild(PageAdvanced);
		}

		if (PageBasic->GetChildrenCount() == 0)
		{
			UVerticalBox* BasicContent = WidgetBlueprint.WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VerticalBox_PageBasicContent"));
			EnsureWidgetGuid(WidgetBlueprint, *BasicContent);
			BasicContent->bIsVariable = true;
			PageBasic->AddChild(BasicContent);
		}

		if (PageInput->GetChildrenCount() == 0)
		{
			UVerticalBox* InputContent = WidgetBlueprint.WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VerticalBox_PageInputContent"));
			EnsureWidgetGuid(WidgetBlueprint, *InputContent);
			InputContent->bIsVariable = true;
			PageInput->AddChild(InputContent);
		}

		if (PageCollection->GetChildrenCount() == 0)
		{
			UVerticalBox* CollectionContent = WidgetBlueprint.WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VerticalBox_PageCollectionContent"));
			EnsureWidgetGuid(WidgetBlueprint, *CollectionContent);
			CollectionContent->bIsVariable = true;
			PageCollection->AddChild(CollectionContent);
		}

		if (PageAdvanced->GetChildrenCount() == 0)
		{
			UVerticalBox* AdvancedContent = WidgetBlueprint.WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VerticalBox_PageAdvancedContent"));
			EnsureWidgetGuid(WidgetBlueprint, *AdvancedContent);
			AdvancedContent->bIsVariable = true;
			PageAdvanced->AddChild(AdvancedContent);
		}

		ConfigureCanvasAnchors(*ScreenRoot, FAnchors(0.0f, 0.0f, 1.0f, 1.0f), FMargin(0.0f), FVector2D::ZeroVector);
		ConfigureOverlaySlot(*PopupFrame);

		PopupFrame->SetWidthOverride(1160.0f);
		PopupFrame->SetHeightOverride(980.0f);

		BackgroundBlurPanel->SetBlurStrength(18.0f);
		BackgroundBlurPanel->SetOverrideAutoRadiusCalculation(true);
		BackgroundBlurPanel->SetBlurRadius(12);
		BackgroundBlurPanel->SetApplyAlphaToBlur(true);
		BackgroundBlurPanel->SetCornerRadius(FVector4(20.0f, 20.0f, 20.0f, 20.0f));

		TintPanel->SetPadding(FMargin(24.0f, 30.0f, 24.0f, 24.0f));
		TintPanel->SetBrush(MakeRoundedBrush(FLinearColor(0.13f, 0.18f, 0.24f, 0.96f), 20.0f, FLinearColor(1.0f, 1.0f, 1.0f, 0.08f), 1.0f));

		TitleText->SetText(FText::FromString(TEXT("UI Playground")));
		TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		{
			FSlateFontInfo Font = TitleText->GetFont();
			Font.Size = 22;
			TitleText->SetFont(Font);
		}

		ConfigureButtonStyle(*ResetButton, FLinearColor(0.34f, 0.40f, 0.48f, 0.96f));
		ConfigureButtonStyle(*SubmitButton, FLinearColor(0.30f, 0.56f, 0.78f, 0.96f));
		ConfigureButtonStyle(*CloseButton, FLinearColor(0.68f, 0.36f, 0.22f, 0.96f));
		ConfigureButtonStyle(*TabBasicButton, FLinearColor(0.28f, 0.46f, 0.62f, 0.96f));
		ConfigureButtonStyle(*TabInputButton, FLinearColor(0.26f, 0.44f, 0.52f, 0.96f));
		ConfigureButtonStyle(*TabCollectionButton, FLinearColor(0.30f, 0.50f, 0.44f, 0.96f));
		ConfigureButtonStyle(*TabAdvancedButton, FLinearColor(0.38f, 0.42f, 0.60f, 0.96f));
		EnsureButtonLabel(WidgetBlueprint, *ResetButton, TEXT("TXT_ResetLabel"), FText::FromString(TEXT("Reset")));
		EnsureButtonLabel(WidgetBlueprint, *SubmitButton, TEXT("TXT_SubmitLabel"), FText::FromString(TEXT("Submit")));
		EnsureButtonLabel(WidgetBlueprint, *CloseButton, TEXT("TXT_CloseLabel"), FText::FromString(TEXT("Close")));
		EnsureButtonLabel(WidgetBlueprint, *TabBasicButton, TEXT("TXT_TabBasicLabel"), FText::FromString(TEXT("Basic")));
		EnsureButtonLabel(WidgetBlueprint, *TabInputButton, TEXT("TXT_TabInputLabel"), FText::FromString(TEXT("Input")));
		EnsureButtonLabel(WidgetBlueprint, *TabCollectionButton, TEXT("TXT_TabCollectionLabel"), FText::FromString(TEXT("Collection")));
		EnsureButtonLabel(WidgetBlueprint, *TabAdvancedButton, TEXT("TXT_TabAdvancedLabel"), FText::FromString(TEXT("Advanced")));

		ConfigureVerticalBoxSlot(*TitleBar, FMargin(0.0f, 0.0f, 0.0f, 22.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*BodyRow, FMargin(0.0f, 0.0f, 0.0f, 18.0f), ESlateSizeRule::Fill, HAlign_Fill, VAlign_Fill);

		ConfigureHorizontalBoxSlot(*TitleText, FMargin(0.0f, 0.0f, 12.0f, 0.0f), ESlateSizeRule::Fill, HAlign_Left, VAlign_Center);
		ConfigureHorizontalBoxSlot(*TitleActions, FMargin(0.0f), ESlateSizeRule::Automatic, HAlign_Right, VAlign_Center);
		ConfigureHorizontalBoxSlot(*ResetButton, FMargin(0.0f, 0.0f, 10.0f, 0.0f), ESlateSizeRule::Automatic, HAlign_Right, VAlign_Center);
		ConfigureHorizontalBoxSlot(*SubmitButton, FMargin(0.0f, 0.0f, 10.0f, 0.0f), ESlateSizeRule::Automatic, HAlign_Right, VAlign_Center);
		ConfigureHorizontalBoxSlot(*CloseButton, FMargin(0.0f), ESlateSizeRule::Automatic, HAlign_Right, VAlign_Center);

		ConfigureHorizontalBoxSlot(*LeftNav, FMargin(0.0f, 0.0f, 18.0f, 0.0f), ESlateSizeRule::Automatic, HAlign_Left, VAlign_Fill);
		ConfigureHorizontalBoxSlot(*MainContent, FMargin(0.0f), ESlateSizeRule::Fill, HAlign_Fill, VAlign_Fill);
		LeftNav->SetPadding(FMargin(0.0f, 14.0f, 0.0f, 10.0f));

		ConfigureVerticalBoxSlot(*TabBasicButton, FMargin(0.0f, 0.0f, 0.0f, 8.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*TabInputButton, FMargin(0.0f, 0.0f, 0.0f, 8.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*TabCollectionButton, FMargin(0.0f, 0.0f, 0.0f, 8.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);
		ConfigureVerticalBoxSlot(*TabAdvancedButton, FMargin(0.0f), ESlateSizeRule::Automatic, HAlign_Fill, VAlign_Center);

		StatusBar->SetPadding(FMargin(18.0f, 12.0f, 18.0f, 12.0f));
		StatusBar->SetBrush(MakeRoundedBrush(FLinearColor(0.18f, 0.23f, 0.30f, 0.96f), 14.0f, FLinearColor(1.0f, 1.0f, 1.0f, 0.06f), 1.0f));
		StatusBar->SetContent(StatusText);

		StatusText->SetColorAndOpacity(FSlateColor(FLinearColor(0.84f, 0.91f, 0.96f, 1.0f)));
		ConfigureVerticalBoxSlot(*StatusText, FMargin(0.0f), ESlateSizeRule::Automatic, HAlign_Left, VAlign_Center);

		PageSwitcher->SetActiveWidgetIndex(static_cast<int32>(ECodexUIPlaygroundSection::Basic));

		if (!ConfigureUIPlaygroundBasicPageWidget(WidgetBlueprint, FilledCircleTexture, OuterRingTexture, SmileIconTexture, OutError))
		{
			return false;
		}

		if (!ConfigureUIPlaygroundInputPageWidget(WidgetBlueprint, OutError))
		{
			return false;
		}

		if (!ConfigureUIPlaygroundCollectionPageWidget(WidgetBlueprint, ListEntryWidgetClass, TileEntryWidgetClass, OutError))
		{
			return false;
		}

		if (!ConfigureUIPlaygroundAdvancedPageWidget(WidgetBlueprint, TileEntryWidgetClass, OutError))
		{
			return false;
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

	bool ConfigureDualTileTransferPopupInteractableBlueprint(
		UBlueprint& Blueprint,
		UStaticMesh& StaticMesh,
		const FText& PromptText,
		const FText& PopupTitle,
		const TArray<int32>& LeftNumbers,
		const TArray<int32>& RightNumbers,
		const bool bAllowDuplicateNumbers,
		FString& OutError)
	{
		CompileBlueprint(&Blueprint);

		ACodexDualTileTransferPopupInteractableActor* DefaultObject =
			Blueprint.GeneratedClass ? Cast<ACodexDualTileTransferPopupInteractableActor>(Blueprint.GeneratedClass->GetDefaultObject()) : nullptr;
		if (DefaultObject == nullptr)
		{
			OutError = FString::Printf(TEXT("Failed to access dual tile transfer popup default object for %s."), *Blueprint.GetName());
			return false;
		}

		UStaticMeshComponent* StaticMeshComponent = DefaultObject->GetStaticMeshComponent();
		UCodexInteractionComponent* InteractionComponent = DefaultObject->GetInteractionComponent();
		if (StaticMeshComponent == nullptr || InteractionComponent == nullptr)
		{
			OutError = FString::Printf(TEXT("Dual tile transfer popup blueprint %s is missing required native components."), *Blueprint.GetName());
			return false;
		}

		StaticMeshComponent->SetStaticMesh(&StaticMesh);
		InteractionComponent->SetInteractionType(ECodexInteractionType::Use);
		InteractionComponent->SetPromptText(PromptText);
		InteractionComponent->SetVisibleDistance(340.0f);
		InteractionComponent->SetInteractableDistance(150.0f);
		DefaultObject->SetPopupTitle(PopupTitle);
		DefaultObject->SetLeftNumbers(LeftNumbers);
		DefaultObject->SetRightNumbers(RightNumbers);
		DefaultObject->SetAllowDuplicateNumbers(bAllowDuplicateNumbers);
		DefaultObject->MarkPackageDirty();
		Blueprint.MarkPackageDirty();
		FBlueprintEditorUtils::MarkBlueprintAsModified(&Blueprint);
		CompileBlueprint(&Blueprint);
		return true;
	}

	bool ConfigureUIPlaygroundPopupInteractableBlueprint(
		UBlueprint& Blueprint,
		UStaticMesh& StaticMesh,
		const FText& PromptText,
		FString& OutError)
	{
		CompileBlueprint(&Blueprint);

		ACodexPopupInteractableActor* DefaultObject = Blueprint.GeneratedClass ? Cast<ACodexPopupInteractableActor>(Blueprint.GeneratedClass->GetDefaultObject()) : nullptr;
		if (DefaultObject == nullptr)
		{
			OutError = FString::Printf(TEXT("Failed to access UI playground popup default object for %s."), *Blueprint.GetName());
			return false;
		}

		UStaticMeshComponent* StaticMeshComponent = DefaultObject->GetStaticMeshComponent();
		UCodexInteractionComponent* InteractionComponent = DefaultObject->GetInteractionComponent();
		if (StaticMeshComponent == nullptr || InteractionComponent == nullptr)
		{
			OutError = FString::Printf(TEXT("UI playground popup blueprint %s is missing required native components."), *Blueprint.GetName());
			return false;
		}

		StaticMeshComponent->SetStaticMesh(&StaticMesh);
		InteractionComponent->SetInteractionType(ECodexInteractionType::Use);
		InteractionComponent->SetPromptText(PromptText);
		InteractionComponent->SetVisibleDistance(360.0f);
		InteractionComponent->SetInteractableDistance(160.0f);

		DefaultObject->SetPopupTitle(FText::FromString(TEXT("UI Playground")));
		DefaultObject->SetPopupMessage(FText::FromString(TEXT("Exercise UMG controls, reusable entries, and drag-and-drop samples.")));
		DefaultObject->SetPopupButtonLayout(ECodexPopupButtonLayout::Ok);

		if (FObjectPropertyBase* PayloadProperty = FindFProperty<FObjectPropertyBase>(DefaultObject->GetClass(), TEXT("UIPlaygroundPayload")))
		{
			UCodexInteractionUIPlaygroundPayload* Payload = NewObject<UCodexInteractionUIPlaygroundPayload>(
				DefaultObject,
				UCodexInteractionUIPlaygroundPayload::StaticClass(),
				TEXT("UIPlaygroundPayload"),
				RF_Public | RF_Transactional);
			if (Payload != nullptr)
			{
				Payload->Title = FText::FromString(TEXT("UI Playground"));
				Payload->StatusText = FText::FromString(TEXT("Click Primary, Secondary, or Ping to compare button behavior."));
				Payload->InitialSection = ECodexUIPlaygroundSection::Basic;
				Payload->BasicDescription = FText::FromString(TEXT("Start here: click Primary, Secondary, or Ping. The disabled button should stay inert while the status bar and progress bar react."));
				Payload->InputText = FText::FromString(TEXT("Sample text"));
				Payload->bToggleValue = true;
				Payload->SliderValue = 0.65f;
				Payload->SpinValue = 3;
				Payload->SelectedPreset = TEXT("Preset B");
				Payload->PresetOptions =
				{
					TEXT("Preset A"),
					TEXT("Preset B"),
					TEXT("Preset C")
				};

				Payload->ListEntries =
				{
					FCodexUIPlaygroundListEntryData{FText::FromString(TEXT("Buttons")), FText::FromString(TEXT("Primary, secondary, disabled, and ping button states.")), FText::FromString(TEXT("Clickable")), ECodexUIPlaygroundSection::Collection, FLinearColor(0.30f, 0.52f, 0.78f, 1.0f), true},
					FCodexUIPlaygroundListEntryData{FText::FromString(TEXT("Text Input")), FText::FromString(TEXT("Single-line and multi-line text editing.")), FText::FromString(TEXT("Editable")), ECodexUIPlaygroundSection::Collection, FLinearColor(0.36f, 0.66f, 0.58f, 1.0f), true},
					FCodexUIPlaygroundListEntryData{FText::FromString(TEXT("Checkbox")), FText::FromString(TEXT("Binary on/off state handling.")), FText::FromString(TEXT("Toggle")), ECodexUIPlaygroundSection::Collection, FLinearColor(0.32f, 0.55f, 0.40f, 1.0f), true},
					FCodexUIPlaygroundListEntryData{FText::FromString(TEXT("Slider")), FText::FromString(TEXT("Continuous value changes with immediate feedback.")), FText::FromString(TEXT("Value")), ECodexUIPlaygroundSection::Collection, FLinearColor(0.44f, 0.52f, 0.78f, 1.0f), true},
					FCodexUIPlaygroundListEntryData{FText::FromString(TEXT("Reusable Entries")), FText::FromString(TEXT("ListView items rendered by a reusable entry widget.")), FText::FromString(TEXT("Virtualized")), ECodexUIPlaygroundSection::Collection, FLinearColor(0.66f, 0.52f, 0.78f, 1.0f), true},
					FCodexUIPlaygroundListEntryData{FText::FromString(TEXT("Selection State")), FText::FromString(TEXT("Selection highlight can be set, cleared, and restored.")), FText::FromString(TEXT("Selectable")), ECodexUIPlaygroundSection::Collection, FLinearColor(0.72f, 0.60f, 0.38f, 1.0f), true},
					FCodexUIPlaygroundListEntryData{FText::FromString(TEXT("Scrolling")), FText::FromString(TEXT("Add rows until the list viewport starts scrolling.")), FText::FromString(TEXT("Overflow")), ECodexUIPlaygroundSection::Collection, FLinearColor(0.32f, 0.62f, 0.66f, 1.0f), true}
				};

				Payload->TileSlots =
				{
					FCodexUIPlaygroundTileSlotData{0, FText::FromString(TEXT("Slot 0")), 10, FLinearColor(0.44f, 0.70f, 0.96f, 1.0f), false, ECodexUIPlaygroundSection::Advanced},
					FCodexUIPlaygroundTileSlotData{1, FText::FromString(TEXT("Slot 1")), 11, FLinearColor(0.54f, 0.72f, 0.90f, 1.0f), false, ECodexUIPlaygroundSection::Advanced},
					FCodexUIPlaygroundTileSlotData{2, FText::FromString(TEXT("Slot 2")), 0, FLinearColor::White, true, ECodexUIPlaygroundSection::Advanced},
					FCodexUIPlaygroundTileSlotData{3, FText::FromString(TEXT("Slot 3")), 12, FLinearColor(0.76f, 0.66f, 0.44f, 1.0f), false, ECodexUIPlaygroundSection::Advanced}
				};

				PayloadProperty->SetObjectPropertyValue_InContainer(DefaultObject, Payload);
			}
		}

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
		FString& OutError,
		bool& bOutWasChanged)
	{
		bOutWasChanged = false;

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
			bOutWasChanged = true;
		}

		if (Actor == nullptr)
		{
			Actor = ActorSubsystem.SpawnActorFromClass(ActorClass, ActorLocation, ActorRotation, false);
			if (Actor == nullptr)
			{
				OutError = FString::Printf(TEXT("Failed to place %s into BasicMap."), *ActorLabel);
				return nullptr;
			}

			bOutWasChanged = true;
		}

		bool bNeedsActorUpdate = false;
		auto BeginActorUpdate = [&Actor, &bNeedsActorUpdate]()
		{
			if (!bNeedsActorUpdate)
			{
				Actor->Modify();
				bNeedsActorUpdate = true;
			}
		};

		if (!Actor->Tags.Contains(InteractionPlacementTag))
		{
			BeginActorUpdate();
			Actor->Tags.AddUnique(InteractionPlacementTag);
		}

		if (!Actor->GetActorLocation().Equals(ActorLocation, 0.01f)
			|| !Actor->GetActorRotation().Equals(ActorRotation, 0.01f))
		{
			BeginActorUpdate();
			Actor->SetActorLocationAndRotation(ActorLocation, ActorRotation, false, nullptr, ETeleportType::TeleportPhysics);
		}

		if (Actor->IsHidden())
		{
			BeginActorUpdate();
			Actor->SetActorHiddenInGame(false);
		}

		if (Actor->GetActorLabel() != ActorLabel)
		{
			BeginActorUpdate();
			Actor->SetActorLabel(ActorLabel);
		}

		if (bNeedsActorUpdate)
		{
			Actor->MarkPackageDirty();
			bOutWasChanged = true;
		}

		return Actor;
	}

	bool PlaceInteractionTestActorsInMap(
		UBlueprint& AppleBlueprint,
		UBlueprint& StrawberryBlueprint,
		UBlueprint& WoodenSignBlueprint,
		UBlueprint& WoodenSignScrollBlueprint,
		UBlueprint& WoodenSignDualTileTransferBlueprint,
		UBlueprint& WoodenSignUIPlaygroundBlueprint,
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
		const FVector WoodenSignDualTileTransferLocation = PlacementOrigin + (Forward * 430.0f);
		const FVector WoodenSignUIPlaygroundLocation = PlacementOrigin + (Forward * 520.0f) + (Right * 72.0f);
		const FRotator PlacementRotation = FRotator::ZeroRotator;
		bool bMapChanged = false;
		bool bActorChanged = false;

		if (SpawnOrUpdatePlacedActor(*ActorSubsystem, AppleBlueprint.GeneratedClass, InteractionPlacementLabelApple, AppleLocation, PlacementRotation, OutError, bActorChanged) == nullptr)
		{
			return false;
		}
		bMapChanged |= bActorChanged;

		if (SpawnOrUpdatePlacedActor(*ActorSubsystem, StrawberryBlueprint.GeneratedClass, InteractionPlacementLabelStrawberry, StrawberryLocation, PlacementRotation, OutError, bActorChanged) == nullptr)
		{
			return false;
		}
		bMapChanged |= bActorChanged;

		if (SpawnOrUpdatePlacedActor(*ActorSubsystem, WoodenSignBlueprint.GeneratedClass, InteractionPlacementLabelWoodenSign, WoodenSignLocation, PlacementRotation, OutError, bActorChanged) == nullptr)
		{
			return false;
		}
		bMapChanged |= bActorChanged;

		if (SpawnOrUpdatePlacedActor(*ActorSubsystem, WoodenSignScrollBlueprint.GeneratedClass, InteractionPlacementLabelWoodenSignScroll, WoodenSignScrollLocation, PlacementRotation, OutError, bActorChanged) == nullptr)
		{
			return false;
		}
		bMapChanged |= bActorChanged;

		if (SpawnOrUpdatePlacedActor(*ActorSubsystem, WoodenSignDualTileTransferBlueprint.GeneratedClass, InteractionPlacementLabelWoodenSignDualTileTransfer, WoodenSignDualTileTransferLocation, PlacementRotation, OutError, bActorChanged) == nullptr)
		{
			return false;
		}
		bMapChanged |= bActorChanged;

		if (SpawnOrUpdatePlacedActor(*ActorSubsystem, WoodenSignUIPlaygroundBlueprint.GeneratedClass, InteractionPlacementLabelWoodenSignUIPlayground, WoodenSignUIPlaygroundLocation, PlacementRotation, OutError, bActorChanged) == nullptr)
		{
			return false;
		}
		bMapChanged |= bActorChanged;

		if (!bMapChanged)
		{
			return true;
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
	UTexture2D* TileRoundedGradientTexture = CreateAsset<UTexture2D>(UIPath, TileRoundedGradientTextureName, TextureFactory);
	if (FilledCircleTexture == nullptr || OuterRingTexture == nullptr || SmileIconTexture == nullptr || TileRoundedGradientTexture == nullptr)
	{
		OutError = TEXT("Failed to create interaction texture assets.");
		return false;
	}

	const TArray64<uint8> FilledCirclePixels = BuildTexturePixels(64, 0.0f, 16.0f);
	const TArray64<uint8> OuterRingPixels = BuildTexturePixels(64, 20.0f, 24.0f);
	const TArray64<uint8> SmileIconPixels = BuildSmileTexturePixels(128);
	const TArray64<uint8> TileRoundedGradientPixels = BuildRoundedGradientTexturePixels(128, 18.0f);
	if (!HasGeneratedAssetVersion(*FilledCircleTexture, FilledCircleTextureVersion))
	{
		ConfigureTexture(*FilledCircleTexture, FilledCirclePixels, 64);
		SetGeneratedAssetVersion(*FilledCircleTexture, FilledCircleTextureVersion);
	}

	if (!HasGeneratedAssetVersion(*OuterRingTexture, OuterRingTextureVersion))
	{
		ConfigureTexture(*OuterRingTexture, OuterRingPixels, 64);
		SetGeneratedAssetVersion(*OuterRingTexture, OuterRingTextureVersion);
	}

	if (!HasGeneratedAssetVersion(*SmileIconTexture, SmileIconTextureVersion))
	{
		ConfigureTexture(*SmileIconTexture, SmileIconPixels, 128);
		SetGeneratedAssetVersion(*SmileIconTexture, SmileIconTextureVersion);
	}

	if (!HasGeneratedAssetVersion(*TileRoundedGradientTexture, TileRoundedGradientTextureVersion))
	{
		ConfigureTexture(*TileRoundedGradientTexture, TileRoundedGradientPixels, 128);
		SetGeneratedAssetVersion(*TileRoundedGradientTexture, TileRoundedGradientTextureVersion);
	}

	UWidgetBlueprint* IndicatorWidget = CreateWidgetBlueprint(UIPath, IndicatorWidgetName, UCodexInteractionIndicatorWidget::StaticClass());
	if (IndicatorWidget == nullptr)
	{
		OutError = TEXT("Failed to create WBP_InteractionIndicator.");
		return false;
	}

	if (!HasGeneratedAssetVersion(*IndicatorWidget, IndicatorWidgetVersion)
		&& !ConfigureInteractionWidgetBlueprint(*IndicatorWidget, *FilledCircleTexture, *OuterRingTexture, OutError))
	{
		return false;
	}

	if (!HasGeneratedAssetVersion(*IndicatorWidget, IndicatorWidgetVersion))
	{
		SetGeneratedAssetVersion(*IndicatorWidget, IndicatorWidgetVersion);
	}

	UWidgetBlueprint* MessagePopupWidget = CreateWidgetBlueprint(UIPath, MessagePopupWidgetName, UCodexInteractionMessagePopupWidget::StaticClass());
	if (MessagePopupWidget == nullptr)
	{
		OutError = TEXT("Failed to create WBP_InteractionMessagePopup.");
		return false;
	}

	if (!HasGeneratedAssetVersion(*MessagePopupWidget, MessagePopupWidgetVersion)
		&& !ConfigureMessagePopupWidgetBlueprint(*MessagePopupWidget, OutError))
	{
		return false;
	}

	if (!HasGeneratedAssetVersion(*MessagePopupWidget, MessagePopupWidgetVersion))
	{
		SetGeneratedAssetVersion(*MessagePopupWidget, MessagePopupWidgetVersion);
	}

	UWidgetBlueprint* ScrollMessagePopupWidget = CreateWidgetBlueprint(UIPath, ScrollMessagePopupWidgetName, UCodexInteractionScrollMessagePopupWidget::StaticClass());
	if (ScrollMessagePopupWidget == nullptr)
	{
		OutError = TEXT("Failed to create WBP_InteractionScrollMessagePopup.");
		return false;
	}

	if (!HasGeneratedAssetVersion(*ScrollMessagePopupWidget, ScrollMessagePopupWidgetVersion)
		&& !ConfigureScrollMessagePopupWidgetBlueprint(*ScrollMessagePopupWidget, *SmileIconTexture, OutError))
	{
		return false;
	}

	if (!HasGeneratedAssetVersion(*ScrollMessagePopupWidget, ScrollMessagePopupWidgetVersion))
	{
		SetGeneratedAssetVersion(*ScrollMessagePopupWidget, ScrollMessagePopupWidgetVersion);
	}

	UWidgetBlueprint* DualTileTransferTileEntryWidget = CreateWidgetBlueprint(
		UIPath,
		DualTileTransferTileEntryWidgetName,
		UCodexInteractionDualTileTransferTileEntryWidget::StaticClass());
	if (DualTileTransferTileEntryWidget == nullptr)
	{
		OutError = TEXT("Failed to create WBP_InteractionDualTileTransferTileEntry.");
		return false;
	}

	if (!HasGeneratedAssetVersion(*DualTileTransferTileEntryWidget, DualTileTransferTileEntryWidgetVersion)
		&& !ConfigureDualTileTransferTileEntryWidgetBlueprint(*DualTileTransferTileEntryWidget, *TileRoundedGradientTexture, OutError))
	{
		return false;
	}

	if (!HasGeneratedAssetVersion(*DualTileTransferTileEntryWidget, DualTileTransferTileEntryWidgetVersion))
	{
		SetGeneratedAssetVersion(*DualTileTransferTileEntryWidget, DualTileTransferTileEntryWidgetVersion);
	}

	UWidgetBlueprint* DualTileTransferPopupWidget = CreateWidgetBlueprint(
		UIPath,
		DualTileTransferPopupWidgetName,
		UCodexInteractionDualTileTransferPopupWidget::StaticClass());
	if (DualTileTransferPopupWidget == nullptr)
	{
		OutError = TEXT("Failed to create WBP_InteractionDualTileTransferPopup.");
		return false;
	}

	if (!HasGeneratedAssetVersion(*DualTileTransferPopupWidget, DualTileTransferPopupWidgetVersion)
		&& !ConfigureDualTileTransferPopupWidgetBlueprint(*DualTileTransferPopupWidget, DualTileTransferTileEntryWidget->GeneratedClass, OutError))
	{
		return false;
	}

	if (!HasGeneratedAssetVersion(*DualTileTransferPopupWidget, DualTileTransferPopupWidgetVersion))
	{
		SetGeneratedAssetVersion(*DualTileTransferPopupWidget, DualTileTransferPopupWidgetVersion);
	}

	UWidgetBlueprint* UIPlaygroundListEntryWidget = CreateWidgetBlueprint(
		UIPath,
		UIPlaygroundListEntryWidgetName,
		LoadNativeClass<UUserWidget>(TEXT("CodexInteractionUIPlaygroundListEntryWidget")));
	if (UIPlaygroundListEntryWidget == nullptr)
	{
		OutError = TEXT("Failed to create WBP_InteractionUIPlaygroundListEntry.");
		return false;
	}

	if (!HasGeneratedAssetVersion(*UIPlaygroundListEntryWidget, UIPlaygroundListEntryWidgetVersion)
		&& !ConfigureUIPlaygroundListEntryWidgetBlueprint(*UIPlaygroundListEntryWidget, OutError))
	{
		return false;
	}

	if (!HasGeneratedAssetVersion(*UIPlaygroundListEntryWidget, UIPlaygroundListEntryWidgetVersion))
	{
		SetGeneratedAssetVersion(*UIPlaygroundListEntryWidget, UIPlaygroundListEntryWidgetVersion);
	}

	UWidgetBlueprint* UIPlaygroundTileEntryWidget = CreateWidgetBlueprint(
		UIPath,
		UIPlaygroundTileEntryWidgetName,
		LoadNativeClass<UUserWidget>(TEXT("CodexInteractionUIPlaygroundTileEntryWidget")));
	if (UIPlaygroundTileEntryWidget == nullptr)
	{
		OutError = TEXT("Failed to create WBP_InteractionUIPlaygroundTileEntry.");
		return false;
	}

	if (!HasGeneratedAssetVersion(*UIPlaygroundTileEntryWidget, UIPlaygroundTileEntryWidgetVersion)
		&& !ConfigureUIPlaygroundTileEntryWidgetBlueprint(*UIPlaygroundTileEntryWidget, *TileRoundedGradientTexture, OutError))
	{
		return false;
	}

	if (!HasGeneratedAssetVersion(*UIPlaygroundTileEntryWidget, UIPlaygroundTileEntryWidgetVersion))
	{
		SetGeneratedAssetVersion(*UIPlaygroundTileEntryWidget, UIPlaygroundTileEntryWidgetVersion);
	}

	UWidgetBlueprint* UIPlaygroundPopupWidget = CreateWidgetBlueprint(
		UIPath,
		UIPlaygroundPopupWidgetName,
		LoadNativeClass<UUserWidget>(TEXT("CodexInteractionUIPlaygroundPopupWidget")));
	if (UIPlaygroundPopupWidget == nullptr)
	{
		OutError = TEXT("Failed to create WBP_InteractionUIPlaygroundPopup.");
		return false;
	}

	if (!HasGeneratedAssetVersion(*UIPlaygroundPopupWidget, UIPlaygroundPopupWidgetVersion)
		&& !ConfigureUIPlaygroundPopupWidgetBlueprint(
			*UIPlaygroundPopupWidget,
			UIPlaygroundListEntryWidget->GeneratedClass,
			UIPlaygroundTileEntryWidget->GeneratedClass,
			*FilledCircleTexture,
			*OuterRingTexture,
			*SmileIconTexture,
			OutError))
	{
		return false;
	}

	if (!HasGeneratedAssetVersion(*UIPlaygroundPopupWidget, UIPlaygroundPopupWidgetVersion))
	{
		SetGeneratedAssetVersion(*UIPlaygroundPopupWidget, UIPlaygroundPopupWidgetVersion);
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
	UBlueprint* WoodenSignDualTileTransferBlueprint = CreateBlueprint(
		BlueprintsPath,
		InteractableWoodenSignDualTileTransferPopupName,
		ACodexDualTileTransferPopupInteractableActor::StaticClass());
	UBlueprint* WoodenSignUIPlaygroundBlueprint = CreateBlueprint(
		BlueprintsPath,
		InteractableWoodenSignUIPlaygroundPopupName,
		LoadNativeClass<ACodexPopupInteractableActor>(TEXT("CodexUIPlaygroundPopupInteractableActor")));
	if (AppleBlueprint == nullptr
		|| StrawberryBlueprint == nullptr
		|| WoodenSignBlueprint == nullptr
		|| WoodenSignScrollBlueprint == nullptr
		|| WoodenSignDualTileTransferBlueprint == nullptr
		|| WoodenSignUIPlaygroundBlueprint == nullptr)
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
	const FText DualTileTransferPromptText = FText::FromString(TEXT("\uC774\uB3D9\uD558\uAE30"));
	const FText DualTileTransferTitleText = FText::FromString(TEXT("\uBC88\uD638 \uC774\uB3D9 \uD14C\uC2A4\uD2B8"));
	const FText UIPlaygroundPromptText = FText::FromString(TEXT("UI Playground"));
	TArray<int32> DualTileTransferLeftNumbers;
	TArray<int32> DualTileTransferRightNumbers;
	for (int32 Number = 1; Number <= 5; ++Number)
	{
		DualTileTransferLeftNumbers.Add(Number);
	}

	for (int32 Number = 6; Number <= 10; ++Number)
	{
		DualTileTransferRightNumbers.Add(Number);
	}

	if (!HasGeneratedAssetVersion(*AppleBlueprint, AppleInteractableBlueprintVersion)
		&& !ConfigureInteractableBlueprint(*AppleBlueprint, *AppleMesh, EatPromptText, OutError))
	{
		return false;
	}

	if (!HasGeneratedAssetVersion(*AppleBlueprint, AppleInteractableBlueprintVersion))
	{
		SetGeneratedAssetVersion(*AppleBlueprint, AppleInteractableBlueprintVersion);
	}

	if (!HasGeneratedAssetVersion(*StrawberryBlueprint, StrawberryInteractableBlueprintVersion)
		&& !ConfigureInteractableBlueprint(*StrawberryBlueprint, *StrawberryMesh, EatPromptText, OutError))
	{
		return false;
	}

	if (!HasGeneratedAssetVersion(*StrawberryBlueprint, StrawberryInteractableBlueprintVersion))
	{
		SetGeneratedAssetVersion(*StrawberryBlueprint, StrawberryInteractableBlueprintVersion);
	}

	if (!HasGeneratedAssetVersion(*WoodenSignBlueprint, WoodenSignPopupBlueprintVersion)
		&& !ConfigurePopupInteractableBlueprint(
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

	if (!HasGeneratedAssetVersion(*WoodenSignBlueprint, WoodenSignPopupBlueprintVersion))
	{
		SetGeneratedAssetVersion(*WoodenSignBlueprint, WoodenSignPopupBlueprintVersion);
	}

	if (!HasGeneratedAssetVersion(*WoodenSignScrollBlueprint, WoodenSignScrollPopupBlueprintVersion)
		&& !ConfigurePopupInteractableBlueprint(
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

	if (!HasGeneratedAssetVersion(*WoodenSignScrollBlueprint, WoodenSignScrollPopupBlueprintVersion))
	{
		SetGeneratedAssetVersion(*WoodenSignScrollBlueprint, WoodenSignScrollPopupBlueprintVersion);
	}

	if (!HasGeneratedAssetVersion(*WoodenSignDualTileTransferBlueprint, WoodenSignDualTileTransferPopupBlueprintVersion)
		&& !ConfigureDualTileTransferPopupInteractableBlueprint(
			*WoodenSignDualTileTransferBlueprint,
			*WoodenSignMesh,
			DualTileTransferPromptText,
			DualTileTransferTitleText,
			DualTileTransferLeftNumbers,
			DualTileTransferRightNumbers,
			false,
			OutError))
	{
		return false;
	}

	if (!HasGeneratedAssetVersion(*WoodenSignDualTileTransferBlueprint, WoodenSignDualTileTransferPopupBlueprintVersion))
	{
		SetGeneratedAssetVersion(*WoodenSignDualTileTransferBlueprint, WoodenSignDualTileTransferPopupBlueprintVersion);
	}

	if (!HasGeneratedAssetVersion(*WoodenSignUIPlaygroundBlueprint, WoodenSignUIPlaygroundPopupBlueprintVersion)
		&& !ConfigureUIPlaygroundPopupInteractableBlueprint(
			*WoodenSignUIPlaygroundBlueprint,
			*WoodenSignMesh,
			UIPlaygroundPromptText,
			OutError))
	{
		return false;
	}

	if (!HasGeneratedAssetVersion(*WoodenSignUIPlaygroundBlueprint, WoodenSignUIPlaygroundPopupBlueprintVersion))
	{
		SetGeneratedAssetVersion(*WoodenSignUIPlaygroundBlueprint, WoodenSignUIPlaygroundPopupBlueprintVersion);
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
			TileRoundedGradientTexture,
			IndicatorWidget,
			MessagePopupWidget,
			ScrollMessagePopupWidget,
			DualTileTransferTileEntryWidget,
			DualTileTransferPopupWidget,
			UIPlaygroundListEntryWidget,
			UIPlaygroundTileEntryWidget,
			UIPlaygroundPopupWidget,
			AppleBlueprint,
			StrawberryBlueprint,
			WoodenSignBlueprint,
			WoodenSignScrollBlueprint,
			WoodenSignDualTileTransferBlueprint,
			WoodenSignUIPlaygroundBlueprint
		});

	if (!PlaceInteractionTestActorsInMap(
		*AppleBlueprint,
		*StrawberryBlueprint,
		*WoodenSignBlueprint,
		*WoodenSignScrollBlueprint,
		*WoodenSignDualTileTransferBlueprint,
		*WoodenSignUIPlaygroundBlueprint,
		OutError))
	{
		return false;
	}

	return true;
}
