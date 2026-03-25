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
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionVertexColor.h"
#include "Materials/MaterialInterface.h"
#include "MeshDescription.h"
#include "ObjectTools.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "StaticMeshAttributes.h"
#include "UObject/MetaData.h"

using namespace UE::Geometry;

namespace
{
	constexpr int32 MaxPrimitiveTriangleCount = 50000;
	constexpr int32 MaxVoxelTriangleCount = 2000000;
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

	bool ReadIntVectorField(const TSharedPtr<FJsonObject>& Object, const TCHAR* FieldName, FIntVector& OutVector, FString& OutError, const FString& Context)
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

		int32 X = 0;
		int32 Y = 0;
		int32 Z = 0;
		if (!ReadIntegerField(*VectorObject, TEXT("x"), X, OutError, FString::Printf(TEXT("%s.%s"), *Context, FieldName))
			|| !ReadIntegerField(*VectorObject, TEXT("y"), Y, OutError, FString::Printf(TEXT("%s.%s"), *Context, FieldName))
			|| !ReadIntegerField(*VectorObject, TEXT("z"), Z, OutError, FString::Printf(TEXT("%s.%s"), *Context, FieldName)))
		{
			return false;
		}

		OutVector = FIntVector(X, Y, Z);
		return true;
	}

	bool ReadColorField(const TSharedPtr<FJsonObject>& Object, const TCHAR* FieldName, FLinearColor& OutColor, FString& OutError, const FString& Context)
	{
		const TSharedPtr<FJsonObject>* ColorObject = nullptr;
		if (!Object->TryGetObjectField(FieldName, ColorObject) || !ColorObject || !ColorObject->IsValid())
		{
			OutError = FString::Printf(TEXT("%s is missing object field '%s'."), *Context, FieldName);
			return false;
		}

		static const TSet<FString> AllowedKeys{TEXT("r"), TEXT("g"), TEXT("b"), TEXT("a")};
		if (!HasOnlyKeys(*ColorObject, AllowedKeys, OutError, FString::Printf(TEXT("%s.%s"), *Context, FieldName)))
		{
			return false;
		}

		double R = 0.0;
		double G = 0.0;
		double B = 0.0;
		double A = 0.0;
		if (!ReadNumberField(*ColorObject, TEXT("r"), R, OutError, FString::Printf(TEXT("%s.%s"), *Context, FieldName))
			|| !ReadNumberField(*ColorObject, TEXT("g"), G, OutError, FString::Printf(TEXT("%s.%s"), *Context, FieldName))
			|| !ReadNumberField(*ColorObject, TEXT("b"), B, OutError, FString::Printf(TEXT("%s.%s"), *Context, FieldName))
			|| !ReadNumberField(*ColorObject, TEXT("a"), A, OutError, FString::Printf(TEXT("%s.%s"), *Context, FieldName)))
		{
			return false;
		}

		OutColor = FLinearColor(static_cast<float>(R), static_cast<float>(G), static_cast<float>(B), static_cast<float>(A));
		return true;
	}

	bool IsFiniteVector(const FVector& Vector)
	{
		return FMath::IsFinite(Vector.X) && FMath::IsFinite(Vector.Y) && FMath::IsFinite(Vector.Z);
	}

	bool IsFiniteColor(const FLinearColor& Color)
	{
		return FMath::IsFinite(Color.R) && FMath::IsFinite(Color.G) && FMath::IsFinite(Color.B) && FMath::IsFinite(Color.A);
	}

	FVector4f GetPrimitiveColor(const FPrototypePrimitiveSpec& Spec)
	{
		return FVector4f(Spec.Color.R, Spec.Color.G, Spec.Color.B, Spec.Color.A);
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

	FTransform MakePrimitiveTransform(const FPrototypePrimitiveTransform& PrimitiveTransform)
	{
		// Primitive transforms are authored around the primitive bounds center.
		return FTransform(FRotator::MakeFromEuler(PrimitiveTransform.RotationDeg), PrimitiveTransform.LocationCm, PrimitiveTransform.Scale);
	}

	void BuildCenteredBoxGeometry(FGeneratedMeshBuffers& Buffers, const FTransform& Transform, double Width, double Depth, double Height, const FVector4f& Color)
	{
		const double HalfDepth = Depth * 0.5;
		const double HalfWidth = Width * 0.5;
		const double HalfHeight = Height * 0.5;

		const FVector P000(-HalfDepth, -HalfWidth, -HalfHeight);
		const FVector P100(HalfDepth, -HalfWidth, -HalfHeight);
		const FVector P110(HalfDepth, HalfWidth, -HalfHeight);
		const FVector P010(-HalfDepth, HalfWidth, -HalfHeight);
		const FVector P001(-HalfDepth, -HalfWidth, HalfHeight);
		const FVector P101(HalfDepth, -HalfWidth, HalfHeight);
		const FVector P111(HalfDepth, HalfWidth, HalfHeight);
		const FVector P011(-HalfDepth, HalfWidth, HalfHeight);

		AppendQuad(Buffers, Transform, P000, P010, P110, P100, Color);
		AppendQuad(Buffers, Transform, P001, P101, P111, P011, Color);
		AppendQuad(Buffers, Transform, P100, P110, P111, P101, Color);
		AppendQuad(Buffers, Transform, P000, P001, P011, P010, Color);
		AppendQuad(Buffers, Transform, P010, P011, P111, P110, Color);
		AppendQuad(Buffers, Transform, P000, P100, P101, P001, Color);
	}

	void BuildPlane(const FPrototypePrimitiveSpec& Spec, FGeneratedMeshBuffers& Buffers)
	{
		const double HalfDepth = Spec.Depth * 0.5;
		const double HalfWidth = Spec.Width * 0.5;
		const FTransform Transform = MakePrimitiveTransform(Spec.Transform);
		const FVector4f Color = GetPrimitiveColor(Spec);

		const FVector A(-HalfDepth, -HalfWidth, 0.0);
		const FVector B(HalfDepth, -HalfWidth, 0.0);
		const FVector C(HalfDepth, HalfWidth, 0.0);
		const FVector D(-HalfDepth, HalfWidth, 0.0);
		AppendQuad(Buffers, Transform, A, B, C, D, Color);
	}

	void BuildBox(const FPrototypePrimitiveSpec& Spec, FGeneratedMeshBuffers& Buffers)
	{
		const FTransform Transform = MakePrimitiveTransform(Spec.Transform);
		const FVector4f Color = GetPrimitiveColor(Spec);
		BuildCenteredBoxGeometry(Buffers, Transform, Spec.Width, Spec.Depth, Spec.Height, Color);
	}

	void BuildCylinder(const FPrototypePrimitiveSpec& Spec, FGeneratedMeshBuffers& Buffers)
	{
		const FTransform Transform = MakePrimitiveTransform(Spec.Transform);
		const double Height = Spec.Height;
		const double HalfHeight = Height * 0.5;
		const int32 Segments = Spec.Segments;
		const FVector4f Color = GetPrimitiveColor(Spec);

		for (int32 SegmentIndex = 0; SegmentIndex < Segments; ++SegmentIndex)
		{
			const double Angle0 = (2.0 * PI * SegmentIndex) / Segments;
			const double Angle1 = (2.0 * PI * (SegmentIndex + 1)) / Segments;

			const FVector Bottom0(Spec.Radius * FMath::Cos(Angle0), Spec.Radius * FMath::Sin(Angle0), -HalfHeight);
			const FVector Bottom1(Spec.Radius * FMath::Cos(Angle1), Spec.Radius * FMath::Sin(Angle1), -HalfHeight);
			const FVector Top0(Bottom0.X, Bottom0.Y, HalfHeight);
			const FVector Top1(Bottom1.X, Bottom1.Y, HalfHeight);

			AppendQuad(Buffers, Transform, Bottom0, Bottom1, Top1, Top0, Color);
			AppendTriangle(Buffers, Transform, Top0, Top1, FVector(0.0, 0.0, HalfHeight), FVector2D(0.0, 0.0), FVector2D(1.0, 0.0), FVector2D(0.5, 0.5), Color);
			AppendTriangle(Buffers, Transform, Bottom0, FVector(0.0, 0.0, -HalfHeight), Bottom1, FVector2D(0.0, 0.0), FVector2D(0.5, 0.5), FVector2D(1.0, 0.0), Color);
		}
	}

	void BuildCone(const FPrototypePrimitiveSpec& Spec, FGeneratedMeshBuffers& Buffers)
	{
		const FTransform Transform = MakePrimitiveTransform(Spec.Transform);
		const double HalfHeight = Spec.Height * 0.5;
		const FVector Apex(0.0, 0.0, HalfHeight);
		const int32 Segments = Spec.Segments;
		const FVector4f Color = GetPrimitiveColor(Spec);

		for (int32 SegmentIndex = 0; SegmentIndex < Segments; ++SegmentIndex)
		{
			const double Angle0 = (2.0 * PI * SegmentIndex) / Segments;
			const double Angle1 = (2.0 * PI * (SegmentIndex + 1)) / Segments;

			const FVector Bottom0(Spec.Radius * FMath::Cos(Angle0), Spec.Radius * FMath::Sin(Angle0), -HalfHeight);
			const FVector Bottom1(Spec.Radius * FMath::Cos(Angle1), Spec.Radius * FMath::Sin(Angle1), -HalfHeight);

			AppendTriangle(Buffers, Transform, Bottom0, Bottom1, Apex, FVector2D(0.0, 0.0), FVector2D(1.0, 0.0), FVector2D(0.5, 1.0), Color);
			AppendTriangle(Buffers, Transform, Bottom0, FVector(0.0, 0.0, -HalfHeight), Bottom1, FVector2D(0.0, 0.0), FVector2D(0.5, 0.5), FVector2D(1.0, 0.0), Color);
		}
	}

	void BuildRamp(const FPrototypePrimitiveSpec& Spec, FGeneratedMeshBuffers& Buffers)
	{
		const double HalfDepth = Spec.Depth * 0.5;
		const double HalfWidth = Spec.Width * 0.5;
		const double HalfHeight = Spec.Height * 0.5;
		const FTransform Transform = MakePrimitiveTransform(Spec.Transform);
		const FVector4f Color = GetPrimitiveColor(Spec);

		const FVector A(-HalfDepth, -HalfWidth, -HalfHeight);
		const FVector B(HalfDepth, -HalfWidth, -HalfHeight);
		const FVector C(HalfDepth, HalfWidth, -HalfHeight);
		const FVector D(-HalfDepth, HalfWidth, -HalfHeight);
		const FVector E(HalfDepth, -HalfWidth, HalfHeight);
		const FVector F(HalfDepth, HalfWidth, HalfHeight);

		AppendQuad(Buffers, Transform, A, D, C, B, Color);
		AppendQuad(Buffers, Transform, B, C, F, E, Color);
		AppendQuad(Buffers, Transform, A, E, F, D, Color);
		AppendTriangle(Buffers, Transform, A, B, E, FVector2D(0.0, 0.0), FVector2D(1.0, 0.0), FVector2D(1.0, 1.0), Color);
		AppendTriangle(Buffers, Transform, D, F, C, FVector2D(0.0, 0.0), FVector2D(1.0, 1.0), FVector2D(1.0, 0.0), Color);
	}

	void BuildStair(const FPrototypePrimitiveSpec& Spec, FGeneratedMeshBuffers& Buffers)
	{
		const double StepDepth = Spec.Depth / Spec.Steps;
		const double StepHeight = Spec.Height / Spec.Steps;
		const double HalfDepth = Spec.Depth * 0.5;
		const double HalfHeight = Spec.Height * 0.5;
		const FTransform ParentTransform = MakePrimitiveTransform(Spec.Transform);
		const FVector4f Color = GetPrimitiveColor(Spec);

		for (int32 StepIndex = 0; StepIndex < Spec.Steps; ++StepIndex)
		{
			const double LocalCenterX = -HalfDepth + (StepDepth * StepIndex) + (StepDepth * 0.5);
			const double LocalCenterZ = -HalfHeight + (StepHeight * (StepIndex + 1) * 0.5);
			FTransform CombinedTransform = ParentTransform;
			CombinedTransform.AddToTranslation(ParentTransform.TransformVector(FVector(LocalCenterX, 0.0, LocalCenterZ)));
			BuildCenteredBoxGeometry(Buffers, CombinedTransform, Spec.Width, StepDepth, StepHeight * (StepIndex + 1), Color);
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

	bool ParsePrimitiveMeshObject(const TSharedPtr<FJsonObject>& Object, FPrototypeShapeDsl& OutDsl, FString& OutError, const FString& Context)
	{
		static const TSet<FString> PrimitiveMeshKeys{TEXT("units"), TEXT("pivot"), TEXT("primitives")};
		if (!HasOnlyKeys(Object, PrimitiveMeshKeys, OutError, Context))
		{
			return false;
		}

		if (!ReadStringField(Object, TEXT("units"), OutDsl.Units, OutError, Context)
			|| !ReadStringField(Object, TEXT("pivot"), OutDsl.Pivot, OutError, Context))
		{
			return false;
		}

		const TArray<TSharedPtr<FJsonValue>>* PrimitiveValues = nullptr;
		if (!Object->TryGetArrayField(TEXT("primitives"), PrimitiveValues) || !PrimitiveValues)
		{
			OutError = Context + TEXT(" is missing the 'primitives' array.");
			return false;
		}

		OutDsl.Primitives.Reset();
		for (int32 PrimitiveIndex = 0; PrimitiveIndex < PrimitiveValues->Num(); ++PrimitiveIndex)
		{
			const TSharedPtr<FJsonObject>* PrimitiveObject = nullptr;
			if (!(*PrimitiveValues)[PrimitiveIndex]->TryGetObject(PrimitiveObject) || !PrimitiveObject || !PrimitiveObject->IsValid())
			{
				OutError = FString::Printf(TEXT("%s primitive %d is not an object."), *Context, PrimitiveIndex);
				return false;
			}

			static const TSet<FString> PrimitiveKeys{TEXT("name"), TEXT("type"), TEXT("transform"), TEXT("color"), TEXT("width"), TEXT("depth"), TEXT("height"), TEXT("radius"), TEXT("segments"), TEXT("steps")};
			const FString PrimitiveContext = FString::Printf(TEXT("%s primitive %d"), *Context, PrimitiveIndex);
			if (!HasOnlyKeys(*PrimitiveObject, PrimitiveKeys, OutError, PrimitiveContext))
			{
				return false;
			}

			FPrototypePrimitiveSpec Primitive;
			if (!ReadStringField(*PrimitiveObject, TEXT("name"), Primitive.Name, OutError, PrimitiveContext)
				|| !ReadStringField(*PrimitiveObject, TEXT("type"), Primitive.Type, OutError, PrimitiveContext)
				|| !ReadColorField(*PrimitiveObject, TEXT("color"), Primitive.Color, OutError, PrimitiveContext))
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

		return true;
	}

	bool ParseVoxelGridObject(const TSharedPtr<FJsonObject>& Object, FPrototypeVoxelGrid& OutGrid, FString& OutError, const FString& Context)
	{
		static const TSet<FString> VoxelGridKeys{TEXT("resolution"), TEXT("color_hex_stream"), TEXT("voxels_hex")};
		if (!HasOnlyKeys(Object, VoxelGridKeys, OutError, Context))
		{
			return false;
		}

		if (!ReadIntVectorField(Object, TEXT("resolution"), OutGrid.Resolution, OutError, Context))
		{
			return false;
		}

		if (!Object->TryGetStringField(TEXT("color_hex_stream"), OutGrid.VoxelsHex)
			&& !Object->TryGetStringField(TEXT("voxels_hex"), OutGrid.VoxelsHex))
		{
			OutError = Context + TEXT(" is missing string field 'color_hex_stream'.");
			return false;
		}

		return true;
	}

	int32 HexNibble(TCHAR Character)
	{
		if (Character >= TEXT('0') && Character <= TEXT('9'))
		{
			return Character - TEXT('0');
		}

		if (Character >= TEXT('a') && Character <= TEXT('f'))
		{
			return 10 + (Character - TEXT('a'));
		}

		if (Character >= TEXT('A') && Character <= TEXT('F'))
		{
			return 10 + (Character - TEXT('A'));
		}

		return INDEX_NONE;
	}

	bool DecodeVoxelHexStream(const FPrototypeVoxelGrid& Grid, TArray<uint32>& OutColors, int32& OutOccupiedCount, FString& OutError)
	{
		OutColors.Reset();
		OutOccupiedCount = 0;

		const int64 VoxelCount = static_cast<int64>(Grid.Resolution.X) * static_cast<int64>(Grid.Resolution.Y) * static_cast<int64>(Grid.Resolution.Z);
		if (VoxelCount <= 0)
		{
			OutError = TEXT("Voxel grid resolution must be positive.");
			return false;
		}

		const int64 ExpectedHexLength = VoxelCount * 6;
		if (Grid.VoxelsHex.Len() != ExpectedHexLength)
		{
			OutError = FString::Printf(TEXT("Voxel grid color_hex_stream length mismatch: expected %lld hex chars, got %d."), ExpectedHexLength, Grid.VoxelsHex.Len());
			return false;
		}

		OutColors.Reserve(static_cast<int32>(VoxelCount));
		for (int32 Offset = 0; Offset < Grid.VoxelsHex.Len(); Offset += 6)
		{
			int32 R0 = HexNibble(Grid.VoxelsHex[Offset]);
			int32 R1 = HexNibble(Grid.VoxelsHex[Offset + 1]);
			int32 G0 = HexNibble(Grid.VoxelsHex[Offset + 2]);
			int32 G1 = HexNibble(Grid.VoxelsHex[Offset + 3]);
			int32 B0 = HexNibble(Grid.VoxelsHex[Offset + 4]);
			int32 B1 = HexNibble(Grid.VoxelsHex[Offset + 5]);
			if (R0 == INDEX_NONE || R1 == INDEX_NONE || G0 == INDEX_NONE || G1 == INDEX_NONE || B0 == INDEX_NONE || B1 == INDEX_NONE)
			{
				OutError = FString::Printf(TEXT("Voxel grid color_hex_stream contains invalid hex at character offset %d."), Offset);
				return false;
			}

			const uint32 Color = static_cast<uint32>((R0 << 20) | (R1 << 16) | (G0 << 12) | (G1 << 8) | (B0 << 4) | B1);
			OutColors.Add(Color);
			if (Color != 0)
			{
				++OutOccupiedCount;
			}
		}

		return true;
	}

	FVector4f ColorToVector4f(uint32 PackedColor)
	{
		const float R = static_cast<float>((PackedColor >> 16) & 0xFF) / 255.0f;
		const float G = static_cast<float>((PackedColor >> 8) & 0xFF) / 255.0f;
		const float B = static_cast<float>(PackedColor & 0xFF) / 255.0f;
		return FVector4f(R, G, B, 1.0f);
	}

	FVector GetVoxelGridMinCorner(const FIntVector& Resolution, float VoxelSizeCm)
	{
		return FVector(
			-static_cast<double>(Resolution.X) * VoxelSizeCm * 0.5,
			-static_cast<double>(Resolution.Y) * VoxelSizeCm * 0.5,
			0.0);
	}

	void AppendVoxelQuad(FGeneratedMeshBuffers& Buffers, const FIntVector& Resolution, float VoxelSizeCm, int32 Axis, int32 NormalSign, int32 Plane, int32 StartU, int32 StartV, int32 SizeU, int32 SizeV, uint32 PackedColor)
	{
		const int32 UAxis = (Axis + 1) % 3;
		const int32 VAxis = (Axis + 2) % 3;
		const FVector MinCorner = GetVoxelGridMinCorner(Resolution, VoxelSizeCm);

		FVector Origin = MinCorner;
		Origin[Axis] += static_cast<double>(Plane) * VoxelSizeCm;
		Origin[UAxis] += static_cast<double>(StartU) * VoxelSizeCm;
		Origin[VAxis] += static_cast<double>(StartV) * VoxelSizeCm;

		FVector DeltaU = FVector::ZeroVector;
		DeltaU[UAxis] = static_cast<double>(SizeU) * VoxelSizeCm;
		FVector DeltaV = FVector::ZeroVector;
		DeltaV[VAxis] = static_cast<double>(SizeV) * VoxelSizeCm;

		const FVector4f Color = ColorToVector4f(PackedColor);
		if (NormalSign > 0)
		{
			AppendQuad(Buffers, FTransform::Identity, Origin, Origin + DeltaU, Origin + DeltaU + DeltaV, Origin + DeltaV, Color);
		}
		else
		{
			AppendQuad(Buffers, FTransform::Identity, Origin, Origin + DeltaV, Origin + DeltaV + DeltaU, Origin + DeltaU, Color);
		}
	}

	struct FVoxelMaskCell
	{
		uint32 Color = 0;
		int8 NormalSign = 0;

		bool IsVisible() const
		{
			return Color != 0 && NormalSign != 0;
		}

		bool Matches(const FVoxelMaskCell& Other) const
		{
			return Color == Other.Color && NormalSign == Other.NormalSign;
		}
	};

	int32 GetVoxelLinearIndex(const FIntVector& Resolution, int32 X, int32 Y, int32 Z)
	{
		return X + (Y * Resolution.X) + (Z * Resolution.X * Resolution.Y);
	}

	uint32 GetVoxelColorAt(const TArray<uint32>& Colors, const FIntVector& Resolution, int32 X, int32 Y, int32 Z)
	{
		if (X < 0 || Y < 0 || Z < 0 || X >= Resolution.X || Y >= Resolution.Y || Z >= Resolution.Z)
		{
			return 0;
		}

		return Colors[GetVoxelLinearIndex(Resolution, X, Y, Z)];
	}

	void CenterBuffersBasePivot(FGeneratedMeshBuffers& Buffers)
	{
		if (Buffers.Positions.IsEmpty())
		{
			return;
		}

		FBox3f Bounds(EForceInit::ForceInitToZero);
		for (const FVector3f& Position : Buffers.Positions)
		{
			Bounds += Position;
		}

		const FVector3f PivotOffset(
			-(Bounds.Min.X + Bounds.Max.X) * 0.5f,
			-(Bounds.Min.Y + Bounds.Max.Y) * 0.5f,
			-Bounds.Min.Z);

		for (FVector3f& Position : Buffers.Positions)
		{
			Position += PivotOffset;
		}
	}
}

namespace PrototypeMeshBuilder
{
	bool ParseMeshPayloadJson(const FString& RawJson, FPrototypeMeshPayload& OutPayload, FString& OutError)
	{
		TSharedPtr<FJsonObject> RootObject;
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(RawJson);
		if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
		{
			OutError = TEXT("Bridge output was not valid JSON.");
			return false;
		}

		static const TSet<FString> TopLevelKeys{TEXT("version"), TEXT("mesh_name"), TEXT("generation_mode"), TEXT("notes"), TEXT("primitive_mesh"), TEXT("voxel_grid")};
		if (!HasOnlyKeys(RootObject, TopLevelKeys, OutError, TEXT("Mesh payload")))
		{
			return false;
		}

		FString GenerationModeString;
		if (!ReadStringField(RootObject, TEXT("version"), OutPayload.Version, OutError, TEXT("Mesh payload"))
			|| !ReadStringField(RootObject, TEXT("mesh_name"), OutPayload.MeshName, OutError, TEXT("Mesh payload"))
			|| !ReadStringField(RootObject, TEXT("generation_mode"), GenerationModeString, OutError, TEXT("Mesh payload"))
			|| !ReadStringField(RootObject, TEXT("notes"), OutPayload.Notes, OutError, TEXT("Mesh payload")))
		{
			return false;
		}

		OutPayload.GenerationMode = PrototypeGenerationModeFromString(GenerationModeString);
		if (!GenerationModeString.Equals(PrototypeGenerationModeToString(OutPayload.GenerationMode), ESearchCase::IgnoreCase))
		{
			OutError = FString::Printf(TEXT("Unsupported generation_mode '%s'."), *GenerationModeString);
			return false;
		}

		OutPayload.PrimitiveShape = FPrototypeShapeDsl();
		OutPayload.PrimitiveShape.Version = OutPayload.Version;
		OutPayload.PrimitiveShape.MeshName = OutPayload.MeshName;
		OutPayload.PrimitiveShape.Notes = OutPayload.Notes;
		OutPayload.PrimitiveShape.RawJson = RawJson;
		OutPayload.VoxelGrid = FPrototypeVoxelGrid();
		OutPayload.VoxelGrid.Version = OutPayload.Version;
		OutPayload.VoxelGrid.MeshName = OutPayload.MeshName;
		OutPayload.VoxelGrid.Notes = OutPayload.Notes;
		OutPayload.VoxelGrid.RawJson = RawJson;
		OutPayload.RawJson = RawJson;

		if (OutPayload.GenerationMode == EPrototypeGenerationMode::Primitive)
		{
			const TSharedPtr<FJsonObject>* PrimitiveMeshObject = nullptr;
			if (!RootObject->TryGetObjectField(TEXT("primitive_mesh"), PrimitiveMeshObject) || !PrimitiveMeshObject || !PrimitiveMeshObject->IsValid())
			{
				OutError = TEXT("Mesh payload is missing object field 'primitive_mesh' for primitive mode.");
				return false;
			}

			if (!ParsePrimitiveMeshObject(*PrimitiveMeshObject, OutPayload.PrimitiveShape, OutError, TEXT("Mesh payload.primitive_mesh")))
			{
				return false;
			}
		}
		else
		{
			const TSharedPtr<FJsonObject>* VoxelGridObject = nullptr;
			if (!RootObject->TryGetObjectField(TEXT("voxel_grid"), VoxelGridObject) || !VoxelGridObject || !VoxelGridObject->IsValid())
			{
				OutError = TEXT("Mesh payload is missing object field 'voxel_grid' for voxel mode.");
				return false;
			}

			if (!ParseVoxelGridObject(*VoxelGridObject, OutPayload.VoxelGrid, OutError, TEXT("Mesh payload.voxel_grid")))
			{
				return false;
			}
		}

		return true;
	}

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
			|| !ReadStringField(RootObject, TEXT("notes"), OutDsl.Notes, OutError, TEXT("Shape DSL")))
		{
			return false;
		}

		if (!ParsePrimitiveMeshObject(RootObject, OutDsl, OutError, TEXT("Shape DSL")))
		{
			return false;
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

			if (!IsFiniteColor(Primitive.Color))
			{
				OutError = Context + TEXT(" contains non-finite color values.");
				return false;
			}

			if (Primitive.Color.R < 0.0f || Primitive.Color.R > 1.0f
				|| Primitive.Color.G < 0.0f || Primitive.Color.G > 1.0f
				|| Primitive.Color.B < 0.0f || Primitive.Color.B > 1.0f
				|| Primitive.Color.A < 0.0f || Primitive.Color.A > 1.0f)
			{
				OutError = Context + TEXT(" color channels must stay within the 0..1 range.");
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
				BuildPlane(Primitive, OutBuffers);
			}
			else if (Primitive.Type.Equals(TEXT("box")))
			{
				BuildBox(Primitive, OutBuffers);
			}
			else if (Primitive.Type.Equals(TEXT("cylinder")))
			{
				BuildCylinder(Primitive, OutBuffers);
			}
			else if (Primitive.Type.Equals(TEXT("cone")))
			{
				BuildCone(Primitive, OutBuffers);
			}
			else if (Primitive.Type.Equals(TEXT("ramp")))
			{
				BuildRamp(Primitive, OutBuffers);
			}
			else if (Primitive.Type.Equals(TEXT("stair")))
			{
				BuildStair(Primitive, OutBuffers);
			}
		}

		if (!OutBuffers.IsValid())
		{
			OutError = TEXT("Generated buffers were empty or inconsistent.");
			return false;
		}

		if (OutBuffers.GetTriangleCount() > MaxPrimitiveTriangleCount)
		{
			OutError = FString::Printf(TEXT("Triangle budget exceeded: %d > %d."), OutBuffers.GetTriangleCount(), MaxPrimitiveTriangleCount);
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

	bool ParseVoxelGridJson(const FString& RawJson, FPrototypeVoxelGrid& OutGrid, FString& OutError)
	{
		TSharedPtr<FJsonObject> RootObject;
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(RawJson);
		if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
		{
			OutError = TEXT("Bridge output was not valid JSON.");
			return false;
		}

		static const TSet<FString> TopLevelKeys{TEXT("version"), TEXT("mesh_name"), TEXT("resolution"), TEXT("color_hex_stream"), TEXT("voxels_hex"), TEXT("notes")};
		if (!HasOnlyKeys(RootObject, TopLevelKeys, OutError, TEXT("Voxel grid")))
		{
			return false;
		}

		if (!ReadStringField(RootObject, TEXT("version"), OutGrid.Version, OutError, TEXT("Voxel grid"))
			|| !ReadStringField(RootObject, TEXT("mesh_name"), OutGrid.MeshName, OutError, TEXT("Voxel grid"))
			|| !ReadStringField(RootObject, TEXT("notes"), OutGrid.Notes, OutError, TEXT("Voxel grid")))
		{
			return false;
		}

		if (!ParseVoxelGridObject(RootObject, OutGrid, OutError, TEXT("Voxel grid")))
		{
			return false;
		}

		OutGrid.RawJson = RawJson;
		return true;
	}

	bool ValidateVoxelGrid(const FPrototypeVoxelGrid& Grid, FString& OutError)
	{
		if (Grid.Resolution.X <= 0 || Grid.Resolution.Y <= 0 || Grid.Resolution.Z <= 0)
		{
			OutError = TEXT("Voxel grid resolution must be positive on all axes.");
			return false;
		}

		if (Grid.Resolution.X != Grid.Resolution.Y || Grid.Resolution.X != Grid.Resolution.Z)
		{
			OutError = TEXT("Voxel grid resolution must be cubic (x == y == z).");
			return false;
		}

		if (!PrototypeIsSupportedVoxelResolution(Grid.Resolution.X))
		{
			OutError = TEXT("Voxel grid resolution must be one of 16, 32, 64, 128, or 256.");
			return false;
		}

		TArray<uint32> DecodedColors;
		int32 OccupiedCount = 0;
		return DecodeVoxelHexStream(Grid, DecodedColors, OccupiedCount, OutError);
	}

	bool CountOccupiedVoxels(const FPrototypeVoxelGrid& Grid, int32& OutOccupiedCount, FString& OutError)
	{
		if (Grid.Resolution.X <= 0 || Grid.Resolution.Y <= 0 || Grid.Resolution.Z <= 0)
		{
			OutError = TEXT("Voxel grid resolution must be positive on all axes.");
			return false;
		}

		if (Grid.Resolution.X != Grid.Resolution.Y || Grid.Resolution.X != Grid.Resolution.Z)
		{
			OutError = TEXT("Voxel grid resolution must be cubic (x == y == z).");
			return false;
		}

		if (!PrototypeIsSupportedVoxelResolution(Grid.Resolution.X))
		{
			OutError = TEXT("Voxel grid resolution must be one of 16, 32, 64, 128, or 256.");
			return false;
		}

		TArray<uint32> DecodedColors;
		return DecodeVoxelHexStream(Grid, DecodedColors, OutOccupiedCount, OutError);
	}

	bool BuildVoxelMeshBuffers(const FPrototypeVoxelGrid& Grid, FGeneratedMeshBuffers& OutBuffers, FString& OutError)
	{
		OutBuffers.Reset();

		if (!ValidateVoxelGrid(Grid, OutError))
		{
			return false;
		}

		TArray<uint32> Colors;
		int32 OccupiedCount = 0;
		if (!DecodeVoxelHexStream(Grid, Colors, OccupiedCount, OutError))
		{
			return false;
		}

		if (OccupiedCount == 0)
		{
			OutError = TEXT("Voxel grid does not contain any filled voxels.");
			return false;
		}

		const FIntVector Resolution = Grid.Resolution;
		const float VoxelSizeCm = 100.0f / static_cast<float>(Resolution.X);
		TArray<FVoxelMaskCell> Mask;

		for (int32 Axis = 0; Axis < 3; ++Axis)
		{
			const int32 UAxis = (Axis + 1) % 3;
			const int32 VAxis = (Axis + 2) % 3;
			const int32 PlaneWidth = Resolution[UAxis];
			const int32 PlaneHeight = Resolution[VAxis];
			Mask.SetNumZeroed(PlaneWidth * PlaneHeight);

			for (int32 Plane = 0; Plane <= Resolution[Axis]; ++Plane)
			{
				int32 MaskIndex = 0;
				for (int32 V = 0; V < PlaneHeight; ++V)
				{
					for (int32 U = 0; U < PlaneWidth; ++U)
					{
						FIntVector NegativeCoord = FIntVector::ZeroValue;
						NegativeCoord[Axis] = Plane - 1;
						NegativeCoord[UAxis] = U;
						NegativeCoord[VAxis] = V;

						FIntVector PositiveCoord = FIntVector::ZeroValue;
						PositiveCoord[Axis] = Plane;
						PositiveCoord[UAxis] = U;
						PositiveCoord[VAxis] = V;

						const uint32 NegativeColor = GetVoxelColorAt(Colors, Resolution, NegativeCoord.X, NegativeCoord.Y, NegativeCoord.Z);
						const uint32 PositiveColor = GetVoxelColorAt(Colors, Resolution, PositiveCoord.X, PositiveCoord.Y, PositiveCoord.Z);

						FVoxelMaskCell& Cell = Mask[MaskIndex++];
						Cell = FVoxelMaskCell();
						if (NegativeColor != 0 && PositiveColor == 0)
						{
							Cell.Color = NegativeColor;
							Cell.NormalSign = 1;
						}
						else if (NegativeColor == 0 && PositiveColor != 0)
						{
							Cell.Color = PositiveColor;
							Cell.NormalSign = -1;
						}
					}
				}

				for (int32 V = 0; V < PlaneHeight; ++V)
				{
					for (int32 U = 0; U < PlaneWidth;)
					{
						const FVoxelMaskCell Cell = Mask[U + (V * PlaneWidth)];
						if (!Cell.IsVisible())
						{
							++U;
							continue;
						}

						int32 Width = 1;
						while (U + Width < PlaneWidth && Mask[(U + Width) + (V * PlaneWidth)].Matches(Cell))
						{
							++Width;
						}

						int32 Height = 1;
						bool bCanGrow = true;
						while (V + Height < PlaneHeight && bCanGrow)
						{
							for (int32 RowU = 0; RowU < Width; ++RowU)
							{
								if (!Mask[(U + RowU) + ((V + Height) * PlaneWidth)].Matches(Cell))
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

						AppendVoxelQuad(OutBuffers, Resolution, VoxelSizeCm, Axis, Cell.NormalSign, Plane, U, V, Width, Height, Cell.Color);

						for (int32 ClearV = 0; ClearV < Height; ++ClearV)
						{
							for (int32 ClearU = 0; ClearU < Width; ++ClearU)
							{
								Mask[(U + ClearU) + ((V + ClearV) * PlaneWidth)] = FVoxelMaskCell();
							}
						}

						U += Width;
					}
				}
			}
		}

		if (!OutBuffers.IsValid())
		{
			OutError = TEXT("Generated voxel mesh buffers were empty or inconsistent.");
			return false;
		}

		if (OutBuffers.GetTriangleCount() > MaxVoxelTriangleCount)
		{
			OutError = FString::Printf(TEXT("Voxel triangle budget exceeded: %d > %d."), OutBuffers.GetTriangleCount(), MaxVoxelTriangleCount);
			return false;
		}

		CenterBuffersBasePivot(OutBuffers);
		return true;
	}

	bool BuildMeshBuffers(const FPrototypeMeshPayload& Payload, FGeneratedMeshBuffers& OutBuffers, FString& OutError)
	{
		if (Payload.GenerationMode == EPrototypeGenerationMode::Voxel)
		{
			return BuildVoxelMeshBuffers(Payload.VoxelGrid, OutBuffers, OutError);
		}

		return BuildMeshBuffers(Payload.PrimitiveShape, OutBuffers, OutError);
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

	bool CreateVertexColorLitMaterialAsset(const FString& PackagePath, const FString& AssetName, UMaterialInterface*& OutMaterial, FString& OutError)
	{
		OutMaterial = nullptr;

		UPackage* Package = CreatePackage(*PackagePath);
		if (!Package)
		{
			OutError = FString::Printf(TEXT("Failed to create material package '%s'."), *PackagePath);
			return false;
		}

		UMaterial* Material = NewObject<UMaterial>(Package, *AssetName, RF_Public | RF_Standalone | RF_Transactional);
		if (!Material)
		{
			OutError = TEXT("Failed to allocate a lit vertex-color material.");
			return false;
		}

		Material->MaterialDomain = MD_Surface;
		Material->BlendMode = BLEND_Opaque;
		Material->TwoSided = false;
		Material->SetShadingModel(MSM_DefaultLit);

		UMaterialEditorOnlyData* MaterialEditorOnly = Material->GetEditorOnlyData();
		if (!MaterialEditorOnly)
		{
			OutError = TEXT("Material editor data is not available.");
			return false;
		}

		UMaterialExpressionVertexColor* VertexColorExpression = NewObject<UMaterialExpressionVertexColor>(Material);
		UMaterialExpressionConstant* RoughnessExpression = NewObject<UMaterialExpressionConstant>(Material);
		UMaterialExpressionConstant* SpecularExpression = NewObject<UMaterialExpressionConstant>(Material);
		if (!VertexColorExpression || !RoughnessExpression || !SpecularExpression)
		{
			OutError = TEXT("Failed to allocate material expressions.");
			return false;
		}

		VertexColorExpression->MaterialExpressionEditorX = -420;
		VertexColorExpression->MaterialExpressionEditorY = -40;
		RoughnessExpression->MaterialExpressionEditorX = -420;
		RoughnessExpression->MaterialExpressionEditorY = 120;
		RoughnessExpression->R = 0.85f;
		SpecularExpression->MaterialExpressionEditorX = -420;
		SpecularExpression->MaterialExpressionEditorY = 220;
		SpecularExpression->R = 0.25f;

		Material->GetExpressionCollection().AddExpression(VertexColorExpression);
		Material->GetExpressionCollection().AddExpression(RoughnessExpression);
		Material->GetExpressionCollection().AddExpression(SpecularExpression);
		MaterialEditorOnly->BaseColor.Connect(0, VertexColorExpression);
		MaterialEditorOnly->Roughness.Connect(0, RoughnessExpression);
		MaterialEditorOnly->Specular.Connect(0, SpecularExpression);

		Material->PreEditChange(nullptr);
		Material->PostEditChange();
		Package->MarkPackageDirty();
		FAssetRegistryModule::AssetCreated(Material);
		OutMaterial = Material;
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
