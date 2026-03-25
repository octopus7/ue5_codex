#pragma once

#include "CoreMinimal.h"

struct FPrototypeMeshRequest
{
	FString Prompt;
	FString AssetName;
	FString ContentPath = TEXT("/Game/Generated/PrototypeMeshes");
	FString Locale = TEXT("ko-KR");
	FString ReasoningEffort = TEXT("medium");
	int32 MaxPrimitiveCount = 32;
};

struct FPrototypePrimitiveTransform
{
	FVector LocationCm = FVector::ZeroVector;
	FVector RotationDeg = FVector::ZeroVector;
	FVector Scale = FVector(1.0, 1.0, 1.0);
};

struct FPrototypePrimitiveSpec
{
	FString Name;
	FString Type;
	FPrototypePrimitiveTransform Transform;
	double Width = 0.0;
	double Depth = 0.0;
	double Height = 0.0;
	double Radius = 0.0;
	int32 Segments = 0;
	int32 Steps = 0;
};

struct FPrototypeShapeDsl
{
	FString Version;
	FString MeshName;
	FString Units;
	FString Pivot;
	TArray<FPrototypePrimitiveSpec> Primitives;
	FString Notes;
	FString RawJson;
};

struct FGeneratedMeshBuffers
{
	TArray<FVector3f> Positions;
	TArray<int32> Indices;
	TArray<FVector3f> Normals;
	TArray<FVector2f> UV0;
	TArray<FVector4f> Colors;

	void Reset()
	{
		Positions.Reset();
		Indices.Reset();
		Normals.Reset();
		UV0.Reset();
		Colors.Reset();
	}

	int32 GetTriangleCount() const
	{
		return Indices.Num() / 3;
	}

	bool IsValid() const
	{
		return Positions.Num() > 0
			&& Positions.Num() == Normals.Num()
			&& Positions.Num() == UV0.Num()
			&& Positions.Num() == Colors.Num()
			&& Indices.Num() > 0
			&& (Indices.Num() % 3) == 0;
	}
};

struct FPrototypeBridgeResult
{
	bool bSuccess = false;
	FString ErrorMessage;
	FString RawDslJson;
	FString RawLastMessage;
	FString Diagnostics;
};

struct FPrototypeBridgeJobHandle
{
	FGuid Id;

	bool IsValid() const
	{
		return Id.IsValid();
	}
};
