// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CodexMoveInputConsumer.h"
#include "CodexTopDownCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS(Blueprintable)
class CODEXUMG_API ACodexTopDownCharacter : public ACharacter, public ICodexMoveInputReceiver
{
	GENERATED_BODY()

public:
	ACodexTopDownCharacter();

	virtual void ConsumeMoveInput_Implementation(FVector2D MoveAxis) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Codex|Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Codex|Camera")
	TObjectPtr<UCameraComponent> TopDownCamera;
};
