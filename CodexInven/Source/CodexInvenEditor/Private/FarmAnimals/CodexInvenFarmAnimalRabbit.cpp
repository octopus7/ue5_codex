#include "FarmAnimals/CodexInvenFarmAnimalBuilders.h"

#include "FarmAnimals/CodexInvenFarmAnimalMeshUtilities.h"

UDynamicMesh* CodexInvenFarmAnimals::BuildRabbitMesh(const ECodexInvenFarmAnimalStyle InStyle)
{
	using namespace CodexInvenFarmAnimalMeshUtilities;

	UDynamicMesh* const Mesh = MakeTransientDynamicMesh();

	switch (InStyle)
	{
	case ECodexInvenFarmAnimalStyle::Retro:
	{
		const FLinearColor Body(0.78f, 0.76f, 0.78f, 1.0f);
		const FLinearColor Accent(0.92f, 0.82f, 0.86f, 1.0f);
		const FLinearColor Detail(0.22f, 0.20f, 0.22f, 1.0f);

		AppendBoxPart(*Mesh, FVector(54.0f, 30.0f, 28.0f), FVector(-2.0f, 0.0f, 24.0f), FRotator::ZeroRotator, Body);
		AppendBoxPart(*Mesh, FVector(22.0f, 20.0f, 20.0f), FVector(24.0f, 0.0f, 38.0f), FRotator::ZeroRotator, Body);
		AppendBoxPart(*Mesh, FVector(8.0f, 8.0f, 28.0f), FVector(24.0f, -7.0f, 62.0f), FRotator::ZeroRotator, Accent);
		AppendBoxPart(*Mesh, FVector(8.0f, 8.0f, 28.0f), FVector(24.0f, 7.0f, 62.0f), FRotator::ZeroRotator, Accent);
		AppendBoxPart(*Mesh, FVector(12.0f, 8.0f, 10.0f), FVector(-22.0f, -8.0f, 12.0f), FRotator::ZeroRotator, Detail);
		AppendBoxPart(*Mesh, FVector(12.0f, 8.0f, 10.0f), FVector(-22.0f, 8.0f, 12.0f), FRotator::ZeroRotator, Detail);
		AppendBoxPart(*Mesh, FVector(10.0f, 8.0f, 16.0f), FVector(6.0f, -8.0f, 10.0f), FRotator::ZeroRotator, Detail);
		AppendBoxPart(*Mesh, FVector(10.0f, 8.0f, 16.0f), FVector(6.0f, 8.0f, 10.0f), FRotator::ZeroRotator, Detail);
		AppendBoxPart(*Mesh, FVector(10.0f, 10.0f, 10.0f), FVector(-30.0f, 0.0f, 34.0f), FRotator::ZeroRotator, Accent);
		break;
	}

	case ECodexInvenFarmAnimalStyle::Refined:
	{
		const FLinearColor Body(0.82f, 0.80f, 0.82f, 1.0f);
		const FLinearColor Accent(0.95f, 0.86f, 0.90f, 1.0f);
		const FLinearColor Detail(0.24f, 0.22f, 0.24f, 1.0f);

		AppendCapsulePart(*Mesh, 12.0f, 34.0f, FVector(-6.0f, 0.0f, 26.0f), FRotator(0.0f, 90.0f, 0.0f), Body, 5, 10, 2, FVector(1.05f, 1.0f, 1.0f));
		AppendSpherePart(*Mesh, 12.0f, FVector(22.0f, 0.0f, 40.0f), FRotator::ZeroRotator, Body, 5, 5, 5);
		AppendCapsulePart(*Mesh, 3.0f, 20.0f, FVector(20.0f, -7.0f, 62.0f), FRotator::ZeroRotator, Accent, 4, 8, 1);
		AppendCapsulePart(*Mesh, 3.0f, 20.0f, FVector(20.0f, 7.0f, 62.0f), FRotator::ZeroRotator, Accent, 4, 8, 1);
		AppendCapsulePart(*Mesh, 3.0f, 18.0f, FVector(-18.0f, -8.0f, 11.0f), FRotator(10.0f, 0.0f, 0.0f), Detail, 4, 8, 1);
		AppendCapsulePart(*Mesh, 3.0f, 18.0f, FVector(-18.0f, 8.0f, 11.0f), FRotator(10.0f, 0.0f, 0.0f), Detail, 4, 8, 1);
		AppendCapsulePart(*Mesh, 3.4f, 14.0f, FVector(8.0f, -8.0f, 10.0f), FRotator(-6.0f, 0.0f, 0.0f), Detail, 4, 8, 1);
		AppendCapsulePart(*Mesh, 3.4f, 14.0f, FVector(8.0f, 8.0f, 10.0f), FRotator(-6.0f, 0.0f, 0.0f), Detail, 4, 8, 1);
		AppendSpherePart(*Mesh, 6.0f, FVector(-30.0f, 0.0f, 34.0f), FRotator::ZeroRotator, Accent, 5, 5, 5);
		break;
	}

	case ECodexInvenFarmAnimalStyle::Toy:
	{
		const FLinearColor Body(0.88f, 0.86f, 0.90f, 1.0f);
		const FLinearColor Accent(0.98f, 0.90f, 0.94f, 1.0f);
		const FLinearColor Detail(0.28f, 0.24f, 0.28f, 1.0f);

		AppendSpherePart(*Mesh, 16.0f, FVector(-6.0f, 0.0f, 28.0f), FRotator::ZeroRotator, Body, 6, 6, 6, FVector(1.4f, 1.0f, 1.0f));
		AppendSpherePart(*Mesh, 14.0f, FVector(20.0f, 0.0f, 42.0f), FRotator::ZeroRotator, Body, 6, 6, 6);
		AppendCapsulePart(*Mesh, 3.2f, 22.0f, FVector(18.0f, -8.0f, 66.0f), FRotator::ZeroRotator, Accent, 4, 8, 1);
		AppendCapsulePart(*Mesh, 3.2f, 22.0f, FVector(18.0f, 8.0f, 66.0f), FRotator::ZeroRotator, Accent, 4, 8, 1);
		AppendCapsulePart(*Mesh, 3.4f, 16.0f, FVector(-18.0f, -8.0f, 11.0f), FRotator(12.0f, 0.0f, 0.0f), Detail, 4, 8, 1);
		AppendCapsulePart(*Mesh, 3.4f, 16.0f, FVector(-18.0f, 8.0f, 11.0f), FRotator(12.0f, 0.0f, 0.0f), Detail, 4, 8, 1);
		AppendCapsulePart(*Mesh, 3.6f, 12.0f, FVector(6.0f, -8.0f, 10.0f), FRotator(-6.0f, 0.0f, 0.0f), Detail, 4, 8, 1);
		AppendCapsulePart(*Mesh, 3.6f, 12.0f, FVector(6.0f, 8.0f, 10.0f), FRotator(-6.0f, 0.0f, 0.0f), Detail, 4, 8, 1);
		AppendSpherePart(*Mesh, 6.0f, FVector(-32.0f, 0.0f, 36.0f), FRotator::ZeroRotator, Accent, 5, 5, 5);
		break;
	}
	}

	FinishMesh(*Mesh);
	return Mesh;
}
