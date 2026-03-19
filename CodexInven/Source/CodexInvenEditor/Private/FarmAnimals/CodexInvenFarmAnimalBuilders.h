#pragma once

#include "CoreMinimal.h"

class UDynamicMesh;

enum class ECodexInvenFarmAnimalType : uint8
{
	Pig,
	Duck,
	Chicken,
	Sheep,
	Cow,
	Rabbit,
	Goose
};

enum class ECodexInvenFarmAnimalStyle : uint8
{
	Retro,
	Refined,
	Toy
};

namespace CodexInvenFarmAnimals
{
	TConstArrayView<ECodexInvenFarmAnimalType> GetAllAnimalTypes();
	TConstArrayView<ECodexInvenFarmAnimalStyle> GetAllStyles();
	FString GetAnimalSlug(ECodexInvenFarmAnimalType InAnimalType);
	FString GetStyleSlug(ECodexInvenFarmAnimalStyle InStyle);
	FText GetAnimalDisplayName(ECodexInvenFarmAnimalType InAnimalType);
	FText GetStyleDisplayName(ECodexInvenFarmAnimalStyle InStyle);
	FString GetMeshPackagePath(ECodexInvenFarmAnimalType InAnimalType, ECodexInvenFarmAnimalStyle InStyle);

	UDynamicMesh* BuildAnimalMesh(ECodexInvenFarmAnimalType InAnimalType, ECodexInvenFarmAnimalStyle InStyle);
	UDynamicMesh* BuildPigMesh(ECodexInvenFarmAnimalStyle InStyle);
	UDynamicMesh* BuildDuckMesh(ECodexInvenFarmAnimalStyle InStyle);
	UDynamicMesh* BuildChickenMesh(ECodexInvenFarmAnimalStyle InStyle);
	UDynamicMesh* BuildSheepMesh(ECodexInvenFarmAnimalStyle InStyle);
	UDynamicMesh* BuildCowMesh(ECodexInvenFarmAnimalStyle InStyle);
	UDynamicMesh* BuildRabbitMesh(ECodexInvenFarmAnimalStyle InStyle);
	UDynamicMesh* BuildGooseMesh(ECodexInvenFarmAnimalStyle InStyle);
}
