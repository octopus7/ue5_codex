// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CodexInvenTopDownCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ACodexInvenProjectile;

UCLASS()
class CODEXINVEN_API ACodexInvenTopDownCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACodexInvenTopDownCharacter();

	UFUNCTION(BlueprintCallable, Category = "Character|Input")
	void MoveInTopDownPlane(const FVector2D& InMoveInput);

	UFUNCTION(BlueprintCallable, Category = "Character|Input")
	void BeginGenericJump();

	UFUNCTION(BlueprintCallable, Category = "Character|Input")
	void EndGenericJump();

	UFUNCTION(BlueprintCallable, Category = "Character|Input")
	void AimAtWorldLocation(const FVector& InWorldLocation);

	UFUNCTION(BlueprintCallable, Category = "Character|Input")
	void FireAtWorldLocation(const FVector& InWorldLocation);

	UFUNCTION(BlueprintPure, Category = "Character|Camera")
	UCameraComponent* GetTopDownCameraComponent() const
	{
		return TopDownCameraComponent;
	}

	UFUNCTION(BlueprintImplementableEvent, Category = "Character|Combat")
	void OnProjectileSpawned(ACodexInvenProjectile* SpawnedProjectile, const FVector& SpawnLocation, const FVector& SpawnDirection);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Camera")
	TObjectPtr<USpringArmComponent> CameraBoom = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Camera")
	TObjectPtr<UCameraComponent> TopDownCameraComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Camera", meta = (ClampMin = "100.0"))
	float CameraBoomLength = 1200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Camera")
	FRotator CameraBoomRotation = FRotator(-60.0f, 0.0f, 0.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Combat", meta = (ClampMin = "0.0"))
	float FireTraceDistance = 5000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Combat", meta = (ClampMin = "0.0"))
	float ProjectileSpawnForwardDistance = 100.0f;

private:
	FVector GetPlanarDirectionTo(const FVector& InWorldLocation) const;
	TSubclassOf<ACodexInvenProjectile> GetConfiguredProjectileClass() const;
};
