#include "Systems/Voxel/CMWVoxelMeshGenerator.h"

#include "Systems/Voxel/CMWVoxelDataAsset.h"

namespace
{
	struct FCMWFaceDefinition
	{
		FIntVector NeighborOffset;
		FVector Normal;
		FVector Tangent;
		FVector CornerOffsets[4];
	};

	const FCMWFaceDefinition FaceDefinitions[] =
	{
		{ FIntVector(1, 0, 0), FVector(1, 0, 0), FVector(0, 1, 0), { FVector(1, 0, 0), FVector(1, 1, 0), FVector(1, 1, 1), FVector(1, 0, 1) } },
		{ FIntVector(0, 1, 0), FVector(0, 1, 0), FVector(-1, 0, 0), { FVector(1, 1, 0), FVector(0, 1, 0), FVector(0, 1, 1), FVector(1, 1, 1) } },
		{ FIntVector(-1, 0, 0), FVector(-1, 0, 0), FVector(0, -1, 0), { FVector(0, 1, 0), FVector(0, 0, 0), FVector(0, 0, 1), FVector(0, 1, 1) } },
		{ FIntVector(0, -1, 0), FVector(0, -1, 0), FVector(1, 0, 0), { FVector(0, 0, 0), FVector(1, 0, 0), FVector(1, 0, 1), FVector(0, 0, 1) } },
		{ FIntVector(0, 0, 1), FVector(0, 0, 1), FVector(1, 0, 0), { FVector(0, 0, 1), FVector(1, 0, 1), FVector(1, 1, 1), FVector(0, 1, 1) } },
		{ FIntVector(0, 0, -1), FVector(0, 0, -1), FVector(1, 0, 0), { FVector(0, 1, 0), FVector(1, 1, 0), FVector(1, 0, 0), FVector(0, 0, 0) } }
	};
}

void FCMWVoxelMeshBuffers::Reset()
{
	Vertices.Reset();
	Triangles.Reset();
	Normals.Reset();
	UVs.Reset();
	Colors.Reset();
	Tangents.Reset();
}

bool FCMWVoxelMeshBuffers::HasGeometry() const
{
	return Vertices.Num() > 0 && Triangles.Num() > 0;
}

bool FCMWVoxelMeshGenerator::BuildMeshBuffers(const UCMWVoxelDataAsset* VoxelDataAsset, FCMWVoxelMeshBuffers& OutMeshBuffers)
{
	OutMeshBuffers.Reset();
	if (!VoxelDataAsset)
	{
		return false;
	}

	const FIntVector Dimensions = VoxelDataAsset->Dimensions;
	const float VoxelSize = VoxelDataAsset->VoxelSize;

	auto AppendQuad = [&OutMeshBuffers](const FVector QuadVertices[4], const FVector& Normal, const FVector& Tangent, const FLinearColor& Color)
	{
		const int32 BaseIndex = OutMeshBuffers.Vertices.Num();
		const FVector2D UVSet[4] =
		{
			FVector2D(0.0f, 0.0f),
			FVector2D(1.0f, 0.0f),
			FVector2D(1.0f, 1.0f),
			FVector2D(0.0f, 1.0f)
		};

		for (int32 CornerIndex = 0; CornerIndex < 4; ++CornerIndex)
		{
			OutMeshBuffers.Vertices.Add(QuadVertices[CornerIndex]);
			OutMeshBuffers.Normals.Add(Normal);
			OutMeshBuffers.UVs.Add(UVSet[CornerIndex]);
			OutMeshBuffers.Colors.Add(Color);
			OutMeshBuffers.Tangents.Add(FProcMeshTangent(Tangent, false));
		}

		OutMeshBuffers.Triangles.Add(BaseIndex + 0);
		OutMeshBuffers.Triangles.Add(BaseIndex + 1);
		OutMeshBuffers.Triangles.Add(BaseIndex + 2);
		OutMeshBuffers.Triangles.Add(BaseIndex + 0);
		OutMeshBuffers.Triangles.Add(BaseIndex + 2);
		OutMeshBuffers.Triangles.Add(BaseIndex + 3);
	};

	for (int32 Z = 0; Z < Dimensions.Z; ++Z)
	{
		for (int32 Y = 0; Y < Dimensions.Y; ++Y)
		{
			for (int32 X = 0; X < Dimensions.X; ++X)
			{
				if (!VoxelDataAsset->IsFilled(X, Y, Z))
				{
					continue;
				}

				const FVector CellOrigin(X * VoxelSize, Y * VoxelSize, Z * VoxelSize);
				const FLinearColor CellColor(VoxelDataAsset->GetVoxelColor(X, Y, Z));

				for (const FCMWFaceDefinition& FaceDefinition : FaceDefinitions)
				{
					const FIntVector Neighbor(FIntVector(X, Y, Z) + FaceDefinition.NeighborOffset);
					if (VoxelDataAsset->IsFilled(Neighbor.X, Neighbor.Y, Neighbor.Z))
					{
						continue;
					}

					FVector QuadVertices[4];
					for (int32 CornerIndex = 0; CornerIndex < 4; ++CornerIndex)
					{
						QuadVertices[CornerIndex] = CellOrigin + FaceDefinition.CornerOffsets[CornerIndex] * VoxelSize;
					}

					AppendQuad(QuadVertices, FaceDefinition.Normal, FaceDefinition.Tangent, CellColor);
				}
			}
		}
	}

	return OutMeshBuffers.HasGeometry();
}
