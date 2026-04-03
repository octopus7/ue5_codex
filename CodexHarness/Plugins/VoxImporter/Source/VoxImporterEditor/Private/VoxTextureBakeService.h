#pragma once

#include "CoreMinimal.h"

class UStaticMesh;

namespace VoxTextureBakeService
{
	bool BakeVertexColorToTexture(UStaticMesh* StaticMesh, TArray<UObject*>& OutCreatedAssets, FString& OutError);
}
