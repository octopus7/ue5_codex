// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "TimerManager.h"
#include "CodexInvenTopDownPlayerController.generated.h"

class ACodexInvenTopDownCharacter;
class UEnhancedInputComponent;
class UCodexInvenInputConfigDataAsset;

UCLASS()
class CODEXINVEN_API ACodexInvenTopDownPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACodexInvenTopDownPlayerController();

	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	int32 InputMappingPriority = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Combat", meta = (ClampMin = "0.01"))
	float AutomaticFireInterval = 0.12f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat", meta = (ClampMin = "0.0"))
	float CursorAimResumeDelay = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool bLogCursorTrace = false;

private:
	const UCodexInvenInputConfigDataAsset* GetInputConfig() const;
	ACodexInvenTopDownCharacter* GetTopDownCharacter() const;
	bool ShouldUseCursorAim() const;
	void ApplyInputMappingContext();
	bool TryGetCursorGroundPoint(FVector& OutWorldPoint) const;
	void FireProjectileOnce();
	void UpdateAimFromCursor() const;
	void HandleMove(const FInputActionValue& InValue);
	void HandleLook(const FInputActionValue& InValue);
	void HandleJumpStarted();
	void HandleJumpCompleted();
	void HandleFireStarted();
	void HandleFireCompleted();
	void HandleAutoFireTick();
	void BindConfiguredInput(UEnhancedInputComponent& InEnhancedInputComponent);

	FTimerHandle AutomaticFireTimerHandle;
	bool bIsAutomaticFireActive = false;
	float LastExplicitLookInputTime = -1.0f;
};
