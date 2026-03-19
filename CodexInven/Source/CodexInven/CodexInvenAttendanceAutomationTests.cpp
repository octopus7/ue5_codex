#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "CodexInvenAttendanceCompactWidget.h"
#include "CodexInvenAttendanceConfigDataAsset.h"
#include "CodexInvenAttendanceScrollableWidget.h"
#include "CodexInvenAttendanceSubsystem.h"
#include "CodexInvenGameInstance.h"
#include "CodexInvenOwnershipComponent.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/StrongObjectPtr.h"

namespace
{
	FCodexInvenAttendanceRewardDefinition MakeReward(const ECodexInvenPickupType InPickupType, const int32 InQuantity)
	{
		FCodexInvenAttendanceRewardDefinition Reward;
		Reward.PickupType = InPickupType;
		Reward.Quantity = InQuantity;
		return Reward;
	}

	FCodexInvenAttendanceEventDefinition MakeEventDefinition(
		const FName InEventId,
		const int32 InPriority,
		const int32 InTotalDays,
		TSubclassOf<UCodexInvenAttendanceWidgetBase> InPopupWidgetClass,
		TArray<FCodexInvenAttendanceRewardDefinition> InRewards)
	{
		FCodexInvenAttendanceEventDefinition EventDefinition;
		EventDefinition.EventId = InEventId;
		EventDefinition.Title = FText::FromString(InEventId.ToString());
		EventDefinition.Priority = InPriority;
		EventDefinition.TotalDays = InTotalDays;
		EventDefinition.PopupWidgetClass = InPopupWidgetClass;
		EventDefinition.Rewards = MoveTemp(InRewards);
		return EventDefinition;
	}

	struct FAttendanceTestContext
	{
		explicit FAttendanceTestContext(const FString& InName, TArray<FCodexInvenAttendanceEventDefinition> InEventDefinitions)
		{
			GameInstance.Reset(NewObject<UCodexInvenGameInstance>(GetTransientPackage()));
			Config.Reset(NewObject<UCodexInvenAttendanceConfigDataAsset>(GameInstance.Get()));
			Config->PopupInitialDelaySeconds = 0.0f;
			Config->ActiveEvents = MoveTemp(InEventDefinitions);
			GameInstance->AttendanceConfig = Config.Get();
			GameInstance->Init();

			AttendanceSubsystem = GameInstance->GetSubsystem<UCodexInvenAttendanceSubsystem>();
			OwnershipComponent.Reset(NewObject<UCodexInvenOwnershipComponent>(GetTransientPackage()));
			StateFilePath = FPaths::Combine(FPaths::ProjectIntermediateDir(), TEXT("AttendanceAutomation"), InName + TEXT(".json"));
			IFileManager::Get().Delete(*StateFilePath, false, true, true);

			check(AttendanceSubsystem != nullptr);
			AttendanceSubsystem->SetDebugStateFilePathOverride(StateFilePath);
			AttendanceSubsystem->ClearDebugTodayOverride();
			AttendanceSubsystem->ReloadStateFromDisk();
		}

		~FAttendanceTestContext()
		{
			if (AttendanceSubsystem != nullptr)
			{
				AttendanceSubsystem->ClearDebugTodayOverride();
				AttendanceSubsystem->ClearDebugStateFilePathOverride();
			}

			IFileManager::Get().Delete(*StateFilePath, false, true, true);

			if (GameInstance.IsValid())
			{
				GameInstance->Shutdown();
			}
		}

		void SetToday(const FString& InToday) const
		{
			AttendanceSubsystem->SetDebugTodayOverride(InToday);
		}

		TStrongObjectPtr<UCodexInvenGameInstance> GameInstance;
		TStrongObjectPtr<UCodexInvenAttendanceConfigDataAsset> Config;
		TStrongObjectPtr<UCodexInvenOwnershipComponent> OwnershipComponent;
		UCodexInvenAttendanceSubsystem* AttendanceSubsystem = nullptr;
		FString StateFilePath;
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCodexInvenAttendanceMissingSaveAutomationTest,
	"CodexInven.Attendance.MissingSaveStartsEmpty",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCodexInvenAttendanceMissingSaveAutomationTest::RunTest(const FString& Parameters)
{
	static_cast<void>(Parameters);

	FAttendanceTestContext Context(
		TEXT("MissingSaveStartsEmpty"),
		{
			MakeEventDefinition(
				TEXT("Event5"),
				10,
				5,
				UCodexInvenAttendanceCompactWidget::StaticClass(),
				{ MakeReward(ECodexInvenPickupType::CylinderRed, 2), MakeReward(ECodexInvenPickupType::CylinderGreen, 2), MakeReward(ECodexInvenPickupType::CubeRed, 1), MakeReward(ECodexInvenPickupType::CylinderBlue, 2), MakeReward(ECodexInvenPickupType::CubeGold, 1) })
		});
	Context.SetToday(TEXT("2026-03-19"));

	TestTrue(TEXT("Reload from a missing save file succeeds"), Context.AttendanceSubsystem->ReloadStateFromDisk());

	const TArray<FCodexInvenAttendancePopupEntry> PendingPopupQueue = Context.AttendanceSubsystem->GetPendingPopupQueueForToday();
	TestEqual(TEXT("A missing save file starts with one pending popup"), PendingPopupQueue.Num(), 1);
	TestEqual(TEXT("The pending popup uses the configured event id"), PendingPopupQueue[0].EventId, FName(TEXT("Event5")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCodexInvenAttendanceRoundTripAutomationTest,
	"CodexInven.Attendance.RoundTrip",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCodexInvenAttendanceRoundTripAutomationTest::RunTest(const FString& Parameters)
{
	static_cast<void>(Parameters);

	const FString FixedToday(TEXT("2026-03-19"));
	const FString SharedStateFilePath = FPaths::Combine(FPaths::ProjectIntermediateDir(), TEXT("AttendanceAutomation"), TEXT("RoundTrip.json"));
	IFileManager::Get().Delete(*SharedStateFilePath, false, true, true);

	{
		FAttendanceTestContext FirstContext(
			TEXT("RoundTrip_First"),
			{
				MakeEventDefinition(
					TEXT("Event5"),
					10,
					5,
					UCodexInvenAttendanceCompactWidget::StaticClass(),
					{ MakeReward(ECodexInvenPickupType::CylinderRed, 2), MakeReward(ECodexInvenPickupType::CylinderGreen, 2), MakeReward(ECodexInvenPickupType::CubeRed, 1), MakeReward(ECodexInvenPickupType::CylinderBlue, 2), MakeReward(ECodexInvenPickupType::CubeGold, 1) })
			});
		FirstContext.AttendanceSubsystem->SetDebugStateFilePathOverride(SharedStateFilePath);
		FirstContext.SetToday(FixedToday);
		FirstContext.AttendanceSubsystem->ReloadStateFromDisk();

		TestTrue(TEXT("The first claim succeeds"), FirstContext.AttendanceSubsystem->TryClaimToday(TEXT("Event5"), *FirstContext.OwnershipComponent));
		TestFalse(TEXT("The same event cannot be claimed twice on the same day"), FirstContext.AttendanceSubsystem->CanClaimToday(TEXT("Event5")));
	}

	{
		FAttendanceTestContext SecondContext(
			TEXT("RoundTrip_Second"),
			{
				MakeEventDefinition(
					TEXT("Event5"),
					10,
					5,
					UCodexInvenAttendanceCompactWidget::StaticClass(),
					{ MakeReward(ECodexInvenPickupType::CylinderRed, 2), MakeReward(ECodexInvenPickupType::CylinderGreen, 2), MakeReward(ECodexInvenPickupType::CubeRed, 1), MakeReward(ECodexInvenPickupType::CylinderBlue, 2), MakeReward(ECodexInvenPickupType::CubeGold, 1) })
			});
		SecondContext.AttendanceSubsystem->SetDebugStateFilePathOverride(SharedStateFilePath);
		SecondContext.SetToday(FixedToday);
		TestTrue(TEXT("The second subsystem instance reloads the shared state file"), SecondContext.AttendanceSubsystem->ReloadStateFromDisk());

		FCodexInvenAttendanceEventPresentationData PresentationData;
		TestTrue(TEXT("Presentation data exists after reload"), SecondContext.AttendanceSubsystem->GetEventPresentationData(TEXT("Event5"), PresentationData));
		TestEqual(TEXT("Reloaded state preserves the claimed day count"), PresentationData.ClaimedDaysCount, 1);
		TestTrue(TEXT("Reloaded state preserves today's claim lockout"), PresentationData.bClaimedToday);
	}

	IFileManager::Get().Delete(*SharedStateFilePath, false, true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCodexInvenAttendanceMalformedSaveAutomationTest,
	"CodexInven.Attendance.MalformedSaveFallsBackToEmpty",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCodexInvenAttendanceMalformedSaveAutomationTest::RunTest(const FString& Parameters)
{
	static_cast<void>(Parameters);

	FAttendanceTestContext Context(
		TEXT("MalformedSaveFallsBackToEmpty"),
		{
			MakeEventDefinition(
				TEXT("Event5"),
				10,
				5,
				UCodexInvenAttendanceCompactWidget::StaticClass(),
				{ MakeReward(ECodexInvenPickupType::CylinderRed, 2), MakeReward(ECodexInvenPickupType::CylinderGreen, 2), MakeReward(ECodexInvenPickupType::CubeRed, 1), MakeReward(ECodexInvenPickupType::CylinderBlue, 2), MakeReward(ECodexInvenPickupType::CubeGold, 1) })
		});
	Context.SetToday(TEXT("2026-03-19"));

	IFileManager::Get().MakeDirectory(*FPaths::GetPath(Context.StateFilePath), true);
	TestTrue(TEXT("Malformed JSON can be written to the test save file"), FFileHelper::SaveStringToFile(TEXT("{ definitely invalid json"), *Context.StateFilePath));
	TestTrue(TEXT("Reload succeeds even when the save file is malformed"), Context.AttendanceSubsystem->ReloadStateFromDisk());

	const TArray<FCodexInvenAttendancePopupEntry> PendingPopupQueue = Context.AttendanceSubsystem->GetPendingPopupQueueForToday();
	TestEqual(TEXT("Malformed save content falls back to an empty state"), PendingPopupQueue.Num(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCodexInvenAttendancePriorityAndCompletionAutomationTest,
	"CodexInven.Attendance.PriorityAndCompletion",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCodexInvenAttendancePriorityAndCompletionAutomationTest::RunTest(const FString& Parameters)
{
	static_cast<void>(Parameters);

	FAttendanceTestContext Context(
		TEXT("PriorityAndCompletion"),
		{
			MakeEventDefinition(
				TEXT("Event5"),
				20,
				5,
				UCodexInvenAttendanceCompactWidget::StaticClass(),
				{ MakeReward(ECodexInvenPickupType::CylinderRed, 2), MakeReward(ECodexInvenPickupType::CylinderGreen, 2), MakeReward(ECodexInvenPickupType::CubeRed, 1), MakeReward(ECodexInvenPickupType::CylinderBlue, 2), MakeReward(ECodexInvenPickupType::CubeGold, 1) }),
			MakeEventDefinition(
				TEXT("Event7"),
				10,
				7,
				UCodexInvenAttendanceCompactWidget::StaticClass(),
				{ MakeReward(ECodexInvenPickupType::CylinderGreen, 3), MakeReward(ECodexInvenPickupType::CylinderBlue, 2), MakeReward(ECodexInvenPickupType::CubeBlue, 1), MakeReward(ECodexInvenPickupType::CylinderRed, 4), MakeReward(ECodexInvenPickupType::CubeGreen, 1), MakeReward(ECodexInvenPickupType::CylinderGold, 2), MakeReward(ECodexInvenPickupType::CubeGold, 1) }),
			MakeEventDefinition(
				TEXT("Event14"),
				30,
				14,
				UCodexInvenAttendanceScrollableWidget::StaticClass(),
				{ MakeReward(ECodexInvenPickupType::CylinderRed, 2), MakeReward(ECodexInvenPickupType::CylinderGreen, 2), MakeReward(ECodexInvenPickupType::CylinderBlue, 2), MakeReward(ECodexInvenPickupType::CubeRed, 1), MakeReward(ECodexInvenPickupType::CylinderRed, 3), MakeReward(ECodexInvenPickupType::CubeGreen, 1), MakeReward(ECodexInvenPickupType::CylinderGold, 2), MakeReward(ECodexInvenPickupType::CubeBlue, 1), MakeReward(ECodexInvenPickupType::CylinderGreen, 4), MakeReward(ECodexInvenPickupType::CylinderBlue, 4), MakeReward(ECodexInvenPickupType::CubeRed, 1), MakeReward(ECodexInvenPickupType::CubeGreen, 1), MakeReward(ECodexInvenPickupType::CylinderGold, 3), MakeReward(ECodexInvenPickupType::CubeGold, 1) })
		});
	Context.SetToday(TEXT("2026-03-19"));

	const FString SeededState = TEXT("{\"SchemaVersion\":1,\"Events\":{\"Event5\":{\"ClaimedDaysCount\":5,\"LastClaimLocalDateKey\":\"2026-03-15\"},\"Event7\":{\"ClaimedDaysCount\":1,\"LastClaimLocalDateKey\":\"2026-03-18\"}}}");
	IFileManager::Get().MakeDirectory(*FPaths::GetPath(Context.StateFilePath), true);
	TestTrue(TEXT("Seeded state file can be written"), FFileHelper::SaveStringToFile(SeededState, *Context.StateFilePath));
	TestTrue(TEXT("Seeded state file reloads successfully"), Context.AttendanceSubsystem->ReloadStateFromDisk());

	const TArray<FCodexInvenAttendancePopupEntry> PendingPopupQueue = Context.AttendanceSubsystem->GetPendingPopupQueueForToday();
	TestEqual(TEXT("Completed events are suppressed from the popup queue"), PendingPopupQueue.Num(), 2);
	if (PendingPopupQueue.Num() == 2)
	{
		TestEqual(TEXT("The lower-priority number opens first"), PendingPopupQueue[0].EventId, FName(TEXT("Event7")));
		TestEqual(TEXT("The highest priority number opens last"), PendingPopupQueue[1].EventId, FName(TEXT("Event14")));
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCodexInvenAttendanceRewardGrantAutomationTest,
	"CodexInven.Attendance.RewardGrantAndPreflight",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCodexInvenAttendanceRewardGrantAutomationTest::RunTest(const FString& Parameters)
{
	static_cast<void>(Parameters);

	FAttendanceTestContext StackableRewardContext(
		TEXT("RewardGrantAndPreflight_Stackable"),
		{
			MakeEventDefinition(
				TEXT("StackableEvent"),
				10,
				1,
				UCodexInvenAttendanceCompactWidget::StaticClass(),
				{ MakeReward(ECodexInvenPickupType::CylinderGreen, 3) })
		});
	StackableRewardContext.SetToday(TEXT("2026-03-19"));

	TestTrue(TEXT("A stackable reward can be claimed"), StackableRewardContext.AttendanceSubsystem->TryClaimToday(TEXT("StackableEvent"), *StackableRewardContext.OwnershipComponent));
	TestEqual(TEXT("Claiming adds the full stackable reward quantity"), StackableRewardContext.OwnershipComponent->GetStackCount(ECodexInvenPickupType::CylinderGreen), 3);
	TestFalse(TEXT("The event cannot be claimed twice on the same day"), StackableRewardContext.AttendanceSubsystem->CanClaimToday(TEXT("StackableEvent")));

	FAttendanceTestContext FullInventoryContext(
		TEXT("RewardGrantAndPreflight_FullInventory"),
		{
			MakeEventDefinition(
				TEXT("UniqueEvent"),
				10,
				1,
				UCodexInvenAttendanceCompactWidget::StaticClass(),
				{ MakeReward(ECodexInvenPickupType::CubeGreen, 1) })
		});
	FullInventoryContext.SetToday(TEXT("2026-03-19"));

	for (int32 SlotIndex = 0; SlotIndex < FullInventoryContext.OwnershipComponent->GetInventoryCapacity(); ++SlotIndex)
	{
		TestTrue(TEXT("Inventory slot can be filled"), FullInventoryContext.OwnershipComponent->AddPickup((SlotIndex % 2) == 0 ? ECodexInvenPickupType::CubeRed : ECodexInvenPickupType::CubeBlue));
	}

	TestFalse(TEXT("CanAddPickup preflight rejects a full inventory"), FullInventoryContext.OwnershipComponent->CanAddPickup(ECodexInvenPickupType::CubeGreen, 1));
	TestFalse(TEXT("Claiming fails cleanly when inventory preflight rejects the reward"), FullInventoryContext.AttendanceSubsystem->TryClaimToday(TEXT("UniqueEvent"), *FullInventoryContext.OwnershipComponent));

	FCodexInvenAttendanceEventPresentationData PresentationData;
	TestTrue(TEXT("Presentation data still exists after a failed claim"), FullInventoryContext.AttendanceSubsystem->GetEventPresentationData(TEXT("UniqueEvent"), PresentationData));
	TestEqual(TEXT("Failed claims do not advance attendance progress"), PresentationData.ClaimedDaysCount, 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCodexInvenAttendanceConfigValidationAutomationTest,
	"CodexInven.Attendance.ConfigValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCodexInvenAttendanceConfigValidationAutomationTest::RunTest(const FString& Parameters)
{
	static_cast<void>(Parameters);

	FAttendanceTestContext Context(
		TEXT("ConfigValidation"),
		{
			MakeEventDefinition(
				TEXT("ValidEvent"),
				10,
				5,
				UCodexInvenAttendanceCompactWidget::StaticClass(),
				{ MakeReward(ECodexInvenPickupType::CylinderRed, 2), MakeReward(ECodexInvenPickupType::CylinderGreen, 2), MakeReward(ECodexInvenPickupType::CubeRed, 1), MakeReward(ECodexInvenPickupType::CylinderBlue, 2), MakeReward(ECodexInvenPickupType::CubeGold, 1) }),
			MakeEventDefinition(
				TEXT("ValidEvent"),
				20,
				5,
				UCodexInvenAttendanceCompactWidget::StaticClass(),
				{ MakeReward(ECodexInvenPickupType::CylinderRed, 2), MakeReward(ECodexInvenPickupType::CylinderGreen, 2), MakeReward(ECodexInvenPickupType::CubeRed, 1), MakeReward(ECodexInvenPickupType::CylinderBlue, 2), MakeReward(ECodexInvenPickupType::CubeGold, 1) }),
			MakeEventDefinition(
				TEXT("RewardCountMismatch"),
				30,
				5,
				UCodexInvenAttendanceCompactWidget::StaticClass(),
				{ MakeReward(ECodexInvenPickupType::CylinderRed, 2), MakeReward(ECodexInvenPickupType::CylinderGreen, 2), MakeReward(ECodexInvenPickupType::CubeRed, 1), MakeReward(ECodexInvenPickupType::CylinderBlue, 2) }),
			MakeEventDefinition(
				TEXT("InvalidNonStackableQuantity"),
				40,
				5,
				UCodexInvenAttendanceCompactWidget::StaticClass(),
				{ MakeReward(ECodexInvenPickupType::CubeRed, 2), MakeReward(ECodexInvenPickupType::CubeRed, 2), MakeReward(ECodexInvenPickupType::CubeRed, 2), MakeReward(ECodexInvenPickupType::CubeRed, 2), MakeReward(ECodexInvenPickupType::CubeRed, 2) }),
			MakeEventDefinition(
				TEXT("LayoutMismatch"),
				50,
				7,
				UCodexInvenAttendanceScrollableWidget::StaticClass(),
				{ MakeReward(ECodexInvenPickupType::CylinderGreen, 2), MakeReward(ECodexInvenPickupType::CylinderGreen, 2), MakeReward(ECodexInvenPickupType::CylinderGreen, 2), MakeReward(ECodexInvenPickupType::CylinderGreen, 2), MakeReward(ECodexInvenPickupType::CylinderGreen, 2), MakeReward(ECodexInvenPickupType::CylinderGreen, 2), MakeReward(ECodexInvenPickupType::CylinderGreen, 2) })
		});
	Context.SetToday(TEXT("2026-03-19"));

	const TArray<FCodexInvenAttendancePopupEntry> PendingPopupQueue = Context.AttendanceSubsystem->GetPendingPopupQueueForToday();
	TestEqual(TEXT("Only the valid event survives config validation"), PendingPopupQueue.Num(), 1);
	if (PendingPopupQueue.Num() == 1)
	{
		TestEqual(TEXT("The valid event remains active"), PendingPopupQueue[0].EventId, FName(TEXT("ValidEvent")));
	}

	return true;
}

#endif
