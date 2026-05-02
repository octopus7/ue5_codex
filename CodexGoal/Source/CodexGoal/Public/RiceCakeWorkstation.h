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

	virtual void Tick(float DeltaSeconds) override;

	void PlayClickFeedback();

private:
	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<UStaticMeshComponent> WorkstationMesh;

	UPROPERTY(VisibleAnywhere, Category="Tiger Shop")
	TObjectPtr<UTextRenderComponent> LabelText;

	FVector BaseMeshScale = FVector::OneVector;
	float FeedbackTimeRemaining = 0.0f;
};
