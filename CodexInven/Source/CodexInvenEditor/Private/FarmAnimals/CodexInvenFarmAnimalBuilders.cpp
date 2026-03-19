#include "FarmAnimals/CodexInvenFarmAnimalBuilders.h"

TConstArrayView<ECodexInvenFarmAnimalType> CodexInvenFarmAnimals::GetAllAnimalTypes()
{
	static const ECodexInvenFarmAnimalType AnimalTypes[] =
	{
		ECodexInvenFarmAnimalType::Pig,
		ECodexInvenFarmAnimalType::Duck,
		ECodexInvenFarmAnimalType::Chicken,
		ECodexInvenFarmAnimalType::Sheep,
		ECodexInvenFarmAnimalType::Cow,
		ECodexInvenFarmAnimalType::Rabbit,
		ECodexInvenFarmAnimalType::Goose
	};

	return TConstArrayView<ECodexInvenFarmAnimalType>(AnimalTypes);
}

TConstArrayView<ECodexInvenFarmAnimalStyle> CodexInvenFarmAnimals::GetAllStyles()
{
	static const ECodexInvenFarmAnimalStyle Styles[] =
	{
		ECodexInvenFarmAnimalStyle::Retro,
		ECodexInvenFarmAnimalStyle::Refined,
		ECodexInvenFarmAnimalStyle::Toy
	};

	return TConstArrayView<ECodexInvenFarmAnimalStyle>(Styles);
}

FString CodexInvenFarmAnimals::GetAnimalSlug(const ECodexInvenFarmAnimalType InAnimalType)
{
	switch (InAnimalType)
	{
	case ECodexInvenFarmAnimalType::Pig: return TEXT("Pig");
	case ECodexInvenFarmAnimalType::Duck: return TEXT("Duck");
	case ECodexInvenFarmAnimalType::Chicken: return TEXT("Chicken");
	case ECodexInvenFarmAnimalType::Sheep: return TEXT("Sheep");
	case ECodexInvenFarmAnimalType::Cow: return TEXT("Cow");
	case ECodexInvenFarmAnimalType::Rabbit: return TEXT("Rabbit");
	case ECodexInvenFarmAnimalType::Goose: return TEXT("Goose");
	}

	return TEXT("UnknownAnimal");
}

FString CodexInvenFarmAnimals::GetStyleSlug(const ECodexInvenFarmAnimalStyle InStyle)
{
	switch (InStyle)
	{
	case ECodexInvenFarmAnimalStyle::Retro: return TEXT("Retro");
	case ECodexInvenFarmAnimalStyle::Refined: return TEXT("Refined");
	case ECodexInvenFarmAnimalStyle::Toy: return TEXT("Toy");
	}

	return TEXT("UnknownStyle");
}

FText CodexInvenFarmAnimals::GetAnimalDisplayName(const ECodexInvenFarmAnimalType InAnimalType)
{
	return FText::FromString(GetAnimalSlug(InAnimalType));
}

FText CodexInvenFarmAnimals::GetStyleDisplayName(const ECodexInvenFarmAnimalStyle InStyle)
{
	return FText::FromString(GetStyleSlug(InStyle));
}

FString CodexInvenFarmAnimals::GetMeshPackagePath(const ECodexInvenFarmAnimalType InAnimalType, const ECodexInvenFarmAnimalStyle InStyle)
{
	const FString StyleSlug = GetStyleSlug(InStyle);
	const FString AnimalSlug = GetAnimalSlug(InAnimalType);
	return FString::Printf(TEXT("/Game/TestGenerated/FarmAnimals/%s/Meshes/SM_FarmAnimal_%s_%s"), *StyleSlug, *AnimalSlug, *StyleSlug);
}

UDynamicMesh* CodexInvenFarmAnimals::BuildAnimalMesh(const ECodexInvenFarmAnimalType InAnimalType, const ECodexInvenFarmAnimalStyle InStyle)
{
	switch (InAnimalType)
	{
	case ECodexInvenFarmAnimalType::Pig:
		return BuildPigMesh(InStyle);
	case ECodexInvenFarmAnimalType::Duck:
		return BuildDuckMesh(InStyle);
	case ECodexInvenFarmAnimalType::Chicken:
		return BuildChickenMesh(InStyle);
	case ECodexInvenFarmAnimalType::Sheep:
		return BuildSheepMesh(InStyle);
	case ECodexInvenFarmAnimalType::Cow:
		return BuildCowMesh(InStyle);
	case ECodexInvenFarmAnimalType::Rabbit:
		return BuildRabbitMesh(InStyle);
	case ECodexInvenFarmAnimalType::Goose:
		return BuildGooseMesh(InStyle);
	}

	return nullptr;
}
