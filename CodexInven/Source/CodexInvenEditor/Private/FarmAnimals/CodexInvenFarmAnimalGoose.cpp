#include "FarmAnimals/CodexInvenFarmAnimalBuilders.h"

#include "FarmAnimals/CodexInvenFarmAnimalMeshUtilities.h"

UDynamicMesh* CodexInvenFarmAnimals::BuildGooseMesh(const ECodexInvenFarmAnimalStyle InStyle)
{
	using namespace CodexInvenFarmAnimalMeshUtilities;

	UDynamicMesh* const Mesh = MakeTransientDynamicMesh();

	switch (InStyle)
	{
	case ECodexInvenFarmAnimalStyle::Retro:
	{
		const FLinearColor Body(0.92f, 0.94f, 0.92f, 1.0f);
		const FLinearColor Beak(0.96f, 0.58f, 0.18f, 1.0f);
		const FLinearColor Wing(0.66f, 0.70f, 0.68f, 1.0f);

		AppendBoxPart(*Mesh, FVector(60.0f, 30.0f, 28.0f), FVector(-4.0f, 0.0f, 24.0f), FRotator::ZeroRotator, Body);
		AppendBoxPart(*Mesh, FVector(12.0f, 12.0f, 28.0f), FVector(18.0f, 0.0f, 48.0f), FRotator::ZeroRotator, Body);
		AppendBoxPart(*Mesh, FVector(18.0f, 18.0f, 18.0f), FVector(18.0f, 0.0f, 66.0f), FRotator::ZeroRotator, Body);
		AppendBoxPart(*Mesh, FVector(18.0f, 18.0f, 6.0f), FVector(34.0f, 0.0f, 62.0f), FRotator::ZeroRotator, Beak);
		AppendBoxPart(*Mesh, FVector(20.0f, 4.0f, 14.0f), FVector(-6.0f, -13.0f, 27.0f), FRotator::ZeroRotator, Wing);
		AppendBoxPart(*Mesh, FVector(20.0f, 4.0f, 14.0f), FVector(-6.0f, 13.0f, 27.0f), FRotator::ZeroRotator, Wing);
		AppendConePart(*Mesh, 6.0f, 0.0f, 10.0f, FVector(-34.0f, 0.0f, 30.0f), FRotator(0.0f, -90.0f, 0.0f), Body, 4);
		AppendCylinderPart(*Mesh, 2.4f, 22.0f, FVector(4.0f, -5.0f, 11.0f), FRotator::ZeroRotator, Beak, 6);
		AppendCylinderPart(*Mesh, 2.4f, 22.0f, FVector(4.0f, 5.0f, 11.0f), FRotator::ZeroRotator, Beak, 6);
		break;
	}

	case ECodexInvenFarmAnimalStyle::Refined:
	{
		const FLinearColor Body(0.96f, 0.97f, 0.95f, 1.0f);
		const FLinearColor Beak(0.98f, 0.62f, 0.22f, 1.0f);
		const FLinearColor Wing(0.72f, 0.74f, 0.72f, 1.0f);

		AppendCapsulePart(*Mesh, 14.0f, 40.0f, FVector(-6.0f, 0.0f, 27.0f), FRotator(0.0f, 90.0f, 0.0f), Body, 5, 10, 2, FVector(1.1f, 1.0f, 1.0f));
		AppendCapsulePart(*Mesh, 4.5f, 26.0f, FVector(16.0f, 0.0f, 50.0f), FRotator::ZeroRotator, Body, 4, 8, 1);
		AppendSpherePart(*Mesh, 10.0f, FVector(16.0f, 0.0f, 68.0f), FRotator::ZeroRotator, Body, 5, 5, 5);
		AppendBoxPart(*Mesh, FVector(16.0f, 20.0f, 6.0f), FVector(30.0f, 0.0f, 64.0f), FRotator::ZeroRotator, Beak);
		AppendSpherePart(*Mesh, 10.0f, FVector(-2.0f, -13.0f, 30.0f), FRotator::ZeroRotator, Wing, 5, 5, 5, FVector(1.2f, 0.4f, 1.0f));
		AppendSpherePart(*Mesh, 10.0f, FVector(-2.0f, 13.0f, 30.0f), FRotator::ZeroRotator, Wing, 5, 5, 5, FVector(1.2f, 0.4f, 1.0f));
		AppendConePart(*Mesh, 5.0f, 0.0f, 9.0f, FVector(-34.0f, 0.0f, 30.0f), FRotator(0.0f, -90.0f, 0.0f), Body, 8);
		AppendCylinderPart(*Mesh, 2.2f, 22.0f, FVector(2.0f, -5.0f, 11.0f), FRotator::ZeroRotator, Beak, 8);
		AppendCylinderPart(*Mesh, 2.2f, 22.0f, FVector(2.0f, 5.0f, 11.0f), FRotator::ZeroRotator, Beak, 8);
		break;
	}

	case ECodexInvenFarmAnimalStyle::Toy:
	{
		const FLinearColor Body(0.98f, 0.99f, 0.97f, 1.0f);
		const FLinearColor Beak(1.0f, 0.66f, 0.24f, 1.0f);
		const FLinearColor Wing(0.78f, 0.80f, 0.78f, 1.0f);

		AppendSpherePart(*Mesh, 18.0f, FVector(-6.0f, 0.0f, 28.0f), FRotator::ZeroRotator, Body, 6, 6, 6, FVector(1.35f, 1.0f, 1.0f));
		AppendCapsulePart(*Mesh, 5.0f, 28.0f, FVector(14.0f, 0.0f, 54.0f), FRotator::ZeroRotator, Body, 4, 8, 1);
		AppendSpherePart(*Mesh, 12.0f, FVector(14.0f, 0.0f, 74.0f), FRotator::ZeroRotator, Body, 6, 6, 6);
		AppendBoxPart(*Mesh, FVector(18.0f, 22.0f, 8.0f), FVector(32.0f, 0.0f, 69.0f), FRotator::ZeroRotator, Beak);
		AppendSpherePart(*Mesh, 8.0f, FVector(-4.0f, -15.0f, 29.0f), FRotator::ZeroRotator, Wing, 5, 5, 5, FVector(1.1f, 0.42f, 1.0f));
		AppendSpherePart(*Mesh, 8.0f, FVector(-4.0f, 15.0f, 29.0f), FRotator::ZeroRotator, Wing, 5, 5, 5, FVector(1.1f, 0.42f, 1.0f));
		AppendConePart(*Mesh, 5.0f, 0.0f, 10.0f, FVector(-34.0f, 0.0f, 30.0f), FRotator(0.0f, -90.0f, 0.0f), Body, 8);
		AppendCylinderPart(*Mesh, 2.6f, 20.0f, FVector(2.0f, -6.0f, 10.0f), FRotator::ZeroRotator, Beak, 8);
		AppendCylinderPart(*Mesh, 2.6f, 20.0f, FVector(2.0f, 6.0f, 10.0f), FRotator::ZeroRotator, Beak, 8);
		break;
	}
	}

	FinishMesh(*Mesh);
	return Mesh;
}
