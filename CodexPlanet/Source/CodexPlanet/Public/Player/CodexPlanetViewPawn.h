// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CodexPlanetViewPawn.generated.h"

class AActor;
class UCameraComponent;
class USceneComponent;
class USpringArmComponent;

UCLASS()
class CODEXPLANET_API ACodexPlanetViewPawn : public APawn
{
	GENERATED_BODY()

public:
	ACodexPlanetViewPawn();

	virtual void Tick(float DeltaSeconds) override;

	void SetFocusActor(AActor* InFocusActor);
	FVector GetFocusPoint() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "View")
	TObjectPtr<USceneComponent> SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "View")
	TObjectPtr<USpringArmComponent> CameraBoom = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "View")
	TObjectPtr<UCameraComponent> Camera = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "View")
	FVector FocusOffset = FVector::ZeroVector;

private:
	TWeakObjectPtr<AActor> FocusActor;

	void UpdateViewAnchor();
};
