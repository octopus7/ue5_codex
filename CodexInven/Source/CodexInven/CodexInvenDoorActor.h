// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CodexInvenDoorActor.generated.h"

class UBoxComponent;
class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class CODEXINVEN_API ACodexInvenDoorActor : public AActor
{
	GENERATED_BODY()

public:
	ACodexInvenDoorActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Door")
	void OpenDoor();

	UFUNCTION(BlueprintCallable, Category = "Door")
	void CloseDoor();

	UFUNCTION(BlueprintCallable, Category = "Door")
	void SetDoorOpen(bool bInShouldBeOpen);

	UFUNCTION(BlueprintPure, Category = "Door")
	bool IsDoorOpen() const
	{
		return bShouldBeOpen;
	}

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	TObjectPtr<USceneComponent> SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	TObjectPtr<UBoxComponent> DoorBlockerComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	TObjectPtr<USceneComponent> DoorPivotComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	TObjectPtr<UStaticMeshComponent> DoorMeshComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door", meta = (ClampMin = "0.0"))
	float OpenYawOffset = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door", meta = (ClampMin = "0.0"))
	float DoorMoveDuration = 0.35f;

private:
	void ApplyDoorVisualState();
	void UpdateDoorBlockerCollision() const;

	FRotator ClosedDoorRelativeRotation = FRotator::ZeroRotator;
	float DoorOpenAlpha = 0.0f;
	bool bShouldBeOpen = false;
};
