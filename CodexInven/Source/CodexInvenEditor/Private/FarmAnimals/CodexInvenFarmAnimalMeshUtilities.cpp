#include "FarmAnimals/CodexInvenFarmAnimalMeshUtilities.h"

#include "GeometryScript/MeshBasicEditFunctions.h"
#include "GeometryScript/MeshNormalsFunctions.h"
#include "GeometryScript/MeshPrimitiveFunctions.h"
#include "GeometryScript/MeshVertexColorFunctions.h"
#include "UDynamicMesh.h"

namespace
{
	FGeometryScriptPrimitiveOptions MakePrimitiveOptions()
	{
		FGeometryScriptPrimitiveOptions Options;
		Options.PolygroupMode = EGeometryScriptPrimitivePolygroupMode::PerFace;
		Options.MaterialID = 0;
		return Options;
	}

	void SetVertexColor(UDynamicMesh& InMesh, const FLinearColor& InColor)
	{
		UGeometryScriptLibrary_MeshVertexColorFunctions::SetMeshConstantVertexColor(
			&InMesh,
			InColor,
			FGeometryScriptColorFlags(),
			true);
	}

	void AppendColoredMesh(UDynamicMesh& InOutMesh, UDynamicMesh& InPartMesh, const FLinearColor& InColor)
	{
		SetVertexColor(InPartMesh, InColor);
		UGeometryScriptLibrary_MeshBasicEditFunctions::AppendMesh(&InOutMesh, &InPartMesh, FTransform::Identity, true);
	}
}

UDynamicMesh* CodexInvenFarmAnimalMeshUtilities::MakeTransientDynamicMesh()
{
	return NewObject<UDynamicMesh>(GetTransientPackage(), NAME_None, RF_Transient);
}

void CodexInvenFarmAnimalMeshUtilities::AppendBoxPart(
	UDynamicMesh& InOutMesh,
	const FVector& InDimensions,
	const FVector& InLocation,
	const FRotator& InRotation,
	const FLinearColor& InColor,
	const FVector& InScale,
	const EGeometryScriptPrimitiveOriginMode InOrigin)
{
	UDynamicMesh* const PartMesh = MakeTransientDynamicMesh();
	UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendBox(
		PartMesh,
		MakePrimitiveOptions(),
		FTransform(InRotation, InLocation, InScale),
		InDimensions.X,
		InDimensions.Y,
		InDimensions.Z,
		0,
		0,
		0,
		InOrigin);
	AppendColoredMesh(InOutMesh, *PartMesh, InColor);
}

void CodexInvenFarmAnimalMeshUtilities::AppendCylinderPart(
	UDynamicMesh& InOutMesh,
	const float InRadius,
	const float InHeight,
	const FVector& InLocation,
	const FRotator& InRotation,
	const FLinearColor& InColor,
	const int32 InRadialSteps,
	const FVector& InScale,
	const EGeometryScriptPrimitiveOriginMode InOrigin)
{
	UDynamicMesh* const PartMesh = MakeTransientDynamicMesh();
	UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendCylinder(
		PartMesh,
		MakePrimitiveOptions(),
		FTransform(InRotation, InLocation, InScale),
		InRadius,
		InHeight,
		InRadialSteps,
		0,
		true,
		InOrigin);
	AppendColoredMesh(InOutMesh, *PartMesh, InColor);
}

void CodexInvenFarmAnimalMeshUtilities::AppendConePart(
	UDynamicMesh& InOutMesh,
	const float InBaseRadius,
	const float InTopRadius,
	const float InHeight,
	const FVector& InLocation,
	const FRotator& InRotation,
	const FLinearColor& InColor,
	const int32 InRadialSteps,
	const FVector& InScale,
	const EGeometryScriptPrimitiveOriginMode InOrigin)
{
	UDynamicMesh* const PartMesh = MakeTransientDynamicMesh();
	UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendCone(
		PartMesh,
		MakePrimitiveOptions(),
		FTransform(InRotation, InLocation, InScale),
		InBaseRadius,
		InTopRadius,
		InHeight,
		InRadialSteps,
		0,
		true,
		InOrigin);
	AppendColoredMesh(InOutMesh, *PartMesh, InColor);
}

void CodexInvenFarmAnimalMeshUtilities::AppendSpherePart(
	UDynamicMesh& InOutMesh,
	const float InRadius,
	const FVector& InLocation,
	const FRotator& InRotation,
	const FLinearColor& InColor,
	const int32 InStepsX,
	const int32 InStepsY,
	const int32 InStepsZ,
	const FVector& InScale,
	const EGeometryScriptPrimitiveOriginMode InOrigin)
{
	UDynamicMesh* const PartMesh = MakeTransientDynamicMesh();
	UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendSphereBox(
		PartMesh,
		MakePrimitiveOptions(),
		FTransform(InRotation, InLocation, InScale),
		InRadius,
		InStepsX,
		InStepsY,
		InStepsZ,
		InOrigin);
	AppendColoredMesh(InOutMesh, *PartMesh, InColor);
}

void CodexInvenFarmAnimalMeshUtilities::AppendCapsulePart(
	UDynamicMesh& InOutMesh,
	const float InRadius,
	const float InLineLength,
	const FVector& InLocation,
	const FRotator& InRotation,
	const FLinearColor& InColor,
	const int32 InHemisphereSteps,
	const int32 InCircleSteps,
	const int32 InSegmentSteps,
	const FVector& InScale,
	const EGeometryScriptPrimitiveOriginMode InOrigin)
{
	UDynamicMesh* const PartMesh = MakeTransientDynamicMesh();
	UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendCapsule(
		PartMesh,
		MakePrimitiveOptions(),
		FTransform(InRotation, InLocation, InScale),
		InRadius,
		InLineLength,
		InHemisphereSteps,
		InCircleSteps,
		InSegmentSteps,
		InOrigin);
	AppendColoredMesh(InOutMesh, *PartMesh, InColor);
}

void CodexInvenFarmAnimalMeshUtilities::FinishMesh(UDynamicMesh& InOutMesh)
{
	UGeometryScriptLibrary_MeshNormalsFunctions::SetPerFaceNormals(&InOutMesh);
}
