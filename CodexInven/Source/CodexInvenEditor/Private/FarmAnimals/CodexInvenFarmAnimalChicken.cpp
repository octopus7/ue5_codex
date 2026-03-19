#include "FarmAnimals/CodexInvenFarmAnimalBuilders.h"

#include "FarmAnimals/CodexInvenFarmAnimalMeshUtilities.h"

UDynamicMesh* CodexInvenFarmAnimals::BuildChickenMesh(const ECodexInvenFarmAnimalStyle InStyle)
{
	using namespace CodexInvenFarmAnimalMeshUtilities;

	UDynamicMesh* const Mesh = MakeTransientDynamicMesh();

	switch (InStyle)
	{
	case ECodexInvenFarmAnimalStyle::Retro:
	{
		const FLinearColor Body(0.87f, 0.80f, 0.68f, 1.0f);
		const FLinearColor Comb(0.78f, 0.15f, 0.16f, 1.0f);
		const FLinearColor Beak(0.92f, 0.66f, 0.18f, 1.0f);

		AppendBoxPart(*Mesh, FVector(46.0f, 34.0f, 40.0f), FVector(0.0f, 0.0f, 30.0f), FRotator::ZeroRotator, Body);
		AppendBoxPart(*Mesh, FVector(20.0f, 18.0f, 18.0f), FVector(24.0f, 0.0f, 48.0f), FRotator::ZeroRotator, Body);
		AppendConePart(*Mesh, 7.0f, 0.0f, 18.0f, FVector(-24.0f, -8.0f, 44.0f), FRotator(-24.0f, 0.0f, 14.0f), Comb, 4);
		AppendConePart(*Mesh, 7.0f, 0.0f, 18.0f, FVector(-24.0f, 8.0f, 44.0f), FRotator(24.0f, 0.0f, -14.0f), Comb, 4);
		AppendBoxPart(*Mesh, FVector(12.0f, 8.0f, 6.0f), FVector(38.0f, 0.0f, 46.0f), FRotator::ZeroRotator, Beak);
		AppendBoxPart(*Mesh, FVector(10.0f, 4.0f, 20.0f), FVector(-8.0f, -15.0f, 32.0f), FRotator::ZeroRotator, Body);
		AppendBoxPart(*Mesh, FVector(10.0f, 4.0f, 20.0f), FVector(-8.0f, 15.0f, 32.0f), FRotator::ZeroRotator, Body);
		AppendCylinderPart(*Mesh, 2.0f, 24.0f, FVector(10.0f, -6.0f, 12.0f), FRotator::ZeroRotator, Beak, 6);
		AppendCylinderPart(*Mesh, 2.0f, 24.0f, FVector(10.0f, 6.0f, 12.0f), FRotator::ZeroRotator, Beak, 6);
		break;
	}

	case ECodexInvenFarmAnimalStyle::Refined:
	{
		const FLinearColor Body(0.92f, 0.85f, 0.72f, 1.0f);
		const FLinearColor Comb(0.82f, 0.18f, 0.18f, 1.0f);
		const FLinearColor Beak(0.96f, 0.72f, 0.24f, 1.0f);
		const FLinearColor Tail(0.58f, 0.30f, 0.18f, 1.0f);

		AppendSpherePart(*Mesh, 17.0f, FVector(-2.0f, 0.0f, 30.0f), FRotator::ZeroRotator, Body, 6, 6, 6, FVector(1.25f, 1.0f, 1.2f));
		AppendSpherePart(*Mesh, 10.0f, FVector(18.0f, 0.0f, 48.0f), FRotator::ZeroRotator, Body, 5, 5, 5);
		AppendConePart(*Mesh, 4.0f, 0.0f, 12.0f, FVector(28.0f, 0.0f, 45.0f), FRotator(0.0f, 90.0f, 0.0f), Beak, 8);
		AppendConePart(*Mesh, 6.0f, 0.0f, 16.0f, FVector(-22.0f, -8.0f, 45.0f), FRotator(-26.0f, -12.0f, 10.0f), Tail, 8);
		AppendConePart(*Mesh, 6.0f, 0.0f, 16.0f, FVector(-22.0f, 8.0f, 45.0f), FRotator(26.0f, -12.0f, -10.0f), Tail, 8);
		AppendConePart(*Mesh, 4.0f, 0.0f, 10.0f, FVector(14.0f, 0.0f, 61.0f), FRotator::ZeroRotator, Comb, 6);
		AppendConePart(*Mesh, 3.5f, 0.0f, 8.0f, FVector(18.0f, 0.0f, 63.0f), FRotator::ZeroRotator, Comb, 6);
		AppendCapsulePart(*Mesh, 2.0f, 18.0f, FVector(8.0f, -6.0f, 11.0f), FRotator::ZeroRotator, Beak, 4, 8, 1);
		AppendCapsulePart(*Mesh, 2.0f, 18.0f, FVector(8.0f, 6.0f, 11.0f), FRotator::ZeroRotator, Beak, 4, 8, 1);
		break;
	}

	case ECodexInvenFarmAnimalStyle::Toy:
	{
		const FLinearColor Body(0.97f, 0.90f, 0.74f, 1.0f);
		const FLinearColor Comb(0.88f, 0.20f, 0.20f, 1.0f);
		const FLinearColor Beak(1.0f, 0.76f, 0.24f, 1.0f);
		const FLinearColor Tail(0.63f, 0.34f, 0.22f, 1.0f);

		AppendSpherePart(*Mesh, 18.0f, FVector(-4.0f, 0.0f, 30.0f), FRotator::ZeroRotator, Body, 6, 6, 6, FVector(1.35f, 1.0f, 1.2f));
		AppendSpherePart(*Mesh, 12.0f, FVector(18.0f, 0.0f, 48.0f), FRotator::ZeroRotator, Body, 6, 6, 6);
		AppendBoxPart(*Mesh, FVector(12.0f, 8.0f, 8.0f), FVector(31.0f, 0.0f, 45.0f), FRotator::ZeroRotator, Beak);
		AppendConePart(*Mesh, 6.0f, 0.0f, 15.0f, FVector(-22.0f, -8.0f, 44.0f), FRotator(-26.0f, -10.0f, 12.0f), Tail, 8);
		AppendConePart(*Mesh, 6.0f, 0.0f, 15.0f, FVector(-22.0f, 8.0f, 44.0f), FRotator(26.0f, -10.0f, -12.0f), Tail, 8);
		AppendConePart(*Mesh, 5.0f, 0.0f, 10.0f, FVector(15.0f, 0.0f, 62.0f), FRotator::ZeroRotator, Comb, 6);
		AppendCylinderPart(*Mesh, 2.4f, 18.0f, FVector(6.0f, -7.0f, 11.0f), FRotator::ZeroRotator, Beak, 8);
		AppendCylinderPart(*Mesh, 2.4f, 18.0f, FVector(6.0f, 7.0f, 11.0f), FRotator::ZeroRotator, Beak, 8);
		break;
	}
	}

	FinishMesh(*Mesh);
	return Mesh;
}
