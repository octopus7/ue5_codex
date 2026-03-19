#include "SafeFitBackgroundWidget.h"

#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateBrush.h"
#include "UObject/UnrealType.h"

namespace
{
	const FLinearColor BleedTintColor(0.84f, 0.36f, 0.20f, 0.22f);
	const FLinearColor SafeEdgeColor(0.18f, 0.92f, 0.62f, 0.96f);
	const FLinearColor CenterLineColor(1.0f, 1.0f, 1.0f, 0.45f);

	void SetCanvasSlotRect(UWidget* InWidget, const FVector2D& InPosition, const FVector2D& InSize)
	{
		if (UCanvasPanelSlot* const CanvasSlot = InWidget != nullptr ? Cast<UCanvasPanelSlot>(InWidget->Slot) : nullptr)
		{
			CanvasSlot->SetPosition(InPosition);
			CanvasSlot->SetSize(FVector2D(FMath::Max(InSize.X, 0.0f), FMath::Max(InSize.Y, 0.0f)));
		}
	}
}

void USafeFitBackgroundWidget::SetBackgroundTexture(UTexture2D* InBackgroundTexture)
{
	if (BackgroundTexture == InBackgroundTexture)
	{
		return;
	}

	BackgroundTexture = InBackgroundTexture;
	RefreshBackgroundBrush();
}

UTexture2D* USafeFitBackgroundWidget::GetBackgroundTexture() const
{
	return BackgroundTexture;
}

void USafeFitBackgroundWidget::SetNormalizedSafeLayout(
	const FVector2D InReferenceCanvasSize,
	const FVector2D InSafeCenterNormalized,
	const FVector2D InSafeSizeNormalized)
{
	ReferenceCanvasSize = InReferenceCanvasSize;
	SafeCenterNormalized = InSafeCenterNormalized;
	SafeSizeNormalized = InSafeSizeNormalized;
	bNeedsLayoutRefresh = true;
	RefreshBackgroundBrush();
}

void USafeFitBackgroundWidget::SetSafeGuideVisible(const bool bInVisible)
{
	if (bShowSafeGuideAtRuntime == bInVisible)
	{
		return;
	}

	bShowSafeGuideAtRuntime = bInVisible;
	RefreshSafeGuideVisibility();
}

bool USafeFitBackgroundWidget::IsSafeGuideVisible() const
{
	return ShouldShowSafeGuide();
}

void USafeFitBackgroundWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetIsFocusable(false);
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	RefreshBackgroundBrush();
	RefreshSafeGuideAppearance();
	RefreshSafeGuideVisibility();
	RefreshDesignTimePreviewLayout();
}

void USafeFitBackgroundWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	RefreshBackgroundBrush();
	RefreshSafeGuideAppearance();
	RefreshSafeGuideVisibility();
	RefreshDesignTimePreviewLayout();
}

void USafeFitBackgroundWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RefreshBackgroundBrush();
	RefreshSafeGuideAppearance();
	RefreshSafeGuideVisibility();
	RefreshDesignTimePreviewLayout();
	bNeedsLayoutRefresh = true;
}

void USafeFitBackgroundWidget::NativeTick(const FGeometry& InGeometry, const float InDeltaTime)
{
	Super::NativeTick(InGeometry, InDeltaTime);

	const FVector2D AvailableSize = InGeometry.GetLocalSize();
	if (!bNeedsLayoutRefresh && CachedAvailableSize.Equals(AvailableSize, 0.1f))
	{
		return;
	}

	RefreshSafeFitLayout(AvailableSize);
}

void USafeFitBackgroundWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	RefreshBackgroundBrush();
	RefreshSafeGuideAppearance();
	RefreshSafeGuideVisibility();
	RefreshDesignTimePreviewLayout();
}

#if WITH_EDITOR
void USafeFitBackgroundWidget::OnDesignerChanged(const FDesignerChangedEventArgs& EventArgs)
{
	Super::OnDesignerChanged(EventArgs);

	bNeedsLayoutRefresh = true;
	if (EventArgs.Size.X > 0.0f && EventArgs.Size.Y > 0.0f)
	{
		RefreshSafeFitLayout(EventArgs.Size);
	}
}

void USafeFitBackgroundWidget::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	bNeedsLayoutRefresh = true;
	RefreshBackgroundBrush();
	RefreshSafeGuideAppearance();
	RefreshSafeGuideVisibility();
	InvalidateLayoutAndVolatility();
	ForceLayoutPrepass();
	RefreshDesignTimePreviewLayout();
}
#endif

void USafeFitBackgroundWidget::RefreshBackgroundBrush()
{
	if (BackgroundImage == nullptr)
	{
		return;
	}

	FSlateBrush Brush = BackgroundImage->GetBrush();
	Brush.SetResourceObject(BackgroundTexture);
	Brush.SetImageSize(GetCanvasPixelSize());
	BackgroundImage->SetBrush(Brush);
	BackgroundImage->SetColorAndOpacity(BackgroundTint);
	bNeedsLayoutRefresh = true;
	RefreshSafeGuideVisibility();

	if (CachedAvailableSize.X > 0.0f && CachedAvailableSize.Y > 0.0f)
	{
		RefreshSafeFitLayout(CachedAvailableSize);
	}
	else
	{
		RefreshDesignTimePreviewLayout();
	}
}

void USafeFitBackgroundWidget::RefreshSafeGuideAppearance()
{
	if (TopBleedTint != nullptr)
	{
		TopBleedTint->SetBrushColor(BleedTintColor);
	}

	if (BottomBleedTint != nullptr)
	{
		BottomBleedTint->SetBrushColor(BleedTintColor);
	}

	if (LeftBleedTint != nullptr)
	{
		LeftBleedTint->SetBrushColor(BleedTintColor);
	}

	if (RightBleedTint != nullptr)
	{
		RightBleedTint->SetBrushColor(BleedTintColor);
	}

	if (SafeTopEdge != nullptr)
	{
		SafeTopEdge->SetBrushColor(SafeEdgeColor);
	}

	if (SafeBottomEdge != nullptr)
	{
		SafeBottomEdge->SetBrushColor(SafeEdgeColor);
	}

	if (SafeLeftEdge != nullptr)
	{
		SafeLeftEdge->SetBrushColor(SafeEdgeColor);
	}

	if (SafeRightEdge != nullptr)
	{
		SafeRightEdge->SetBrushColor(SafeEdgeColor);
	}

	if (VerticalCenterLine != nullptr)
	{
		VerticalCenterLine->SetBrushColor(CenterLineColor);
	}

	if (HorizontalCenterLine != nullptr)
	{
		HorizontalCenterLine->SetBrushColor(CenterLineColor);
	}
}

void USafeFitBackgroundWidget::RefreshSafeGuideVisibility()
{
	const ESlateVisibility GuideVisibility = ShouldShowSafeGuide() ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed;

	if (TopBleedTint != nullptr)
	{
		TopBleedTint->SetVisibility(GuideVisibility);
	}

	if (BottomBleedTint != nullptr)
	{
		BottomBleedTint->SetVisibility(GuideVisibility);
	}

	if (LeftBleedTint != nullptr)
	{
		LeftBleedTint->SetVisibility(GuideVisibility);
	}

	if (RightBleedTint != nullptr)
	{
		RightBleedTint->SetVisibility(GuideVisibility);
	}

	if (SafeTopEdge != nullptr)
	{
		SafeTopEdge->SetVisibility(GuideVisibility);
	}

	if (SafeBottomEdge != nullptr)
	{
		SafeBottomEdge->SetVisibility(GuideVisibility);
	}

	if (SafeLeftEdge != nullptr)
	{
		SafeLeftEdge->SetVisibility(GuideVisibility);
	}

	if (SafeRightEdge != nullptr)
	{
		SafeRightEdge->SetVisibility(GuideVisibility);
	}

	if (VerticalCenterLine != nullptr)
	{
		VerticalCenterLine->SetVisibility(GuideVisibility);
	}

	if (HorizontalCenterLine != nullptr)
	{
		HorizontalCenterLine->SetVisibility(GuideVisibility);
	}
}

void USafeFitBackgroundWidget::RefreshDesignTimePreviewLayout()
{
	if (!IsDesignTime())
	{
		return;
	}

	const FVector2D PreviewSize = GetDesignTimePreviewSize();
	if (PreviewSize.X > 0.0f && PreviewSize.Y > 0.0f)
	{
		RefreshSafeFitLayout(PreviewSize);
	}
}

FVector2D USafeFitBackgroundWidget::GetDesignTimePreviewSize() const
{
#if WITH_EDITORONLY_DATA
	return FVector2D(
		FMath::Max(DesignTimeSize.X, 0.0f),
		FMath::Max(DesignTimeSize.Y, 0.0f));
#else
	return FVector2D::ZeroVector;
#endif
}

void USafeFitBackgroundWidget::RefreshSafeFitLayout(const FVector2D& InAvailableSize)
{
	if (BackgroundSizeBox == nullptr)
	{
		return;
	}

	const FVector2D CanvasSize = GetCanvasPixelSize();
	const FVector2D SafeCenterNormalizedClamped = GetValidatedSafeCenterNormalized();
	const FVector2D SafeHalfSizeNormalized = SafeSizeNormalized * 0.5f;
	const FVector2D SafeMinFromCenter = SafeCenterNormalizedClamped - SafeHalfSizeNormalized;
	const FVector2D SafeMinNormalizedClamped = FVector2D(
		FMath::Clamp(SafeMinFromCenter.X, 0.0f, 1.0f),
		FMath::Clamp(SafeMinFromCenter.Y, 0.0f, 1.0f));
	const FVector2D SafeSizeNormalizedClamped = GetValidatedSafeSizeNormalized(CanvasSize, SafeMinNormalizedClamped);
	const FVector2D SafeMin = FVector2D(
		FMath::Clamp(SafeCenterNormalizedClamped.X - (SafeSizeNormalizedClamped.X * 0.5f), 0.0f, 1.0f - SafeSizeNormalizedClamped.X),
		FMath::Clamp(SafeCenterNormalizedClamped.Y - (SafeSizeNormalizedClamped.Y * 0.5f), 0.0f, 1.0f - SafeSizeNormalizedClamped.Y));
	const FVector2D SafeOrigin = CanvasSize * SafeMin;
	const FVector2D SafeSize = CanvasSize * SafeSizeNormalizedClamped;
	const FVector2D SafeMax = SafeOrigin + SafeSize;
	const FVector2D AvailableSize(FMath::Max(InAvailableSize.X, 1.0f), FMath::Max(InAvailableSize.Y, 1.0f));
	const FVector2D SafeCenterForLayout = SafeMin + (SafeSizeNormalizedClamped * 0.5f);
	const float OutlineThickness = FMath::Max(2.0f, FMath::RoundToFloat(FMath::Min(CanvasSize.X, CanvasSize.Y) * 0.002f));
	const float CenterLineThickness = FMath::Max(1.0f, FMath::RoundToFloat(OutlineThickness * 0.5f));

	BackgroundSizeBox->SetWidthOverride(CanvasSize.X);
	BackgroundSizeBox->SetHeightOverride(CanvasSize.Y);
	BackgroundSizeBox->SetRenderTransformPivot(SafeCenterForLayout);

	// Scale the authored canvas from the hard-safe rect. The parent viewport clips any extra bleed.
	const float RenderScale = FMath::Min(AvailableSize.X / SafeSize.X, AvailableSize.Y / SafeSize.Y);
	BackgroundSizeBox->SetRenderScale(FVector2D(RenderScale, RenderScale));

	if (UCanvasPanelSlot* const BackgroundSlot = Cast<UCanvasPanelSlot>(BackgroundSizeBox->Slot))
	{
		BackgroundSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
		BackgroundSlot->SetAlignment(SafeCenterForLayout);
		BackgroundSlot->SetPosition(FVector2D::ZeroVector);
		BackgroundSlot->SetSize(CanvasSize);
		BackgroundSlot->SetAutoSize(false);
	}

	SetCanvasSlotRect(TopBleedTint, FVector2D(0.0f, 0.0f), FVector2D(CanvasSize.X, SafeOrigin.Y));
	SetCanvasSlotRect(BottomBleedTint, FVector2D(0.0f, SafeMax.Y), FVector2D(CanvasSize.X, CanvasSize.Y - SafeMax.Y));
	SetCanvasSlotRect(LeftBleedTint, FVector2D(0.0f, SafeOrigin.Y), FVector2D(SafeOrigin.X, SafeSize.Y));
	SetCanvasSlotRect(RightBleedTint, FVector2D(SafeMax.X, SafeOrigin.Y), FVector2D(CanvasSize.X - SafeMax.X, SafeSize.Y));

	SetCanvasSlotRect(SafeTopEdge, SafeOrigin, FVector2D(SafeSize.X, OutlineThickness));
	SetCanvasSlotRect(SafeBottomEdge, FVector2D(SafeOrigin.X, SafeMax.Y - OutlineThickness), FVector2D(SafeSize.X, OutlineThickness));
	SetCanvasSlotRect(SafeLeftEdge, SafeOrigin, FVector2D(OutlineThickness, SafeSize.Y));
	SetCanvasSlotRect(SafeRightEdge, FVector2D(SafeMax.X - OutlineThickness, SafeOrigin.Y), FVector2D(OutlineThickness, SafeSize.Y));

	SetCanvasSlotRect(
		VerticalCenterLine,
		FVector2D((CanvasSize.X * 0.5f) - (CenterLineThickness * 0.5f), 0.0f),
		FVector2D(CenterLineThickness, CanvasSize.Y));
	SetCanvasSlotRect(
		HorizontalCenterLine,
		FVector2D(0.0f, (CanvasSize.Y * 0.5f) - (CenterLineThickness * 0.5f)),
		FVector2D(CanvasSize.X, CenterLineThickness));

	CachedAvailableSize = InAvailableSize;
	bNeedsLayoutRefresh = false;
}

FVector2D USafeFitBackgroundWidget::GetCanvasPixelSize() const
{
	if (BackgroundTexture != nullptr && BackgroundTexture->GetSizeX() > 0 && BackgroundTexture->GetSizeY() > 0)
	{
		return FVector2D(static_cast<float>(BackgroundTexture->GetSizeX()), static_cast<float>(BackgroundTexture->GetSizeY()));
	}

	return GetValidatedReferenceCanvasSize();
}

FVector2D USafeFitBackgroundWidget::GetValidatedReferenceCanvasSize() const
{
	return FVector2D(
		FMath::Max(ReferenceCanvasSize.X, 1.0f),
		FMath::Max(ReferenceCanvasSize.Y, 1.0f));
}

FVector2D USafeFitBackgroundWidget::GetValidatedSafeCenterNormalized() const
{
	return FVector2D(
		FMath::Clamp(SafeCenterNormalized.X, 0.0f, 1.0f),
		FMath::Clamp(SafeCenterNormalized.Y, 0.0f, 1.0f));
}

FVector2D USafeFitBackgroundWidget::GetValidatedSafeSizeNormalized(
	const FVector2D& InCanvasSize,
	const FVector2D& InSafeMinNormalized) const
{
	const FVector2D MinNormalized(
		1.0f / FMath::Max(InCanvasSize.X, 1.0f),
		1.0f / FMath::Max(InCanvasSize.Y, 1.0f));

	return FVector2D(
		FMath::Clamp(SafeSizeNormalized.X, MinNormalized.X, 1.0f - InSafeMinNormalized.X),
		FMath::Clamp(SafeSizeNormalized.Y, MinNormalized.Y, 1.0f - InSafeMinNormalized.Y));
}

bool USafeFitBackgroundWidget::ShouldShowSafeGuide() const
{
	return IsDesignTime() ? bShowSafeGuideInDesigner : bShowSafeGuideAtRuntime;
}
