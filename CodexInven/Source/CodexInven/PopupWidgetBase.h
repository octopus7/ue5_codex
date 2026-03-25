#pragma once

#include "Blueprint/UserWidget.h"
#include "PopupWidgetBase.generated.h"

UENUM(BlueprintType)
enum class EPopupWidgetResult : uint8
{
	None,
	Dismissed,
	Confirmed,
	Cancelled
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPopupWidgetClosed, EPopupWidgetResult);

UCLASS(Abstract)
class CODEXINVEN_API UPopupWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnPopupWidgetClosed& OnPopupClosed();

	UFUNCTION(BlueprintCallable, Category = "Popup")
	void ClosePopupWithResult(EPopupWidgetResult InResult);

	UFUNCTION(BlueprintCallable, Category = "Popup")
	void DismissPopup();

	UFUNCTION(BlueprintPure, Category = "Popup")
	bool IsPopupClosed() const;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual void HandleBackRequested();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Popup")
	bool bCloseOnBack = true;

private:
	static bool IsBackCloseKey(const FKey& InKey);

	FOnPopupWidgetClosed PopupClosed;
	bool bHasClosed = false;
};
