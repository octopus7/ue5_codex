#pragma once

#include "CoreMinimal.h"

class UMaterial;
class UTexture2D;

class FCodexBasicMapFloorBuilder
{
public:
	static bool RunBuild(FString& OutError);

private:
	static bool CreateOrUpdateTexture(UTexture2D*& OutTexture, FString& OutError);
	static bool CreateOrUpdateMaterial(UTexture2D& Texture, UMaterial*& OutMaterial, FString& OutError);
	static bool ApplyMaterialToBasicMapFloor(UMaterial& Material, FString& OutError);
};
