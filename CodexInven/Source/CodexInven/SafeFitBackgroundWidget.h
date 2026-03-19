#pragma once

#include "Blueprint/UserWidget.h"
#include "SafeFitBackgroundWidget.generated.h"

class UBorder;
class UCanvasPanel;
class UImage;
class USizeBox;
class UTexture2D;
struct FPropertyChangedEvent;

UCLASS()
class CODEXINVEN_API USafeFitBackgroundWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Safe Fit")
	void SetBackgroundTexture(UTexture2D* InBackgroundTexture);

	UFUNCTION(BlueprintPure, Category = "Safe Fit")
	UTexture2D* GetBackgroundTexture() const;

	UFUNCTION(BlueprintCallable, Category = "Safe Fit")
	void SetNormalizedSafeLayout(
		FVector2D InReferenceCanvasSize,
		FVector2D InSafeCenterNormalized,
		FVector2D InSafeSizeNormalized);

	UFUNCTION(BlueprintCallable, Category = "Safe Fit")
	void SetSafeGuideVisible(bool bInVisible);

	UFUNCTION(BlueprintPure, Category = "Safe Fit")
	bool IsSafeGuideVisible() const;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& InGeometry, float InDeltaTime) override;
	virtual void SynchronizeProperties() override;
#if WITH_EDITOR
	virtual void OnDesignerChanged(const FDesignerChangedEventArgs& EventArgs) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Safe Fit", meta = (AllowPrivateAccess = "true", ClampMin = "1.0", DesignerRebuild))
	FVector2D ReferenceCanvasSize = FVector2D(2340.0f, 1440.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Safe Fit", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0", DesignerRebuild))
	FVector2D SafeCenterNormalized = FVector2D(0.5f, 0.5f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Safe Fit", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0", DesignerRebuild))
	FVector2D SafeSizeNormalized = FVector2D(0.82051283f, 0.75f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance", meta = (AllowPrivateAccess = "true", DesignerRebuild))
	TObjectPtr<UTexture2D> BackgroundTexture = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance", meta = (AllowPrivateAccess = "true", DesignerRebuild))
	FLinearColor BackgroundTint = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guide", meta = (AllowPrivateAccess = "true", DesignerRebuild))
	bool bShowSafeGuideInDesigner = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guide", meta = (AllowPrivateAccess = "true", DesignerRebuild))
	bool bShowSafeGuideAtRuntime = false;

private:
	void RefreshBackgroundBrush();
	void RefreshSafeGuideAppearance();
	void RefreshSafeGuideVisibility();
	void RefreshSafeFitLayout(const FVector2D& InAvailableSize);
	void RefreshDesignTimePreviewLayout();
	FVector2D GetDesignTimePreviewSize() const;
	FVector2D GetCanvasPixelSize() const;
	FVector2D GetValidatedReferenceCanvasSize() const;
	FVector2D GetValidatedSafeCenterNormalized() const;
	FVector2D GetValidatedSafeSizeNormalized(const FVector2D& InCanvasSize, const FVector2D& InSafeMinNormalized) const;
	bool ShouldShowSafeGuide() const;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCanvasPanel> RootCanvasPanel = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<USizeBox> BackgroundSizeBox = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCanvasPanel> BackgroundCanvasPanel = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> BackgroundImage = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> TopBleedTint = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> BottomBleedTint = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> LeftBleedTint = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> RightBleedTint = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> SafeTopEdge = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> SafeBottomEdge = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> SafeLeftEdge = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> SafeRightEdge = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> VerticalCenterLine = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> HorizontalCenterLine = nullptr;

	FVector2D CachedAvailableSize = FVector2D(-1.0f, -1.0f);
	bool bNeedsLayoutRefresh = true;
};
