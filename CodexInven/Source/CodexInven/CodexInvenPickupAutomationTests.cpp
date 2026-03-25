// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "CodexInvenOwnershipComponent.h"
#include "CodexInvenGameInstance.h"
#include "CodexInvenInventoryIconSubsystem.h"
#include "CodexInvenPickupData.h"
#include "CodexInvenPickupSpawner.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCodexInvenPickupDefinitionAutomationTest,
	"CodexInven.Pickups.Definitions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCodexInvenPickupDefinitionAutomationTest::RunTest(const FString& Parameters)
{
	const TConstArrayView<ECodexInvenPickupType> PickupTypes = CodexInvenPickupData::GetAllPickupTypes();
	TestEqual(TEXT("All nine pickup types are registered"), PickupTypes.Num(), 9);

	for (const ECodexInvenPickupType PickupType : PickupTypes)
	{
		const FCodexInvenPickupDefinition* Definition = CodexInvenPickupData::FindPickupDefinition(PickupType);
		TestNotNull(TEXT("Pickup definition exists"), Definition);

		if (Definition == nullptr)
		{
			continue;
		}

		TestFalse(TEXT("Display name is not empty"), Definition->DisplayName.IsEmpty());
		TestTrue(TEXT("Pickup mesh path is configured"), Definition->WorldMesh.ToSoftObjectPath().IsValid());
		TestTrue(TEXT("Pickup icon path is configured"), Definition->InventoryIcon.ToSoftObjectPath().IsValid());
		TestNotNull(TEXT("Pickup mesh asset exists"), Definition->WorldMesh.LoadSynchronous());
		TestNotNull(TEXT("Pickup icon asset exists"), Definition->InventoryIcon.LoadSynchronous());

		switch (PickupType)
		{
		case ECodexInvenPickupType::CubeRed:
		case ECodexInvenPickupType::CubeGreen:
		case ECodexInvenPickupType::CubeBlue:
			TestEqual(TEXT("Common cubes use common rarity"), Definition->Rarity, ECodexInvenPickupRarity::Common);
			TestFalse(TEXT("Common cubes do not use metallic material"), Definition->bUseMetallicMaterial);
			TestFalse(TEXT("Cube pickups are unique"), Definition->bStackable);
			TestEqual(TEXT("Cube pickups use cube mesh"), Definition->MeshKind, ECodexInvenPickupMeshKind::Cube);
			break;

		case ECodexInvenPickupType::CubeGold:
			TestEqual(TEXT("Gold cube uses gold rarity"), Definition->Rarity, ECodexInvenPickupRarity::Gold);
			TestTrue(TEXT("Gold cube uses metallic material"), Definition->bUseMetallicMaterial);
			TestFalse(TEXT("Cube pickups are unique"), Definition->bStackable);
			TestEqual(TEXT("Cube pickups use cube mesh"), Definition->MeshKind, ECodexInvenPickupMeshKind::Cube);
			break;

		case ECodexInvenPickupType::CylinderRed:
		case ECodexInvenPickupType::CylinderGreen:
		case ECodexInvenPickupType::CylinderBlue:
			TestEqual(TEXT("Common cylinders use common rarity"), Definition->Rarity, ECodexInvenPickupRarity::Common);
			TestFalse(TEXT("Common cylinders do not use metallic material"), Definition->bUseMetallicMaterial);
			TestTrue(TEXT("Cylinder pickups are stackable"), Definition->bStackable);
			TestEqual(TEXT("Cylinder pickups use cylinder mesh"), Definition->MeshKind, ECodexInvenPickupMeshKind::Cylinder);
			break;

		case ECodexInvenPickupType::CylinderGold:
			TestEqual(TEXT("Gold cylinder uses gold rarity"), Definition->Rarity, ECodexInvenPickupRarity::Gold);
			TestTrue(TEXT("Gold cylinder uses metallic material"), Definition->bUseMetallicMaterial);
			TestTrue(TEXT("Cylinder pickups are stackable"), Definition->bStackable);
			TestEqual(TEXT("Cylinder pickups use cylinder mesh"), Definition->MeshKind, ECodexInvenPickupMeshKind::Cylinder);
			break;

		case ECodexInvenPickupType::Key:
			TestEqual(TEXT("Key uses gold rarity"), Definition->Rarity, ECodexInvenPickupRarity::Gold);
			TestTrue(TEXT("Key uses metallic material"), Definition->bUseMetallicMaterial);
			TestFalse(TEXT("Key remains unique"), Definition->bStackable);
			TestEqual(TEXT("Key uses key mesh"), Definition->MeshKind, ECodexInvenPickupMeshKind::Key);
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

	TestEqual(TEXT("Default inventory capacity starts at 100"), OwnershipComponent->GetInventoryCapacity(), 100);
	TestEqual(TEXT("Default inventory starts empty"), OwnershipComponent->GetOccupiedSlotCount(), 0);

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

	const TArray<FCodexInvenInventorySlotData> EmptySnapshot = OwnershipComponent->BuildInventorySnapshot();
	TestEqual(TEXT("Empty snapshot contains the default 100 slots"), EmptySnapshot.Num(), 100);
	TestTrue(TEXT("First snapshot slot starts empty"), EmptySnapshot[0].bIsEmpty);

	TestTrue(TEXT("First green cylinder pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CylinderGreen));
	TestTrue(TEXT("Second green cylinder pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CylinderGreen));
	TestTrue(TEXT("First blue cube pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CubeBlue));
	TestTrue(TEXT("Second blue cube pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CubeBlue));
	TestTrue(TEXT("Red cube pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CubeRed));

	const TArray<FCodexInvenInventorySlotData> Snapshot = OwnershipComponent->BuildInventorySnapshot();
	TestEqual(TEXT("Snapshot keeps full capacity with empty slots"), Snapshot.Num(), 100);
	if (Snapshot.Num() != 100)
	{
		return false;
	}

	TestFalse(TEXT("Slot 0 is now occupied"), Snapshot[0].bIsEmpty);
	TestEqual(TEXT("First slot keeps the first stackable pickup"), Snapshot[0].PickupType, ECodexInvenPickupType::CylinderGreen);
	TestTrue(TEXT("First slot is stackable"), Snapshot[0].bStackable);
	TestEqual(TEXT("First slot stack quantity increments in place"), Snapshot[0].Quantity, 2);

	TestEqual(TEXT("Slot 1 is first blue cube"), Snapshot[1].PickupType, ECodexInvenPickupType::CubeBlue);
	TestEqual(TEXT("Slot 1 keeps the first blue cube id"), Snapshot[1].UniqueInstanceId, 1);
	TestEqual(TEXT("Slot 2 is second blue cube"), Snapshot[2].PickupType, ECodexInvenPickupType::CubeBlue);
	TestEqual(TEXT("Slot 2 keeps the second blue cube id"), Snapshot[2].UniqueInstanceId, 2);
	TestEqual(TEXT("Slot 3 is red cube"), Snapshot[3].PickupType, ECodexInvenPickupType::CubeRed);
	TestEqual(TEXT("Slot 3 keeps the red cube id"), Snapshot[3].UniqueInstanceId, 3);
	TestTrue(TEXT("Next slot remains empty"), Snapshot[4].bIsEmpty);

	TestTrue(TEXT("Clearing slot 1 succeeds"), OwnershipComponent->ClearInventorySlot(1));
	TestTrue(TEXT("New cube pickup refills the first empty slot"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CubeGreen));

	const TArray<FCodexInvenInventorySlotData> RefilledSnapshot = OwnershipComponent->BuildInventorySnapshot();
	TestEqual(TEXT("First empty slot is reused"), RefilledSnapshot[1].PickupType, ECodexInvenPickupType::CubeGreen);
	TestEqual(TEXT("Refilled slot gets a new unique id"), RefilledSnapshot[1].UniqueInstanceId, 4);

	TestTrue(TEXT("Gold cylinder pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CylinderGold));
	TestTrue(TEXT("Gold cube pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CubeGold));

	const TArray<FCodexInvenInventorySlotData> GoldSnapshot = OwnershipComponent->BuildInventorySnapshot();
	TestEqual(TEXT("Gold cylinder slot uses gold rarity"), GoldSnapshot[4].Rarity, ECodexInvenPickupRarity::Gold);
	TestTrue(TEXT("Gold cylinder remains stackable"), GoldSnapshot[4].bStackable);
	TestEqual(TEXT("Gold cube slot uses gold rarity"), GoldSnapshot[5].Rarity, ECodexInvenPickupRarity::Gold);
	TestFalse(TEXT("Gold cube remains unique"), GoldSnapshot[5].bStackable);

	TestTrue(TEXT("Increasing inventory capacity succeeds"), OwnershipComponent->IncreaseInventoryCapacity(10));
	TestEqual(TEXT("Inventory capacity increases by 10"), OwnershipComponent->GetInventoryCapacity(), 110);
	TestEqual(TEXT("Snapshot length tracks expanded capacity"), OwnershipComponent->BuildInventorySnapshot().Num(), 110);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCodexInvenInventorySlotMutationAutomationTest,
	"CodexInven.Pickups.InventorySlotMutation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCodexInvenInventorySlotMutationAutomationTest::RunTest(const FString& Parameters)
{
	UCodexInvenOwnershipComponent* OwnershipComponent = NewObject<UCodexInvenOwnershipComponent>();
	TestNotNull(TEXT("Ownership component can be created for slot mutation"), OwnershipComponent);
	if (OwnershipComponent == nullptr)
	{
		return false;
	}

	TestTrue(TEXT("First cube pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CubeRed));
	TestTrue(TEXT("Second cube pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CubeBlue));
	TestTrue(TEXT("First cylinder pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CylinderGreen));

	TestFalse(TEXT("Move rejects same source and target slot"), OwnershipComponent->MoveInventorySlot(0, 0));
	TestFalse(TEXT("Swap rejects same source and target slot"), OwnershipComponent->SwapInventorySlots(1, 1));
	TestFalse(TEXT("Move rejects invalid source slot"), OwnershipComponent->MoveInventorySlot(-1, 2));
	TestFalse(TEXT("Swap rejects invalid target slot"), OwnershipComponent->SwapInventorySlots(0, 100));

	TestTrue(TEXT("Move into an empty slot succeeds"), OwnershipComponent->MoveInventorySlot(1, 5));

	TArray<FCodexInvenInventorySlotData> Snapshot = OwnershipComponent->BuildInventorySnapshot();
	TestEqual(TEXT("Source slot is empty after move"), Snapshot[1].bIsEmpty, true);
	TestEqual(TEXT("Target slot now contains moved cube"), Snapshot[5].PickupType, ECodexInvenPickupType::CubeBlue);
	TestEqual(TEXT("Moved cube keeps its unique id"), Snapshot[5].UniqueInstanceId, 2);

	TestTrue(TEXT("Swap between occupied slots succeeds"), OwnershipComponent->SwapInventorySlots(0, 2));

	Snapshot = OwnershipComponent->BuildInventorySnapshot();
	TestEqual(TEXT("Slot 0 now contains the cylinder"), Snapshot[0].PickupType, ECodexInvenPickupType::CylinderGreen);
	TestTrue(TEXT("Cylinder stack quantity is preserved across swap"), Snapshot[0].bStackable && Snapshot[0].Quantity == 1);
	TestEqual(TEXT("Slot 2 now contains the red cube"), Snapshot[2].PickupType, ECodexInvenPickupType::CubeRed);
	TestEqual(TEXT("Red cube keeps its unique id across swap"), Snapshot[2].UniqueInstanceId, 1);
	TestTrue(TEXT("Moved blue cube remains in slot 5"), !Snapshot[5].bIsEmpty && Snapshot[5].PickupType == ECodexInvenPickupType::CubeBlue && Snapshot[5].UniqueInstanceId == 2);

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
	TestTrue(TEXT("Empty text includes consumables header"), EmptyText.Contains(TEXT("Consumables")));
	TestTrue(TEXT("Empty text includes equipment header"), EmptyText.Contains(TEXT("Equipment")));
	TestTrue(TEXT("Empty text includes zero medkit count"), EmptyText.Contains(TEXT("Medkit Pouch: 0")));
	TestTrue(TEXT("Empty text includes zero pill bottle count"), EmptyText.Contains(TEXT("Pill Bottle: 0")));
	TestTrue(TEXT("Empty text includes empty backpack entry"), EmptyText.Contains(TEXT("Utility Backpack: None")));
	TestTrue(TEXT("Empty text includes empty tool case entry"), EmptyText.Contains(TEXT("Tool Case: None")));
	TestTrue(TEXT("Empty text includes zero totals"), EmptyText.Contains(TEXT("Stacked Items: 0")) && EmptyText.Contains(TEXT("Unique Items: 0")));

	TestTrue(TEXT("Cylinder green pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CylinderGreen));
	TestTrue(TEXT("Second cylinder green pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CylinderGreen));
	TestTrue(TEXT("First cube blue pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CubeBlue));
	TestTrue(TEXT("Second cube blue pickup is added"), OwnershipComponent->AddPickup(ECodexInvenPickupType::CubeBlue));

	const FString PopulatedText = OwnershipComponent->BuildDebugOwnershipText().ToString();
	TestTrue(TEXT("Populated text includes energy drink stack"), PopulatedText.Contains(TEXT("Energy Drink Can: 2")));
	TestTrue(TEXT("Populated text includes backpack instance list"), PopulatedText.Contains(TEXT("Utility Backpack: #1, #2")));
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

		const FIntPoint ImportedSize = IconTexture->GetImportedSize();
		TestEqual(TEXT("Inventory icon width is 64"), ImportedSize.X, 64);
		TestEqual(TEXT("Inventory icon height is 64"), ImportedSize.Y, 64);
		TestTrue(TEXT("Inventory icon is a saved asset"), IconTexture->GetPathName().StartsWith(TEXT("/Game/Art/Pickups/Icons/")));
	}

	UTexture2D* GoldBackgroundTexture = IconSubsystem->GetInventorySlotBackground(ECodexInvenPickupRarity::Gold);
	TestNotNull(TEXT("Gold inventory slot background exists"), GoldBackgroundTexture);
	if (GoldBackgroundTexture != nullptr)
	{
		TestEqual(TEXT("Gold slot background width is 64"), GoldBackgroundTexture->GetSizeX(), 64);
		TestEqual(TEXT("Gold slot background height is 64"), GoldBackgroundTexture->GetSizeY(), 64);
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
