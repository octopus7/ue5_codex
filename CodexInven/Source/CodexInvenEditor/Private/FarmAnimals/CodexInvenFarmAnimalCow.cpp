#include "FarmAnimals/CodexInvenFarmAnimalBuilders.h"

#include "FarmAnimals/CodexInvenFarmAnimalMeshUtilities.h"

namespace
{
	using namespace CodexInvenFarmAnimalMeshUtilities;

	void AddCowLegs(UDynamicMesh& InOutMesh, const FLinearColor& InColor)
	{
		AppendCylinderPart(InOutMesh, 4.4f, 32.0f, FVector(-18.0f, -12.0f, 16.0f), FRotator::ZeroRotator, InColor, 8);
		AppendCylinderPart(InOutMesh, 4.4f, 32.0f, FVector(-18.0f, 12.0f, 16.0f), FRotator::ZeroRotator, InColor, 8);
		AppendCylinderPart(InOutMesh, 4.4f, 32.0f, FVector(20.0f, -12.0f, 16.0f), FRotator::ZeroRotator, InColor, 8);
		AppendCylinderPart(InOutMesh, 4.4f, 32.0f, FVector(20.0f, 12.0f, 16.0f), FRotator::ZeroRotator, InColor, 8);
	}
}

UDynamicMesh* CodexInvenFarmAnimals::BuildCowMesh(const ECodexInvenFarmAnimalStyle InStyle)
{
	using namespace CodexInvenFarmAnimalMeshUtilities;

	UDynamicMesh* const Mesh = MakeTransientDynamicMesh();

	switch (InStyle)
	{
	case ECodexInvenFarmAnimalStyle::Retro:
	{
		const FLinearColor Body(0.90f, 0.90f, 0.88f, 1.0f);
		const FLinearColor Patch(0.12f, 0.12f, 0.12f, 1.0f);
		const FLinearColor Horn(0.78f, 0.66f, 0.42f, 1.0f);

		AppendBoxPart(*Mesh, FVector(84.0f, 44.0f, 42.0f), FVector(0.0f, 0.0f, 40.0f), FRotator::ZeroRotator, Body);
		AppendBoxPart(*Mesh, FVector(10.0f, 24.0f, 18.0f), FVector(-6.0f, -16.0f, 42.0f), FRotator::ZeroRotator, Patch);
		AppendBoxPart(*Mesh, FVector(16.0f, 14.0f, 14.0f), FVector(16.0f, 14.0f, 52.0f), FRotator::ZeroRotator, Patch);
		AppendBoxPart(*Mesh, FVector(26.0f, 24.0f, 24.0f), FVector(52.0f, 0.0f, 42.0f), FRotator::ZeroRotator, Body);
		AppendBoxPart(*Mesh, FVector(12.0f, 20.0f, 10.0f), FVector(66.0f, 0.0f, 34.0f), FRotator::ZeroRotator, Patch);
		AppendConePart(*Mesh, 4.0f, 0.0f, 10.0f, FVector(46.0f, -10.0f, 58.0f), FRotator(-10.0f, 0.0f, -18.0f), Horn, 4);
		AppendConePart(*Mesh, 4.0f, 0.0f, 10.0f, FVector(46.0f, 10.0f, 58.0f), FRotator(10.0f, 0.0f, 18.0f), Horn, 4);
		AddCowLegs(*Mesh, Patch);
		AppendCylinderPart(*Mesh, 2.2f, 24.0f, FVector(-44.0f, 0.0f, 52.0f), FRotator(0.0f, 0.0f, 18.0f), Patch, 6);
		break;
	}

	case ECodexInvenFarmAnimalStyle::Refined:
	{
		const FLinearColor Body(0.94f, 0.94f, 0.92f, 1.0f);
		const FLinearColor Patch(0.14f, 0.14f, 0.14f, 1.0f);
		const FLinearColor Horn(0.82f, 0.72f, 0.50f, 1.0f);

		AppendCapsulePart(*Mesh, 20.0f, 50.0f, FVector(-2.0f, 0.0f, 42.0f), FRotator(0.0f, 90.0f, 0.0f), Body, 5, 10, 2, FVector(1.15f, 1.0f, 1.0f));
		AppendSpherePart(*Mesh, 16.0f, FVector(42.0f, 0.0f, 44.0f), FRotator::ZeroRotator, Body, 5, 5, 5, FVector(1.1f, 0.95f, 0.95f));
		AppendBoxPart(*Mesh, FVector(18.0f, 24.0f, 12.0f), FVector(56.0f, 0.0f, 34.0f), FRotator::ZeroRotator, Patch);
		AppendSpherePart(*Mesh, 10.0f, FVector(-8.0f, -16.0f, 44.0f), FRotator::ZeroRotator, Patch, 5, 5, 5, FVector(0.8f, 0.45f, 1.0f));
		AppendSpherePart(*Mesh, 10.0f, FVector(12.0f, 14.0f, 52.0f), FRotator::ZeroRotator, Patch, 5, 5, 5, FVector(0.9f, 0.5f, 0.8f));
		AppendConePart(*Mesh, 3.5f, 0.0f, 10.0f, FVector(34.0f, -10.0f, 60.0f), FRotator(-14.0f, 0.0f, -18.0f), Horn, 8);
		AppendConePart(*Mesh, 3.5f, 0.0f, 10.0f, FVector(34.0f, 10.0f, 60.0f), FRotator(14.0f, 0.0f, 18.0f), Horn, 8);
		AddCowLegs(*Mesh, Patch);
		AppendCylinderPart(*Mesh, 1.8f, 28.0f, FVector(-48.0f, 0.0f, 54.0f), FRotator(0.0f, 0.0f, 20.0f), Patch, 8);
		break;
	}

	case ECodexInvenFarmAnimalStyle::Toy:
	{
		const FLinearColor Body(0.96f, 0.96f, 0.94f, 1.0f);
		const FLinearColor Patch(0.16f, 0.16f, 0.16f, 1.0f);
		const FLinearColor Horn(0.88f, 0.78f, 0.54f, 1.0f);

		AppendSpherePart(*Mesh, 22.0f, FVector(-4.0f, 0.0f, 42.0f), FRotator::ZeroRotator, Body, 6, 6, 6, FVector(1.5f, 1.0f, 1.0f));
		AppendSpherePart(*Mesh, 17.0f, FVector(34.0f, 0.0f, 46.0f), FRotator::ZeroRotator, Body, 6, 6, 6);
		AppendSpherePart(*Mesh, 9.0f, FVector(-10.0f, -16.0f, 42.0f), FRotator::ZeroRotator, Patch, 5, 5, 5, FVector(0.8f, 0.4f, 1.0f));
		AppendSpherePart(*Mesh, 9.0f, FVector(8.0f, 14.0f, 50.0f), FRotator::ZeroRotator, Patch, 5, 5, 5, FVector(0.8f, 0.4f, 0.8f));
		AppendBoxPart(*Mesh, FVector(16.0f, 22.0f, 10.0f), FVector(47.0f, 0.0f, 36.0f), FRotator::ZeroRotator, Patch);
		AppendConePart(*Mesh, 4.0f, 0.0f, 11.0f, FVector(26.0f, -10.0f, 62.0f), FRotator(-14.0f, 0.0f, -16.0f), Horn, 8);
		AppendConePart(*Mesh, 4.0f, 0.0f, 11.0f, FVector(26.0f, 10.0f, 62.0f), FRotator(14.0f, 0.0f, 16.0f), Horn, 8);
		AddCowLegs(*Mesh, Patch);
		AppendCylinderPart(*Mesh, 2.0f, 24.0f, FVector(-46.0f, 0.0f, 54.0f), FRotator(0.0f, 0.0f, 24.0f), Patch, 8);
		break;
	}
	}

	FinishMesh(*Mesh);
	return Mesh;
}
