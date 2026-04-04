#include "Voxel/CMWVoxelStaticMeshBuilder.h"

#include "Engine/StaticMesh.h"
#include "MeshDescription.h"
#include "MeshDescriptionBuilder.h"
#include "StaticMeshAttributes.h"
#include "StaticMeshResources.h"
#include "Systems/Voxel/CMWVoxelDataAsset.h"
#include "Systems/Voxel/CMWVoxelMeshGenerator.h"

bool FCMWVoxelStaticMeshBuilder::BuildStaticMesh(UStaticMesh* StaticMesh, const UCMWVoxelDataAsset& VoxelDataAsset, UMaterialInterface* SharedMaterial, FString& OutError)
{
	if (!StaticMesh)
	{
		OutError = TEXT("Static mesh asset pointer was null.");
		return false;
	}

	FCMWVoxelMeshBuffers MeshBuffers;
	if (!FCMWVoxelMeshGenerator::BuildMeshBuffers(&VoxelDataAsset, MeshBuffers))
	{
		OutError = TEXT("Voxel mesh generator returned no geometry.");
		return false;
	}

	FMeshDescription MeshDescription;
	FStaticMeshAttributes Attributes(MeshDescription);
	Attributes.Register();

	FMeshDescriptionBuilder Builder;
	Builder.SetMeshDescription(&MeshDescription);
	Builder.EnablePolyGroups();
	Builder.SetNumUVLayers(1);

	const FPolygonGroupID PolygonGroup = Builder.AppendPolygonGroup(TEXT("VoxelMaterial"));

	for (int32 TriangleIndex = 0; TriangleIndex < MeshBuffers.Triangles.Num(); TriangleIndex += 3)
	{
		FVertexInstanceID Instances[3];

		for (int32 CornerIndex = 0; CornerIndex < 3; ++CornerIndex)
		{
			const int32 VertexBufferIndex = MeshBuffers.Triangles[TriangleIndex + CornerIndex];
			const FVertexID VertexID = Builder.AppendVertex(MeshBuffers.Vertices[VertexBufferIndex]);
			Instances[CornerIndex] = Builder.AppendInstance(VertexID);

			Builder.SetInstanceNormal(Instances[CornerIndex], MeshBuffers.Normals[VertexBufferIndex]);
			Builder.SetInstanceUV(Instances[CornerIndex], MeshBuffers.UVs[VertexBufferIndex], 0);
			Builder.SetInstanceColor(Instances[CornerIndex], FVector4f(MeshBuffers.Colors[VertexBufferIndex]));
			Builder.SetInstanceTangentSpace(
				Instances[CornerIndex],
				MeshBuffers.Normals[VertexBufferIndex],
				MeshBuffers.Tangents[VertexBufferIndex].TangentX,
				MeshBuffers.Tangents[VertexBufferIndex].bFlipTangentY ? -1.0f : 1.0f);
		}

		Builder.AppendTriangle(Instances[0], Instances[1], Instances[2], PolygonGroup);
	}

	StaticMesh->GetStaticMaterials().Reset();
	if (SharedMaterial)
	{
		StaticMesh->GetStaticMaterials().Add(FStaticMaterial(SharedMaterial));
	}

	StaticMesh->InitResources();
	StaticMesh->SetLightingGuid();
	StaticMesh->SetImportVersion(EImportStaticMeshVersion::LastVersion);

	TArray<const FMeshDescription*> MeshDescriptions;
	MeshDescriptions.Add(&MeshDescription);

	UStaticMesh::FBuildMeshDescriptionsParams BuildParameters;
	BuildParameters.bBuildSimpleCollision = true;
	BuildParameters.bCommitMeshDescription = true;
	BuildParameters.bFastBuild = false;
	BuildParameters.bMarkPackageDirty = true;
	BuildParameters.bUseHashAsGuid = false;

	StaticMesh->BuildFromMeshDescriptions(MeshDescriptions, BuildParameters);
	StaticMesh->PostEditChange();
	return true;
}
