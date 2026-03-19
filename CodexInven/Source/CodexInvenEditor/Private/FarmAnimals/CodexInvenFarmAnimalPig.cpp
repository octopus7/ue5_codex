#include "FarmAnimals/CodexInvenFarmAnimalBuilders.h"

#include "FarmAnimals/CodexInvenFarmAnimalMeshUtilities.h"

namespace
{
	using namespace CodexInvenFarmAnimalMeshUtilities;

	void AddPigLegPair(UDynamicMesh& InOutMesh, const FLinearColor& InColor, const float InX)
	{
		AppendCylinderPart(InOutMesh, 5.0f, 22.0f, FVector(InX, -12.0f, 11.0f), FRotator::ZeroRotator, InColor, 8);
		AppendCylinderPart(InOutMesh, 5.0f, 22.0f, FVector(InX, 12.0f, 11.0f), FRotator::ZeroRotator, InColor, 8);
	}
}

UDynamicMesh* CodexInvenFarmAnimals::BuildPigMesh(const ECodexInvenFarmAnimalStyle InStyle)
{
	using namespace CodexInvenFarmAnimalMeshUtilities;

	UDynamicMesh* const Mesh = MakeTransientDynamicMesh();

	switch (InStyle)
	{
	case ECodexInvenFarmAnimalStyle::Retro:
	{
		const FLinearColor Body(0.94f, 0.66f, 0.72f, 1.0f);
		const FLinearColor Accent(0.84f, 0.50f, 0.62f, 1.0f);
		const FLinearColor Detail(0.34f, 0.18f, 0.22f, 1.0f);

		AppendBoxPart(*Mesh, FVector(74.0f, 38.0f, 36.0f), FVector(0.0f, 0.0f, 32.0f), FRotator::ZeroRotator, Body);
		AppendBoxPart(*Mesh, FVector(30.0f, 24.0f, 24.0f), FVector(42.0f, 0.0f, 34.0f), FRotator::ZeroRotator, Body);
		AppendBoxPart(*Mesh, FVector(12.0f, 16.0f, 12.0f), FVector(58.0f, 0.0f, 32.0f), FRotator::ZeroRotator, Accent);
		AppendConePart(*Mesh, 6.0f, 0.0f, 12.0f, FVector(34.0f, -8.0f, 52.0f), FRotator(10.0f, 0.0f, -22.0f), Body, 4);
		AppendConePart(*Mesh, 6.0f, 0.0f, 12.0f, FVector(34.0f, 8.0f, 52.0f), FRotator(-10.0f, 0.0f, 22.0f), Body, 4);
		AddPigLegPair(*Mesh, Detail, -18.0f);
		AddPigLegPair(*Mesh, Detail, 18.0f);
		AppendCylinderPart(*Mesh, 2.2f, 12.0f, FVector(-40.0f, 0.0f, 40.0f), FRotator(0.0f, 90.0f, 40.0f), Accent, 6);
		break;
	}

	case ECodexInvenFarmAnimalStyle::Refined:
	{
		const FLinearColor Body(0.96f, 0.72f, 0.78f, 1.0f);
		const FLinearColor Accent(0.90f, 0.58f, 0.68f, 1.0f);
		const FLinearColor Detail(0.30f, 0.18f, 0.18f, 1.0f);

		AppendCapsulePart(*Mesh, 18.0f, 42.0f, FVector(-4.0f, 0.0f, 35.0f), FRotator(0.0f, 90.0f, 0.0f), Body, 5, 10, 2);
		AppendSpherePart(*Mesh, 18.0f, FVector(40.0f, 0.0f, 36.0f), FRotator::ZeroRotator, Body, 6, 6, 6, FVector(1.0f, 0.95f, 0.9f));
		AppendCylinderPart(*Mesh, 7.5f, 14.0f, FVector(54.0f, 0.0f, 31.0f), FRotator(0.0f, 90.0f, 0.0f), Accent, 10, FVector(1.0f, 1.2f, 1.0f));
		AppendConePart(*Mesh, 5.0f, 0.0f, 13.0f, FVector(34.0f, -10.0f, 52.0f), FRotator(12.0f, 0.0f, -28.0f), Body, 8);
		AppendConePart(*Mesh, 5.0f, 0.0f, 13.0f, FVector(34.0f, 10.0f, 52.0f), FRotator(-12.0f, 0.0f, 28.0f), Body, 8);
		AddPigLegPair(*Mesh, Detail, -20.0f);
		AddPigLegPair(*Mesh, Detail, 14.0f);
		AppendCylinderPart(*Mesh, 2.0f, 14.0f, FVector(-44.0f, 0.0f, 42.0f), FRotator(0.0f, 90.0f, 56.0f), Accent, 8);
		break;
	}

	case ECodexInvenFarmAnimalStyle::Toy:
	{
		const FLinearColor Body(0.98f, 0.75f, 0.81f, 1.0f);
		const FLinearColor Accent(0.93f, 0.60f, 0.72f, 1.0f);
		const FLinearColor Detail(0.26f, 0.19f, 0.22f, 1.0f);

		AppendSpherePart(*Mesh, 22.0f, FVector(-2.0f, 0.0f, 36.0f), FRotator::ZeroRotator, Body, 6, 6, 6, FVector(1.4f, 1.0f, 1.0f));
		AppendSpherePart(*Mesh, 18.0f, FVector(34.0f, 0.0f, 40.0f), FRotator::ZeroRotator, Body, 6, 6, 6);
		AppendSpherePart(*Mesh, 8.5f, FVector(50.0f, 0.0f, 35.0f), FRotator::ZeroRotator, Accent, 5, 5, 5, FVector(1.2f, 1.0f, 0.8f));
		AppendConePart(*Mesh, 5.0f, 0.0f, 14.0f, FVector(30.0f, -9.0f, 58.0f), FRotator(18.0f, 0.0f, -20.0f), Body, 8);
		AppendConePart(*Mesh, 5.0f, 0.0f, 14.0f, FVector(30.0f, 9.0f, 58.0f), FRotator(-18.0f, 0.0f, 20.0f), Body, 8);
		AppendCylinderPart(*Mesh, 5.5f, 18.0f, FVector(-18.0f, -13.0f, 9.0f), FRotator::ZeroRotator, Detail, 8);
		AppendCylinderPart(*Mesh, 5.5f, 18.0f, FVector(-18.0f, 13.0f, 9.0f), FRotator::ZeroRotator, Detail, 8);
		AppendCylinderPart(*Mesh, 5.5f, 18.0f, FVector(12.0f, -13.0f, 9.0f), FRotator::ZeroRotator, Detail, 8);
		AppendCylinderPart(*Mesh, 5.5f, 18.0f, FVector(12.0f, 13.0f, 9.0f), FRotator::ZeroRotator, Detail, 8);
		AppendCylinderPart(*Mesh, 2.4f, 10.0f, FVector(-40.0f, 0.0f, 42.0f), FRotator(0.0f, 90.0f, 64.0f), Accent, 8);
		break;
	}
	}

	FinishMesh(*Mesh);
	return Mesh;
}
