#include "PrototypeMeshGeneration.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "DynamicMesh/DynamicMeshAttributeSet.h"
#include "DynamicMesh/DynamicMeshOverlay.h"
#include "DynamicMeshToMeshDescription.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MeshDescription.h"
#include "ObjectTools.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "StaticMeshAttributes.h"
#include "UObject/MetaData.h"

using namespace UE::Geometry;

namespace
{
	constexpr int32 MaxTriangleCount = 50000;
	constexpr int32 MaxSegments = 128;
	constexpr int32 MaxSteps = 32;

	bool HasOnlyKeys(const TSharedPtr<FJsonObject>& Object, const TSet<FString>& AllowedKeys, FString& OutError, const FString& Context)
	{
		for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : Object->Values)
		{
			if (!AllowedKeys.Contains(Pair.Key))
			{
				OutError = FString::Printf(TEXT("%s contains unsupported field '%s'."), *Context, *Pair.Key);
				return false;
			}
		}

		return true;
	}

	bool ReadStringField(const TSharedPtr<FJsonObject>& Object, const TCHAR* FieldName, FString& OutValue, FString& OutError, const FString& Context)
	{
		if (!Object->TryGetStringField(FieldName, OutValue))
		{
			OutError = FString::Printf(TEXT("%s is missing string field '%s'."), *Context, FieldName);
			return false;
		}

		return true;
	}

	bool ReadNumberField(const TSharedPtr<FJsonObject>& Object, const TCHAR* FieldName, double& OutValue, FString& OutError, const FString& Context)
	{
		if (!Object->TryGetNumberField(FieldName, OutValue))
		{
			OutError = FString::Printf(TEXT("%s is missing numeric field '%s'."), *Context, FieldName);
			return false;
		}

		return true;
	}

	bool ReadIntegerField(const TSharedPtr<FJsonObject>& Object, const TCHAR* FieldName, int32& OutValue, FString& OutError, const FString& Context)
	{
		double NumericValue = 0.0;
		if (!ReadNumberField(Object, FieldName, NumericValue, OutError, Context))
		{
			return false;
		}

		OutValue = static_cast<int32>(NumericValue);
		if (!FMath::IsNearlyEqual(NumericValue, static_cast<double>(OutValue)))
		{
			OutError = FString::Printf(TEXT("%s field '%s' must be an integer."), *Context, FieldName);
			return false;
		}

		return true;
	}

	bool ReadVectorField(const TSharedPtr<FJsonObject>& Object, const TCHAR* FieldName, FVector& OutVector, FString& OutError, const FString& Context)
	{
		const TSharedPtr<FJsonObject>* VectorObject = nullptr;
		if (!Object->TryGetObjectField(FieldName, VectorObject) || !VectorObject || !VectorObject->IsValid())
		{
			OutError = FString::Printf(TEXT("%s is missing object field '%s'."), *Context, FieldName);
			return false;
		}

		static const TSet<FString> AllowedKeys{TEXT("x"), TEXT("y"), TEXT("z")};
		if (!HasOnlyKeys(*VectorObject, AllowedKeys, OutError, FString::Printf(TEXT("%s.%s"), *Context, FieldName)))
		{
			return false;
		}

		double X = 0.0;
		double Y = 0.0;
		double Z = 0.0;
		if (!ReadNumberField(*VectorObject, TEXT("x"), X, OutError, FString::Printf(TEXT("%s.%s"), *Context, FieldName))
			|| !ReadNumberField(*VectorObject, TEXT("y"), Y, OutError, FString::Printf(TEXT("%s.%s"), *Context, FieldName))
			|| !ReadNumberField(*VectorObject, TEXT("z"), Z, OutError, FString::Printf(TEXT("%s.%s"), *Context, FieldName)))
		{
			return false;
		}

		OutVector = FVector(X, Y, Z);
		return true;
	}

	bool IsFiniteVector(const FVector& Vector)
	{
		return FMath::IsFinite(Vector.X) && FMath::IsFinite(Vector.Y) && FMath::IsFinite(Vector.Z);
	}

	FVector4f GetPrimitiveColor(const FPrototypePrimitiveSpec& Spec, int32 PrimitiveIndex)
	{
		static const FVector4f Palette[] = {
			FVector4f(0.95f, 0.76f, 0.28f, 1.0f),
			FVector4f(0.29f, 0.66f, 0.94f, 1.0f),
			FVector4f(0.33f, 0.84f, 0.56f, 1.0f),
			FVector4f(0.96f, 0.43f, 0.33f, 1.0f),
			FVector4f(0.64f, 0.51f, 0.93f, 1.0f),
			FVector4f(0.96f, 0.63f, 0.23f, 1.0f),
			FVector4f(0.22f, 0.78f, 0.78f, 1.0f),
			FVector4f(0.90f, 0.42f, 0.70f, 1.0f)
		};

		uint32 Hash = GetTypeHash(Spec.Type);
		Hash = HashCombine(Hash, GetTypeHash(PrimitiveIndex));
		Hash = HashCombine(Hash, GetTypeHash(Spec.Name));
		return Palette[Hash % UE_ARRAY_COUNT(Palette)];
	}

	void AppendTriangle(FGeneratedMeshBuffers& Buffers, const FTransform& Transform, const FVector& A, const FVector& B, const FVector& C, const FVector2D& UvA, const FVector2D& UvB, const FVector2D& UvC, const FVector4f& Color)
	{
		const FVector PA = Transform.TransformPosition(A);
		const FVector PB = Transform.TransformPosition(B);
		const FVector PC = Transform.TransformPosition(C);
		const FVector3f Normal = FVector3f(FVector::CrossProduct(PB - PA, PC - PA).GetSafeNormal());
		const int32 BaseIndex = Buffers.Positions.Num();

		Buffers.Positions.Add(FVector3f(PA));
		Buffers.Positions.Add(FVector3f(PC));
		Buffers.Positions.Add(FVector3f(PB));
		Buffers.Normals.Add(Normal);
		Buffers.Normals.Add(Normal);
		Buffers.Normals.Add(Normal);
		Buffers.UV0.Add(FVector2f(UvA));
		Buffers.UV0.Add(FVector2f(UvC));
		Buffers.UV0.Add(FVector2f(UvB));
		Buffers.Colors.Add(Color);
		Buffers.Colors.Add(Color);
		Buffers.Colors.Add(Color);
		Buffers.Indices.Add(BaseIndex);
		Buffers.Indices.Add(BaseIndex + 1);
		Buffers.Indices.Add(BaseIndex + 2);
	}

	void AppendQuad(FGeneratedMeshBuffers& Buffers, const FTransform& Transform, const FVector& A, const FVector& B, const FVector& C, const FVector& D, const FVector4f& Color)
	{
		AppendTriangle(Buffers, Transform, A, B, C, FVector2D(0.0, 0.0), FVector2D(1.0, 0.0), FVector2D(1.0, 1.0), Color);
		AppendTriangle(Buffers, Transform, A, C, D, FVector2D(0.0, 0.0), FVector2D(1.0, 1.0), FVector2D(0.0, 1.0), Color);
	}

	void BuildPlane(const FPrototypePrimitiveSpec& Spec, int32 PrimitiveIndex, FGeneratedMeshBuffers& Buffers)
	{
		const double HalfDepth = Spec.Depth * 0.5;
		const double HalfWidth = Spec.Width * 0.5;
		const FTransform Transform(FRotator::MakeFromEuler(Spec.Transform.RotationDeg), Spec.Transform.LocationCm, Spec.Transform.Scale);
		const FVector4f Color = GetPrimitiveColor(Spec, PrimitiveIndex);

		const FVector A(-HalfDepth, -HalfWidth, 0.0);
		const FVector B(HalfDepth, -HalfWidth, 0.0);
		const FVector C(HalfDepth, HalfWidth, 0.0);
		const FVector D(-HalfDepth, HalfWidth, 0.0);
		AppendQuad(Buffers, Transform, A, B, C, D, Color);
	}

	void BuildBox(const FPrototypePrimitiveSpec& Spec, int32 PrimitiveIndex, FGeneratedMeshBuffers& Buffers)
	{
		const double HalfDepth = Spec.Depth * 0.5;
		const double HalfWidth = Spec.Width * 0.5;
		const double Height = Spec.Height;
		const FTransform Transform(FRotator::MakeFromEuler(Spec.Transform.RotationDeg), Spec.Transform.LocationCm, Spec.Transform.Scale);
		const FVector4f Color = GetPrimitiveColor(Spec, PrimitiveIndex);

		const FVector P000(-HalfDepth, -HalfWidth, 0.0);
		const FVector P100(HalfDepth, -HalfWidth, 0.0);
		const FVector P110(HalfDepth, HalfWidth, 0.0);
		const FVector P010(-HalfDepth, HalfWidth, 0.0);
		const FVector P001(-HalfDepth, -HalfWidth, Height);
		const FVector P101(HalfDepth, -HalfWidth, Height);
		const FVector P111(HalfDepth, HalfWidth, Height);
		const FVector P011(-HalfDepth, HalfWidth, Height);

		AppendQuad(Buffers, Transform, P000, P010, P110, P100, Color);
		AppendQuad(Buffers, Transform, P001, P101, P111, P011, Color);
		AppendQuad(Buffers, Transform, P100, P110, P111, P101, Color);
		AppendQuad(Buffers, Transform, P000, P001, P011, P010, Color);
		AppendQuad(Buffers, Transform, P010, P011, P111, P110, Color);
		AppendQuad(Buffers, Transform, P000, P100, P101, P001, Color);
	}

	void BuildCylinder(const FPrototypePrimitiveSpec& Spec, int32 PrimitiveIndex, FGeneratedMeshBuffers& Buffers)
	{
		const FTransform Transform(FRotator::MakeFromEuler(Spec.Transform.RotationDeg), Spec.Transform.LocationCm, Spec.Transform.Scale);
		const double Height = Spec.Height;
		const int32 Segments = Spec.Segments;
		const FVector4f Color = GetPrimitiveColor(Spec, PrimitiveIndex);

		for (int32 SegmentIndex = 0; SegmentIndex < Segments; ++SegmentIndex)
		{
			const double Angle0 = (2.0 * PI * SegmentIndex) / Segments;
			const double Angle1 = (2.0 * PI * (SegmentIndex + 1)) / Segments;

			const FVector Bottom0(Spec.Radius * FMath::Cos(Angle0), Spec.Radius * FMath::Sin(Angle0), 0.0);
			const FVector Bottom1(Spec.Radius * FMath::Cos(Angle1), Spec.Radius * FMath::Sin(Angle1), 0.0);
			const FVector Top0(Bottom0.X, Bottom0.Y, Height);
			const FVector Top1(Bottom1.X, Bottom1.Y, Height);

			AppendQuad(Buffers, Transform, Bottom0, Bottom1, Top1, Top0, Color);
			AppendTriangle(Buffers, Transform, Top0, Top1, FVector(0.0, 0.0, Height), FVector2D(0.0, 0.0), FVector2D(1.0, 0.0), FVector2D(0.5, 0.5), Color);
			AppendTriangle(Buffers, Transform, Bottom0, FVector(0.0, 0.0, 0.0), Bottom1, FVector2D(0.0, 0.0), FVector2D(0.5, 0.5), FVector2D(1.0, 0.0), Color);
		}
	}

	void BuildCone(const FPrototypePrimitiveSpec& Spec, int32 PrimitiveIndex, FGeneratedMeshBuffers& Buffers)
	{
		const FTransform Transform(FRotator::MakeFromEuler(Spec.Transform.RotationDeg), Spec.Transform.LocationCm, Spec.Transform.Scale);
		const FVector Apex(0.0, 0.0, Spec.Height);
		const int32 Segments = Spec.Segments;
		const FVector4f Color = GetPrimitiveColor(Spec, PrimitiveIndex);

		for (int32 SegmentIndex = 0; SegmentIndex < Segments; ++SegmentIndex)
		{
			const double Angle0 = (2.0 * PI * SegmentIndex) / Segments;
			const double Angle1 = (2.0 * PI * (SegmentIndex + 1)) / Segments;

			const FVector Bottom0(Spec.Radius * FMath::Cos(Angle0), Spec.Radius * FMath::Sin(Angle0), 0.0);
			const FVector Bottom1(Spec.Radius * FMath::Cos(Angle1), Spec.Radius * FMath::Sin(Angle1), 0.0);

			AppendTriangle(Buffers, Transform, Bottom0, Bottom1, Apex, FVector2D(0.0, 0.0), FVector2D(1.0, 0.0), FVector2D(0.5, 1.0), Color);
			AppendTriangle(Buffers, Transform, Bottom0, FVector(0.0, 0.0, 0.0), Bottom1, FVector2D(0.0, 0.0), FVector2D(0.5, 0.5), FVector2D(1.0, 0.0), Color);
		}
	}

	void BuildRamp(const FPrototypePrimitiveSpec& Spec, int32 PrimitiveIndex, FGeneratedMeshBuffers& Buffers)
	{
		const double HalfDepth = Spec.Depth * 0.5;
		const double HalfWidth = Spec.Width * 0.5;
		const double Height = Spec.Height;
		const FTransform Transform(FRotator::MakeFromEuler(Spec.Transform.RotationDeg), Spec.Transform.LocationCm, Spec.Transform.Scale);
		const FVector4f Color = GetPrimitiveColor(Spec, PrimitiveIndex);

		const FVector A(-HalfDepth, -HalfWidth, 0.0);
		const FVector B(HalfDepth, -HalfWidth, 0.0);
		const FVector C(HalfDepth, HalfWidth, 0.0);
		const FVector D(-HalfDepth, HalfWidth, 0.0);
		const FVector E(HalfDepth, -HalfWidth, Height);
		const FVector F(HalfDepth, HalfWidth, Height);

		AppendQuad(Buffers, Transform, A, D, C, B, Color);
		AppendQuad(Buffers, Transform, B, C, F, E, Color);
		AppendQuad(Buffers, Transform, A, E, F, D, Color);
		AppendTriangle(Buffers, Transform, A, B, E, FVector2D(0.0, 0.0), FVector2D(1.0, 0.0), FVector2D(1.0, 1.0), Color);
		AppendTriangle(Buffers, Transform, D, F, C, FVector2D(0.0, 0.0), FVector2D(1.0, 1.0), FVector2D(1.0, 0.0), Color);
	}

	void BuildStair(const FPrototypePrimitiveSpec& Spec, int32 PrimitiveIndex, FGeneratedMeshBuffers& Buffers)
	{
		const double StepDepth = Spec.Depth / Spec.Steps;
		const double StepHeight = Spec.Height / Spec.Steps;

		for (int32 StepIndex = 0; StepIndex < Spec.Steps; ++StepIndex)
		{
			FPrototypePrimitiveSpec StepSpec = Spec;
			StepSpec.Type = TEXT("box");
			StepSpec.Depth = StepDepth;
			StepSpec.Height = StepHeight * (StepIndex + 1);
			StepSpec.Transform.LocationCm = Spec.Transform.LocationCm;
			StepSpec.Transform.LocationCm.X += (-Spec.Depth * 0.5) + (StepDepth * StepIndex) + (StepDepth * 0.5);
			BuildBox(StepSpec, PrimitiveIndex, Buffers);
		}
	}

	bool ValidatePositive(const TCHAR* Name, double Value, FString& OutError, const FString& Context)
	{
		if (!FMath::IsFinite(Value) || Value <= 0.0)
		{
			OutError = FString::Printf(TEXT("%s field '%s' must be a finite positive value."), *Context, Name);
			return false;
		}

		return true;
	}
}

namespace PrototypeMeshBuilder
{
	bool ParseShapeDslJson(const FString& RawJson, FPrototypeShapeDsl& OutDsl, FString& OutError)
	{
		TSharedPtr<FJsonObject> RootObject;
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(RawJson);
		if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
		{
			OutError = TEXT("Bridge output was not valid JSON.");
			return false;
		}

		static const TSet<FString> TopLevelKeys{TEXT("version"), TEXT("mesh_name"), TEXT("units"), TEXT("pivot"), TEXT("primitives"), TEXT("notes")};
		if (!HasOnlyKeys(RootObject, TopLevelKeys, OutError, TEXT("Shape DSL")))
		{
			return false;
		}

		if (!ReadStringField(RootObject, TEXT("version"), OutDsl.Version, OutError, TEXT("Shape DSL"))
			|| !ReadStringField(RootObject, TEXT("mesh_name"), OutDsl.MeshName, OutError, TEXT("Shape DSL"))
			|| !ReadStringField(RootObject, TEXT("units"), OutDsl.Units, OutError, TEXT("Shape DSL"))
			|| !ReadStringField(RootObject, TEXT("pivot"), OutDsl.Pivot, OutError, TEXT("Shape DSL"))
			|| !ReadStringField(RootObject, TEXT("notes"), OutDsl.Notes, OutError, TEXT("Shape DSL")))
		{
			return false;
		}

		const TArray<TSharedPtr<FJsonValue>>* PrimitiveValues = nullptr;
		if (!RootObject->TryGetArrayField(TEXT("primitives"), PrimitiveValues) || !PrimitiveValues)
		{
			OutError = TEXT("Shape DSL is missing the 'primitives' array.");
			return false;
		}

		OutDsl.Primitives.Reset();
		for (int32 PrimitiveIndex = 0; PrimitiveIndex < PrimitiveValues->Num(); ++PrimitiveIndex)
		{
			const TSharedPtr<FJsonObject>* PrimitiveObject = nullptr;
			if (!(*PrimitiveValues)[PrimitiveIndex]->TryGetObject(PrimitiveObject) || !PrimitiveObject || !PrimitiveObject->IsValid())
			{
				OutError = FString::Printf(TEXT("Primitive %d is not an object."), PrimitiveIndex);
				return false;
			}

			static const TSet<FString> PrimitiveKeys{TEXT("name"), TEXT("type"), TEXT("transform"), TEXT("width"), TEXT("depth"), TEXT("height"), TEXT("radius"), TEXT("segments"), TEXT("steps")};
			const FString PrimitiveContext = FString::Printf(TEXT("Primitive %d"), PrimitiveIndex);
			if (!HasOnlyKeys(*PrimitiveObject, PrimitiveKeys, OutError, PrimitiveContext))
			{
				return false;
			}

			FPrototypePrimitiveSpec Primitive;
			if (!ReadStringField(*PrimitiveObject, TEXT("name"), Primitive.Name, OutError, PrimitiveContext)
				|| !ReadStringField(*PrimitiveObject, TEXT("type"), Primitive.Type, OutError, PrimitiveContext))
			{
				return false;
			}

			const TSharedPtr<FJsonObject>* TransformObject = nullptr;
			if (!(*PrimitiveObject)->TryGetObjectField(TEXT("transform"), TransformObject) || !TransformObject || !TransformObject->IsValid())
			{
				OutError = FString::Printf(TEXT("%s is missing object field 'transform'."), *PrimitiveContext);
				return false;
			}

			static const TSet<FString> TransformKeys{TEXT("location_cm"), TEXT("rotation_deg"), TEXT("scale")};
			if (!HasOnlyKeys(*TransformObject, TransformKeys, OutError, PrimitiveContext + TEXT(".transform")))
			{
				return false;
			}

			if (!ReadVectorField(*TransformObject, TEXT("location_cm"), Primitive.Transform.LocationCm, OutError, PrimitiveContext + TEXT(".transform"))
				|| !ReadVectorField(*TransformObject, TEXT("rotation_deg"), Primitive.Transform.RotationDeg, OutError, PrimitiveContext + TEXT(".transform"))
				|| !ReadVectorField(*TransformObject, TEXT("scale"), Primitive.Transform.Scale, OutError, PrimitiveContext + TEXT(".transform")))
			{
				return false;
			}

			if ((*PrimitiveObject)->HasField(TEXT("width")) && !ReadNumberField(*PrimitiveObject, TEXT("width"), Primitive.Width, OutError, PrimitiveContext))
			{
				return false;
			}

			if ((*PrimitiveObject)->HasField(TEXT("depth")) && !ReadNumberField(*PrimitiveObject, TEXT("depth"), Primitive.Depth, OutError, PrimitiveContext))
			{
				return false;
			}

			if ((*PrimitiveObject)->HasField(TEXT("height")) && !ReadNumberField(*PrimitiveObject, TEXT("height"), Primitive.Height, OutError, PrimitiveContext))
			{
				return false;
			}

			if ((*PrimitiveObject)->HasField(TEXT("radius")) && !ReadNumberField(*PrimitiveObject, TEXT("radius"), Primitive.Radius, OutError, PrimitiveContext))
			{
				return false;
			}

			if ((*PrimitiveObject)->HasField(TEXT("segments")) && !ReadIntegerField(*PrimitiveObject, TEXT("segments"), Primitive.Segments, OutError, PrimitiveContext))
			{
				return false;
			}

			if ((*PrimitiveObject)->HasField(TEXT("steps")) && !ReadIntegerField(*PrimitiveObject, TEXT("steps"), Primitive.Steps, OutError, PrimitiveContext))
			{
				return false;
			}

			OutDsl.Primitives.Add(MoveTemp(Primitive));
		}

		OutDsl.RawJson = RawJson;
		return true;
	}

	bool ValidateShapeDsl(const FPrototypeShapeDsl& Dsl, FString& OutError)
	{
		if (Dsl.Primitives.IsEmpty())
		{
			OutError = TEXT("Shape DSL must contain at least one primitive.");
			return false;
		}

		if (Dsl.Primitives.Num() > 32)
		{
			OutError = TEXT("Shape DSL exceeds the primitive limit of 32.");
			return false;
		}

		if (!Dsl.Units.Equals(TEXT("cm")))
		{
			OutError = TEXT("Only centimeter units are supported.");
			return false;
		}

		if (!Dsl.Pivot.Equals(TEXT("base_center")) && !Dsl.Pivot.Equals(TEXT("center")))
		{
			OutError = TEXT("Pivot must be either 'base_center' or 'center'.");
			return false;
		}

		for (int32 PrimitiveIndex = 0; PrimitiveIndex < Dsl.Primitives.Num(); ++PrimitiveIndex)
		{
			const FPrototypePrimitiveSpec& Primitive = Dsl.Primitives[PrimitiveIndex];
			const FString Context = FString::Printf(TEXT("Primitive %d (%s)"), PrimitiveIndex, *Primitive.Type);

			if (!IsFiniteVector(Primitive.Transform.LocationCm)
				|| !IsFiniteVector(Primitive.Transform.RotationDeg)
				|| !IsFiniteVector(Primitive.Transform.Scale))
			{
				OutError = Context + TEXT(" contains non-finite transform values.");
				return false;
			}

			if (Primitive.Transform.Scale.X <= 0.0 || Primitive.Transform.Scale.Y <= 0.0 || Primitive.Transform.Scale.Z <= 0.0)
			{
				OutError = Context + TEXT(" scale must be positive on all axes.");
				return false;
			}

			if (Primitive.Type.Equals(TEXT("box")) || Primitive.Type.Equals(TEXT("ramp")) || Primitive.Type.Equals(TEXT("stair")))
			{
				if (!ValidatePositive(TEXT("width"), Primitive.Width, OutError, Context)
					|| !ValidatePositive(TEXT("depth"), Primitive.Depth, OutError, Context)
					|| !ValidatePositive(TEXT("height"), Primitive.Height, OutError, Context))
				{
					return false;
				}
			}
			else if (Primitive.Type.Equals(TEXT("plane")))
			{
				if (!ValidatePositive(TEXT("width"), Primitive.Width, OutError, Context)
					|| !ValidatePositive(TEXT("depth"), Primitive.Depth, OutError, Context))
				{
					return false;
				}
			}
			else if (Primitive.Type.Equals(TEXT("cylinder")) || Primitive.Type.Equals(TEXT("cone")))
			{
				if (!ValidatePositive(TEXT("radius"), Primitive.Radius, OutError, Context)
					|| !ValidatePositive(TEXT("height"), Primitive.Height, OutError, Context))
				{
					return false;
				}

				if (Primitive.Segments < 3 || Primitive.Segments > MaxSegments)
				{
					OutError = Context + TEXT(" segments must be between 3 and 128.");
					return false;
				}
			}
			else
			{
				OutError = Context + TEXT(" uses an unsupported primitive type.");
				return false;
			}

			if (Primitive.Type.Equals(TEXT("stair")))
			{
				if (Primitive.Steps < 1 || Primitive.Steps > MaxSteps)
				{
					OutError = Context + TEXT(" steps must be between 1 and 32.");
					return false;
				}
			}
		}

		return true;
	}

	bool BuildMeshBuffers(const FPrototypeShapeDsl& Dsl, FGeneratedMeshBuffers& OutBuffers, FString& OutError)
	{
		OutBuffers.Reset();

		if (!ValidateShapeDsl(Dsl, OutError))
		{
			return false;
		}

		for (int32 PrimitiveIndex = 0; PrimitiveIndex < Dsl.Primitives.Num(); ++PrimitiveIndex)
		{
			const FPrototypePrimitiveSpec& Primitive = Dsl.Primitives[PrimitiveIndex];
			if (Primitive.Type.Equals(TEXT("plane")))
			{
				BuildPlane(Primitive, PrimitiveIndex, OutBuffers);
			}
			else if (Primitive.Type.Equals(TEXT("box")))
			{
				BuildBox(Primitive, PrimitiveIndex, OutBuffers);
			}
			else if (Primitive.Type.Equals(TEXT("cylinder")))
			{
				BuildCylinder(Primitive, PrimitiveIndex, OutBuffers);
			}
			else if (Primitive.Type.Equals(TEXT("cone")))
			{
				BuildCone(Primitive, PrimitiveIndex, OutBuffers);
			}
			else if (Primitive.Type.Equals(TEXT("ramp")))
			{
				BuildRamp(Primitive, PrimitiveIndex, OutBuffers);
			}
			else if (Primitive.Type.Equals(TEXT("stair")))
			{
				BuildStair(Primitive, PrimitiveIndex, OutBuffers);
			}
		}

		if (!OutBuffers.IsValid())
		{
			OutError = TEXT("Generated buffers were empty or inconsistent.");
			return false;
		}

		if (OutBuffers.GetTriangleCount() > MaxTriangleCount)
		{
			OutError = FString::Printf(TEXT("Triangle budget exceeded: %d > %d."), OutBuffers.GetTriangleCount(), MaxTriangleCount);
			return false;
		}

		FBox3f Bounds(EForceInit::ForceInitToZero);
		for (const FVector3f& Position : OutBuffers.Positions)
		{
			Bounds += Position;
		}

		FVector3f PivotOffset = FVector3f::ZeroVector;
		if (Dsl.Pivot.Equals(TEXT("center")))
		{
			PivotOffset = -Bounds.GetCenter();
		}
		else
		{
			PivotOffset = FVector3f(-(Bounds.Min.X + Bounds.Max.X) * 0.5f, -(Bounds.Min.Y + Bounds.Max.Y) * 0.5f, -Bounds.Min.Z);
		}

		for (FVector3f& Position : OutBuffers.Positions)
		{
			Position += PivotOffset;
		}

		return true;
	}

	bool BuildDynamicMesh(const FGeneratedMeshBuffers& Buffers, FDynamicMesh3& OutMesh, FString& OutError)
	{
		if (!Buffers.IsValid())
		{
			OutError = TEXT("Cannot build a dynamic mesh from invalid buffers.");
			return false;
		}

		OutMesh.Clear();
		OutMesh.EnableAttributes();
		OutMesh.Attributes()->EnablePrimaryColors();

		FDynamicMeshUVOverlay* UvOverlay = OutMesh.Attributes()->PrimaryUV();
		FDynamicMeshNormalOverlay* NormalOverlay = OutMesh.Attributes()->PrimaryNormals();
		FDynamicMeshColorOverlay* ColorOverlay = OutMesh.Attributes()->PrimaryColors();
		if (!UvOverlay || !NormalOverlay || !ColorOverlay)
		{
			OutError = TEXT("Failed to initialize dynamic mesh attributes.");
			return false;
		}

		TArray<int32> VertexIds;
		VertexIds.Reserve(Buffers.Positions.Num());
		for (const FVector3f& Position : Buffers.Positions)
		{
			VertexIds.Add(OutMesh.AppendVertex(FVector3d(Position)));
		}

		for (int32 TriangleIndex = 0; TriangleIndex < Buffers.GetTriangleCount(); ++TriangleIndex)
		{
			const int32 IndexBase = TriangleIndex * 3;
			const int32 VertexIndex0 = Buffers.Indices[IndexBase];
			const int32 VertexIndex1 = Buffers.Indices[IndexBase + 1];
			const int32 VertexIndex2 = Buffers.Indices[IndexBase + 2];

			const int32 TriangleId = OutMesh.AppendTriangle(VertexIds[VertexIndex0], VertexIds[VertexIndex1], VertexIds[VertexIndex2], 0);
			if (TriangleId < 0)
			{
				OutError = FString::Printf(TEXT("Failed to append triangle %d to preview mesh."), TriangleIndex);
				return false;
			}

			const FIndex3i UvElements(
				UvOverlay->AppendElement(Buffers.UV0[VertexIndex0]),
				UvOverlay->AppendElement(Buffers.UV0[VertexIndex1]),
				UvOverlay->AppendElement(Buffers.UV0[VertexIndex2]));

			const FIndex3i NormalElements(
				NormalOverlay->AppendElement(Buffers.Normals[VertexIndex0]),
				NormalOverlay->AppendElement(Buffers.Normals[VertexIndex1]),
				NormalOverlay->AppendElement(Buffers.Normals[VertexIndex2]));

			const FIndex3i ColorElements(
				ColorOverlay->AppendElement(Buffers.Colors[VertexIndex0]),
				ColorOverlay->AppendElement(Buffers.Colors[VertexIndex1]),
				ColorOverlay->AppendElement(Buffers.Colors[VertexIndex2]));

			UvOverlay->SetTriangle(TriangleId, UvElements);
			NormalOverlay->SetTriangle(TriangleId, NormalElements);
			ColorOverlay->SetTriangle(TriangleId, ColorElements);
		}

		return true;
	}

	bool BuildStaticMeshAsset(const FString& PackagePath, const FString& AssetName, const FDynamicMesh3& DynamicMesh, UStaticMesh*& OutStaticMesh, FString& OutError)
	{
		OutStaticMesh = nullptr;

		UPackage* Package = CreatePackage(*PackagePath);
		if (!Package)
		{
			OutError = FString::Printf(TEXT("Failed to create package '%s'."), *PackagePath);
			return false;
		}

		FMeshDescription MeshDescription;
		FStaticMeshAttributes Attributes(MeshDescription);
		Attributes.Register();

		FDynamicMeshToMeshDescription Converter;
		Converter.Convert(&DynamicMesh, MeshDescription, false);

		UStaticMesh* StaticMesh = NewObject<UStaticMesh>(Package, *AssetName, RF_Public | RF_Standalone | RF_Transactional);
		if (!StaticMesh)
		{
			OutError = TEXT("Failed to allocate a UStaticMesh asset.");
			return false;
		}

		FStaticMeshSourceModel& SourceModel = StaticMesh->AddSourceModel();
		SourceModel.BuildSettings.bRecomputeNormals = false;
		SourceModel.BuildSettings.bRecomputeTangents = true;
		SourceModel.BuildSettings.bGenerateLightmapUVs = false;
		SourceModel.BuildSettings.bUseFullPrecisionUVs = true;

		StaticMesh->CreateMeshDescription(0, MoveTemp(MeshDescription));
		StaticMesh->CommitMeshDescription(0);
		StaticMesh->SetStaticMaterials({FStaticMaterial(UMaterial::GetDefaultMaterial(MD_Surface))});
		StaticMesh->SetImportVersion(EImportStaticMeshVersion::LastVersion);
		StaticMesh->Build();
		StaticMesh->PostEditChange();
		Package->MarkPackageDirty();
		FAssetRegistryModule::AssetCreated(StaticMesh);

		OutStaticMesh = StaticMesh;
		return true;
	}

	bool WriteMeshBuffersObjFile(const FString& FilePath, const FGeneratedMeshBuffers& Buffers, FString& OutError)
	{
		if (!Buffers.IsValid())
		{
			OutError = TEXT("Cannot write OBJ for invalid mesh buffers.");
			return false;
		}

		const FString DirectoryPath = FPaths::GetPath(FilePath);
		if (!IFileManager::Get().DirectoryExists(*DirectoryPath) && !IFileManager::Get().MakeDirectory(*DirectoryPath, true))
		{
			OutError = FString::Printf(TEXT("Failed to create OBJ output directory: %s"), *DirectoryPath);
			return false;
		}

		FString ObjText;
		ObjText.Reserve(Buffers.Positions.Num() * 96);
		ObjText += TEXT("# PrototypeMeshBuilder debug OBJ\n");
		ObjText += TEXT("# Units: centimeters\n");
		ObjText += TEXT("# Vertex format: v x y z r g b\n");

		for (int32 VertexIndex = 0; VertexIndex < Buffers.Positions.Num(); ++VertexIndex)
		{
			const FVector3f& Position = Buffers.Positions[VertexIndex];
			const FVector4f& Color = Buffers.Colors[VertexIndex];
			ObjText += FString::Printf(
				TEXT("v %.6f %.6f %.6f %.6f %.6f %.6f\n"),
				Position.X,
				Position.Y,
				Position.Z,
				Color.X,
				Color.Y,
				Color.Z);
		}

		for (const FVector2f& Uv : Buffers.UV0)
		{
			ObjText += FString::Printf(TEXT("vt %.6f %.6f\n"), Uv.X, Uv.Y);
		}

		for (const FVector3f& Normal : Buffers.Normals)
		{
			ObjText += FString::Printf(TEXT("vn %.6f %.6f %.6f\n"), Normal.X, Normal.Y, Normal.Z);
		}

		for (int32 TriangleIndex = 0; TriangleIndex < Buffers.GetTriangleCount(); ++TriangleIndex)
		{
			const int32 IndexBase = TriangleIndex * 3;
			const int32 A = Buffers.Indices[IndexBase] + 1;
			const int32 B = Buffers.Indices[IndexBase + 1] + 1;
			const int32 C = Buffers.Indices[IndexBase + 2] + 1;
			ObjText += FString::Printf(TEXT("f %d/%d/%d %d/%d/%d %d/%d/%d\n"), A, A, A, B, B, B, C, C, C);
		}

		if (!FFileHelper::SaveStringToFile(ObjText, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
		{
			OutError = FString::Printf(TEXT("Failed to write OBJ file: %s"), *FilePath);
			return false;
		}

		return true;
	}

	bool WriteTextFileUtf8(const FString& FilePath, const FString& Contents, FString& OutError)
	{
		const FString DirectoryPath = FPaths::GetPath(FilePath);
		if (!IFileManager::Get().DirectoryExists(*DirectoryPath) && !IFileManager::Get().MakeDirectory(*DirectoryPath, true))
		{
			OutError = FString::Printf(TEXT("Failed to create text output directory: %s"), *DirectoryPath);
			return false;
		}

		if (!FFileHelper::SaveStringToFile(Contents, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
		{
			OutError = FString::Printf(TEXT("Failed to write text file: %s"), *FilePath);
			return false;
		}

		return true;
	}

	bool CreateVertexColorMaterialAsset(const FString& PackagePath, const FString& AssetName, UMaterialInstanceConstant*& OutMaterial, FString& OutError)
	{
		OutMaterial = nullptr;

		if (!GEngine || !GEngine->VertexColorViewModeMaterial_ColorOnly)
		{
			OutError = TEXT("Engine vertex color material is not available.");
			return false;
		}

		UPackage* Package = CreatePackage(*PackagePath);
		if (!Package)
		{
			OutError = FString::Printf(TEXT("Failed to create material package '%s'."), *PackagePath);
			return false;
		}

		UMaterialInstanceConstant* MaterialInstance = NewObject<UMaterialInstanceConstant>(Package, *AssetName, RF_Public | RF_Standalone | RF_Transactional);
		if (!MaterialInstance)
		{
			OutError = TEXT("Failed to allocate a vertex-color material instance.");
			return false;
		}

		MaterialInstance->SetParentEditorOnly(GEngine->VertexColorViewModeMaterial_ColorOnly);
		MaterialInstance->PostEditChange();
		Package->MarkPackageDirty();
		FAssetRegistryModule::AssetCreated(MaterialInstance);
		OutMaterial = MaterialInstance;
		return true;
	}

	bool ApplyStaticMeshMaterial(UStaticMesh* StaticMesh, UMaterialInterface* Material, FString& OutError)
	{
		if (!StaticMesh)
		{
			OutError = TEXT("Static mesh is required when assigning a material.");
			return false;
		}

		if (!Material)
		{
			OutError = TEXT("Material is required when assigning a material.");
			return false;
		}

		StaticMesh->Modify();
		StaticMesh->SetStaticMaterials({FStaticMaterial(Material)});
		StaticMesh->Build();
		StaticMesh->PostEditChange();
		StaticMesh->MarkPackageDirty();
		return true;
	}

	bool WriteAssetMetadata(UObject* Asset, const FString& MetadataJson, FString& OutError)
	{
		if (!Asset)
		{
			OutError = TEXT("Asset is required for metadata writing.");
			return false;
		}

		UPackage* Package = Asset->GetOutermost();
		if (!Package)
		{
			OutError = TEXT("Asset package is not available.");
			return false;
		}

		FMetaData& MetaData = Package->GetMetaData();
		MetaData.SetValue(Asset, TEXT("PrototypeMeshBuilder"), TEXT("1"));
		MetaData.SetValue(Asset, TEXT("PrototypeMeshBuilder.Metadata"), *MetadataJson);
		Package->MarkPackageDirty();
		return true;
	}

	FString NormalizeContentPath(const FString& InPath)
	{
		FString Result = InPath.TrimStartAndEnd();
		if (Result.IsEmpty())
		{
			Result = TEXT("/Game/Generated/PrototypeMeshes");
		}

		Result.ReplaceInline(TEXT("\\"), TEXT("/"));
		while (Result.EndsWith(TEXT("/")))
		{
			Result.LeftChopInline(1, EAllowShrinking::No);
		}

		return Result;
	}

	FString SanitizeAssetName(const FString& InName)
	{
		FString Result = InName.TrimStartAndEnd();
		if (Result.IsEmpty())
		{
			Result = TEXT("PrototypeMesh");
		}

		Result = ObjectTools::SanitizeObjectName(Result);
		if (Result.IsEmpty())
		{
			Result = TEXT("PrototypeMesh");
		}

		return Result;
	}
}
