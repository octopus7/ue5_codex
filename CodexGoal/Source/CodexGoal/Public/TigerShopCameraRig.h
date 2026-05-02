// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TigerShopCameraRig.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class CODEXGOAL_API ATigerShopCameraRig : public AActor
{
	GENERATED_BODY()

public:
	ATigerShopCameraRig();

	void AddPanInput(const FVector2D& Delta);
	void SetPanOffset(const FVector2D& NewPanOffset);

	UCameraComponent* GetCameraComponent() const { return CameraComponent; }

private:
	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<USceneComponent> RigRoot;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category="Tiger Shop")
	FVector2D MinPanBounds = FVector2D(-600.0f, -400.0f);

	UPROPERTY(EditDefaultsOnly, Category="Tiger Shop")
	FVector2D MaxPanBounds = FVector2D(600.0f, 400.0f);

	FVector2D PanOffset = FVector2D::ZeroVector;
};
