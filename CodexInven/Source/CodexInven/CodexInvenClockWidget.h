#pragma once

#include "Blueprint/UserWidget.h"
#include "CodexInvenClockWidget.generated.h"

class UBorder;
class UTextBlock;

UCLASS()
class CODEXINVEN_API UCodexInvenClockWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& InGeometry, float InDeltaTime) override;

private:
	void RefreshClockVisuals();
	void RefreshDigitalClockText(const FDateTime& InLocalTime);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ClockTextBlock = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> HourHand = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> MinuteHand = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SecondHand = nullptr;

	int64 LastRenderedUnixSecond = INDEX_NONE;
};
