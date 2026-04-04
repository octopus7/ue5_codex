#include "Systems/Voxel/CMWVoxelActor.h"

#include "Materials/MaterialInterface.h"
#include "ProceduralMeshComponent.h"
#include "Systems/Game/CMWGameDataAsset.h"
#include "Systems/Game/CMWGameInstance.h"
#include "Systems/Voxel/CMWVoxelDataAsset.h"
#include "Systems/Voxel/CMWVoxelMeshGenerator.h"

ACMWVoxelActor::ACMWVoxelActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
	ProceduralMeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
	ProceduralMeshComponent->bUseAsyncCooking = true;
	RootComponent = ProceduralMeshComponent;
}

void ACMWVoxelActor::BeginPlay()
{
	Super::BeginPlay();
	RebuildVoxelMesh();
}

void ACMWVoxelActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (bRebuildOnConstruction)
	{
		RebuildVoxelMesh();
	}
}

void ACMWVoxelActor::RebuildVoxelMesh()
{
	ProceduralMeshComponent->ClearAllMeshSections();

	FCMWVoxelMeshBuffers MeshBuffers;
	if (!FCMWVoxelMeshGenerator::BuildMeshBuffers(VoxelDataAsset, MeshBuffers))
	{
		return;
	}

	ProceduralMeshComponent->CreateMeshSection_LinearColor(
		0,
		MeshBuffers.Vertices,
		MeshBuffers.Triangles,
		MeshBuffers.Normals,
		MeshBuffers.UVs,
		MeshBuffers.Colors,
		MeshBuffers.Tangents,
		true);

	if (UMaterialInterface* SharedMaterial = ResolveSharedMaterial())
	{
		ProceduralMeshComponent->SetMaterial(0, SharedMaterial);
	}
}

UMaterialInterface* ACMWVoxelActor::ResolveSharedMaterial() const
{
	if (SharedMaterialOverride)
	{
		return SharedMaterialOverride;
	}

	if (const UCMWGameInstance* GameInstance = UCMWGameInstance::Get(this))
	{
		if (const UCMWGameDataAsset* GameData = GameInstance->GetGameData())
		{
			return GameData->SharedVoxelMaterial;
		}
	}

	return nullptr;
}
