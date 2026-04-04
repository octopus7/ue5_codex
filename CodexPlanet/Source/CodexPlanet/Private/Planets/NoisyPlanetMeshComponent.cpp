// Copyright Epic Games, Inc. All Rights Reserved.

#include "Planets/NoisyPlanetMeshComponent.h"

#include "KismetProceduralMeshLibrary.h"

UNoisyPlanetMeshComponent::UNoisyPlanetMeshComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	bUseComplexAsSimpleCollision = true;
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetCollisionObjectType(ECC_WorldStatic);
	SetCollisionResponseToAllChannels(ECR_Block);
	CastShadow = true;
	bCastDynamicShadow = true;
}

void UNoisyPlanetMeshComponent::RebuildPlanetMesh()
{
	const int32 SafeLatitudeSegments = FMath::Max(6, LatitudeSegments);
	const int32 SafeLongitudeSegments = FMath::Max(8, LongitudeSegments);

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector2D> UVs;
	TArray<FLinearColor> VertexColors;
	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;

	Vertices.Reserve((SafeLongitudeSegments * (SafeLatitudeSegments - 1)) + 2);
	UVs.Reserve((SafeLongitudeSegments * (SafeLatitudeSegments - 1)) + 2);

	Vertices.Add(FVector(0.0f, 0.0f, SampleSurfaceRadius(FVector::UpVector)));
	UVs.Add(FVector2D(0.5f, 0.0f));

	for (int32 LatitudeIndex = 1; LatitudeIndex < SafeLatitudeSegments; ++LatitudeIndex)
	{
		const float LatitudeAlpha = static_cast<float>(LatitudeIndex) / static_cast<float>(SafeLatitudeSegments);
		const float Theta = LatitudeAlpha * PI;
		const float SinTheta = FMath::Sin(Theta);
		const float CosTheta = FMath::Cos(Theta);

		for (int32 LongitudeIndex = 0; LongitudeIndex < SafeLongitudeSegments; ++LongitudeIndex)
		{
			const float LongitudeAlpha = static_cast<float>(LongitudeIndex) / static_cast<float>(SafeLongitudeSegments);
			const float Phi = LongitudeAlpha * 2.0f * PI;

			const FVector UnitDirection(
				SinTheta * FMath::Cos(Phi),
				SinTheta * FMath::Sin(Phi),
				CosTheta);

			const float SurfaceRadius = SampleSurfaceRadius(UnitDirection);

			Vertices.Add(UnitDirection * SurfaceRadius);
			UVs.Add(FVector2D(LongitudeAlpha, LatitudeAlpha));
		}
	}

	const int32 BottomVertexIndex = Vertices.Add(FVector(0.0f, 0.0f, -SampleSurfaceRadius(-FVector::UpVector)));
	UVs.Add(FVector2D(0.5f, 1.0f));

	Triangles.Reserve(SafeLongitudeSegments * ((SafeLatitudeSegments - 2) * 6 + 6));

	const int32 FirstRingStart = 1;
	const int32 LastRingStart = BottomVertexIndex - SafeLongitudeSegments;

	for (int32 LongitudeIndex = 0; LongitudeIndex < SafeLongitudeSegments; ++LongitudeIndex)
	{
		const int32 CurrentVertex = FirstRingStart + LongitudeIndex;
		const int32 NextVertex = FirstRingStart + ((LongitudeIndex + 1) % SafeLongitudeSegments);

		Triangles.Add(0);
		Triangles.Add(CurrentVertex);
		Triangles.Add(NextVertex);
	}

	for (int32 LatitudeIndex = 0; LatitudeIndex < SafeLatitudeSegments - 2; ++LatitudeIndex)
	{
		const int32 CurrentRingStart = FirstRingStart + (LatitudeIndex * SafeLongitudeSegments);
		const int32 NextRingStart = CurrentRingStart + SafeLongitudeSegments;

		for (int32 LongitudeIndex = 0; LongitudeIndex < SafeLongitudeSegments; ++LongitudeIndex)
		{
			const int32 CurrentVertex = CurrentRingStart + LongitudeIndex;
			const int32 NextVertex = CurrentRingStart + ((LongitudeIndex + 1) % SafeLongitudeSegments);
			const int32 BelowVertex = NextRingStart + LongitudeIndex;
			const int32 BelowNextVertex = NextRingStart + ((LongitudeIndex + 1) % SafeLongitudeSegments);

			Triangles.Add(CurrentVertex);
			Triangles.Add(BelowVertex);
			Triangles.Add(NextVertex);

			Triangles.Add(NextVertex);
			Triangles.Add(BelowVertex);
			Triangles.Add(BelowNextVertex);
		}
	}

	for (int32 LongitudeIndex = 0; LongitudeIndex < SafeLongitudeSegments; ++LongitudeIndex)
	{
		const int32 CurrentVertex = LastRingStart + LongitudeIndex;
		const int32 NextVertex = LastRingStart + ((LongitudeIndex + 1) % SafeLongitudeSegments);

		Triangles.Add(CurrentVertex);
		Triangles.Add(BottomVertexIndex);
		Triangles.Add(NextVertex);
	}

	for (int32 TriangleIndex = 0; TriangleIndex < Triangles.Num(); TriangleIndex += 3)
	{
		const FVector& A = Vertices[Triangles[TriangleIndex]];
		const FVector& B = Vertices[Triangles[TriangleIndex + 1]];
		const FVector& C = Vertices[Triangles[TriangleIndex + 2]];

		const FVector FaceNormal = FVector::CrossProduct(B - A, C - A);
		const FVector TriangleCenter = (A + B + C) / 3.0f;

		if (FVector::DotProduct(FaceNormal, TriangleCenter) < 0.0f)
		{
			Swap(Triangles[TriangleIndex + 1], Triangles[TriangleIndex + 2]);
		}
	}

	VertexColors.Init(FLinearColor::White, Vertices.Num());
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UVs, Normals, Tangents);

	ClearAllMeshSections();
	CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
	SetMaterial(0, nullptr);
}

float UNoisyPlanetMeshComponent::SampleSurfaceRadius(const FVector& UnitDirection) const
{
	const float NoiseSample = FMath::PerlinNoise3D((UnitDirection * NoiseFrequency) + NoiseOffset);
	return BaseRadius + (NoiseSample * NoiseAmplitude);
}
