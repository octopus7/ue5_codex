#include "Systems/Input/CMWTopDownPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Pawn.h"
#include "InputMappingContext.h"
#include "Math/Plane.h"
#include "Systems/Game/CMWGameDataAsset.h"
#include "Systems/Game/CMWGameInstance.h"
#include "Systems/Input/CMWTopDownCharacter.h"
#include "Systems/UI/CMWMinimapSubsystem.h"
#include "Systems/UI/CMWMinimapWidget.h"

ACMWTopDownPlayerController::ACMWTopDownPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ACMWTopDownPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ApplyGameplayInputMode();
	InitializeInputMappingContext();
	EnsureMinimapWidget();
}

void ACMWTopDownPlayerController::BeginPlayingState()
{
	Super::BeginPlayingState();

	ApplyGameplayInputMode();
	InitializeInputMappingContext();
	EnsureMinimapWidget();
}

void ACMWTopDownPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (ResolveCursorAimLocation(CachedAimWorldLocation))
	{
		bHasAimWorldLocation = true;

		if (ACMWTopDownCharacter* ControlledCharacter = GetPawn<ACMWTopDownCharacter>())
		{
			ControlledCharacter->SetAimWorldLocation(CachedAimWorldLocation);
		}
	}

	if (APawn* ControlledPawn = GetPawn())
	{
		if (UCMWMinimapSubsystem* MinimapSubsystem = GetWorld() ? GetWorld()->GetSubsystem<UCMWMinimapSubsystem>() : nullptr)
		{
			MinimapSubsystem->NotifyPlayerLocation(ControlledPawn->GetActorLocation());
		}
	}
}

void ACMWTopDownPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ApplyGameplayInputMode();
	InitializeInputMappingContext();
	EnsureMinimapWidget();
}

bool ACMWTopDownPlayerController::GetAimWorldLocation(FVector& OutAimWorldLocation) const
{
	if (!bHasAimWorldLocation)
	{
		return false;
	}

	OutAimWorldLocation = CachedAimWorldLocation;
	return true;
}

void ACMWTopDownPlayerController::ApplyGameplayInputMode() const
{
	FInputModeGameOnly InputMode;
	const_cast<ThisClass*>(this)->SetInputMode(InputMode);
	const_cast<ThisClass*>(this)->SetShowMouseCursor(true);
	const_cast<ThisClass*>(this)->SetIgnoreMoveInput(false);
	const_cast<ThisClass*>(this)->SetIgnoreLookInput(false);
}

void ACMWTopDownPlayerController::InitializeInputMappingContext() const
{
	const UCMWGameInstance* GameInstance = UCMWGameInstance::Get(this);
	const UCMWGameDataAsset* GameData = GameInstance ? GameInstance->GetGameData() : nullptr;
	if (!GameData || !GameData->PlayerMappingContext)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		EnhancedInputSubsystem->ClearAllMappings();
		EnhancedInputSubsystem->AddMappingContext(GameData->PlayerMappingContext, 0);
	}
}

void ACMWTopDownPlayerController::EnsureMinimapWidget()
{
	if (MinimapWidget || !IsLocalController())
	{
		return;
	}

	const UCMWGameInstance* GameInstance = UCMWGameInstance::Get(this);
	const UCMWGameDataAsset* GameData = GameInstance ? GameInstance->GetGameData() : nullptr;

	TSubclassOf<UUserWidget> WidgetClass = UCMWMinimapWidget::StaticClass();
	if (GameData && GameData->MinimapWidgetClass)
	{
		WidgetClass = GameData->MinimapWidgetClass;
	}

	MinimapWidget = CreateWidget<UUserWidget>(this, WidgetClass);
	if (!MinimapWidget)
	{
		return;
	}

	MinimapWidget->AddToViewport(10);

	if (GameData)
	{
		MinimapWidget->SetDesiredSizeInViewport(GameData->MinimapSize);
		MinimapWidget->SetAnchorsInViewport(GameData->MinimapAnchors);
		MinimapWidget->SetAlignmentInViewport(GameData->MinimapAlignment);
		MinimapWidget->SetPositionInViewport(GameData->MinimapPosition, false);
	}
}

bool ACMWTopDownPlayerController::ResolveCursorAimLocation(FVector& OutAimWorldLocation) const
{
	FHitResult HitResult;
	if (GetHitResultUnderCursor(ECC_Visibility, true, HitResult))
	{
		OutAimWorldLocation = HitResult.ImpactPoint;
		return true;
	}

	FVector WorldOrigin;
	FVector WorldDirection;
	if (!DeprojectMousePositionToWorld(WorldOrigin, WorldDirection))
	{
		return false;
	}

	const APawn* ControlledPawn = GetPawn();
	const float PlaneHeight = ControlledPawn ? ControlledPawn->GetActorLocation().Z : 0.0f;
	const FPlane GroundPlane(FVector(0.0f, 0.0f, PlaneHeight), FVector::UpVector);
	OutAimWorldLocation = FMath::RayPlaneIntersection(WorldOrigin, WorldDirection, GroundPlane);
	return true;
}
