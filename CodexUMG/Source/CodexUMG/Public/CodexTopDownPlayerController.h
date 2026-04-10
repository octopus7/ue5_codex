// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "CodexTopDownPlayerController.generated.h"

UCLASS(Blueprintable)
class CODEXUMG_API ACodexTopDownPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void ApplyTopDownInputMappingContext();
	void BindTopDownMoveAction();
	void BindTopDownFireAction();
	void BindTopDownInteractAction();
	void HandleMoveInput(const FInputActionValue& InputValue);
	void HandleFireInput(const FInputActionValue& InputValue);
	void HandleInteractInput(const FInputActionValue& InputValue);

	UPROPERTY(Transient)
	bool bHasAppliedTopDownInputContext = false;

	UPROPERTY(Transient)
	bool bHasBoundMoveAction = false;

	UPROPERTY(Transient)
	bool bHasBoundFireAction = false;

	UPROPERTY(Transient)
	bool bHasBoundInteractAction = false;
};
