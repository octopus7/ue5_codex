#pragma once

#include "Blueprint/UserWidget.h"

#include "CodexInvenClockMvvmWidget.generated.h"

class UCodexInvenClockMvvmViewModel;

UCLASS()
class CODEXINVEN_API UCodexInvenClockMvvmWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& InGeometry, float InDeltaTime) override;

private:
	UCodexInvenClockMvvmViewModel* ResolveClockViewModel();
	void RefreshClockViewModel();

	UPROPERTY(Transient)
	TObjectPtr<UCodexInvenClockMvvmViewModel> RuntimeClockViewModel = nullptr;

	int64 LastRenderedUnixSecond = INDEX_NONE;
};
