// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenTopDownPlayerController.h"

#include "CodexInvenGameInstance.h"
#include "CodexInvenInputConfigDataAsset.h"
#include "CodexInvenTopDownCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/EngineTypes.h"
#include "Engine/LocalPlayer.h"

ACodexInvenTopDownPlayerController::ACodexInvenTopDownPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = false;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ACodexInvenTopDownPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	ApplyInputMappingContext();
	UpdateAimFromCursor();
}

void ACodexInvenTopDownPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		BindConfiguredInput(*EnhancedInputComponent);
	}
}

const UCodexInvenInputConfigDataAsset* ACodexInvenTopDownPlayerController::GetInputConfig() const
{
	const UCodexInvenGameInstance* CodexInvenGameInstance = Cast<UCodexInvenGameInstance>(GetGameInstance());
	return CodexInvenGameInstance != nullptr ? CodexInvenGameInstance->GetInputConfig() : nullptr;
}

ACodexInvenTopDownCharacter* ACodexInvenTopDownPlayerController::GetTopDownCharacter() const
{
	return Cast<ACodexInvenTopDownCharacter>(GetPawn());
}

void ACodexInvenTopDownPlayerController::ApplyInputMappingContext()
{
	const UCodexInvenInputConfigDataAsset* InputConfig = GetInputConfig();
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (InputConfig == nullptr || InputConfig->InputMappingContext == nullptr || LocalPlayer == nullptr)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		InputSubsystem->AddMappingContext(InputConfig->InputMappingContext, InputMappingPriority);
	}
}

bool ACodexInvenTopDownPlayerController::TryGetCursorGroundPoint(FVector& OutWorldPoint) const
{
	FHitResult HitResult;
	if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult))
	{
		OutWorldPoint = HitResult.ImpactPoint;
		return true;
	}

	FVector MouseWorldLocation;
	FVector MouseWorldDirection;
	if (!DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection))
	{
		return false;
	}

	const APawn* ControlledPawn = GetPawn();
	const float GroundHeight = ControlledPawn != nullptr ? ControlledPawn->GetActorLocation().Z : 0.0f;
	const FPlane GroundPlane(FVector(0.0f, 0.0f, GroundHeight), FVector::UpVector);
	OutWorldPoint = FMath::LinePlaneIntersection(MouseWorldLocation, MouseWorldLocation + (MouseWorldDirection * 100000.0f), GroundPlane);
	return true;
}

void ACodexInvenTopDownPlayerController::UpdateAimFromCursor() const
{
	ACodexInvenTopDownCharacter* ControlledCharacter = GetTopDownCharacter();
	if (ControlledCharacter == nullptr)
	{
		return;
	}

	FVector CursorWorldPoint;
	if (TryGetCursorGroundPoint(CursorWorldPoint))
	{
		ControlledCharacter->AimAtWorldLocation(CursorWorldPoint);
	}
}

void ACodexInvenTopDownPlayerController::HandleMove(const FInputActionValue& InValue)
{
	if (ACodexInvenTopDownCharacter* ControlledCharacter = GetTopDownCharacter())
	{
		ControlledCharacter->MoveInTopDownPlane(InValue.Get<FVector2D>());
		UpdateAimFromCursor();
	}
}

void ACodexInvenTopDownPlayerController::HandleLook(const FInputActionValue& InValue)
{
	if (ACodexInvenTopDownCharacter* ControlledCharacter = GetTopDownCharacter())
	{
		const FVector2D LookInput = InValue.Get<FVector2D>();
		if (!LookInput.IsNearlyZero())
		{
			const FVector AimTarget = ControlledCharacter->GetActorLocation() + FVector(LookInput.Y, LookInput.X, 0.0f) * 1000.0f;
			ControlledCharacter->AimAtWorldLocation(AimTarget);
			return;
		}
	}

	UpdateAimFromCursor();
}

void ACodexInvenTopDownPlayerController::HandleJumpStarted()
{
	if (ACodexInvenTopDownCharacter* ControlledCharacter = GetTopDownCharacter())
	{
		ControlledCharacter->BeginGenericJump();
	}
}

void ACodexInvenTopDownPlayerController::HandleJumpCompleted()
{
	if (ACodexInvenTopDownCharacter* ControlledCharacter = GetTopDownCharacter())
	{
		ControlledCharacter->EndGenericJump();
	}
}

void ACodexInvenTopDownPlayerController::HandleFireStarted()
{
	if (ACodexInvenTopDownCharacter* ControlledCharacter = GetTopDownCharacter())
	{
		FVector CursorWorldPoint;
		if (TryGetCursorGroundPoint(CursorWorldPoint))
		{
			ControlledCharacter->FireAtWorldLocation(CursorWorldPoint);
			return;
		}

		ControlledCharacter->FireAtWorldLocation(ControlledCharacter->GetActorLocation() + ControlledCharacter->GetActorForwardVector() * 1000.0f);
	}
}

void ACodexInvenTopDownPlayerController::BindConfiguredInput(UEnhancedInputComponent& InEnhancedInputComponent)
{
	const UCodexInvenInputConfigDataAsset* InputConfig = GetInputConfig();
	if (InputConfig == nullptr || !InputConfig->HasRequiredBindings())
	{
		return;
	}

	InEnhancedInputComponent.BindAction(InputConfig->MoveAction, ETriggerEvent::Triggered, this, &ThisClass::HandleMove);
	InEnhancedInputComponent.BindAction(InputConfig->LookAction, ETriggerEvent::Triggered, this, &ThisClass::HandleLook);
	InEnhancedInputComponent.BindAction(InputConfig->JumpAction, ETriggerEvent::Started, this, &ThisClass::HandleJumpStarted);
	InEnhancedInputComponent.BindAction(InputConfig->JumpAction, ETriggerEvent::Completed, this, &ThisClass::HandleJumpCompleted);
	InEnhancedInputComponent.BindAction(InputConfig->FireAction, ETriggerEvent::Started, this, &ThisClass::HandleFireStarted);
}
