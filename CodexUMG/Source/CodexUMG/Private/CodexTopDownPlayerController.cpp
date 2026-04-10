// Copyright Epic Games, Inc. All Rights Reserved.

#include "CodexTopDownPlayerController.h"

#include "CodexFireInputConsumer.h"
#include "CodexGameInstance.h"
#include "CodexMoveInputConsumer.h"
#include "CodexTopDownInputConfigDataAsset.h"
#include "Interaction/CodexInteractionSubsystem.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"

void ACodexTopDownPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ApplyTopDownInputMappingContext();
}

void ACodexTopDownPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	BindTopDownMoveAction();
	BindTopDownFireAction();
	BindTopDownInteractAction();
}

void ACodexTopDownPlayerController::ApplyTopDownInputMappingContext()
{
	if (bHasAppliedTopDownInputContext || !IsLocalController())
	{
		return;
	}

	const UCodexGameInstance* CodexGameInstance = GetGameInstance<UCodexGameInstance>();
	const UCodexTopDownInputConfigDataAsset* InputConfig = CodexGameInstance ? CodexGameInstance->GetTopDownInputConfig() : nullptr;
	const UInputMappingContext* MappingContext = InputConfig ? InputConfig->GetDefaultMappingContext() : nullptr;

	if (!InputConfig || !MappingContext)
	{
		UE_LOG(LogTemp, Warning, TEXT("Top-down input config is missing on %s."), *GetName());
		return;
	}

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSubsystem)
	{
		return;
	}

	InputSubsystem->AddMappingContext(MappingContext, InputConfig->GetMappingPriority());
	bHasAppliedTopDownInputContext = true;
}

void ACodexTopDownPlayerController::BindTopDownMoveAction()
{
	if (bHasBoundMoveAction || !InputComponent)
	{
		return;
	}

	const UCodexGameInstance* CodexGameInstance = GetGameInstance<UCodexGameInstance>();
	const UCodexTopDownInputConfigDataAsset* InputConfig = CodexGameInstance ? CodexGameInstance->GetTopDownInputConfig() : nullptr;
	const UInputAction* MoveAction = InputConfig ? InputConfig->GetMoveAction() : nullptr;

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent || !MoveAction)
	{
		return;
	}

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACodexTopDownPlayerController::HandleMoveInput);
	bHasBoundMoveAction = true;
}

void ACodexTopDownPlayerController::BindTopDownFireAction()
{
	if (bHasBoundFireAction || !InputComponent)
	{
		return;
	}

	const UCodexGameInstance* CodexGameInstance = GetGameInstance<UCodexGameInstance>();
	const UCodexTopDownInputConfigDataAsset* InputConfig = CodexGameInstance ? CodexGameInstance->GetTopDownInputConfig() : nullptr;
	const UInputAction* FireAction = InputConfig ? InputConfig->GetFireAction() : nullptr;

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent || !FireAction)
	{
		return;
	}

	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ACodexTopDownPlayerController::HandleFireInput);
	bHasBoundFireAction = true;
}

void ACodexTopDownPlayerController::BindTopDownInteractAction()
{
	if (bHasBoundInteractAction || !InputComponent)
	{
		return;
	}

	const UCodexGameInstance* CodexGameInstance = GetGameInstance<UCodexGameInstance>();
	const UCodexTopDownInputConfigDataAsset* InputConfig = CodexGameInstance ? CodexGameInstance->GetTopDownInputConfig() : nullptr;
	const UInputAction* InteractAction = InputConfig ? InputConfig->GetInteractAction() : nullptr;

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent || !InteractAction)
	{
		return;
	}

	EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ACodexTopDownPlayerController::HandleInteractInput);
	bHasBoundInteractAction = true;
}

void ACodexTopDownPlayerController::HandleMoveInput(const FInputActionValue& InputValue)
{
	const FVector2D MoveAxis = InputValue.Get<FVector2D>();
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn || !ControlledPawn->GetClass()->ImplementsInterface(UCodexMoveInputReceiver::StaticClass()))
	{
		return;
	}

	ICodexMoveInputReceiver::Execute_ConsumeMoveInput(ControlledPawn, MoveAxis);
}

void ACodexTopDownPlayerController::HandleFireInput(const FInputActionValue& InputValue)
{
	(void)InputValue;

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn || !ControlledPawn->GetClass()->ImplementsInterface(UCodexFireInputReceiver::StaticClass()))
	{
		return;
	}

	ICodexFireInputReceiver::Execute_ConsumeFireInput(ControlledPawn);
}

void ACodexTopDownPlayerController::HandleInteractInput(const FInputActionValue& InputValue)
{
	(void)InputValue;

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (UCodexInteractionSubsystem* InteractionSubsystem = World->GetSubsystem<UCodexInteractionSubsystem>())
	{
		InteractionSubsystem->RequestInteraction(this);
	}
}
