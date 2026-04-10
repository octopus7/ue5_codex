#include "Vox/CodexVoxMeshBuilder.h"

#include "MeshDescriptionBuilder.h"
#include "StaticMeshAttributes.h"

namespace
{
	enum class ECodexVoxelFace : uint8
	{
		NegativeX,
		PositiveX,
		NegativeY,
		PositiveY,
		NegativeZ,
		PositiveZ
	};

	int32 MakeVoxelKey(const FIntVector& Position, const FIntVector& Size)
	{
		return Position.X + (Position.Y * Size.X) + (Position.Z * Size.X * Size.Y);
	}

	FVector GetFaceNormal(ECodexVoxelFace Face)
	{
		switch (Face)
		{
		case ECodexVoxelFace::NegativeX:
			return FVector(-1.0, 0.0, 0.0);
		case ECodexVoxelFace::PositiveX:
			return FVector(1.0, 0.0, 0.0);
		case ECodexVoxelFace::NegativeY:
			return FVector(0.0, -1.0, 0.0);
		case ECodexVoxelFace::PositiveY:
			return FVector(0.0, 1.0, 0.0);
		case ECodexVoxelFace::NegativeZ:
			return FVector(0.0, 0.0, -1.0);
		case ECodexVoxelFace::PositiveZ:
		default:
			return FVector(0.0, 0.0, 1.0);
		}
	}

	FIntVector GetNeighborOffset(ECodexVoxelFace Face)
	{
		switch (Face)
		{
		case ECodexVoxelFace::NegativeX:
			return FIntVector(-1, 0, 0);
		case ECodexVoxelFace::PositiveX:
			return FIntVector(1, 0, 0);
		case ECodexVoxelFace::NegativeY:
			return FIntVector(0, -1, 0);
		case ECodexVoxelFace::PositiveY:
			return FIntVector(0, 1, 0);
		case ECodexVoxelFace::NegativeZ:
			return FIntVector(0, 0, -1);
		case ECodexVoxelFace::PositiveZ:
		default:
			return FIntVector(0, 0, 1);
		}
	}

	void GetFaceCorners(const FIntVector& VoxelPosition, ECodexVoxelFace Face, FVector OutCorners[4])
	{
		const float X0 = static_cast<float>(VoxelPosition.X);
		const float X1 = static_cast<float>(VoxelPosition.X + 1);
		const float Y0 = static_cast<float>(VoxelPosition.Y);
		const float Y1 = static_cast<float>(VoxelPosition.Y + 1);
		const float Z0 = static_cast<float>(VoxelPosition.Z);
		const float Z1 = static_cast<float>(VoxelPosition.Z + 1);

		switch (Face)
		{
		case ECodexVoxelFace::NegativeX:
			OutCorners[0] = FVector(X0, Y0, Z0);
			OutCorners[1] = FVector(X0, Y0, Z1);
			OutCorners[2] = FVector(X0, Y1, Z1);
			OutCorners[3] = FVector(X0, Y1, Z0);
			break;
		case ECodexVoxelFace::PositiveX:
			OutCorners[0] = FVector(X1, Y0, Z0);
			OutCorners[1] = FVector(X1, Y1, Z0);
			OutCorners[2] = FVector(X1, Y1, Z1);
			OutCorners[3] = FVector(X1, Y0, Z1);
			break;
		case ECodexVoxelFace::NegativeY:
			OutCorners[0] = FVector(X0, Y0, Z0);
			OutCorners[1] = FVector(X1, Y0, Z0);
			OutCorners[2] = FVector(X1, Y0, Z1);
			OutCorners[3] = FVector(X0, Y0, Z1);
			break;
		case ECodexVoxelFace::PositiveY:
			OutCorners[0] = FVector(X0, Y1, Z0);
			OutCorners[1] = FVector(X0, Y1, Z1);
			OutCorners[2] = FVector(X1, Y1, Z1);
			OutCorners[3] = FVector(X1, Y1, Z0);
			break;
		case ECodexVoxelFace::NegativeZ:
			OutCorners[0] = FVector(X0, Y0, Z0);
			OutCorners[1] = FVector(X0, Y1, Z0);
			OutCorners[2] = FVector(X1, Y1, Z0);
			OutCorners[3] = FVector(X1, Y0, Z0);
			break;
		case ECodexVoxelFace::PositiveZ:
		default:
			OutCorners[0] = FVector(X0, Y0, Z1);
			OutCorners[1] = FVector(X1, Y0, Z1);
			OutCorners[2] = FVector(X1, Y1, Z1);
			OutCorners[3] = FVector(X0, Y1, Z1);
			break;
		}
	}
}

bool FCodexVoxMeshBuilder::BuildMeshDescription(const CodexVox::FMeshBuildInput& Input, CodexVox::FMeshBuildOutput& OutBuildOutput, FString& OutError)
{
	if (Input.Model == nullptr)
	{
		OutError = TEXT("Mesh build input is missing a parsed VOX model.");
		return false;
	}

	const CodexVox::FParsedModel& Model = *Input.Model;
	if (Model.Voxels.IsEmpty())
	{
		OutError = TEXT("Parsed VOX model does not contain any voxels.");
		return false;
	}

	TSet<int32> OccupiedVoxels;
	OccupiedVoxels.Reserve(Model.Voxels.Num());

	FBox OccupiedBounds(EForceInit::ForceInit);
	for (const CodexVox::FVoxel& Voxel : Model.Voxels)
	{
		OccupiedVoxels.Add(MakeVoxelKey(Voxel.Position, Model.Size));
		OccupiedBounds += FVector(Voxel.Position);
		OccupiedBounds += FVector(Voxel.Position + FIntVector(1, 1, 1));
	}

	FVector Pivot = OccupiedBounds.GetCenter();
	if (Input.PivotRule == CodexVox::EPivotRule::GroundCentered)
	{
		Pivot = FVector(
			(OccupiedBounds.Min.X + OccupiedBounds.Max.X) * 0.5,
			OccupiedBounds.Min.Y,
			(OccupiedBounds.Min.Z + OccupiedBounds.Max.Z) * 0.5);
	}

	OutBuildOutput = CodexVox::FMeshBuildOutput();

	FStaticMeshAttributes Attributes(OutBuildOutput.MeshDescription);
	Attributes.Register();

	FMeshDescriptionBuilder Builder;
	Builder.SetMeshDescription(&OutBuildOutput.MeshDescription);
	Builder.SetNumUVLayers(1);
	Builder.SuspendMeshDescriptionIndexing();

	const FPolygonGroupID PolygonGroupId = Builder.AppendPolygonGroup(CodexVox::MaterialSlotName);
	const FVector2D FaceUvs[4] =
	{
		FVector2D(0.0, 0.0),
		FVector2D(1.0, 0.0),
		FVector2D(1.0, 1.0),
		FVector2D(0.0, 1.0)
	};

	for (const CodexVox::FVoxel& Voxel : Model.Voxels)
	{
		for (uint8 FaceIndex = 0; FaceIndex < 6; ++FaceIndex)
		{
			const ECodexVoxelFace Face = static_cast<ECodexVoxelFace>(FaceIndex);
			const FIntVector NeighborPosition = Voxel.Position + GetNeighborOffset(Face);
			if (OccupiedVoxels.Contains(MakeVoxelKey(NeighborPosition, Model.Size)))
			{
				continue;
			}

			const FColor PaletteColor = Model.Palette[Voxel.ColorIndex];
			const FLinearColor LinearColor = FLinearColor::FromSRGBColor(PaletteColor);
			const FVector4f VertexColor(LinearColor.R, LinearColor.G, LinearColor.B, LinearColor.A);

			FVector VoxCorners[4];
			GetFaceCorners(Voxel.Position, Face, VoxCorners);

			FVector UnrealCorners[4];
			for (int32 CornerIndex = 0; CornerIndex < 4; ++CornerIndex)
			{
				const FVector LocalVoxPosition = VoxCorners[CornerIndex] - Pivot;
				UnrealCorners[CornerIndex] = CodexVox::TransformVectorToUnreal(LocalVoxPosition) * Input.VoxelSize;
			}

			const FVector UnrealNormal = CodexVox::TransformVectorToUnreal(GetFaceNormal(Face)).GetSafeNormal();
			FVector UnrealTangent = (UnrealCorners[1] - UnrealCorners[0]).GetSafeNormal();
			FVector UnrealBitangent = (UnrealCorners[3] - UnrealCorners[0]).GetSafeNormal();
			if (UnrealTangent.IsNearlyZero() || FMath::Abs(FVector::DotProduct(UnrealTangent, UnrealNormal)) > 0.999)
			{
				UnrealTangent = FVector::VectorPlaneProject(UnrealBitangent, UnrealNormal).GetSafeNormal();
			}

			if (UnrealBitangent.IsNearlyZero())
			{
				UnrealBitangent = FVector::CrossProduct(UnrealNormal, UnrealTangent).GetSafeNormal();
			}

			const float BinormalSign = FVector::DotProduct(FVector::CrossProduct(UnrealNormal, UnrealTangent), UnrealBitangent) < 0.0f ? -1.0f : 1.0f;

			FVertexID VertexIds[4];
			FVertexInstanceID VertexInstanceIds[4];
			for (int32 CornerIndex = 0; CornerIndex < 4; ++CornerIndex)
			{
				VertexIds[CornerIndex] = Builder.AppendVertex(UnrealCorners[CornerIndex]);
				VertexInstanceIds[CornerIndex] = Builder.AppendInstance(VertexIds[CornerIndex]);
				Builder.SetInstanceNormal(VertexInstanceIds[CornerIndex], UnrealNormal);
				Builder.SetInstanceTangentSpace(VertexInstanceIds[CornerIndex], UnrealNormal, UnrealTangent, BinormalSign);
				Builder.SetInstanceUV(VertexInstanceIds[CornerIndex], FaceUvs[CornerIndex], 0);
				Builder.SetInstanceColor(VertexInstanceIds[CornerIndex], VertexColor);
			}

			const FVector WindingNormal = FVector::CrossProduct(
				UnrealCorners[1] - UnrealCorners[0],
				UnrealCorners[2] - UnrealCorners[0]).GetSafeNormal();
			const bool bNeedsWindingFlip = FVector::DotProduct(WindingNormal, UnrealNormal) < 0.0f;

			if (bNeedsWindingFlip)
			{
				Builder.AppendTriangle(VertexInstanceIds[0], VertexInstanceIds[1], VertexInstanceIds[2], PolygonGroupId);
				Builder.AppendTriangle(VertexInstanceIds[0], VertexInstanceIds[2], VertexInstanceIds[3], PolygonGroupId);
			}
			else
			{
				Builder.AppendTriangle(VertexInstanceIds[0], VertexInstanceIds[2], VertexInstanceIds[1], PolygonGroupId);
				Builder.AppendTriangle(VertexInstanceIds[0], VertexInstanceIds[3], VertexInstanceIds[2], PolygonGroupId);
			}

			++OutBuildOutput.ExposedFaceCount;
		}
	}

	Builder.ResumeMeshDescriptionIndexing();
	Builder.SetAllEdgesHardness(true);
	OutBuildOutput.LocalBounds = Builder.ComputeBoundingBox();

	if (OutBuildOutput.ExposedFaceCount == 0)
	{
		OutError = TEXT("VOX mesh build produced zero visible faces.");
		return false;
	}

	return true;
}
