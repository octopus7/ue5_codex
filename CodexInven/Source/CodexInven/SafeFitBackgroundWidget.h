#pragma once

#include "Blueprint/UserWidget.h"
#include "SafeFitBackgroundWidget.generated.h"

class UCanvasPanel;
class UImage;
class USizeBox;
class UTexture2D;

UCLASS()
class CODEXINVEN_API USafeFitBackgroundWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Safe Fit")
	void SetBackgroundTexture(UTexture2D* InBackgroundTexture);

	UFUNCTION(BlueprintPure, Category = "Safe Fit")
	UTexture2D* GetBackgroundTexture() const;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& InGeometry, float InDeltaTime) override;
	virtual void SynchronizeProperties() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Safe Fit", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	FVector2D AuthoredCanvasSize = FVector2D(2340.0f, 1440.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Safe Fit", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	FVector2D HardSafeOrigin = FVector2D(210.0f, 180.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Safe Fit", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	FVector2D HardSafeSize = FVector2D(1920.0f, 1080.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTexture2D> BackgroundTexture = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance", meta = (AllowPrivateAccess = "true"))
	FLinearColor BackgroundTint = FLinearColor::White;

private:
	void BuildWidgetTreeIfNeeded();
	void RefreshBackgroundBrush();
	void RefreshSafeFitLayout(const FVector2D& InAvailableSize);
	FVector2D GetValidatedCanvasSize() const;
	FVector2D GetValidatedSafeOrigin(const FVector2D& InCanvasSize) const;
	FVector2D GetValidatedSafeSize(const FVector2D& InCanvasSize, const FVector2D& InSafeOrigin) const;

	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> RootCanvasPanel = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<USizeBox> BackgroundSizeBox = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UImage> BackgroundImage = nullptr;

	FVector2D CachedAvailableSize = FVector2D(-1.0f, -1.0f);
	bool bNeedsLayoutRefresh = true;
};
