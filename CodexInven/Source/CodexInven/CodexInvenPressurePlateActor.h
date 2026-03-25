// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "CodexInvenPressurePlateActor.generated.h"

class ACodexInvenDoorActor;
class APawn;
class UBoxComponent;
class UPrimitiveComponent;
class USceneComponent;
class UStaticMeshComponent;
struct FHitResult;

UCLASS()
class CODEXINVEN_API ACodexInvenPressurePlateActor : public AActor
{
	GENERATED_BODY()

public:
	ACodexInvenPressurePlateActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pressure Plate")
	TObjectPtr<USceneComponent> SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pressure Plate")
	TObjectPtr<UBoxComponent> TriggerBoxComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pressure Plate")
	TObjectPtr<UStaticMeshComponent> PlateMeshComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pressure Plate", meta = (ClampMin = "0.0"))
	float PlateLowerDistance = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pressure Plate", meta = (ClampMin = "0.0"))
	float PlateMoveDuration = 0.2f;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Pressure Plate")
	TArray<TObjectPtr<ACodexInvenDoorActor>> ConnectedDoors;

private:
	void ApplyPlateVisualState();
	void RefreshPressedState();
	void UpdateConnectedDoors() const;

	UFUNCTION()
	void HandleTriggerBoxBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void HandleTriggerBoxEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	TSet<TWeakObjectPtr<APawn>> OverlappingPlayerPawns;
	FVector RaisedPlateRelativeLocation = FVector::ZeroVector;
	float PlatePressAlpha = 0.0f;
	bool bShouldBePressed = false;
};
