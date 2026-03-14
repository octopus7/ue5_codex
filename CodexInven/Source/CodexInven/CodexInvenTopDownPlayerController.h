// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
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
	virtual void SetupInputComponent() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	int32 InputMappingPriority = 0;

private:
	const UCodexInvenInputConfigDataAsset* GetInputConfig() const;
	ACodexInvenTopDownCharacter* GetTopDownCharacter() const;
	void ApplyInputMappingContext();
	bool TryGetCursorGroundPoint(FVector& OutWorldPoint) const;
	void UpdateAimFromCursor() const;
	void HandleMove(const FInputActionValue& InValue);
	void HandleLook(const FInputActionValue& InValue);
	void HandleJumpStarted();
	void HandleJumpCompleted();
	void HandleFireStarted();
	void BindConfiguredInput(UEnhancedInputComponent& InEnhancedInputComponent);
};
