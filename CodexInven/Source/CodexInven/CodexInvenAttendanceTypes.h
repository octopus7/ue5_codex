#pragma once

#include "CoreMinimal.h"
#include "CodexInvenPickupData.h"
#include "Templates/SubclassOf.h"

#include "CodexInvenAttendanceTypes.generated.h"

class UCodexInvenAttendanceWidgetBase;

UENUM(BlueprintType)
enum class ECodexInvenAttendanceDayState : uint8
{
	Claimed,
	Claimable,
	Locked
};

USTRUCT(BlueprintType)
struct CODEXINVEN_API FCodexInvenAttendanceRewardDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attendance")
	ECodexInvenPickupType PickupType = ECodexInvenPickupType::CylinderRed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attendance", meta = (ClampMin = "1"))
	int32 Quantity = 1;
};

USTRUCT(BlueprintType)
struct CODEXINVEN_API FCodexInvenAttendanceEventDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attendance")
	FName EventId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attendance")
	FText Title;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attendance")
	int32 Priority = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attendance", meta = (ClampMin = "1"))
	int32 TotalDays = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attendance")
	TSubclassOf<UCodexInvenAttendanceWidgetBase> PopupWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attendance")
	TArray<FCodexInvenAttendanceRewardDefinition> Rewards;
};

USTRUCT(BlueprintType)
struct CODEXINVEN_API FCodexInvenAttendancePopupEntry
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attendance")
	FName EventId;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attendance")
	int32 Priority = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attendance")
	TSubclassOf<UCodexInvenAttendanceWidgetBase> PopupWidgetClass = nullptr;
};

USTRUCT(BlueprintType)
struct CODEXINVEN_API FCodexInvenAttendanceDayPresentationData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attendance")
	int32 DayNumber = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attendance")
	ECodexInvenAttendanceDayState State = ECodexInvenAttendanceDayState::Locked;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attendance")
	FCodexInvenAttendanceRewardDefinition Reward;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attendance")
	FText RewardText;
};

USTRUCT(BlueprintType)
struct CODEXINVEN_API FCodexInvenAttendanceEventPresentationData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attendance")
	FName EventId;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attendance")
	FText Title;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attendance")
	int32 Priority = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attendance")
	int32 TotalDays = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attendance")
	int32 ClaimedDaysCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attendance")
	bool bClaimedToday = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attendance")
	bool bCanClaimToday = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attendance")
	bool bIsCompleted = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attendance")
	int32 NextClaimDayNumber = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attendance")
	FCodexInvenAttendanceRewardDefinition NextReward;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attendance")
	TSubclassOf<UCodexInvenAttendanceWidgetBase> PopupWidgetClass = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attendance")
	TArray<FCodexInvenAttendanceDayPresentationData> DayEntries;
};
