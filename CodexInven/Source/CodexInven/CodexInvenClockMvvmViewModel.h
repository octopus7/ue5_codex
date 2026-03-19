#pragma once

#include "MVVMViewModelBase.h"

#include "CodexInvenClockMvvmViewModel.generated.h"

UCLASS(BlueprintType)
class CODEXINVEN_API UCodexInvenClockMvvmViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Clock")
	void SetDigitalClockText(const FText& InDigitalClockText);

	UFUNCTION(BlueprintCallable, Category = "Clock")
	void SetHourHandAngle(float InHourHandAngle);

	UFUNCTION(BlueprintCallable, Category = "Clock")
	void SetMinuteHandAngle(float InMinuteHandAngle);

	UFUNCTION(BlueprintCallable, Category = "Clock")
	void SetSecondHandAngle(float InSecondHandAngle);

	UPROPERTY(BlueprintReadOnly, Category = "Clock", FieldNotify)
	FText DigitalClockText;

	UPROPERTY(BlueprintReadOnly, Category = "Clock", FieldNotify)
	float HourHandAngle = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Clock", FieldNotify)
	float MinuteHandAngle = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Clock", FieldNotify)
	float SecondHandAngle = 0.0f;
};
