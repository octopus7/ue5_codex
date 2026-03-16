// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "CodexInvenOwnershipComponent.h"
#include "CodexInvenPickupData.h"
#include "CodexInvenPickupSpawner.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCodexInvenPickupDefinitionAutomationTest,
	"CodexInven.Pickups.Definitions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCodexInvenPickupDefinitionAutomationTest::RunTest(const FString& Parameters)
{
	const TConstArrayView<ECodexInvenPickupType> PickupTypes = CodexInvenPickupData::GetAllPickupTypes();
	TestEqual(TEXT("All six pickup types are registered"), PickupTypes.Num(), 6);

	for (const ECodexInvenPickupType PickupType : PickupTypes)
	{
		const FCodexInvenPickupDefinition* Definition = CodexInvenPickupData::FindPickupDefinition(PickupType);
		TestNotNull(TEXT("Pickup definition exists"), Definition);

		if (Definition == nullptr)
		{
			continue;
		}

		TestFalse(TEXT("Display name is not empty"), Definition->DisplayName.IsEmpty());

		switch (PickupType)
		{
		case ECodexInvenPickupType::CubeRed:
		case ECodexInvenPickupType::CubeGreen:
		case ECodexInvenPickupType::CubeBlue:
			TestFalse(TEXT("Cube pickups are unique"), Definition->bStackable);
			TestEqual(TEXT("Cube pickups use cube mesh"), Definition->MeshKind, ECodexInvenPickupMeshKind::Cube);
			break;

		case ECodexInvenPickupType::CylinderRed:
		case ECodexInvenPickupType::CylinderGreen:
		case ECodexInvenPickupType::CylinderBlue:
			TestTrue(TEXT("Cylinder pickups are stackable"), Definition->bStackable);
			TestEqual(TEXT("Cylinder pickups use cylinder mesh"), Definition->MeshKind, ECodexInvenPickupMeshKind::Cylinder);
			break;
		}
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCodexInvenOwnershipComponentAutomationTest,
	"CodexInven.Pickups.Ownership",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCodexInvenOwnershipComponentAutomationTest::RunTest(const FString& Parameters)
{
	UCodexInvenOwnershipComponent* OwnershipComponent = NewObject<UCodexInvenOwnershipComponent>();
	TestNotNull(TEXT("Ownership component can be created"), OwnershipComponent);
	if (OwnershipComponent == nullptr)
	{
		return false;
	}

	TestTrue(TEXT("First cylinder pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CylinderRed));
	TestTrue(TEXT("Second cylinder pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CylinderRed));
	TestEqual(TEXT("Cylinder stack count increments"), OwnershipComponent->GetStackCount(ECodexInvenPickupType::CylinderRed), 2);
	TestEqual(TEXT("Stacked pickups do not create unique entries"), OwnershipComponent->GetUniquePickups().Num(), 0);

	TestTrue(TEXT("First cube pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CubeBlue));
	TestTrue(TEXT("Second cube pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CubeBlue));
	TestEqual(TEXT("Cube stack count remains zero"), OwnershipComponent->GetStackCount(ECodexInvenPickupType::CubeBlue), 0);
	TestEqual(TEXT("Unique pickups are stored individually"), OwnershipComponent->GetUniquePickups().Num(), 2);

	int32 CubeBlueCount = 0;
	TSet<int32> UniqueIds;
	for (const FCodexInvenOwnedUniquePickup& UniquePickup : OwnershipComponent->GetUniquePickups())
	{
		if (UniquePickup.Type == ECodexInvenPickupType::CubeBlue)
		{
			++CubeBlueCount;
			UniqueIds.Add(UniquePickup.InstanceId);
		}
	}

	TestEqual(TEXT("Cube entries are stored per pickup"), CubeBlueCount, 2);
	TestEqual(TEXT("Cube unique ids remain distinct"), UniqueIds.Num(), 2);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCodexInvenPickupSpawnerAutomationTest,
	"CodexInven.Pickups.Spawner",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCodexInvenPickupSpawnerAutomationTest::RunTest(const FString& Parameters)
{
	FRandomStream RandomStream(1337);

	for (int32 SampleIndex = 0; SampleIndex < 256; ++SampleIndex)
	{
		const FVector Offset = ACodexInvenPickupSpawner::SampleSpawnOffset2D(RandomStream, 200.0f, 600.0f);
		const float Radius = Offset.Size2D();

		TestTrue(TEXT("Spawn radius stays above minimum"), Radius >= 200.0f - KINDA_SMALL_NUMBER);
		TestTrue(TEXT("Spawn radius stays below maximum"), Radius <= 600.0f + KINDA_SMALL_NUMBER);
		TestTrue(TEXT("Spawn offset remains planar"), FMath::IsNearlyZero(Offset.Z));
	}

	TestTrue(TEXT("Spawner can add pickup below the cap"), ACodexInvenPickupSpawner::CanSpawnWithActiveCount(23, 24));
	TestFalse(TEXT("Spawner stops at the active pickup cap"), ACodexInvenPickupSpawner::CanSpawnWithActiveCount(24, 24));

	return true;
}

#endif
