// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "CodexInvenOwnershipComponent.h"
#include "CodexInvenGameInstance.h"
#include "CodexInvenInventoryIconSubsystem.h"
#include "CodexInvenPickupData.h"
#include "CodexInvenPickupSpawner.h"
#include "Engine/Texture2D.h"

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
	FCodexInvenInventorySnapshotAutomationTest,
	"CodexInven.Pickups.InventorySnapshot",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCodexInvenInventorySnapshotAutomationTest::RunTest(const FString& Parameters)
{
	UCodexInvenOwnershipComponent* OwnershipComponent = NewObject<UCodexInvenOwnershipComponent>();
	TestNotNull(TEXT("Ownership component can be created for snapshot"), OwnershipComponent);
	if (OwnershipComponent == nullptr)
	{
		return false;
	}

	TestEqual(TEXT("Empty snapshot starts with zero slots"), OwnershipComponent->BuildInventorySnapshot().Num(), 0);

	TestTrue(TEXT("First blue cube pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CubeBlue));
	TestTrue(TEXT("Second blue cube pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CubeBlue));
	TestTrue(TEXT("Red cube pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CubeRed));
	TestTrue(TEXT("First green cylinder pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CylinderGreen));
	TestTrue(TEXT("Second green cylinder pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CylinderGreen));

	const TArray<FCodexInvenInventorySlotData> Snapshot = OwnershipComponent->BuildInventorySnapshot();
	TestEqual(TEXT("Snapshot expands unique items and collapses stacks"), Snapshot.Num(), 4);
	if (Snapshot.Num() != 4)
	{
		return false;
	}

	TestEqual(TEXT("Slot 0 follows pickup enum order"), Snapshot[0].PickupType, ECodexInvenPickupType::CubeRed);
	TestFalse(TEXT("Cube red slot is unique"), Snapshot[0].bStackable);
	TestEqual(TEXT("Cube red keeps its unique id"), Snapshot[0].UniqueInstanceId, 3);

	TestEqual(TEXT("Slot 1 is first blue cube"), Snapshot[1].PickupType, ECodexInvenPickupType::CubeBlue);
	TestEqual(TEXT("First blue cube stays in instance id order"), Snapshot[1].UniqueInstanceId, 1);
	TestEqual(TEXT("Slot 2 is second blue cube"), Snapshot[2].PickupType, ECodexInvenPickupType::CubeBlue);
	TestEqual(TEXT("Second blue cube stays in instance id order"), Snapshot[2].UniqueInstanceId, 2);

	TestEqual(TEXT("Slot 3 collapses green cylinders"), Snapshot[3].PickupType, ECodexInvenPickupType::CylinderGreen);
	TestTrue(TEXT("Green cylinder slot is stackable"), Snapshot[3].bStackable);
	TestEqual(TEXT("Green cylinder slot stores stack quantity"), Snapshot[3].Quantity, 2);
	TestEqual(TEXT("Stacked slots do not carry unique ids"), Snapshot[3].UniqueInstanceId, INDEX_NONE);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCodexInvenOwnershipDebugTextAutomationTest,
	"CodexInven.Pickups.OwnershipDebugText",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCodexInvenOwnershipDebugTextAutomationTest::RunTest(const FString& Parameters)
{
	UCodexInvenOwnershipComponent* OwnershipComponent = NewObject<UCodexInvenOwnershipComponent>();
	TestNotNull(TEXT("Ownership component can be created for debug text"), OwnershipComponent);
	if (OwnershipComponent == nullptr)
	{
		return false;
	}

	const FString EmptyText = OwnershipComponent->BuildDebugOwnershipText().ToString();
	TestTrue(TEXT("Empty text includes header"), EmptyText.Contains(TEXT("Ownership Debug")));
	TestTrue(TEXT("Empty text includes zero cylinder count"), EmptyText.Contains(TEXT("Cylinder Red: 0")));
	TestTrue(TEXT("Empty text includes empty cube entry"), EmptyText.Contains(TEXT("Cube Blue: None")));
	TestTrue(TEXT("Empty text includes zero totals"), EmptyText.Contains(TEXT("Stacked Items: 0")) && EmptyText.Contains(TEXT("Unique Items: 0")));

	TestTrue(TEXT("Cylinder green pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CylinderGreen));
	TestTrue(TEXT("Second cylinder green pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CylinderGreen));
	TestTrue(TEXT("First cube blue pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CubeBlue));
	TestTrue(TEXT("Second cube blue pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CubeBlue));

	const FString PopulatedText = OwnershipComponent->BuildDebugOwnershipText().ToString();
	TestTrue(TEXT("Populated text includes cylinder stack"), PopulatedText.Contains(TEXT("Cylinder Green: 2")));
	TestTrue(TEXT("Populated text includes cube instance list"), PopulatedText.Contains(TEXT("Cube Blue: #1, #2")));
	TestTrue(TEXT("Populated text includes updated totals"), PopulatedText.Contains(TEXT("Stacked Items: 2")) && PopulatedText.Contains(TEXT("Unique Items: 2")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCodexInvenInventoryIconSubsystemAutomationTest,
	"CodexInven.Pickups.InventoryIcons",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCodexInvenInventoryIconSubsystemAutomationTest::RunTest(const FString& Parameters)
{
	UCodexInvenGameInstance* GameInstance = NewObject<UCodexInvenGameInstance>();
	TestNotNull(TEXT("Game instance can be created for icon subsystem"), GameInstance);
	if (GameInstance == nullptr)
	{
		return false;
	}

	GameInstance->Init();

	UCodexInvenInventoryIconSubsystem* IconSubsystem = GameInstance->GetSubsystem<UCodexInvenInventoryIconSubsystem>();
	TestNotNull(TEXT("Inventory icon subsystem initializes"), IconSubsystem);
	if (IconSubsystem == nullptr)
	{
		GameInstance->Shutdown();
		return false;
	}

	for (const ECodexInvenPickupType PickupType : CodexInvenPickupData::GetAllPickupTypes())
	{
		UTexture2D* IconTexture = IconSubsystem->GetInventoryIcon(PickupType);
		TestNotNull(TEXT("Inventory icon exists"), IconTexture);

		if (IconTexture == nullptr)
		{
			continue;
		}

		TestEqual(TEXT("Inventory icon width is 64"), IconTexture->GetSizeX(), 64);
		TestEqual(TEXT("Inventory icon height is 64"), IconTexture->GetSizeY(), 64);
	}

	GameInstance->Shutdown();
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
