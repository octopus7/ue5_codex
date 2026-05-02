// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TemporaryTigerCharacter.generated.h"

class UStaticMeshComponent;

UCLASS()
class CODEXGOAL_API ATemporaryTigerCharacter : public AActor
{
	GENERATED_BODY()

public:
	ATemporaryTigerCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void PlayProductionPulse();

private:
	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<USceneComponent> SkeletonRoot;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<USceneComponent> BodyBone;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<USceneComponent> HeadBone;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<USceneComponent> LeftArmBone;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<USceneComponent> RightArmBone;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<UStaticMeshComponent> HeadMesh;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<UStaticMeshComponent> LeftArmMesh;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<UStaticMeshComponent> RightArmMesh;

	FVector DefaultHeadLocation = FVector::ZeroVector;
	FRotator DefaultLeftArmRotation = FRotator::ZeroRotator;
	FRotator DefaultRightArmRotation = FRotator::ZeroRotator;
	float PulseTimeRemaining = 0.0f;
};
