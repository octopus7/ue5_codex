// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CodexInvenPickupData.h"
#include "CodexInvenPickupActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UPrimitiveComponent;
class UMaterialInstanceDynamic;

UCLASS()
class CODEXINVEN_API ACodexInvenPickupActor : public AActor
{
	GENERATED_BODY()

public:
	ACodexInvenPickupActor();

	virtual void OnConstruction(const FTransform& InTransform) override;

	void InitializeFromPickupType(ECodexInvenPickupType InPickupType);
	ECodexInvenPickupType GetPickupType() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<USphereComponent> PickupSphereComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UStaticMeshComponent> PickupMeshComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup", meta = (ClampMin = "0.0"))
	float PickupRadius = 150.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	FVector CubeVisualScale = FVector(1.0f, 1.0f, 1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	FVector CylinderVisualScale = FVector(1.0f, 1.0f, 0.5f);

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Pickup")
	ECodexInvenPickupType PickupType = ECodexInvenPickupType::CubeRed;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> PickupMaterialInstance = nullptr;

private:
	void ApplyPickupDefinition();

	UFUNCTION()
	void HandlePickupSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};
