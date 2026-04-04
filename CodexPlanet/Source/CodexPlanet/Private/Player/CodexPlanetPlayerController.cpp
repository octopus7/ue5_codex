// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/CodexPlanetPlayerController.h"

#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "Interaction/CodexPlacementSurface.h"
#include "Planets/CodexPlanetActor.h"
#include "Player/CodexPlanetViewPawn.h"
#include "Props/CodexPlaceablePropActor.h"
#include "Props/CodexPlacementPreviewActor.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Engine/StaticMesh.h"

ACodexPlanetPlayerController::ACodexPlanetPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	InitializeInputDefinitions();
}

void ACodexPlanetPlayerController::BeginPlay()
{
	Super::BeginPlay();

	LoadPlacementCatalog();
	RemainingPlacementSlots = MaxPlacementSlots;
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	RefreshInputMode();
	RegisterInputMapping();
	ResolveObservedPlanet();
	ApplyPlanetFocus();
	EnsureOrbitControlsWidget();
	RefreshOrbitControlsWidget();
}

void ACodexPlanetPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ApplyPlanetFocus();
}

void ACodexPlanetPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	UpdatePlacementPreview();
}

void ACodexPlanetPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (DragHoldAction)
		{
			EnhancedInputComponent->BindAction(DragHoldAction, ETriggerEvent::Started, this, &ACodexPlanetPlayerController::HandleDragHoldStarted);
			EnhancedInputComponent->BindAction(DragHoldAction, ETriggerEvent::Completed, this, &ACodexPlanetPlayerController::HandleDragHoldEnded);
			EnhancedInputComponent->BindAction(DragHoldAction, ETriggerEvent::Canceled, this, &ACodexPlanetPlayerController::HandleDragHoldEnded);
		}

		if (DragRotateAction)
		{
			EnhancedInputComponent->BindAction(DragRotateAction, ETriggerEvent::Triggered, this, &ACodexPlanetPlayerController::HandleDragRotate);
		}

		if (TogglePlacementModeAction)
		{
			EnhancedInputComponent->BindAction(TogglePlacementModeAction, ETriggerEvent::Started, this, &ACodexPlanetPlayerController::HandleTogglePlacementMode);
		}

		if (ConfirmPlacementAction)
		{
			EnhancedInputComponent->BindAction(ConfirmPlacementAction, ETriggerEvent::Started, this, &ACodexPlanetPlayerController::HandlePlacementConfirm);
		}

		if (CancelPlacementAction)
		{
			EnhancedInputComponent->BindAction(CancelPlacementAction, ETriggerEvent::Started, this, &ACodexPlanetPlayerController::HandlePlacementCancel);
		}

		if (SelectNextPlacementPropAction)
		{
			EnhancedInputComponent->BindAction(SelectNextPlacementPropAction, ETriggerEvent::Started, this, &ACodexPlanetPlayerController::HandleSelectNextPlacementProp);
		}

		if (SelectPreviousPlacementPropAction)
		{
			EnhancedInputComponent->BindAction(SelectPreviousPlacementPropAction, ETriggerEvent::Started, this, &ACodexPlanetPlayerController::HandleSelectPreviousPlacementProp);
		}
	}
}

ACodexPlanetActor* ACodexPlanetPlayerController::GetObservedPlanet() const
{
	return ObservedPlanet;
}

FQuat ACodexPlanetPlayerController::BuildTrackballDeltaRotation(const FRotator& CameraRotation, const FVector2D& DragDelta, const float DegreesPerUnit)
{
	if (DragDelta.IsNearlyZero() || DegreesPerUnit <= 0.0f)
	{
		return FQuat::Identity;
	}

	const FRotationMatrix CameraBasis(CameraRotation);
	FVector RotationAxis = (CameraBasis.GetScaledAxis(EAxis::Z) * -DragDelta.X) + (CameraBasis.GetScaledAxis(EAxis::Y) * DragDelta.Y);
	const float RotationMagnitude = RotationAxis.Size();

	if (RotationMagnitude <= KINDA_SMALL_NUMBER)
	{
		return FQuat::Identity;
	}

	RotationAxis /= RotationMagnitude;

	const float RotationAngleDegrees = RotationMagnitude * DegreesPerUnit;
	return FQuat(RotationAxis, FMath::DegreesToRadians(RotationAngleDegrees));
}

FQuat ACodexPlanetPlayerController::BuildSurfaceAlignedRotation(const FVector& SurfaceNormal, const FVector& ViewForward)
{
	const FVector SafeSurfaceNormal = SurfaceNormal.GetSafeNormal();

	if (SafeSurfaceNormal.IsNearlyZero())
	{
		return FQuat::Identity;
	}

	FVector ProjectedForward = FVector::VectorPlaneProject(ViewForward, SafeSurfaceNormal).GetSafeNormal();

	if (ProjectedForward.IsNearlyZero())
	{
		ProjectedForward = FVector::VectorPlaneProject(FVector::ForwardVector, SafeSurfaceNormal).GetSafeNormal();
	}

	if (ProjectedForward.IsNearlyZero())
	{
		ProjectedForward = FVector::VectorPlaneProject(FVector::RightVector, SafeSurfaceNormal).GetSafeNormal();
	}

	return FRotationMatrix::MakeFromZX(SafeSurfaceNormal, ProjectedForward).ToQuat();
}

void ACodexPlanetPlayerController::SetObservedPlanet(ACodexPlanetActor* InPlanet)
{
	ObservedPlanet = InPlanet;
	ApplyPlanetFocus();
}

void ACodexPlanetPlayerController::LoadPlacementCatalog()
{
	PlacementCatalog.Reset();

	struct FCatalogSeed
	{
		const TCHAR* Id;
		const TCHAR* AssetPath;
		FVector PlacementScale;
	};

	const TArray<FCatalogSeed> CatalogSeeds = {
		{TEXT("Bridge"), TEXT("/Game/Props/Vox/SM_VOX_Bridge.SM_VOX_Bridge"), FVector(0.55f, 0.55f, 0.55f)},
		{TEXT("Statue"), TEXT("/Game/Props/Vox/SM_VOX_Statue.SM_VOX_Statue"), FVector(0.55f, 0.55f, 0.55f)},
		{TEXT("PalmTree"), TEXT("/Game/Props/Vox/SM_VOX_PalmTree.SM_VOX_PalmTree"), FVector(0.50f, 0.50f, 0.50f)},
		{TEXT("Rock"), TEXT("/Game/Props/Vox/SM_VOX_Rock.SM_VOX_Rock"), FVector(0.65f, 0.65f, 0.65f)},
		{TEXT("Bush"), TEXT("/Game/Props/Vox/SM_VOX_Bush.SM_VOX_Bush"), FVector(0.60f, 0.60f, 0.60f)},
	};

	for (const FCatalogSeed& CatalogSeed : CatalogSeeds)
	{
		if (UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, CatalogSeed.AssetPath))
		{
			FCodexPlacementCatalogEntry Entry;
			Entry.Id = CatalogSeed.Id;
			Entry.Mesh = Mesh;
			Entry.PlacementScale = CatalogSeed.PlacementScale;
			PlacementCatalog.Add(Entry);
		}
	}

	if (PlacementCatalog.Num() == 0)
	{
		if (UStaticMesh* FallbackMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")))
		{
			FCodexPlacementCatalogEntry Entry;
			Entry.Id = TEXT("FallbackCube");
			Entry.Mesh = FallbackMesh;
			Entry.PlacementScale = FVector(0.7f, 0.7f, 0.7f);
			PlacementCatalog.Add(Entry);
		}
	}

	ActivePlacementCatalogIndex = 0;
}

const FCodexPlacementCatalogEntry* ACodexPlanetPlayerController::GetActivePlacementCatalogEntry() const
{
	return PlacementCatalog.IsValidIndex(ActivePlacementCatalogIndex) ? &PlacementCatalog[ActivePlacementCatalogIndex] : nullptr;
}

void ACodexPlanetPlayerController::InitializeInputDefinitions()
{
	if (!PlanetViewMappingContext)
	{
		PlanetViewMappingContext = CreateDefaultSubobject<UInputMappingContext>(TEXT("PlanetViewMappingContext"));
	}

	if (!DragRotateAction)
	{
		DragRotateAction = CreateDefaultSubobject<UInputAction>(TEXT("DragRotateAction"));
		DragRotateAction->ValueType = EInputActionValueType::Axis2D;
	}

	if (!DragHoldAction)
	{
		DragHoldAction = CreateDefaultSubobject<UInputAction>(TEXT("DragHoldAction"));
		DragHoldAction->ValueType = EInputActionValueType::Boolean;
	}

	if (!TogglePlacementModeAction)
	{
		TogglePlacementModeAction = CreateDefaultSubobject<UInputAction>(TEXT("TogglePlacementModeAction"));
		TogglePlacementModeAction->ValueType = EInputActionValueType::Boolean;
	}

	if (!ConfirmPlacementAction)
	{
		ConfirmPlacementAction = CreateDefaultSubobject<UInputAction>(TEXT("ConfirmPlacementAction"));
		ConfirmPlacementAction->ValueType = EInputActionValueType::Boolean;
	}

	if (!CancelPlacementAction)
	{
		CancelPlacementAction = CreateDefaultSubobject<UInputAction>(TEXT("CancelPlacementAction"));
		CancelPlacementAction->ValueType = EInputActionValueType::Boolean;
	}

	if (!SelectNextPlacementPropAction)
	{
		SelectNextPlacementPropAction = CreateDefaultSubobject<UInputAction>(TEXT("SelectNextPlacementPropAction"));
		SelectNextPlacementPropAction->ValueType = EInputActionValueType::Boolean;
	}

	if (!SelectPreviousPlacementPropAction)
	{
		SelectPreviousPlacementPropAction = CreateDefaultSubobject<UInputAction>(TEXT("SelectPreviousPlacementPropAction"));
		SelectPreviousPlacementPropAction->ValueType = EInputActionValueType::Boolean;
	}

	if (PlanetViewMappingContext)
	{
		PlanetViewMappingContext->MapKey(DragRotateAction, EKeys::Mouse2D);
		PlanetViewMappingContext->MapKey(DragHoldAction, EKeys::LeftMouseButton);
		PlanetViewMappingContext->MapKey(TogglePlacementModeAction, EKeys::Tab);
		PlanetViewMappingContext->MapKey(ConfirmPlacementAction, EKeys::LeftMouseButton);
		PlanetViewMappingContext->MapKey(CancelPlacementAction, EKeys::RightMouseButton);
		PlanetViewMappingContext->MapKey(SelectNextPlacementPropAction, EKeys::E);
		PlanetViewMappingContext->MapKey(SelectPreviousPlacementPropAction, EKeys::Q);
	}
}

void ACodexPlanetPlayerController::RegisterInputMapping() const
{
	if (!PlanetViewMappingContext)
	{
		return;
	}

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSubsystem->AddMappingContext(PlanetViewMappingContext, 0);
		}
	}
}

void ACodexPlanetPlayerController::ResolveObservedPlanet()
{
	if (ObservedPlanet || !GetWorld())
	{
		return;
	}

	for (TActorIterator<ACodexPlanetActor> It(GetWorld()); It; ++It)
	{
		ObservedPlanet = *It;
		break;
	}
}

void ACodexPlanetPlayerController::ApplyPlanetFocus()
{
	ResolveObservedPlanet();

	if (ACodexPlanetViewPawn* ViewPawn = Cast<ACodexPlanetViewPawn>(GetPawn()))
	{
		ViewPawn->SetFocusActor(ObservedPlanet);
	}
}

void ACodexPlanetPlayerController::RefreshInputMode()
{
	if (bPlacementModeActive)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
	}
	else
	{
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}

	bShowMouseCursor = true;
}

void ACodexPlanetPlayerController::HandleDragHoldStarted()
{
	if (bPlacementModeActive)
	{
		return;
	}

	bTrackballDragActive = true;
}

void ACodexPlanetPlayerController::HandleDragHoldEnded()
{
	bTrackballDragActive = false;
}

void ACodexPlanetPlayerController::HandleDragRotate(const FInputActionValue& InputValue)
{
	if (!bTrackballDragActive || bPlacementModeActive)
	{
		return;
	}

	const FVector2D DragDelta = InputValue.Get<FVector2D>();

	if (DragDelta.IsNearlyZero())
	{
		return;
	}

	ApplyTrackballRotation(DragDelta);
}

void ACodexPlanetPlayerController::ApplyTrackballRotation(const FVector2D& DragDelta)
{
	ResolveObservedPlanet();

	if (!ObservedPlanet || !PlayerCameraManager)
	{
		return;
	}

	const FQuat DeltaRotation = BuildTrackballDeltaRotation(PlayerCameraManager->GetCameraRotation(), DragDelta, DragRotationDegreesPerUnit);

	if (!DeltaRotation.Equals(FQuat::Identity))
	{
		ObservedPlanet->ApplyTrackballRotation(DeltaRotation);
	}
}

void ACodexPlanetPlayerController::HandleTogglePlacementMode()
{
	SetPlacementModeActive(!bPlacementModeActive);
}

void ACodexPlanetPlayerController::HandlePlacementConfirm()
{
	if (!bPlacementModeActive || !bHasValidPlacementPreview || RemainingPlacementSlots <= 0)
	{
		if (RemainingPlacementSlots <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Placement blocked because no slots remain."));
		}

		return;
	}

	if (SpawnPlacedProp(CachedPlacementSurfaceHit))
	{
		RemainingPlacementSlots = FMath::Max(0, RemainingPlacementSlots - 1);
		RefreshOrbitControlsWidget();
		UpdatePlacementPreview();
	}
}

void ACodexPlanetPlayerController::HandlePlacementCancel()
{
	if (bPlacementModeActive)
	{
		SetPlacementModeActive(false);
	}
}

void ACodexPlanetPlayerController::HandleSelectNextPlacementProp()
{
	CyclePlacementCatalog(1);
}

void ACodexPlanetPlayerController::HandleSelectPreviousPlacementProp()
{
	CyclePlacementCatalog(-1);
}

void ACodexPlanetPlayerController::CyclePlacementCatalog(const int32 Direction)
{
	if (PlacementCatalog.Num() == 0 || Direction == 0)
	{
		return;
	}

	ActivePlacementCatalogIndex = (ActivePlacementCatalogIndex + Direction + PlacementCatalog.Num()) % PlacementCatalog.Num();

	if (const FCodexPlacementCatalogEntry* ActiveEntry = GetActivePlacementCatalogEntry())
	{
		UE_LOG(LogTemp, Log, TEXT("Selected placement prop: %s"), *ActiveEntry->Id.ToString());
	}

	RefreshOrbitControlsWidget();

	if (bPlacementModeActive)
	{
		UpdatePlacementPreview();
	}
}

void ACodexPlanetPlayerController::EnsureOrbitControlsWidget()
{
	if (OrbitControlsWidgetInstance)
	{
		return;
	}

	UClass* WidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("/Game/UI/WBP_PlanetOrbitControls.WBP_PlanetOrbitControls_C"));

	if (!WidgetClass)
	{
		return;
	}

	OrbitControlsWidgetInstance = CreateWidget<UUserWidget>(this, WidgetClass);

	if (OrbitControlsWidgetInstance)
	{
		OrbitControlsWidgetInstance->AddToViewport(5);
		UE_LOG(LogTemp, Log, TEXT("Orbit controls widget created and added to viewport."));
	}
}

void ACodexPlanetPlayerController::RefreshOrbitControlsWidget()
{
	const FCodexPlacementCatalogEntry* ActiveEntry = GetActivePlacementCatalogEntry();
	const FString SelectedPropName = ActiveEntry ? ActiveEntry->Id.ToString() : TEXT("None");
	const FString FooterMessage = FString::Printf(TEXT("Selected: %s | Slots Left: %d"), *SelectedPropName, RemainingPlacementSlots);

	if (OrbitControlsWidgetInstance)
	{
		if (UTextBlock* FooterText = Cast<UTextBlock>(OrbitControlsWidgetInstance->GetWidgetFromName(TEXT("FooterText"))))
		{
			FooterText->SetText(FText::FromString(FooterMessage));
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Placement HUD state | Selected=%s | Slots=%d"), *SelectedPropName, RemainingPlacementSlots);
}

void ACodexPlanetPlayerController::SetPlacementModeActive(const bool bEnabled)
{
	bPlacementModeActive = bEnabled;
	bTrackballDragActive = false;
	RefreshInputMode();

	if (!bPlacementModeActive)
	{
		HidePlacementPreview();
	}
	else
	{
		UpdatePlacementPreview();
	}
}

void ACodexPlanetPlayerController::EnsurePlacementPreviewActor()
{
	if (PlacementPreviewActor || !GetWorld())
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PlacementPreviewActor = GetWorld()->SpawnActor<ACodexPlacementPreviewActor>(ACodexPlacementPreviewActor::StaticClass(), FTransform::Identity, SpawnParameters);

	if (PlacementPreviewActor)
	{
		if (const FCodexPlacementCatalogEntry* ActiveEntry = GetActivePlacementCatalogEntry())
		{
			PlacementPreviewActor->ConfigurePreviewMesh(ActiveEntry->Mesh);
		}

		PlacementPreviewActor->SetActorHiddenInGame(true);
	}
}

void ACodexPlanetPlayerController::UpdatePlacementPreview()
{
	if (!bPlacementModeActive)
	{
		return;
	}

	if (RemainingPlacementSlots <= 0)
	{
		HidePlacementPreview();
		return;
	}

	EnsurePlacementPreviewActor();

	const FCodexPlacementCatalogEntry* ActiveEntry = GetActivePlacementCatalogEntry();

	if (!PlacementPreviewActor || !ActiveEntry || !ActiveEntry->Mesh)
	{
		return;
	}

	FCodexPlacementSurfaceHit SurfaceHit;

	if (!ResolvePlacementSurfaceUnderCursor(SurfaceHit))
	{
		HidePlacementPreview();
		return;
	}

	CachedPlacementSurfaceHit = SurfaceHit;
	bHasValidPlacementPreview = true;
	PlacementPreviewActor->ConfigurePreviewMesh(ActiveEntry->Mesh);
	PlacementPreviewActor->SetActorTransform(BuildPlacementTransform(SurfaceHit));
	PlacementPreviewActor->SetActorHiddenInGame(false);
}

void ACodexPlanetPlayerController::HidePlacementPreview()
{
	bHasValidPlacementPreview = false;

	if (PlacementPreviewActor)
	{
		PlacementPreviewActor->SetActorHiddenInGame(true);
	}
}

bool ACodexPlanetPlayerController::ResolvePlacementSurfaceUnderCursor(FCodexPlacementSurfaceHit& OutSurfaceHit) const
{
	FHitResult HitResult;

	if (!GetHitResultUnderCursor(ECC_Visibility, true, HitResult))
	{
		return false;
	}

	AActor* HitActor = HitResult.GetActor();

	if (!IsValid(HitActor) || !HitActor->GetClass()->ImplementsInterface(UCodexPlacementSurface::StaticClass()))
	{
		return false;
	}

	if (ICodexPlacementSurface* PlacementSurface = Cast<ICodexPlacementSurface>(HitActor))
	{
		return PlacementSurface->ResolvePlacementSurfaceHit(HitResult, OutSurfaceHit);
	}

	return false;
}

FTransform ACodexPlanetPlayerController::BuildPlacementTransform(const FCodexPlacementSurfaceHit& SurfaceHit) const
{
	const FCodexPlacementCatalogEntry* ActiveEntry = GetActivePlacementCatalogEntry();
	const FVector ViewForward = PlayerCameraManager ? PlayerCameraManager->GetCameraRotation().Vector() : FVector::ForwardVector;
	const FQuat AlignmentRotation = BuildSurfaceAlignedRotation(SurfaceHit.WorldNormal, ViewForward);
	const FVector PlacementLocation = SurfaceHit.WorldPosition + (SurfaceHit.WorldNormal.GetSafeNormal() * GetPlacementSurfaceOffset());

	return FTransform(AlignmentRotation, PlacementLocation, ActiveEntry ? ActiveEntry->PlacementScale : FVector::OneVector);
}

float ACodexPlanetPlayerController::GetPlacementSurfaceOffset() const
{
	const FCodexPlacementCatalogEntry* ActiveEntry = GetActivePlacementCatalogEntry();

	if (!ActiveEntry || !ActiveEntry->Mesh)
	{
		return PlacementSurfacePadding;
	}

	return (ActiveEntry->Mesh->GetBounds().BoxExtent.Z * ActiveEntry->PlacementScale.Z) + PlacementSurfacePadding;
}

bool ACodexPlanetPlayerController::SpawnPlacedProp(const FCodexPlacementSurfaceHit& SurfaceHit)
{
	const FCodexPlacementCatalogEntry* ActiveEntry = GetActivePlacementCatalogEntry();

	if (!GetWorld() || !ActiveEntry || !ActiveEntry->Mesh)
	{
		return false;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (ACodexPlaceablePropActor* PlacedPropActor = GetWorld()->SpawnActor<ACodexPlaceablePropActor>(ACodexPlaceablePropActor::StaticClass(), BuildPlacementTransform(SurfaceHit), SpawnParameters))
	{
		PlacedPropActor->ConfigurePropMesh(ActiveEntry->Mesh);
		return true;
	}

	return false;
}
