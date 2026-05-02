// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "TigerCurrencyComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTigerCurrencyComponentTest,
	"CodexGoal.TigerShop.CurrencyUpgrade",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTigerCurrencyComponentTest::RunTest(const FString& Parameters)
{
	UTigerCurrencyComponent* Currency = NewObject<UTigerCurrencyComponent>();
	TestNotNull(TEXT("Currency component can be created"), Currency);

	TestEqual(TEXT("Initial rice cake count"), Currency->GetRiceCakeCount(), static_cast<int64>(0));
	TestEqual(TEXT("Initial click power"), Currency->GetClickPower(), 1);
	TestEqual(TEXT("Initial upgrade cost"), Currency->GetNextClickUpgradeCost(), static_cast<int64>(10));

	Currency->AddRiceCakes(9);
	TestEqual(TEXT("Click production adds rice cakes"), Currency->GetRiceCakeCount(), static_cast<int64>(9));
	TestFalse(TEXT("Upgrade is unavailable before cost is met"), Currency->CanBuyClickUpgrade());

	Currency->AddRiceCakes(1);
	TestTrue(TEXT("Upgrade is available at cost"), Currency->CanBuyClickUpgrade());
	TestTrue(TEXT("Upgrade purchase succeeds"), Currency->TryBuyClickUpgrade());
	TestEqual(TEXT("Upgrade spends rice cakes"), Currency->GetRiceCakeCount(), static_cast<int64>(0));
	TestEqual(TEXT("Upgrade increases click power"), Currency->GetClickPower(), 2);
	TestEqual(TEXT("Upgrade level increments"), Currency->GetClickUpgradeLevel(), 1);
	TestEqual(TEXT("Next upgrade cost increases"), Currency->GetNextClickUpgradeCost(), static_cast<int64>(25));

	Currency->SetState(42, 5, 3);
	TestEqual(TEXT("Saved count restores"), Currency->GetRiceCakeCount(), static_cast<int64>(42));
	TestEqual(TEXT("Saved click power restores"), Currency->GetClickPower(), 5);
	TestEqual(TEXT("Saved upgrade level restores"), Currency->GetClickUpgradeLevel(), 3);

	return true;
}

#endif
