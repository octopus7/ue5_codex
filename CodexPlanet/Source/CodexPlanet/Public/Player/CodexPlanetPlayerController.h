// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interaction/CodexPlacementSurface.h"
#include "CodexPlanetPlayerController.generated.h"

class ACodexPlanetActor;
class ACodexPlacementPreviewActor;
class ACodexPlaceablePropActor;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UStaticMesh;
class UUserWidget;

USTRUCT()
struct FCodexPlacementCatalogEntry
{
	GENERATED_BODY()

	UPROPERTY()
	FName Id = NAME_None;

	UPROPERTY()
	TObjectPtr<UStaticMesh> Mesh = nullptr;

	UPROPERTY()
	FVector PlacementScale = FVector::OneVector;
};

UCLASS()
class CODEXPLANET_API ACodexPlanetPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACodexPlanetPlayerController();

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

	static FQuat BuildTrackballDeltaRotation(const FRotator& CameraRotation, const FVector2D& DragDelta, float DegreesPerUnit);
	static FQuat BuildSurfaceAlignedRotation(const FVector& SurfaceNormal, const FVector& ViewForward);

	ACodexPlanetActor* GetObservedPlanet() const;
	void SetObservedPlanet(ACodexPlanetActor* InPlanet);

private:
	UPROPERTY()
	TObjectPtr<UInputMappingContext> PlanetViewMappingContext = nullptr;

	UPROPERTY()
	TObjectPtr<UInputAction> DragRotateAction = nullptr;

	UPROPERTY()
	TObjectPtr<UInputAction> DragHoldAction = nullptr;

	UPROPERTY()
	TObjectPtr<UInputAction> TogglePlacementModeAction = nullptr;

	UPROPERTY()
	TObjectPtr<UInputAction> ConfirmPlacementAction = nullptr;

	UPROPERTY()
	TObjectPtr<UInputAction> CancelPlacementAction = nullptr;

	UPROPERTY()
	TObjectPtr<UInputAction> SelectNextPlacementPropAction = nullptr;

	UPROPERTY()
	TObjectPtr<UInputAction> SelectPreviousPlacementPropAction = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ACodexPlanetActor> ObservedPlanet = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ACodexPlacementPreviewActor> PlacementPreviewActor = nullptr;

	UPROPERTY(Transient)
	TArray<FCodexPlacementCatalogEntry> PlacementCatalog;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> OrbitControlsWidgetInstance = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	float DragRotationDegreesPerUnit = 0.16f;

	UPROPERTY(EditDefaultsOnly, Category = "Placement")
	float PlacementSurfacePadding = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Placement")
	int32 MaxPlacementSlots = 20;

	bool bTrackballDragActive = false;
	bool bPlacementModeActive = false;
	bool bHasValidPlacementPreview = false;
	int32 ActivePlacementCatalogIndex = 0;
	int32 RemainingPlacementSlots = 20;
	FCodexPlacementSurfaceHit CachedPlacementSurfaceHit;

	void InitializeInputDefinitions();
	void LoadPlacementCatalog();
	const FCodexPlacementCatalogEntry* GetActivePlacementCatalogEntry() const;
	void RegisterInputMapping() const;
	void ResolveObservedPlanet();
	void ApplyPlanetFocus();
	void RefreshInputMode();
	void HandleDragHoldStarted();
	void HandleDragHoldEnded();
	void HandleDragRotate(const FInputActionValue& InputValue);
	void ApplyTrackballRotation(const FVector2D& DragDelta);
	void HandleTogglePlacementMode();
	void HandlePlacementConfirm();
	void HandlePlacementCancel();
	void HandleSelectNextPlacementProp();
	void HandleSelectPreviousPlacementProp();
	void CyclePlacementCatalog(int32 Direction);
	void EnsureOrbitControlsWidget();
	void RefreshOrbitControlsWidget();
	void SetPlacementModeActive(bool bEnabled);
	void EnsurePlacementPreviewActor();
	void UpdatePlacementPreview();
	void HidePlacementPreview();
	bool ResolvePlacementSurfaceUnderCursor(FCodexPlacementSurfaceHit& OutSurfaceHit) const;
	FTransform BuildPlacementTransform(const FCodexPlacementSurfaceHit& SurfaceHit) const;
	float GetPlacementSurfaceOffset() const;
	bool SpawnPlacedProp(const FCodexPlacementSurfaceHit& SurfaceHit);
};
