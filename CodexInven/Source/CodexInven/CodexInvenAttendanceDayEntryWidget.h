#pragma once

#include "Blueprint/UserWidget.h"

#include "CodexInvenAttendanceTypes.h"
#include "CodexInvenAttendanceDayEntryWidget.generated.h"

class UBorder;
class UTextBlock;

UCLASS()
class CODEXINVEN_API UCodexInvenAttendanceDayEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetPresentationData(const FCodexInvenAttendanceDayPresentationData& InPresentationData);

protected:
	virtual void NativeOnInitialized() override;

private:
	void BuildWidgetTreeIfNeeded();
	void RefreshVisualState();

	UPROPERTY(Transient)
	TObjectPtr<UBorder> RootBorder = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> DayNumberTextBlock = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> RewardTextBlock = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> StateTextBlock = nullptr;

	FCodexInvenAttendanceDayPresentationData PresentationData;
	bool bHasPresentationData = false;
};
