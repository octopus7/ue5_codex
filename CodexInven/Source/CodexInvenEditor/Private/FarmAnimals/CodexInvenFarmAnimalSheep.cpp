#include "FarmAnimals/CodexInvenFarmAnimalBuilders.h"

#include "FarmAnimals/CodexInvenFarmAnimalMeshUtilities.h"

namespace
{
	using namespace CodexInvenFarmAnimalMeshUtilities;

	void AddSheepLegs(UDynamicMesh& InOutMesh, const FLinearColor& InColor)
	{
		AppendCylinderPart(InOutMesh, 3.8f, 26.0f, FVector(-14.0f, -10.0f, 13.0f), FRotator::ZeroRotator, InColor, 8);
		AppendCylinderPart(InOutMesh, 3.8f, 26.0f, FVector(-14.0f, 10.0f, 13.0f), FRotator::ZeroRotator, InColor, 8);
		AppendCylinderPart(InOutMesh, 3.8f, 26.0f, FVector(16.0f, -10.0f, 13.0f), FRotator::ZeroRotator, InColor, 8);
		AppendCylinderPart(InOutMesh, 3.8f, 26.0f, FVector(16.0f, 10.0f, 13.0f), FRotator::ZeroRotator, InColor, 8);
	}
}

UDynamicMesh* CodexInvenFarmAnimals::BuildSheepMesh(const ECodexInvenFarmAnimalStyle InStyle)
{
	using namespace CodexInvenFarmAnimalMeshUtilities;

	UDynamicMesh* const Mesh = MakeTransientDynamicMesh();

	switch (InStyle)
	{
	case ECodexInvenFarmAnimalStyle::Retro:
	{
		const FLinearColor Wool(0.88f, 0.86f, 0.80f, 1.0f);
		const FLinearColor Face(0.22f, 0.18f, 0.16f, 1.0f);

		AppendBoxPart(*Mesh, FVector(58.0f, 40.0f, 38.0f), FVector(0.0f, 0.0f, 34.0f), FRotator::ZeroRotator, Wool);
		AppendBoxPart(*Mesh, FVector(12.0f, 12.0f, 8.0f), FVector(-16.0f, -14.0f, 52.0f), FRotator::ZeroRotator, Wool);
		AppendBoxPart(*Mesh, FVector(12.0f, 12.0f, 8.0f), FVector(-16.0f, 14.0f, 52.0f), FRotator::ZeroRotator, Wool);
		AppendBoxPart(*Mesh, FVector(24.0f, 18.0f, 18.0f), FVector(38.0f, 0.0f, 40.0f), FRotator::ZeroRotator, Face);
		AppendConePart(*Mesh, 4.0f, 0.0f, 10.0f, FVector(32.0f, -8.0f, 55.0f), FRotator(12.0f, 0.0f, -16.0f), Face, 4);
		AppendConePart(*Mesh, 4.0f, 0.0f, 10.0f, FVector(32.0f, 8.0f, 55.0f), FRotator(-12.0f, 0.0f, 16.0f), Face, 4);
		AddSheepLegs(*Mesh, Face);
		break;
	}

	case ECodexInvenFarmAnimalStyle::Refined:
	{
		const FLinearColor Wool(0.90f, 0.90f, 0.86f, 1.0f);
		const FLinearColor Face(0.25f, 0.21f, 0.18f, 1.0f);

		AppendCapsulePart(*Mesh, 18.0f, 34.0f, FVector(0.0f, 0.0f, 36.0f), FRotator(0.0f, 90.0f, 0.0f), Wool, 5, 10, 2, FVector(1.1f, 1.1f, 1.0f));
		AppendSpherePart(*Mesh, 12.0f, FVector(-18.0f, -12.0f, 44.0f), FRotator::ZeroRotator, Wool, 5, 5, 5);
		AppendSpherePart(*Mesh, 12.0f, FVector(-18.0f, 12.0f, 44.0f), FRotator::ZeroRotator, Wool, 5, 5, 5);
		AppendSpherePart(*Mesh, 12.0f, FVector(0.0f, 0.0f, 50.0f), FRotator::ZeroRotator, Wool, 5, 5, 5);
		AppendSpherePart(*Mesh, 12.0f, FVector(18.0f, -10.0f, 44.0f), FRotator::ZeroRotator, Wool, 5, 5, 5);
		AppendSpherePart(*Mesh, 12.0f, FVector(18.0f, 10.0f, 44.0f), FRotator::ZeroRotator, Wool, 5, 5, 5);
		AppendCapsulePart(*Mesh, 7.0f, 14.0f, FVector(38.0f, 0.0f, 40.0f), FRotator(0.0f, 90.0f, 0.0f), Face, 4, 8, 1);
		AppendConePart(*Mesh, 3.6f, 0.0f, 8.0f, FVector(30.0f, -8.0f, 54.0f), FRotator(12.0f, 0.0f, -18.0f), Face, 8);
		AppendConePart(*Mesh, 3.6f, 0.0f, 8.0f, FVector(30.0f, 8.0f, 54.0f), FRotator(-12.0f, 0.0f, 18.0f), Face, 8);
		AddSheepLegs(*Mesh, Face);
		break;
	}

	case ECodexInvenFarmAnimalStyle::Toy:
	{
		const FLinearColor Wool(0.96f, 0.94f, 0.90f, 1.0f);
		const FLinearColor Face(0.28f, 0.22f, 0.18f, 1.0f);

		AppendSpherePart(*Mesh, 17.0f, FVector(-8.0f, 0.0f, 38.0f), FRotator::ZeroRotator, Wool, 6, 6, 6);
		AppendSpherePart(*Mesh, 17.0f, FVector(10.0f, 0.0f, 38.0f), FRotator::ZeroRotator, Wool, 6, 6, 6);
		AppendSpherePart(*Mesh, 14.0f, FVector(0.0f, -14.0f, 36.0f), FRotator::ZeroRotator, Wool, 6, 6, 6);
		AppendSpherePart(*Mesh, 14.0f, FVector(0.0f, 14.0f, 36.0f), FRotator::ZeroRotator, Wool, 6, 6, 6);
		AppendSpherePart(*Mesh, 12.0f, FVector(30.0f, 0.0f, 40.0f), FRotator::ZeroRotator, Face, 5, 5, 5, FVector(1.15f, 0.95f, 0.95f));
		AppendConePart(*Mesh, 3.6f, 0.0f, 8.0f, FVector(22.0f, -8.0f, 56.0f), FRotator(12.0f, 0.0f, -18.0f), Face, 8);
		AppendConePart(*Mesh, 3.6f, 0.0f, 8.0f, FVector(22.0f, 8.0f, 56.0f), FRotator(-12.0f, 0.0f, 18.0f), Face, 8);
		AddSheepLegs(*Mesh, Face);
		break;
	}
	}

	FinishMesh(*Mesh);
	return Mesh;
}
