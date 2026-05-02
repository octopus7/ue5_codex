// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TigerShopGameMode.generated.h"

class ARiceCakeWorkstation;
class ATemporaryTigerCharacter;
class ATigerShopCameraRig;
class UStaticMesh;

UCLASS()
class CODEXGOAL_API ATigerShopGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATigerShopGameMode();

protected:
	virtual void BeginPlay() override;

private:
	void SpawnPrototypeScene();
	void SpawnStaticMeshProp(UStaticMesh* Mesh, const FVector& Location, const FRotator& Rotation, const FVector& Scale);

	UPROPERTY()
	TObjectPtr<ATigerShopCameraRig> CameraRig;

	UPROPERTY()
	TObjectPtr<ARiceCakeWorkstation> Workstation;

	UPROPERTY()
	TObjectPtr<ATemporaryTigerCharacter> TemporaryTiger;
};
