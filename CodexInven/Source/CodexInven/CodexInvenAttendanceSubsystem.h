#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "CodexInvenAttendanceTypes.h"
#include "CodexInvenAttendanceSubsystem.generated.h"

class UCodexInvenAttendanceConfigDataAsset;
class UCodexInvenOwnershipComponent;

UCLASS()
class CODEXINVEN_API UCodexInvenAttendanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	TArray<FCodexInvenAttendancePopupEntry> GetPendingPopupQueueForToday();
	bool GetEventPresentationData(FName InEventId, FCodexInvenAttendanceEventPresentationData& OutPresentationData);
	bool CanClaimToday(FName InEventId);
	bool TryClaimToday(FName InEventId, UCodexInvenOwnershipComponent& InOwnershipComponent);
	FString GetStateFilePath() const;
	bool ReloadStateFromDisk();

#if WITH_DEV_AUTOMATION_TESTS
	void SetDebugTodayOverride(const FString& InTodayOverride);
	void ClearDebugTodayOverride();
	void SetDebugStateFilePathOverride(const FString& InStateFilePathOverride);
	void ClearDebugStateFilePathOverride();
#endif

private:
	struct FCodexInvenAttendanceEventState
	{
		int32 ClaimedDaysCount = 0;
		FString LastClaimLocalDateKey;
	};

	void EnsureConfigCache();
	void RebuildValidatedEventCache(const UCodexInvenAttendanceConfigDataAsset* InConfig);
	const FCodexInvenAttendanceEventDefinition* FindValidatedEvent(FName InEventId) const;
	const FCodexInvenAttendanceEventState& GetEventStateOrDefault(FName InEventId) const;
	FCodexInvenAttendanceEventState& FindOrAddEventState(FName InEventId);
	FString GetTodayLocalDateKey() const;
	bool BuildPresentationData(
		const FCodexInvenAttendanceEventDefinition& InEventDefinition,
		const FCodexInvenAttendanceEventState& InEventState,
		const FString& InTodayLocalDateKey,
		FCodexInvenAttendanceEventPresentationData& OutPresentationData) const;
	bool IsEventCompleted(const FCodexInvenAttendanceEventDefinition& InEventDefinition, const FCodexInvenAttendanceEventState& InEventState) const;
	bool HasClaimedOnDate(const FCodexInvenAttendanceEventState& InEventState, const FString& InTodayLocalDateKey) const;
	bool LoadStateFromDisk(FString& OutMessage);
	bool SaveStateToDisk(FString& OutError) const;
	bool SerializeStateToJsonString(FString& OutJsonString) const;
	bool DeserializeStateFromJsonString(const FString& InJsonString, FString& OutWarning);

	UPROPERTY(Transient)
	TObjectPtr<const UCodexInvenAttendanceConfigDataAsset> CachedSourceConfig = nullptr;

	TArray<FCodexInvenAttendanceEventDefinition> ValidatedEvents;
	TMap<FName, FCodexInvenAttendanceEventState> EventStates;

#if WITH_DEV_AUTOMATION_TESTS
	FString DebugTodayOverride;
	FString DebugStateFilePathOverride;
#endif
};
