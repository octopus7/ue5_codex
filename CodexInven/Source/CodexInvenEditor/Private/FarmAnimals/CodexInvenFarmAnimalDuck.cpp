#include "FarmAnimals/CodexInvenFarmAnimalBuilders.h"

#include "FarmAnimals/CodexInvenFarmAnimalMeshUtilities.h"

UDynamicMesh* CodexInvenFarmAnimals::BuildDuckMesh(const ECodexInvenFarmAnimalStyle InStyle)
{
	using namespace CodexInvenFarmAnimalMeshUtilities;

	UDynamicMesh* const Mesh = MakeTransientDynamicMesh();

	switch (InStyle)
	{
	case ECodexInvenFarmAnimalStyle::Retro:
	{
		const FLinearColor Body(0.86f, 0.88f, 0.78f, 1.0f);
		const FLinearColor Bill(0.94f, 0.70f, 0.20f, 1.0f);
		const FLinearColor Detail(0.20f, 0.18f, 0.16f, 1.0f);

		AppendBoxPart(*Mesh, FVector(56.0f, 32.0f, 28.0f), FVector(-2.0f, 0.0f, 24.0f), FRotator::ZeroRotator, Body);
		AppendBoxPart(*Mesh, FVector(22.0f, 18.0f, 18.0f), FVector(30.0f, 0.0f, 40.0f), FRotator::ZeroRotator, Body);
		AppendBoxPart(*Mesh, FVector(16.0f, 18.0f, 6.0f), FVector(46.0f, 0.0f, 36.0f), FRotator::ZeroRotator, Bill);
		AppendBoxPart(*Mesh, FVector(18.0f, 4.0f, 14.0f), FVector(-6.0f, -14.0f, 27.0f), FRotator::ZeroRotator, Detail);
		AppendBoxPart(*Mesh, FVector(18.0f, 4.0f, 14.0f), FVector(-6.0f, 14.0f, 27.0f), FRotator::ZeroRotator, Detail);
		AppendConePart(*Mesh, 7.0f, 0.0f, 12.0f, FVector(-30.0f, 0.0f, 32.0f), FRotator(0.0f, -90.0f, 0.0f), Body, 4);
		AppendCylinderPart(*Mesh, 2.4f, 20.0f, FVector(8.0f, -5.0f, 10.0f), FRotator::ZeroRotator, Bill, 6);
		AppendCylinderPart(*Mesh, 2.4f, 20.0f, FVector(8.0f, 5.0f, 10.0f), FRotator::ZeroRotator, Bill, 6);
		break;
	}

	case ECodexInvenFarmAnimalStyle::Refined:
	{
		const FLinearColor Body(0.92f, 0.94f, 0.86f, 1.0f);
		const FLinearColor Bill(0.96f, 0.72f, 0.22f, 1.0f);
		const FLinearColor Wing(0.68f, 0.74f, 0.64f, 1.0f);

		AppendCapsulePart(*Mesh, 14.0f, 36.0f, FVector(-4.0f, 0.0f, 27.0f), FRotator(0.0f, 90.0f, 0.0f), Body, 5, 10, 2, FVector(1.15f, 1.0f, 1.0f));
		AppendCapsulePart(*Mesh, 5.0f, 18.0f, FVector(20.0f, 0.0f, 38.0f), FRotator::ZeroRotator, Body, 4, 8, 1);
		AppendSpherePart(*Mesh, 11.0f, FVector(20.0f, 0.0f, 52.0f), FRotator::ZeroRotator, Body, 5, 5, 5);
		AppendBoxPart(*Mesh, FVector(14.0f, 20.0f, 6.0f), FVector(33.0f, 0.0f, 47.0f), FRotator::ZeroRotator, Bill);
		AppendSpherePart(*Mesh, 9.0f, FVector(-2.0f, -13.0f, 29.0f), FRotator::ZeroRotator, Wing, 5, 5, 5, FVector(1.1f, 0.45f, 1.0f));
		AppendSpherePart(*Mesh, 9.0f, FVector(-2.0f, 13.0f, 29.0f), FRotator::ZeroRotator, Wing, 5, 5, 5, FVector(1.1f, 0.45f, 1.0f));
		AppendConePart(*Mesh, 6.0f, 0.0f, 10.0f, FVector(-30.0f, 0.0f, 32.0f), FRotator(0.0f, -90.0f, 0.0f), Body, 8);
		AppendCylinderPart(*Mesh, 2.2f, 20.0f, FVector(6.0f, -5.0f, 10.0f), FRotator::ZeroRotator, Bill, 8);
		AppendCylinderPart(*Mesh, 2.2f, 20.0f, FVector(6.0f, 5.0f, 10.0f), FRotator::ZeroRotator, Bill, 8);
		break;
	}

	case ECodexInvenFarmAnimalStyle::Toy:
	{
		const FLinearColor Body(0.96f, 0.98f, 0.88f, 1.0f);
		const FLinearColor Bill(1.0f, 0.78f, 0.24f, 1.0f);
		const FLinearColor Wing(0.74f, 0.78f, 0.68f, 1.0f);

		AppendSpherePart(*Mesh, 18.0f, FVector(-4.0f, 0.0f, 28.0f), FRotator::ZeroRotator, Body, 6, 6, 6, FVector(1.4f, 1.0f, 1.0f));
		AppendSpherePart(*Mesh, 13.0f, FVector(22.0f, 0.0f, 50.0f), FRotator::ZeroRotator, Body, 6, 6, 6);
		AppendBoxPart(*Mesh, FVector(18.0f, 22.0f, 8.0f), FVector(39.0f, 0.0f, 45.0f), FRotator::ZeroRotator, Bill);
		AppendSpherePart(*Mesh, 8.0f, FVector(-2.0f, -15.0f, 28.0f), FRotator::ZeroRotator, Wing, 5, 5, 5, FVector(1.1f, 0.45f, 1.0f));
		AppendSpherePart(*Mesh, 8.0f, FVector(-2.0f, 15.0f, 28.0f), FRotator::ZeroRotator, Wing, 5, 5, 5, FVector(1.1f, 0.45f, 1.0f));
		AppendConePart(*Mesh, 5.0f, 0.0f, 11.0f, FVector(-30.0f, 0.0f, 34.0f), FRotator(0.0f, -90.0f, 0.0f), Body, 8);
		AppendCylinderPart(*Mesh, 2.8f, 18.0f, FVector(4.0f, -6.0f, 9.0f), FRotator::ZeroRotator, Bill, 8);
		AppendCylinderPart(*Mesh, 2.8f, 18.0f, FVector(4.0f, 6.0f, 9.0f), FRotator::ZeroRotator, Bill, 8);
		break;
	}
	}

	FinishMesh(*Mesh);
	return Mesh;
}
