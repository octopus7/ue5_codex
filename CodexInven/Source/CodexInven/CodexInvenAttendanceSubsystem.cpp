#include "CodexInvenAttendanceSubsystem.h"

#include "Algo/Sort.h"
#include "CodexInvenAttendanceConfigDataAsset.h"
#include "CodexInvenAttendanceWidgetBase.h"
#include "CodexInvenGameInstance.h"
#include "CodexInvenOwnershipComponent.h"
#include "Dom/JsonObject.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

DEFINE_LOG_CATEGORY_STATIC(LogCodexInvenAttendance, Log, All);

namespace
{
	constexpr int32 AttendanceStateSchemaVersion = 1;
	const TCHAR* AttendanceStateRelativePath = TEXT("Attendance/AttendanceState.json");

	FText BuildRewardText(const FCodexInvenAttendanceRewardDefinition& InReward)
	{
		const FCodexInvenPickupDefinition* const Definition = CodexInvenPickupData::FindPickupDefinition(InReward.PickupType);
		const FString DisplayName = Definition != nullptr ? Definition->DisplayName : TEXT("Unknown Reward");
		return FText::Format(
			FText::FromString(TEXT("{0} x{1}")),
			FText::FromString(DisplayName),
			FText::AsNumber(InReward.Quantity));
	}
}

void UCodexInvenAttendanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FString LoadMessage;
	LoadStateFromDisk(LoadMessage);
	if (!LoadMessage.IsEmpty())
	{
		UE_LOG(LogCodexInvenAttendance, Warning, TEXT("%s"), *LoadMessage);
	}

	EnsureConfigCache();
}

void UCodexInvenAttendanceSubsystem::Deinitialize()
{
	CachedSourceConfig = nullptr;
	ValidatedEvents.Reset();
	EventStates.Reset();

	Super::Deinitialize();
}

TArray<FCodexInvenAttendancePopupEntry> UCodexInvenAttendanceSubsystem::GetPendingPopupQueueForToday()
{
	EnsureConfigCache();

	TArray<FCodexInvenAttendancePopupEntry> PopupQueue;
	const FString TodayLocalDateKey = GetTodayLocalDateKey();

	for (const FCodexInvenAttendanceEventDefinition& EventDefinition : ValidatedEvents)
	{
		const FCodexInvenAttendanceEventState& EventState = GetEventStateOrDefault(EventDefinition.EventId);
		if (IsEventCompleted(EventDefinition, EventState) || HasClaimedOnDate(EventState, TodayLocalDateKey))
		{
			continue;
		}

		FCodexInvenAttendancePopupEntry& PopupEntry = PopupQueue.AddDefaulted_GetRef();
		PopupEntry.EventId = EventDefinition.EventId;
		PopupEntry.Priority = EventDefinition.Priority;
		PopupEntry.PopupWidgetClass = EventDefinition.PopupWidgetClass;
	}

	return PopupQueue;
}

bool UCodexInvenAttendanceSubsystem::GetEventPresentationData(
	const FName InEventId,
	FCodexInvenAttendanceEventPresentationData& OutPresentationData)
{
	EnsureConfigCache();

	const FCodexInvenAttendanceEventDefinition* const EventDefinition = FindValidatedEvent(InEventId);
	if (EventDefinition == nullptr)
	{
		return false;
	}

	return BuildPresentationData(
		*EventDefinition,
		GetEventStateOrDefault(InEventId),
		GetTodayLocalDateKey(),
		OutPresentationData);
}

bool UCodexInvenAttendanceSubsystem::CanClaimToday(const FName InEventId)
{
	FCodexInvenAttendanceEventPresentationData PresentationData;
	return GetEventPresentationData(InEventId, PresentationData) && PresentationData.bCanClaimToday;
}

bool UCodexInvenAttendanceSubsystem::TryClaimToday(const FName InEventId, UCodexInvenOwnershipComponent& InOwnershipComponent)
{
	EnsureConfigCache();

	const FCodexInvenAttendanceEventDefinition* const EventDefinition = FindValidatedEvent(InEventId);
	if (EventDefinition == nullptr)
	{
		return false;
	}

	FCodexInvenAttendanceEventState& EventState = FindOrAddEventState(InEventId);
	const int32 ClaimedDaysCount = FMath::Clamp(EventState.ClaimedDaysCount, 0, EventDefinition->TotalDays);
	const FString TodayLocalDateKey = GetTodayLocalDateKey();
	if (ClaimedDaysCount >= EventDefinition->TotalDays || HasClaimedOnDate(EventState, TodayLocalDateKey))
	{
		return false;
	}

	const FCodexInvenAttendanceRewardDefinition& Reward = EventDefinition->Rewards[ClaimedDaysCount];
	if (!InOwnershipComponent.CanAddPickup(Reward.PickupType, Reward.Quantity))
	{
		return false;
	}

	for (int32 RewardIndex = 0; RewardIndex < Reward.Quantity; ++RewardIndex)
	{
		if (!InOwnershipComponent.AddPickup(Reward.PickupType))
		{
			UE_LOG(
				LogCodexInvenAttendance,
				Error,
				TEXT("Attendance claim for event %s failed while granting reward %s (%d/%d)."),
				*InEventId.ToString(),
				*StaticEnum<ECodexInvenPickupType>()->GetNameStringByValue(static_cast<int64>(Reward.PickupType)),
				RewardIndex + 1,
				Reward.Quantity);
			return false;
		}
	}

	EventState.ClaimedDaysCount = ClaimedDaysCount + 1;
	EventState.LastClaimLocalDateKey = TodayLocalDateKey;

	FString SaveError;
	if (!SaveStateToDisk(SaveError))
	{
		UE_LOG(LogCodexInvenAttendance, Warning, TEXT("%s"), *SaveError);
	}

	return true;
}

FString UCodexInvenAttendanceSubsystem::GetStateFilePath() const
{
#if WITH_DEV_AUTOMATION_TESTS
	if (!DebugStateFilePathOverride.IsEmpty())
	{
		return DebugStateFilePathOverride;
	}
#endif

	return FPaths::Combine(FPaths::ProjectSavedDir(), AttendanceStateRelativePath);
}

bool UCodexInvenAttendanceSubsystem::ReloadStateFromDisk()
{
	FString LoadMessage;
	const bool bLoadedState = LoadStateFromDisk(LoadMessage);
	if (!LoadMessage.IsEmpty())
	{
		UE_LOG(LogCodexInvenAttendance, Warning, TEXT("%s"), *LoadMessage);
	}

	return bLoadedState;
}

#if WITH_DEV_AUTOMATION_TESTS
void UCodexInvenAttendanceSubsystem::SetDebugTodayOverride(const FString& InTodayOverride)
{
	DebugTodayOverride = InTodayOverride;
}

void UCodexInvenAttendanceSubsystem::ClearDebugTodayOverride()
{
	DebugTodayOverride.Reset();
}

void UCodexInvenAttendanceSubsystem::SetDebugStateFilePathOverride(const FString& InStateFilePathOverride)
{
	DebugStateFilePathOverride = InStateFilePathOverride;
}

void UCodexInvenAttendanceSubsystem::ClearDebugStateFilePathOverride()
{
	DebugStateFilePathOverride.Reset();
}
#endif

void UCodexInvenAttendanceSubsystem::EnsureConfigCache()
{
	const UCodexInvenGameInstance* const CodexInvenGameInstance = Cast<UCodexInvenGameInstance>(GetGameInstance());
	const UCodexInvenAttendanceConfigDataAsset* const AttendanceConfig =
		CodexInvenGameInstance != nullptr ? CodexInvenGameInstance->GetAttendanceConfig() : nullptr;

	if (CachedSourceConfig == AttendanceConfig)
	{
		return;
	}

	CachedSourceConfig = AttendanceConfig;
	RebuildValidatedEventCache(AttendanceConfig);
}

void UCodexInvenAttendanceSubsystem::RebuildValidatedEventCache(const UCodexInvenAttendanceConfigDataAsset* InConfig)
{
	ValidatedEvents.Reset();

	if (InConfig == nullptr)
	{
		UE_LOG(LogCodexInvenAttendance, Warning, TEXT("Attendance config is not assigned. No attendance events will be active."));
		return;
	}

	TSet<FName> SeenEventIds;
	for (const FCodexInvenAttendanceEventDefinition& EventDefinition : InConfig->ActiveEvents)
	{
		if (EventDefinition.EventId.IsNone())
		{
			UE_LOG(LogCodexInvenAttendance, Warning, TEXT("Skipped an attendance event with an empty EventId."));
			continue;
		}

		if (SeenEventIds.Contains(EventDefinition.EventId))
		{
			UE_LOG(LogCodexInvenAttendance, Warning, TEXT("Skipped duplicate attendance event id %s."), *EventDefinition.EventId.ToString());
			continue;
		}

		if (EventDefinition.TotalDays <= 0)
		{
			UE_LOG(LogCodexInvenAttendance, Warning, TEXT("Skipped attendance event %s because TotalDays must be positive."), *EventDefinition.EventId.ToString());
			continue;
		}

		if (EventDefinition.Rewards.Num() != EventDefinition.TotalDays)
		{
			UE_LOG(
				LogCodexInvenAttendance,
				Warning,
				TEXT("Skipped attendance event %s because Rewards.Num() (%d) does not match TotalDays (%d)."),
				*EventDefinition.EventId.ToString(),
				EventDefinition.Rewards.Num(),
				EventDefinition.TotalDays);
			continue;
		}

		if (EventDefinition.PopupWidgetClass == nullptr)
		{
			UE_LOG(LogCodexInvenAttendance, Warning, TEXT("Skipped attendance event %s because PopupWidgetClass is not assigned."), *EventDefinition.EventId.ToString());
			continue;
		}

		const UCodexInvenAttendanceWidgetBase* const WidgetDefaultObject = EventDefinition.PopupWidgetClass->GetDefaultObject<UCodexInvenAttendanceWidgetBase>();
		if (WidgetDefaultObject == nullptr || !WidgetDefaultObject->SupportsConfiguredDayCount(EventDefinition.TotalDays))
		{
			UE_LOG(
				LogCodexInvenAttendance,
				Warning,
				TEXT("Skipped attendance event %s because widget class %s does not support %d day entries."),
				*EventDefinition.EventId.ToString(),
				*GetNameSafe(EventDefinition.PopupWidgetClass),
				EventDefinition.TotalDays);
			continue;
		}

		bool bHasInvalidReward = false;
		for (const FCodexInvenAttendanceRewardDefinition& Reward : EventDefinition.Rewards)
		{
			const FCodexInvenPickupDefinition* const PickupDefinition = CodexInvenPickupData::FindPickupDefinition(Reward.PickupType);
			if (PickupDefinition == nullptr)
			{
				UE_LOG(
					LogCodexInvenAttendance,
					Warning,
					TEXT("Skipped attendance event %s because it contains an unknown reward pickup type."),
					*EventDefinition.EventId.ToString());
				bHasInvalidReward = true;
				break;
			}

			if (Reward.Quantity <= 0)
			{
				UE_LOG(
					LogCodexInvenAttendance,
					Warning,
					TEXT("Skipped attendance event %s because reward quantities must be positive."),
					*EventDefinition.EventId.ToString());
				bHasInvalidReward = true;
				break;
			}

			if (!PickupDefinition->bStackable && Reward.Quantity != 1)
			{
				UE_LOG(
					LogCodexInvenAttendance,
					Warning,
					TEXT("Skipped attendance event %s because non-stackable reward %s must use Quantity 1."),
					*EventDefinition.EventId.ToString(),
					*PickupDefinition->DisplayName);
				bHasInvalidReward = true;
				break;
			}
		}

		if (bHasInvalidReward)
		{
			continue;
		}

		SeenEventIds.Add(EventDefinition.EventId);
		ValidatedEvents.Add(EventDefinition);
	}

	Algo::Sort(ValidatedEvents, [](const FCodexInvenAttendanceEventDefinition& Left, const FCodexInvenAttendanceEventDefinition& Right)
	{
		return Left.Priority == Right.Priority
			? Left.EventId.LexicalLess(Right.EventId)
			: Left.Priority < Right.Priority;
	});
}

const FCodexInvenAttendanceEventDefinition* UCodexInvenAttendanceSubsystem::FindValidatedEvent(const FName InEventId) const
{
	for (const FCodexInvenAttendanceEventDefinition& EventDefinition : ValidatedEvents)
	{
		if (EventDefinition.EventId == InEventId)
		{
			return &EventDefinition;
		}
	}

	return nullptr;
}

const UCodexInvenAttendanceSubsystem::FCodexInvenAttendanceEventState& UCodexInvenAttendanceSubsystem::GetEventStateOrDefault(const FName InEventId) const
{
	static const FCodexInvenAttendanceEventState EmptyState;

	if (const FCodexInvenAttendanceEventState* const ExistingState = EventStates.Find(InEventId))
	{
		return *ExistingState;
	}

	return EmptyState;
}

UCodexInvenAttendanceSubsystem::FCodexInvenAttendanceEventState& UCodexInvenAttendanceSubsystem::FindOrAddEventState(const FName InEventId)
{
	return EventStates.FindOrAdd(InEventId);
}

FString UCodexInvenAttendanceSubsystem::GetTodayLocalDateKey() const
{
#if WITH_DEV_AUTOMATION_TESTS
	if (!DebugTodayOverride.IsEmpty())
	{
		return DebugTodayOverride;
	}
#endif

	return FDateTime::Now().ToString(TEXT("%Y-%m-%d"));
}

bool UCodexInvenAttendanceSubsystem::BuildPresentationData(
	const FCodexInvenAttendanceEventDefinition& InEventDefinition,
	const FCodexInvenAttendanceEventState& InEventState,
	const FString& InTodayLocalDateKey,
	FCodexInvenAttendanceEventPresentationData& OutPresentationData) const
{
	const int32 ClaimedDaysCount = FMath::Clamp(InEventState.ClaimedDaysCount, 0, InEventDefinition.TotalDays);

	OutPresentationData = FCodexInvenAttendanceEventPresentationData();
	OutPresentationData.EventId = InEventDefinition.EventId;
	OutPresentationData.Title = InEventDefinition.Title;
	OutPresentationData.Priority = InEventDefinition.Priority;
	OutPresentationData.TotalDays = InEventDefinition.TotalDays;
	OutPresentationData.ClaimedDaysCount = ClaimedDaysCount;
	OutPresentationData.bClaimedToday = HasClaimedOnDate(InEventState, InTodayLocalDateKey);
	OutPresentationData.bIsCompleted = ClaimedDaysCount >= InEventDefinition.TotalDays;
	OutPresentationData.bCanClaimToday = !OutPresentationData.bClaimedToday && !OutPresentationData.bIsCompleted;
	OutPresentationData.PopupWidgetClass = InEventDefinition.PopupWidgetClass;

	if (OutPresentationData.bCanClaimToday)
	{
		OutPresentationData.NextClaimDayNumber = ClaimedDaysCount + 1;
		OutPresentationData.NextReward = InEventDefinition.Rewards[ClaimedDaysCount];
	}

	OutPresentationData.DayEntries.Reserve(InEventDefinition.TotalDays);
	for (int32 RewardIndex = 0; RewardIndex < InEventDefinition.Rewards.Num(); ++RewardIndex)
	{
		FCodexInvenAttendanceDayPresentationData& DayEntry = OutPresentationData.DayEntries.AddDefaulted_GetRef();
		DayEntry.DayNumber = RewardIndex + 1;
		DayEntry.Reward = InEventDefinition.Rewards[RewardIndex];
		DayEntry.RewardText = BuildRewardText(DayEntry.Reward);

		if (RewardIndex < ClaimedDaysCount)
		{
			DayEntry.State = ECodexInvenAttendanceDayState::Claimed;
		}
		else if (RewardIndex == ClaimedDaysCount && OutPresentationData.bCanClaimToday)
		{
			DayEntry.State = ECodexInvenAttendanceDayState::Claimable;
		}
		else
		{
			DayEntry.State = ECodexInvenAttendanceDayState::Locked;
		}
	}

	return true;
}

bool UCodexInvenAttendanceSubsystem::IsEventCompleted(
	const FCodexInvenAttendanceEventDefinition& InEventDefinition,
	const FCodexInvenAttendanceEventState& InEventState) const
{
	return FMath::Clamp(InEventState.ClaimedDaysCount, 0, InEventDefinition.TotalDays) >= InEventDefinition.TotalDays;
}

bool UCodexInvenAttendanceSubsystem::HasClaimedOnDate(
	const FCodexInvenAttendanceEventState& InEventState,
	const FString& InTodayLocalDateKey) const
{
	return !InTodayLocalDateKey.IsEmpty() && InEventState.LastClaimLocalDateKey == InTodayLocalDateKey;
}

bool UCodexInvenAttendanceSubsystem::LoadStateFromDisk(FString& OutMessage)
{
	EventStates.Reset();

	const FString StateFilePath = GetStateFilePath();
	if (!FPaths::FileExists(StateFilePath))
	{
		return true;
	}

	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *StateFilePath))
	{
		OutMessage = FString::Printf(TEXT("Failed to load attendance state file %s."), *StateFilePath);
		return false;
	}

	return DeserializeStateFromJsonString(JsonString, OutMessage);
}

bool UCodexInvenAttendanceSubsystem::SaveStateToDisk(FString& OutError) const
{
	FString JsonString;
	if (!SerializeStateToJsonString(JsonString))
	{
		OutError = TEXT("Failed to serialize attendance state to JSON.");
		return false;
	}

	const FString StateFilePath = GetStateFilePath();
	if (!IFileManager::Get().MakeDirectory(*FPaths::GetPath(StateFilePath), true))
	{
		OutError = FString::Printf(TEXT("Failed to create attendance save directory for %s."), *StateFilePath);
		return false;
	}

	if (!FFileHelper::SaveStringToFile(JsonString, *StateFilePath))
	{
		OutError = FString::Printf(TEXT("Failed to save attendance state file %s."), *StateFilePath);
		return false;
	}

	return true;
}

bool UCodexInvenAttendanceSubsystem::SerializeStateToJsonString(FString& OutJsonString) const
{
	TSharedRef<FJsonObject> RootObject = MakeShared<FJsonObject>();
	RootObject->SetNumberField(TEXT("SchemaVersion"), AttendanceStateSchemaVersion);

	TSharedRef<FJsonObject> EventsObject = MakeShared<FJsonObject>();

	TArray<FName> SortedEventIds;
	EventStates.GenerateKeyArray(SortedEventIds);
	SortedEventIds.Sort(FNameLexicalLess());

	for (const FName EventId : SortedEventIds)
	{
		const FCodexInvenAttendanceEventState* const EventState = EventStates.Find(EventId);
		if (EventState == nullptr)
		{
			continue;
		}

		TSharedRef<FJsonObject> EventStateObject = MakeShared<FJsonObject>();
		EventStateObject->SetNumberField(TEXT("ClaimedDaysCount"), FMath::Max(0, EventState->ClaimedDaysCount));
		EventStateObject->SetStringField(TEXT("LastClaimLocalDateKey"), EventState->LastClaimLocalDateKey);
		EventsObject->SetObjectField(EventId.ToString(), EventStateObject);
	}

	RootObject->SetObjectField(TEXT("Events"), EventsObject);

	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&OutJsonString);
	return FJsonSerializer::Serialize(RootObject, JsonWriter);
}

bool UCodexInvenAttendanceSubsystem::DeserializeStateFromJsonString(const FString& InJsonString, FString& OutWarning)
{
	TSharedPtr<FJsonObject> RootObject;
	const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(InJsonString);
	if (!FJsonSerializer::Deserialize(JsonReader, RootObject) || !RootObject.IsValid())
	{
		OutWarning = FString::Printf(
			TEXT("Attendance state file %s is malformed. Falling back to an empty attendance state."),
			*GetStateFilePath());
		return true;
	}

	int32 SchemaVersion = 0;
	if (!RootObject->TryGetNumberField(TEXT("SchemaVersion"), SchemaVersion) || SchemaVersion != AttendanceStateSchemaVersion)
	{
		OutWarning = FString::Printf(
			TEXT("Attendance state file %s uses an unsupported schema version. Falling back to an empty attendance state."),
			*GetStateFilePath());
		return true;
	}

	const TSharedPtr<FJsonObject>* EventsObject = nullptr;
	if (!RootObject->TryGetObjectField(TEXT("Events"), EventsObject) || EventsObject == nullptr || !EventsObject->IsValid())
	{
		return true;
	}

	for (const TPair<FString, TSharedPtr<FJsonValue>>& EventPair : (*EventsObject)->Values)
	{
		const TSharedPtr<FJsonObject>* EventStateObject = nullptr;
		if (!EventPair.Value.IsValid() || !EventPair.Value->TryGetObject(EventStateObject) || EventStateObject == nullptr || !EventStateObject->IsValid())
		{
			continue;
		}

		FCodexInvenAttendanceEventState& EventState = EventStates.FindOrAdd(FName(*EventPair.Key));
		int32 ClaimedDaysCount = 0;
		(*EventStateObject)->TryGetNumberField(TEXT("ClaimedDaysCount"), ClaimedDaysCount);
		EventState.ClaimedDaysCount = FMath::Max(0, ClaimedDaysCount);
		(*EventStateObject)->TryGetStringField(TEXT("LastClaimLocalDateKey"), EventState.LastClaimLocalDateKey);
	}

	return true;
}
