#pragma once

#include "CodexInvenAttendanceTypes.h"
#include "PopupWidgetBase.h"
#include "CodexInvenAttendanceWidgetBase.generated.h"

class UButton;
class UPanelWidget;
class UTextBlock;
class UCodexInvenAttendanceDayEntryWidget;
class UCodexInvenAttendanceSubsystem;
class UCodexInvenOwnershipComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCodexInvenAttendanceWidgetClosed, FName);

UCLASS(Abstract)
class CODEXINVEN_API UCodexInvenAttendanceWidgetBase : public UPopupWidgetBase
{
	GENERATED_BODY()

public:
	void InitializeForEvent(FName InEventId);
	FName GetEventId() const;
	virtual bool SupportsConfiguredDayCount(int32 InTotalDays) const;
	FOnCodexInvenAttendanceWidgetClosed& OnAttendanceWidgetClosed();

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void HandleBackRequested() override;

	UPROPERTY(EditDefaultsOnly, Category = "Attendance|Layout", meta = (ClampMin = "0"))
	int32 ExpectedTotalDays = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Attendance|Layout", meta = (ClampMin = "80.0"))
	float DayEntryWidth = 136.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Attendance|Layout", meta = (ClampMin = "120.0"))
	float DayEntryHeight = 176.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Attendance|Layout", meta = (ClampMin = "0.0"))
	float DayEntrySpacing = 12.0f;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EventTitleTextBlock = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StatusTextBlock = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ClaimButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> DayEntryPanel = nullptr;

private:
	UCodexInvenAttendanceSubsystem* GetAttendanceSubsystem() const;
	UCodexInvenOwnershipComponent* GetOwnershipComponent() const;
	void RefreshFromSubsystem();
	void RebuildDayEntries(const FCodexInvenAttendanceEventPresentationData& InPresentationData);
	bool CanCurrentRewardFitInventory(const FCodexInvenAttendanceEventPresentationData& InPresentationData) const;
	FText BuildStatusText(const FCodexInvenAttendanceEventPresentationData& InPresentationData, bool bInCanFitReward) const;
	void CloseAttendanceWidget(EPopupWidgetResult InPopupResult = EPopupWidgetResult::Dismissed);

	UFUNCTION()
	void HandleClaimButtonClicked();

	UFUNCTION()
	void HandleCloseButtonClicked();

	UPROPERTY(Transient)
	TObjectPtr<UCodexInvenAttendanceSubsystem> RuntimeAttendanceSubsystem = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UCodexInvenAttendanceDayEntryWidget>> RuntimeDayEntryWidgets;

	FOnCodexInvenAttendanceWidgetClosed AttendanceWidgetClosed;
	FName EventId;
	FText StatusOverrideText;
	bool bHasStatusOverrideText = false;
};
