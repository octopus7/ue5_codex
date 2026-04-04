#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CMWVoxelActor.generated.h"

class UCMWVoxelDataAsset;
class UMaterialInterface;
class UProceduralMeshComponent;

UCLASS()
class CODEXMAZEWALKER_API ACMWVoxelActor : public AActor
{
	GENERATED_BODY()

public:
	ACMWVoxelActor();

	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable, Category = "Voxel")
	void RebuildVoxelMesh();

protected:
	UMaterialInterface* ResolveSharedMaterial() const;

	UPROPERTY(VisibleAnywhere, Category = "Voxel")
	TObjectPtr<UProceduralMeshComponent> ProceduralMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Voxel")
	TObjectPtr<UCMWVoxelDataAsset> VoxelDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Voxel")
	TObjectPtr<UMaterialInterface> SharedMaterialOverride;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Voxel")
	bool bRebuildOnConstruction = true;
};
