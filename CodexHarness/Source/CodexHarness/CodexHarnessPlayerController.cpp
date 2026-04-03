#include "CodexHarnessPlayerController.h"

#include "CodexHarnessCharacter.h"
#include "CodexHarnessGameInstance.h"
#include "CodexHarnessGameMode.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Input/CodexHarnessInputConfigDataAsset.h"
#include "InputActionValue.h"

ACodexHarnessPlayerController::ACodexHarnessPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = false;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ACodexHarnessPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	ApplyInputMappingContext();
}

void ACodexHarnessPlayerController::PlayerTick(const float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	UpdateAimFromCursor();
}

void ACodexHarnessPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	const UCodexHarnessInputConfigDataAsset* const InputConfig = GetInputConfig();
	UEnhancedInputComponent* const EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (InputConfig == nullptr || EnhancedInputComponent == nullptr)
	{
		return;
	}

	if (InputConfig->MoveAction != nullptr)
	{
		EnhancedInputComponent->BindAction(InputConfig->MoveAction, ETriggerEvent::Triggered, this, &ThisClass::HandleMove);
		EnhancedInputComponent->BindAction(InputConfig->MoveAction, ETriggerEvent::Completed, this, &ThisClass::HandleMove);
	}

	if (InputConfig->FireAction != nullptr)
	{
		EnhancedInputComponent->BindAction(InputConfig->FireAction, ETriggerEvent::Started, this, &ThisClass::HandleFireStarted);
	}

	if (InputConfig->RestartAction != nullptr)
	{
		EnhancedInputComponent->BindAction(InputConfig->RestartAction, ETriggerEvent::Started, this, &ThisClass::HandleRestartStarted);
	}
}

const UCodexHarnessInputConfigDataAsset* ACodexHarnessPlayerController::GetInputConfig() const
{
	const UCodexHarnessGameInstance* const GameInstance = Cast<UCodexHarnessGameInstance>(GetGameInstance());
	return GameInstance != nullptr ? GameInstance->GetInputConfig() : nullptr;
}

void ACodexHarnessPlayerController::ApplyInputMappingContext()
{
	const UCodexHarnessInputConfigDataAsset* const InputConfig = GetInputConfig();
	ULocalPlayer* const LocalPlayer = GetLocalPlayer();
	if (InputConfig == nullptr || InputConfig->InputMappingContext == nullptr || LocalPlayer == nullptr)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* const InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		InputSubsystem->AddMappingContext(InputConfig->InputMappingContext, InputMappingPriority);
	}
}

void ACodexHarnessPlayerController::HandleMove(const FInputActionValue& InputValue)
{
	ACodexHarnessCharacter* const ControlledCharacter = Cast<ACodexHarnessCharacter>(GetPawn());
	if (ControlledCharacter == nullptr || !ControlledCharacter->IsAlive())
	{
		return;
	}

	ControlledCharacter->MoveInTopDownPlane(InputValue.Get<FVector2D>());
}

void ACodexHarnessPlayerController::HandleFireStarted()
{
	ACodexHarnessCharacter* const ControlledCharacter = Cast<ACodexHarnessCharacter>(GetPawn());
	if (ControlledCharacter == nullptr || !ControlledCharacter->IsAlive())
	{
		return;
	}

	FVector CursorGroundPoint;
	if (TryGetCursorGroundPoint(CursorGroundPoint))
	{
		ControlledCharacter->FireAtWorldLocation(CursorGroundPoint);
		return;
	}

	ControlledCharacter->FireAtWorldLocation(ControlledCharacter->GetActorLocation() + (ControlledCharacter->GetActorForwardVector() * 1000.0f));
}

void ACodexHarnessPlayerController::HandleRestartStarted()
{
	if (ACodexHarnessGameMode* const CodexHarnessGameMode = GetWorld() != nullptr
		? GetWorld()->GetAuthGameMode<ACodexHarnessGameMode>()
		: nullptr)
	{
		CodexHarnessGameMode->RequestRestart();
	}
}

bool ACodexHarnessPlayerController::TryGetCursorGroundPoint(FVector& OutWorldPoint) const
{
	FVector MouseWorldLocation;
	FVector MouseWorldDirection;
	if (!DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection))
	{
		return false;
	}

	const APawn* const ControlledPawn = GetPawn();
	const float GroundHeight = ControlledPawn != nullptr ? ControlledPawn->GetActorLocation().Z : 0.0f;
	const FVector TraceEnd = MouseWorldLocation + (MouseWorldDirection * 100000.0f);
	const FPlane GroundPlane(FVector(0.0f, 0.0f, GroundHeight), FVector::UpVector);
	OutWorldPoint = FMath::LinePlaneIntersection(MouseWorldLocation, TraceEnd, GroundPlane);
	return true;
}

void ACodexHarnessPlayerController::UpdateAimFromCursor() const
{
	ACodexHarnessCharacter* const ControlledCharacter = Cast<ACodexHarnessCharacter>(GetPawn());
	if (ControlledCharacter == nullptr || !ControlledCharacter->IsAlive())
	{
		return;
	}

	FVector CursorGroundPoint;
	if (TryGetCursorGroundPoint(CursorGroundPoint))
	{
		ControlledCharacter->AimAtWorldLocation(CursorGroundPoint);
	}
}
