#pragma once

#include "Blueprint/UserWidget.h"
#include "CodexInvenPickupPageWidget.generated.h"

class UCanvasPanel;
class UTexture2D;
class USafeFitBackgroundWidget;

UCLASS()
class CODEXINVEN_API UCodexInvenPickupPageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Pickup Page")
	void SetBackgroundTexture(UTexture2D* InBackgroundTexture);

	UFUNCTION(BlueprintPure, Category = "Pickup Page")
	UTexture2D* GetBackgroundTexture() const;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void SynchronizeProperties() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup Page", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTexture2D> BackgroundTexture = nullptr;

private:
	void BuildWidgetTreeIfNeeded();
	void RefreshBackgroundState();

	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> RootCanvasPanel = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<USafeFitBackgroundWidget> BackgroundWidget = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> ContentCanvasPanel = nullptr;
};
