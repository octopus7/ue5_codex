#include "VoxMeshBuilder.h"

#include "MeshDescription.h"
#include "MeshDescriptionBuilder.h"
#include "StaticMeshAttributes.h"

namespace
{
	struct FVoxGrid
	{
		FIntVector Size = FIntVector::ZeroValue;
		TArray<uint8> Cells;

		void Initialize(const FIntVector& InSize)
		{
			Size = InSize;
			Cells.Init(0, Size.X * Size.Y * Size.Z);
		}

		bool IsInside(int32 X, int32 Y, int32 Z) const
		{
			return X >= 0 && Y >= 0 && Z >= 0 && X < Size.X && Y < Size.Y && Z < Size.Z;
		}

		int32 ToIndex(int32 X, int32 Y, int32 Z) const
		{
			return X + (Y * Size.X) + (Z * Size.X * Size.Y);
		}

		uint8 Get(int32 X, int32 Y, int32 Z) const
		{
			if (!IsInside(X, Y, Z))
			{
				return 0;
			}

			return Cells[ToIndex(X, Y, Z)];
		}

		void Set(int32 X, int32 Y, int32 Z, uint8 ColorIndex)
		{
			Cells[ToIndex(X, Y, Z)] = ColorIndex;
		}
	};

	struct FMaskCell
	{
		uint8 ColorIndex = 0;
	};

	FVector AxisUnitVector(int32 AxisIndex)
	{
		if (AxisIndex == 0)
		{
			return FVector(1.0, 0.0, 0.0);
		}

		if (AxisIndex == 1)
		{
			return FVector(0.0, 1.0, 0.0);
		}

		return FVector(0.0, 0.0, 1.0);
	}

	FVector MakePosition(float D, float U, float V, int32 AxisIndex, int32 UAxis, int32 VAxis)
	{
		FVector Position(0.0, 0.0, 0.0);
		Position[AxisIndex] = D;
		Position[UAxis] = U;
		Position[VAxis] = V;
		return Position;
	}

	void AddQuad(
		FMeshDescriptionBuilder& Builder,
		const FPolygonGroupID PolygonGroup,
		const FVector& P0,
		const FVector& P1,
		const FVector& P2,
		const FVector& P3,
		const FVector& Normal,
		const FVector& Tangent,
		const FVector4f& Color)
	{
		auto AppendVertexInstance = [&Builder, &Normal, &Tangent, &Color](const FVector& Position, const FVector2D& UV)
		{
			const FVertexID VertexId = Builder.AppendVertex(Position);
			const FVertexInstanceID InstanceId = Builder.AppendInstance(VertexId);
			Builder.SetInstanceUV(InstanceId, UV, 0);
			Builder.SetInstanceTangentSpace(InstanceId, Normal, Tangent, 1.0f);
			Builder.SetInstanceColor(InstanceId, Color);
			return InstanceId;
		};

		const FVertexInstanceID I0 = AppendVertexInstance(P0, FVector2D(0.0, 0.0));
		const FVertexInstanceID I1 = AppendVertexInstance(P1, FVector2D(1.0, 0.0));
		const FVertexInstanceID I2 = AppendVertexInstance(P2, FVector2D(1.0, 1.0));
		const FVertexInstanceID I3 = AppendVertexInstance(P3, FVector2D(0.0, 1.0));

		Builder.AppendTriangle(I0, I1, I2, PolygonGroup);
		Builder.AppendTriangle(I0, I2, I3, PolygonGroup);
	}
}

bool FVoxMeshBuilder::BuildMeshDescription(const FVoxModelData& Model, FMeshDescription& OutMeshDescription, FString& OutError, float VoxelSize)
{
	if (VoxelSize <= 0.0f)
	{
		OutError = TEXT("Voxel size must be greater than zero.");
		return false;
	}

	if (Model.Size.X <= 0 || Model.Size.Y <= 0 || Model.Size.Z <= 0)
	{
		OutError = TEXT("Model size must be positive on every axis.");
		return false;
	}

	if (Model.Voxels.IsEmpty())
	{
		OutError = TEXT("Model has no voxels.");
		return false;
	}

	FVoxGrid Grid;
	Grid.Initialize(Model.Size);

	FIntVector BoundsMin(INT32_MAX, INT32_MAX, INT32_MAX);
	FIntVector BoundsMax(INT32_MIN, INT32_MIN, INT32_MIN);

	for (const FVoxVoxel& Voxel : Model.Voxels)
	{
		if (!Grid.IsInside(Voxel.X, Voxel.Y, Voxel.Z))
		{
			OutError = FString::Printf(TEXT("Voxel (%d, %d, %d) is outside model bounds."), Voxel.X, Voxel.Y, Voxel.Z);
			return false;
		}

		Grid.Set(Voxel.X, Voxel.Y, Voxel.Z, Voxel.ColorIndex);

		BoundsMin.X = FMath::Min(BoundsMin.X, Voxel.X);
		BoundsMin.Y = FMath::Min(BoundsMin.Y, Voxel.Y);
		BoundsMin.Z = FMath::Min(BoundsMin.Z, Voxel.Z);

		BoundsMax.X = FMath::Max(BoundsMax.X, Voxel.X + 1);
		BoundsMax.Y = FMath::Max(BoundsMax.Y, Voxel.Y + 1);
		BoundsMax.Z = FMath::Max(BoundsMax.Z, Voxel.Z + 1);
	}

	const FVector PivotOffset = (FVector(BoundsMin) + FVector(BoundsMax)) * 0.5f * VoxelSize;

	FStaticMeshAttributes Attributes(OutMeshDescription);
	Attributes.Register();

	FMeshDescriptionBuilder Builder;
	Builder.SetMeshDescription(&OutMeshDescription);
	Builder.SetNumUVLayers(1);

	const FPolygonGroupID PolygonGroup = Builder.AppendPolygonGroup(TEXT("VoxelMaterial"));

	for (int32 AxisIndex = 0; AxisIndex < 3; ++AxisIndex)
	{
		const int32 UAxis = (AxisIndex + 1) % 3;
		const int32 VAxis = (AxisIndex + 2) % 3;
		const int32 PlaneCount = Model.Size[AxisIndex] + 1;
		const int32 MaskWidth = Model.Size[UAxis];
		const int32 MaskHeight = Model.Size[VAxis];

		TArray<FMaskCell> Mask;
		Mask.SetNumZeroed(MaskWidth * MaskHeight);

		for (int32 Sign = -1; Sign <= 1; Sign += 2)
		{
			for (int32 Plane = 0; Plane < PlaneCount; ++Plane)
			{
				for (FMaskCell& Cell : Mask)
				{
					Cell.ColorIndex = 0;
				}

				for (int32 V = 0; V < MaskHeight; ++V)
				{
					for (int32 U = 0; U < MaskWidth; ++U)
					{
						FIntVector FilledCoord(0, 0, 0);
						FIntVector EmptyCoord(0, 0, 0);

						FilledCoord[UAxis] = U;
						FilledCoord[VAxis] = V;
						EmptyCoord[UAxis] = U;
						EmptyCoord[VAxis] = V;

						if (Sign > 0)
						{
							FilledCoord[AxisIndex] = Plane - 1;
							EmptyCoord[AxisIndex] = Plane;
						}
						else
						{
							FilledCoord[AxisIndex] = Plane;
							EmptyCoord[AxisIndex] = Plane - 1;
						}

						const uint8 FilledColor = Grid.Get(FilledCoord.X, FilledCoord.Y, FilledCoord.Z);
						const uint8 EmptyColor = Grid.Get(EmptyCoord.X, EmptyCoord.Y, EmptyCoord.Z);
						if (FilledColor != 0 && EmptyColor == 0)
						{
							Mask[U + (V * MaskWidth)].ColorIndex = FilledColor;
						}
					}
				}

				for (int32 V = 0; V < MaskHeight; ++V)
				{
					for (int32 U = 0; U < MaskWidth;)
					{
						const uint8 ColorIndex = Mask[U + (V * MaskWidth)].ColorIndex;
						if (ColorIndex == 0)
						{
							++U;
							continue;
						}

						int32 Width = 1;
						while (U + Width < MaskWidth && Mask[(U + Width) + (V * MaskWidth)].ColorIndex == ColorIndex)
						{
							++Width;
						}

						int32 Height = 1;
						bool bCanGrow = true;
						while (V + Height < MaskHeight && bCanGrow)
						{
							for (int32 DeltaU = 0; DeltaU < Width; ++DeltaU)
							{
								if (Mask[(U + DeltaU) + ((V + Height) * MaskWidth)].ColorIndex != ColorIndex)
								{
									bCanGrow = false;
									break;
								}
							}

							if (bCanGrow)
							{
								++Height;
							}
						}

						for (int32 ClearV = 0; ClearV < Height; ++ClearV)
						{
							for (int32 ClearU = 0; ClearU < Width; ++ClearU)
							{
								Mask[(U + ClearU) + ((V + ClearV) * MaskWidth)].ColorIndex = 0;
							}
						}

						const FVector Normal = AxisUnitVector(AxisIndex) * static_cast<double>(Sign);
						const FVector Tangent = AxisUnitVector(UAxis);

						const float PlanePosition = static_cast<float>(Plane) * VoxelSize;
						const float U0 = static_cast<float>(U) * VoxelSize;
						const float V0 = static_cast<float>(V) * VoxelSize;
						const float U1 = static_cast<float>(U + Width) * VoxelSize;
						const float V1 = static_cast<float>(V + Height) * VoxelSize;

						FVector P0;
						FVector P1;
						FVector P2;
						FVector P3;

						if (Sign > 0)
						{
							P0 = MakePosition(PlanePosition, U0, V0, AxisIndex, UAxis, VAxis);
							P1 = MakePosition(PlanePosition, U0, V1, AxisIndex, UAxis, VAxis);
							P2 = MakePosition(PlanePosition, U1, V1, AxisIndex, UAxis, VAxis);
							P3 = MakePosition(PlanePosition, U1, V0, AxisIndex, UAxis, VAxis);
						}
						else
						{
							P0 = MakePosition(PlanePosition, U0, V0, AxisIndex, UAxis, VAxis);
							P1 = MakePosition(PlanePosition, U1, V0, AxisIndex, UAxis, VAxis);
							P2 = MakePosition(PlanePosition, U1, V1, AxisIndex, UAxis, VAxis);
							P3 = MakePosition(PlanePosition, U0, V1, AxisIndex, UAxis, VAxis);
						}

						P0 -= PivotOffset;
						P1 -= PivotOffset;
						P2 -= PivotOffset;
						P3 -= PivotOffset;

						const FColor FaceColor = Model.GetColor(ColorIndex);
						const FVector4f PackedColor(
							static_cast<float>(FaceColor.R) / 255.0f,
							static_cast<float>(FaceColor.G) / 255.0f,
							static_cast<float>(FaceColor.B) / 255.0f,
							static_cast<float>(FaceColor.A) / 255.0f);

						AddQuad(Builder, PolygonGroup, P0, P1, P2, P3, Normal, Tangent, PackedColor);
						U += Width;
					}
				}
			}
		}
	}

	if (OutMeshDescription.Vertices().Num() == 0 || OutMeshDescription.Triangles().Num() == 0)
	{
		OutError = TEXT("No visible surface was generated from the voxel model.");
		return false;
	}

	return true;
}
