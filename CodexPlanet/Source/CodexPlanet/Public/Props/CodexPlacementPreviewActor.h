// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CodexPlacementPreviewActor.generated.h"

class UStaticMesh;
class UStaticMeshComponent;

UCLASS()
class CODEXPLANET_API ACodexPlacementPreviewActor : public AActor
{
	GENERATED_BODY()

public:
	ACodexPlacementPreviewActor();

	void ConfigurePreviewMesh(UStaticMesh* InStaticMesh);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Preview")
	TObjectPtr<UStaticMeshComponent> PreviewMeshComponent = nullptr;
};
