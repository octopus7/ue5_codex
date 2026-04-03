#pragma once

#include "CoreMinimal.h"

class UStaticMesh;

namespace VoxReconstructionService
{
	bool GenerateSmoothReconstruction(UStaticMesh* SourceMesh, UObject*& OutCreatedAsset, FString& OutError);
}
