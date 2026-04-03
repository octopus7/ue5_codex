#include "VoxSmoothMeshBuilder.h"

#include "Generators/MarchingCubes.h"
#include "MeshDescription.h"
#include "MeshDescriptionBuilder.h"
#include "StaticMeshAttributes.h"

namespace
{
	struct FVoxColorGrid
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

	struct FDensityGrid
	{
		FIntVector MinCell = FIntVector::ZeroValue;
		FIntVector Size = FIntVector::ZeroValue;
		TArray<float> Samples;

		void Initialize(const FIntVector& InMinCell, const FIntVector& InSize)
		{
			MinCell = InMinCell;
			Size = InSize;
			Samples.Init(0.0f, Size.X * Size.Y * Size.Z);
		}

		bool IsInside(int32 X, int32 Y, int32 Z) const
		{
			return X >= 0 && Y >= 0 && Z >= 0 && X < Size.X && Y < Size.Y && Z < Size.Z;
		}

		int32 ToIndex(int32 X, int32 Y, int32 Z) const
		{
			return X + (Y * Size.X) + (Z * Size.X * Size.Y);
		}

		float Get(int32 X, int32 Y, int32 Z) const
		{
			if (!IsInside(X, Y, Z))
			{
				return 0.0f;
			}

			return Samples[ToIndex(X, Y, Z)];
		}

		void Set(int32 X, int32 Y, int32 Z, float Value)
		{
			Samples[ToIndex(X, Y, Z)] = Value;
		}
	};

	void ComputeOccupiedBounds(const FVoxModelData& Model, FIntVector& OutMin, FIntVector& OutMax)
	{
		OutMin = FIntVector(INT32_MAX, INT32_MAX, INT32_MAX);
		OutMax = FIntVector(INT32_MIN, INT32_MIN, INT32_MIN);

		for (const FVoxVoxel& Voxel : Model.Voxels)
		{
			OutMin.X = FMath::Min(OutMin.X, Voxel.X);
			OutMin.Y = FMath::Min(OutMin.Y, Voxel.Y);
			OutMin.Z = FMath::Min(OutMin.Z, Voxel.Z);

			OutMax.X = FMath::Max(OutMax.X, Voxel.X + 1);
			OutMax.Y = FMath::Max(OutMax.Y, Voxel.Y + 1);
			OutMax.Z = FMath::Max(OutMax.Z, Voxel.Z + 1);
		}
	}

	void BuildColorGrid(const FVoxModelData& Model, FVoxColorGrid& OutGrid)
	{
		OutGrid.Initialize(Model.Size);
		for (const FVoxVoxel& Voxel : Model.Voxels)
		{
			if (OutGrid.IsInside(Voxel.X, Voxel.Y, Voxel.Z))
			{
				OutGrid.Set(Voxel.X, Voxel.Y, Voxel.Z, Voxel.ColorIndex);
			}
		}
	}

	void BuildDensityGrid(const FVoxModelData& Model, const FIntVector& MinCell, const FIntVector& Size, FDensityGrid& OutGrid)
	{
		OutGrid.Initialize(MinCell, Size);

		for (const FVoxVoxel& Voxel : Model.Voxels)
		{
			const FIntVector LocalCoord = FIntVector(Voxel.X, Voxel.Y, Voxel.Z) - MinCell;
			if (OutGrid.IsInside(LocalCoord.X, LocalCoord.Y, LocalCoord.Z))
			{
				OutGrid.Set(LocalCoord.X, LocalCoord.Y, LocalCoord.Z, 1.0f);
			}
		}
	}

	void BlurDensityAxis(const TArray<float>& Source, TArray<float>& Destination, const FIntVector& Size, int32 AxisIndex)
	{
		Destination.SetNumZeroed(Source.Num());

		const auto Sample = [&Source, &Size](int32 X, int32 Y, int32 Z) -> float
		{
			if (X < 0 || Y < 0 || Z < 0 || X >= Size.X || Y >= Size.Y || Z >= Size.Z)
			{
				return 0.0f;
			}

			return Source[X + (Y * Size.X) + (Z * Size.X * Size.Y)];
		};

		for (int32 Z = 0; Z < Size.Z; ++Z)
		{
			for (int32 Y = 0; Y < Size.Y; ++Y)
			{
				for (int32 X = 0; X < Size.X; ++X)
				{
					FIntVector Coord(X, Y, Z);
					FIntVector Prev = Coord;
					FIntVector Next = Coord;
					Prev[AxisIndex] -= 1;
					Next[AxisIndex] += 1;

					Destination[X + (Y * Size.X) + (Z * Size.X * Size.Y)] =
						(Sample(Prev.X, Prev.Y, Prev.Z) +
						(2.0f * Sample(Coord.X, Coord.Y, Coord.Z)) +
						Sample(Next.X, Next.Y, Next.Z)) * 0.25f;
				}
			}
		}
	}

	void ApplyDensityBlur(FDensityGrid& DensityGrid, float BlendAmount)
	{
		if (BlendAmount <= KINDA_SMALL_NUMBER)
		{
			return;
		}

		TArray<float> BlurX;
		TArray<float> BlurY;
		TArray<float> BlurZ;
		BlurDensityAxis(DensityGrid.Samples, BlurX, DensityGrid.Size, 0);
		BlurDensityAxis(BlurX, BlurY, DensityGrid.Size, 1);
		BlurDensityAxis(BlurY, BlurZ, DensityGrid.Size, 2);

		const float ClampedBlend = FMath::Clamp(BlendAmount, 0.0f, 1.0f);
		for (int32 SampleIndex = 0; SampleIndex < DensityGrid.Samples.Num(); ++SampleIndex)
		{
			DensityGrid.Samples[SampleIndex] = FMath::Lerp(DensityGrid.Samples[SampleIndex], BlurZ[SampleIndex], ClampedBlend);
		}
	}

	float SampleDensity(const FDensityGrid& DensityGrid, const FVector& Position)
	{
		const FVector GridSpace = Position - FVector(DensityGrid.MinCell) - FVector(0.5f, 0.5f, 0.5f);
		const FIntVector Base(
			FMath::FloorToInt(GridSpace.X),
			FMath::FloorToInt(GridSpace.Y),
			FMath::FloorToInt(GridSpace.Z));

		const FVector Fraction(
			GridSpace.X - static_cast<float>(Base.X),
			GridSpace.Y - static_cast<float>(Base.Y),
			GridSpace.Z - static_cast<float>(Base.Z));

		const float C000 = DensityGrid.Get(Base.X, Base.Y, Base.Z);
		const float C100 = DensityGrid.Get(Base.X + 1, Base.Y, Base.Z);
		const float C010 = DensityGrid.Get(Base.X, Base.Y + 1, Base.Z);
		const float C110 = DensityGrid.Get(Base.X + 1, Base.Y + 1, Base.Z);
		const float C001 = DensityGrid.Get(Base.X, Base.Y, Base.Z + 1);
		const float C101 = DensityGrid.Get(Base.X + 1, Base.Y, Base.Z + 1);
		const float C011 = DensityGrid.Get(Base.X, Base.Y + 1, Base.Z + 1);
		const float C111 = DensityGrid.Get(Base.X + 1, Base.Y + 1, Base.Z + 1);

		const float X00 = FMath::Lerp(C000, C100, Fraction.X);
		const float X10 = FMath::Lerp(C010, C110, Fraction.X);
		const float X01 = FMath::Lerp(C001, C101, Fraction.X);
		const float X11 = FMath::Lerp(C011, C111, Fraction.X);

		const float Y0 = FMath::Lerp(X00, X10, Fraction.Y);
		const float Y1 = FMath::Lerp(X01, X11, Fraction.Y);
		return FMath::Lerp(Y0, Y1, Fraction.Z);
	}

	FVector SampleGradient(const FDensityGrid& DensityGrid, const FVector& Position, float Step)
	{
		const float SampleStep = FMath::Max(0.05f, Step);
		const float DX = SampleDensity(DensityGrid, Position + FVector(SampleStep, 0.0f, 0.0f)) -
			SampleDensity(DensityGrid, Position - FVector(SampleStep, 0.0f, 0.0f));
		const float DY = SampleDensity(DensityGrid, Position + FVector(0.0f, SampleStep, 0.0f)) -
			SampleDensity(DensityGrid, Position - FVector(0.0f, SampleStep, 0.0f));
		const float DZ = SampleDensity(DensityGrid, Position + FVector(0.0f, 0.0f, SampleStep)) -
			SampleDensity(DensityGrid, Position - FVector(0.0f, 0.0f, SampleStep));
		return FVector(DX, DY, DZ);
	}

	float SquaredDistanceToVoxelBounds(const FVector& Position, int32 X, int32 Y, int32 Z)
	{
		const double MinX = static_cast<double>(X);
		const double MinY = static_cast<double>(Y);
		const double MinZ = static_cast<double>(Z);
		const double MaxX = MinX + 1.0;
		const double MaxY = MinY + 1.0;
		const double MaxZ = MinZ + 1.0;

		const double DX = Position.X < MinX ? MinX - Position.X : (Position.X > MaxX ? Position.X - MaxX : 0.0);
		const double DY = Position.Y < MinY ? MinY - Position.Y : (Position.Y > MaxY ? Position.Y - MaxY : 0.0);
		const double DZ = Position.Z < MinZ ? MinZ - Position.Z : (Position.Z > MaxZ ? Position.Z - MaxZ : 0.0);

		return static_cast<float>((DX * DX) + (DY * DY) + (DZ * DZ));
	}

	uint8 FindNearestColorIndex(const FVoxColorGrid& Grid, TConstArrayView<FVoxVoxel> Voxels, const FVector& Position)
	{
		const FIntVector Base(
			FMath::FloorToInt(Position.X),
			FMath::FloorToInt(Position.Y),
			FMath::FloorToInt(Position.Z));

		for (int32 Radius = 1; Radius <= 3; ++Radius)
		{
			float BestDistanceSquared = TNumericLimits<float>::Max();
			uint8 BestColorIndex = 0;

			for (int32 Z = Base.Z - Radius; Z <= Base.Z + Radius; ++Z)
			{
				for (int32 Y = Base.Y - Radius; Y <= Base.Y + Radius; ++Y)
				{
					for (int32 X = Base.X - Radius; X <= Base.X + Radius; ++X)
					{
						const uint8 ColorIndex = Grid.Get(X, Y, Z);
						if (ColorIndex == 0)
						{
							continue;
						}

						const float DistanceSquared = SquaredDistanceToVoxelBounds(Position, X, Y, Z);
						if (DistanceSquared < BestDistanceSquared)
						{
							BestDistanceSquared = DistanceSquared;
							BestColorIndex = ColorIndex;
						}
					}
				}
			}

			if (BestColorIndex != 0)
			{
				return BestColorIndex;
			}
		}

		float BestDistanceSquared = TNumericLimits<float>::Max();
		uint8 BestColorIndex = 0;
		for (const FVoxVoxel& Voxel : Voxels)
		{
			const float DistanceSquared = SquaredDistanceToVoxelBounds(Position, Voxel.X, Voxel.Y, Voxel.Z);
			if (DistanceSquared < BestDistanceSquared)
			{
				BestDistanceSquared = DistanceSquared;
				BestColorIndex = Voxel.ColorIndex;
			}
		}

		return BestColorIndex;
	}

	FVector ComputeTangent(const FVector& Normal)
	{
		FVector Tangent = FVector::CrossProduct(FVector::UpVector, Normal);
		if (Tangent.SquaredLength() < KINDA_SMALL_NUMBER)
		{
			Tangent = FVector::CrossProduct(FVector::RightVector, Normal);
		}
		return Tangent.GetSafeNormal();
	}
}

bool FVoxSmoothMeshBuilder::BuildSmoothMeshDescription(
	const FVoxModelData& Model,
	FMeshDescription& OutMeshDescription,
	FString& OutError,
	float VoxelSize,
	const FVoxSmoothBuildSettings& Settings,
	FVoxSmoothBuildMetadata* OutMetadata)
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

	FIntVector BoundsMin;
	FIntVector BoundsMax;
	ComputeOccupiedBounds(Model, BoundsMin, BoundsMax);

	const int32 Padding = FMath::Max(1, Settings.PaddingVoxels);
	const FIntVector DensityMin = BoundsMin - FIntVector(Padding, Padding, Padding);
	const FIntVector DensitySize = (BoundsMax - BoundsMin) + FIntVector(Padding * 2, Padding * 2, Padding * 2);

	FDensityGrid DensityGrid;
	BuildDensityGrid(Model, DensityMin, DensitySize, DensityGrid);
	ApplyDensityBlur(DensityGrid, Settings.DensityBlurBlend);

	FVoxColorGrid ColorGrid;
	BuildColorGrid(Model, ColorGrid);

	const FVector DensityExtent(DensitySize);
	const float MaxDensityExtent = FMath::Max3(DensityExtent.X, DensityExtent.Y, DensityExtent.Z);
	float ResolutionScale = FMath::Max(1.0f, Settings.DesiredResolutionScale);
	if (Settings.MaxGridDimension > 0 && (MaxDensityExtent * ResolutionScale) > static_cast<float>(Settings.MaxGridDimension))
	{
		ResolutionScale = static_cast<float>(Settings.MaxGridDimension) / MaxDensityExtent;
		ResolutionScale = FMath::Max(1.0f, ResolutionScale);
	}

	UE::Geometry::FMarchingCubes MarchingCubes;
	MarchingCubes.IsoValue = Settings.IsoValue;
	MarchingCubes.CubeSize = 1.0 / static_cast<double>(ResolutionScale);
	MarchingCubes.Bounds = UE::Geometry::TAxisAlignedBox3<double>(
		FVector3d(DensityMin.X, DensityMin.Y, DensityMin.Z),
		FVector3d(DensityMin.X + DensitySize.X, DensityMin.Y + DensitySize.Y, DensityMin.Z + DensitySize.Z));
	MarchingCubes.bEnableValueCaching = false;
	MarchingCubes.bParallelCompute = true;
	MarchingCubes.SafetyMaxDimension = Settings.MaxGridDimension > 0 ? Settings.MaxGridDimension + 4 : 4096;
	MarchingCubes.Implicit = [&DensityGrid](UE::Math::TVector<double> Position) -> double
	{
		return static_cast<double>(SampleDensity(DensityGrid, FVector(Position.X, Position.Y, Position.Z)));
	};
	MarchingCubes.Generate();

	if (MarchingCubes.Vertices.IsEmpty() || MarchingCubes.Triangles.IsEmpty())
	{
		OutError = TEXT("No visible smooth surface was generated from the voxel model.");
		return false;
	}

	if (OutMetadata)
	{
		OutMetadata->ResolutionScaleUsed = ResolutionScale;
		OutMetadata->GridResolution = FIntVector(MarchingCubes.CellDimensions.X, MarchingCubes.CellDimensions.Y, MarchingCubes.CellDimensions.Z);
	}

	const float GradientStep = 0.5f / ResolutionScale;
	const FVector PivotOffset = (FVector(BoundsMin) + FVector(BoundsMax)) * 0.5f * VoxelSize;

	TArray<FVector> VertexNormals;
	VertexNormals.Init(FVector::ZeroVector, MarchingCubes.Vertices.Num());

	for (const UE::Geometry::FIndex3i& Triangle : MarchingCubes.Triangles)
	{
		const FVector A = FVector(MarchingCubes.Vertices[Triangle.A]);
		const FVector B = FVector(MarchingCubes.Vertices[Triangle.B]);
		const FVector C = FVector(MarchingCubes.Vertices[Triangle.C]);
		const FVector TriangleNormal = FVector::CrossProduct(B - A, C - A);
		if (TriangleNormal.SquaredLength() > KINDA_SMALL_NUMBER)
		{
			VertexNormals[Triangle.A] += TriangleNormal;
			VertexNormals[Triangle.B] += TriangleNormal;
			VertexNormals[Triangle.C] += TriangleNormal;
		}
	}

	TArray<FColor> VertexColors;
	VertexColors.SetNum(MarchingCubes.Vertices.Num());
	for (int32 VertexIndex = 0; VertexIndex < MarchingCubes.Vertices.Num(); ++VertexIndex)
	{
		FVector& VertexNormal = VertexNormals[VertexIndex];
		if (!VertexNormal.Normalize())
		{
			const FVector Gradient = SampleGradient(DensityGrid, FVector(MarchingCubes.Vertices[VertexIndex]), GradientStep);
			VertexNormal = (-Gradient).GetSafeNormal(UE_SMALL_NUMBER, FVector::UpVector);
		}

		const uint8 ColorIndex = FindNearestColorIndex(ColorGrid, Model.Voxels, FVector(MarchingCubes.Vertices[VertexIndex]));
		VertexColors[VertexIndex] = Model.GetColor(ColorIndex);
	}

	FStaticMeshAttributes Attributes(OutMeshDescription);
	Attributes.Register();

	FMeshDescriptionBuilder Builder;
	Builder.SetMeshDescription(&OutMeshDescription);
	Builder.SetNumUVLayers(1);
	const FPolygonGroupID PolygonGroup = Builder.AppendPolygonGroup(TEXT("VoxelMaterial"));

	TArray<FVertexID> VertexIds;
	VertexIds.SetNum(MarchingCubes.Vertices.Num());
	for (int32 VertexIndex = 0; VertexIndex < MarchingCubes.Vertices.Num(); ++VertexIndex)
	{
		const FVector WorldPosition = (FVector(MarchingCubes.Vertices[VertexIndex]) * VoxelSize) - PivotOffset;
		VertexIds[VertexIndex] = Builder.AppendVertex(WorldPosition);
	}

	for (const UE::Geometry::FIndex3i& Triangle : MarchingCubes.Triangles)
	{
		const int32 CornerIndices[3] = { Triangle.A, Triangle.B, Triangle.C };
		FVertexInstanceID Instances[3];

		for (int32 Corner = 0; Corner < 3; ++Corner)
		{
			const int32 VertexIndex = CornerIndices[Corner];
			Instances[Corner] = Builder.AppendInstance(VertexIds[VertexIndex]);
			Builder.SetInstanceUV(Instances[Corner], FVector2D(0.0f, 0.0f), 0);
			Builder.SetInstanceTangentSpace(Instances[Corner], VertexNormals[VertexIndex], ComputeTangent(VertexNormals[VertexIndex]), 1.0f);
			Builder.SetInstanceColor(Instances[Corner], FVector4f(FLinearColor::FromSRGBColor(VertexColors[VertexIndex])));
		}

		Builder.AppendTriangle(Instances[0], Instances[1], Instances[2], PolygonGroup);
	}

	return true;
}
