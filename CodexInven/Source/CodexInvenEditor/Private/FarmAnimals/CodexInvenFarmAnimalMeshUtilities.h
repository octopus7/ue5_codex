#pragma once

#include "CoreMinimal.h"
#include "GeometryScript/MeshPrimitiveFunctions.h"

class UDynamicMesh;

namespace CodexInvenFarmAnimalMeshUtilities
{
	UDynamicMesh* MakeTransientDynamicMesh();

	void AppendBoxPart(
		UDynamicMesh& InOutMesh,
		const FVector& InDimensions,
		const FVector& InLocation,
		const FRotator& InRotation,
		const FLinearColor& InColor,
		const FVector& InScale = FVector::OneVector,
		EGeometryScriptPrimitiveOriginMode InOrigin = EGeometryScriptPrimitiveOriginMode::Center);

	void AppendCylinderPart(
		UDynamicMesh& InOutMesh,
		float InRadius,
		float InHeight,
		const FVector& InLocation,
		const FRotator& InRotation,
		const FLinearColor& InColor,
		int32 InRadialSteps = 12,
		const FVector& InScale = FVector::OneVector,
		EGeometryScriptPrimitiveOriginMode InOrigin = EGeometryScriptPrimitiveOriginMode::Center);

	void AppendConePart(
		UDynamicMesh& InOutMesh,
		float InBaseRadius,
		float InTopRadius,
		float InHeight,
		const FVector& InLocation,
		const FRotator& InRotation,
		const FLinearColor& InColor,
		int32 InRadialSteps = 12,
		const FVector& InScale = FVector::OneVector,
		EGeometryScriptPrimitiveOriginMode InOrigin = EGeometryScriptPrimitiveOriginMode::Center);

	void AppendSpherePart(
		UDynamicMesh& InOutMesh,
		float InRadius,
		const FVector& InLocation,
		const FRotator& InRotation,
		const FLinearColor& InColor,
		int32 InStepsX = 6,
		int32 InStepsY = 6,
		int32 InStepsZ = 6,
		const FVector& InScale = FVector::OneVector,
		EGeometryScriptPrimitiveOriginMode InOrigin = EGeometryScriptPrimitiveOriginMode::Center);

	void AppendCapsulePart(
		UDynamicMesh& InOutMesh,
		float InRadius,
		float InLineLength,
		const FVector& InLocation,
		const FRotator& InRotation,
		const FLinearColor& InColor,
		int32 InHemisphereSteps = 5,
		int32 InCircleSteps = 10,
		int32 InSegmentSteps = 0,
		const FVector& InScale = FVector::OneVector,
		EGeometryScriptPrimitiveOriginMode InOrigin = EGeometryScriptPrimitiveOriginMode::Center);

	void FinishMesh(UDynamicMesh& InOutMesh);
}
