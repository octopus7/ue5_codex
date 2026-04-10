// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CodexInteractionAssetBuilder.h"

#include "CodexTopDownInputConfigDataAsset.h"
#include "Interaction/CodexInteractableActor.h"
#include "Interaction/CodexInteractionAssetPaths.h"
#include "Interaction/CodexInteractionComponent.h"
#include "Interaction/CodexInteractionIndicatorWidget.h"
#include "Interaction/CodexInteractionTypes.h"
#include "AssetToolsModule.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/TextBlock.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "Factories/BlueprintFactory.h"
#include "Factories/Texture2dFactoryNew.h"
#include "FileHelpers.h"
#include "GameFramework/PlayerStart.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "InputEditorModule.h"
#include "InputMappingContext.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/PackageName.h"
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
	static const FName InteractionPlacementTag = TEXT("CodexInteractionTestPlacement");

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

	void ConfigureInteractMappings(UInputMappingContext& MappingContext, UInputAction& InteractAction)
	{
		MappingContext.UnmapAllKeysFromAction(&InteractAction);

		FEnhancedActionKeyMapping& FKeyMapping = MappingContext.MapKey(&InteractAction, EKeys::F);
		FKeyMapping.Modifiers.Reset();
		MappingContext.MarkPackageDirty();
	}

	void ConfigureInputConfig(UCodexTopDownInputConfigDataAsset& InputConfig, UInputMappingContext& MappingContext, UInputAction& InteractAction)
	{
		if (InputConfig.DefaultMappingContext == nullptr)
		{
			InputConfig.DefaultMappingContext = &MappingContext;
		}

		InputConfig.InteractAction = &InteractAction;
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

	bool PlaceInteractionTestActorsInMap(UBlueprint& AppleBlueprint, UBlueprint& StrawberryBlueprint, FString& OutError)
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
		const FRotator PlacementRotation = FRotator::ZeroRotator;

		if (SpawnOrUpdatePlacedActor(*ActorSubsystem, AppleBlueprint.GeneratedClass, InteractionPlacementLabelApple, AppleLocation, PlacementRotation, OutError) == nullptr)
		{
			return false;
		}

		if (SpawnOrUpdatePlacedActor(*ActorSubsystem, StrawberryBlueprint.GeneratedClass, InteractionPlacementLabelStrawberry, StrawberryLocation, PlacementRotation, OutError) == nullptr)
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

	EnsureDirectory(UIPath);
	EnsureDirectory(InputActionsPath);
	EnsureDirectory(InputContextsPath);
	EnsureDirectory(InputDataPath);
	EnsureDirectory(BlueprintsPath);

	UInputAction_Factory* InputActionFactory = NewObject<UInputAction_Factory>();
	InputActionFactory->InputActionClass = UInputAction::StaticClass();
	InputActionFactory->bEditAfterNew = false;

	UInputAction* InteractAction = CreateAsset<UInputAction>(InputActionsPath, InteractActionName, InputActionFactory);

	UInputMappingContext* MappingContext = LoadAsset<UInputMappingContext>(MappingContextObjectPath);
	UCodexTopDownInputConfigDataAsset* InputConfig = LoadAsset<UCodexTopDownInputConfigDataAsset>(InputConfigObjectPath);

	if (InteractAction == nullptr || MappingContext == nullptr || InputConfig == nullptr)
	{
		OutError = TEXT("Top-down input assets are missing. Run the base bootstrap before building interaction assets.");
		return false;
	}

	ConfigureInteractAction(*InteractAction);
	ConfigureInteractMappings(*MappingContext, *InteractAction);
	ConfigureInputConfig(*InputConfig, *MappingContext, *InteractAction);

	UTexture2DFactoryNew* TextureFactory = NewObject<UTexture2DFactoryNew>();
	TextureFactory->bEditAfterNew = false;

	UTexture2D* FilledCircleTexture = CreateAsset<UTexture2D>(UIPath, FilledCircleTextureName, TextureFactory);
	UTexture2D* OuterRingTexture = CreateAsset<UTexture2D>(UIPath, OuterRingTextureName, TextureFactory);
	if (FilledCircleTexture == nullptr || OuterRingTexture == nullptr)
	{
		OutError = TEXT("Failed to create interaction texture assets.");
		return false;
	}

	const TArray64<uint8> FilledCirclePixels = BuildTexturePixels(64, 0.0f, 16.0f);
	const TArray64<uint8> OuterRingPixels = BuildTexturePixels(64, 20.0f, 24.0f);
	ConfigureTexture(*FilledCircleTexture, FilledCirclePixels, 64);
	ConfigureTexture(*OuterRingTexture, OuterRingPixels, 64);

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

	UStaticMesh* AppleMesh = LoadAsset<UStaticMesh>(AppleMeshObjectPath);
	UStaticMesh* StrawberryMesh = LoadAsset<UStaticMesh>(StrawberryMeshObjectPath);
	if (AppleMesh == nullptr || StrawberryMesh == nullptr)
	{
		OutError = TEXT("Required food meshes were not found for the interaction test blueprints.");
		return false;
	}

	UBlueprint* AppleBlueprint = CreateBlueprint(BlueprintsPath, InteractableAppleName, ACodexInteractableActor::StaticClass());
	UBlueprint* StrawberryBlueprint = CreateBlueprint(BlueprintsPath, InteractableStrawberryName, ACodexInteractableActor::StaticClass());
	if (AppleBlueprint == nullptr || StrawberryBlueprint == nullptr)
	{
		OutError = TEXT("Failed to create one or more interactable test blueprints.");
		return false;
	}

	if (!ConfigureInteractableBlueprint(*AppleBlueprint, *AppleMesh, FText::FromString(TEXT("먹기")), OutError))
	{
		return false;
	}

	if (!ConfigureInteractableBlueprint(*StrawberryBlueprint, *StrawberryMesh, FText::FromString(TEXT("먹기")), OutError))
	{
		return false;
	}

	SaveAssets(
		{
			InteractAction,
			MappingContext,
			InputConfig,
			FilledCircleTexture,
			OuterRingTexture,
			IndicatorWidget,
			AppleBlueprint,
			StrawberryBlueprint
		});

	if (!PlaceInteractionTestActorsInMap(*AppleBlueprint, *StrawberryBlueprint, OutError))
	{
		return false;
	}

	return true;
}
