#pragma once

#include "CoreMinimal.h"

class UMaterial;

class FCodexVoxMaterialBuilder
{
public:
	static bool CreateOrUpdateSharedMaterial(UMaterial*& OutMaterial, FString& OutError);
};
