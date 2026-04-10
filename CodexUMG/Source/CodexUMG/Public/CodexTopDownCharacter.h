// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CodexFireInputConsumer.h"
#include "GameFramework/Character.h"
#include "CodexMoveInputConsumer.h"
#include "CodexTopDownCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS(Blueprintable)
class CODEXUMG_API ACodexTopDownCharacter : public ACharacter, public ICodexMoveInputReceiver, public ICodexFireInputReceiver
{
	GENERATED_BODY()

public:
	ACodexTopDownCharacter();

	virtual void ConsumeMoveInput_Implementation(FVector2D MoveAxis) override;
	virtual void ConsumeFireInput_Implementation() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Codex|Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Codex|Camera")
	TObjectPtr<UCameraComponent> TopDownCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Codex|Combat", meta = (ClampMin = "0.0"))
	float ProjectileSpawnDistance = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Codex|Combat", meta = (ClampMin = "0.0"))
	float ProjectileSpawnHeight = 40.0f;
};
