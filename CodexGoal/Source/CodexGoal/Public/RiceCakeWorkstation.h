// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RiceCakeWorkstation.generated.h"

class UStaticMeshComponent;
class UTextRenderComponent;

UCLASS()
class CODEXGOAL_API ARiceCakeWorkstation : public AActor
{
	GENERATED_BODY()

public:
	ARiceCakeWorkstation();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void PlayClickFeedback();

private:
	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<UStaticMeshComponent> WorkstationMesh;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<UStaticMeshComponent> TrayMesh;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<UStaticMeshComponent> RiceCakeA;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<UStaticMeshComponent> RiceCakeB;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<UStaticMeshComponent> RiceCakeC;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<UStaticMeshComponent> MalletHandle;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<UStaticMeshComponent> MalletHead;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<UTextRenderComponent> LabelText;

	void FaceLabelToCamera() const;

	FVector BaseMeshScale = FVector::OneVector;
	float FeedbackTimeRemaining = 0.0f;
};
