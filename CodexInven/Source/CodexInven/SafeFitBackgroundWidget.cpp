#include "SafeFitBackgroundWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateBrush.h"

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

void USafeFitBackgroundWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetIsFocusable(false);
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	BuildWidgetTreeIfNeeded();
	RefreshBackgroundBrush();
}

void USafeFitBackgroundWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	BuildWidgetTreeIfNeeded();
	RefreshBackgroundBrush();
}

void USafeFitBackgroundWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BuildWidgetTreeIfNeeded();
	RefreshBackgroundBrush();
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

	BuildWidgetTreeIfNeeded();
	RefreshBackgroundBrush();
}

void USafeFitBackgroundWidget::BuildWidgetTreeIfNeeded()
{
	if (WidgetTree == nullptr)
	{
		WidgetTree = NewObject<UWidgetTree>(this, TEXT("SafeFitBackgroundWidgetTree"));
	}

	if (RootCanvasPanel == nullptr)
	{
		RootCanvasPanel = Cast<UCanvasPanel>(WidgetTree->FindWidget(TEXT("RootCanvasPanel")));
	}

	if (RootCanvasPanel == nullptr)
	{
		RootCanvasPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvasPanel"));
		RootCanvasPanel->SetClipping(EWidgetClipping::ClipToBounds);
		WidgetTree->RootWidget = RootCanvasPanel;
	}

	if (BackgroundSizeBox == nullptr)
	{
		BackgroundSizeBox = Cast<USizeBox>(WidgetTree->FindWidget(TEXT("BackgroundSizeBox")));
	}

	if (BackgroundSizeBox == nullptr && RootCanvasPanel != nullptr)
	{
		BackgroundSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("BackgroundSizeBox"));
		BackgroundSizeBox->SetVisibility(ESlateVisibility::HitTestInvisible);

		if (UCanvasPanelSlot* const BackgroundSlot = RootCanvasPanel->AddChildToCanvas(BackgroundSizeBox))
		{
			BackgroundSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
			BackgroundSlot->SetAlignment(FVector2D(0.5f, 0.5f));
			BackgroundSlot->SetPosition(FVector2D::ZeroVector);
		}
	}

	if (BackgroundImage == nullptr)
	{
		BackgroundImage = Cast<UImage>(WidgetTree->FindWidget(TEXT("BackgroundImage")));
	}

	if (BackgroundImage == nullptr && BackgroundSizeBox != nullptr)
	{
		BackgroundImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("BackgroundImage"));
		BackgroundImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		BackgroundSizeBox->SetContent(BackgroundImage);
	}
}

void USafeFitBackgroundWidget::RefreshBackgroundBrush()
{
	if (BackgroundImage == nullptr)
	{
		return;
	}

	FSlateBrush Brush = BackgroundImage->GetBrush();
	Brush.SetResourceObject(BackgroundTexture);
	Brush.SetImageSize(GetValidatedCanvasSize());
	BackgroundImage->SetBrush(Brush);
	BackgroundImage->SetColorAndOpacity(BackgroundTint);
	bNeedsLayoutRefresh = true;
}

void USafeFitBackgroundWidget::RefreshSafeFitLayout(const FVector2D& InAvailableSize)
{
	if (BackgroundSizeBox == nullptr)
	{
		return;
	}

	const FVector2D CanvasSize = GetValidatedCanvasSize();
	const FVector2D SafeOrigin = GetValidatedSafeOrigin(CanvasSize);
	const FVector2D SafeSize = GetValidatedSafeSize(CanvasSize, SafeOrigin);
	const FVector2D AvailableSize(FMath::Max(InAvailableSize.X, 1.0f), FMath::Max(InAvailableSize.Y, 1.0f));
	const FVector2D SafeCenterNormalized = (SafeOrigin + (SafeSize * 0.5f)) / CanvasSize;

	BackgroundSizeBox->SetWidthOverride(CanvasSize.X);
	BackgroundSizeBox->SetHeightOverride(CanvasSize.Y);
	BackgroundSizeBox->SetRenderTransformPivot(SafeCenterNormalized);

	// Scale the authored canvas from the hard-safe rect. The parent viewport clips any extra bleed.
	const float RenderScale = FMath::Min(AvailableSize.X / SafeSize.X, AvailableSize.Y / SafeSize.Y);
	BackgroundSizeBox->SetRenderScale(FVector2D(RenderScale, RenderScale));

	if (UCanvasPanelSlot* const BackgroundSlot = Cast<UCanvasPanelSlot>(BackgroundSizeBox->Slot))
	{
		BackgroundSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
		BackgroundSlot->SetAlignment(SafeCenterNormalized);
		BackgroundSlot->SetPosition(FVector2D::ZeroVector);
		BackgroundSlot->SetSize(CanvasSize);
		BackgroundSlot->SetAutoSize(false);
	}

	CachedAvailableSize = InAvailableSize;
	bNeedsLayoutRefresh = false;
}

FVector2D USafeFitBackgroundWidget::GetValidatedCanvasSize() const
{
	return FVector2D(
		FMath::Max(AuthoredCanvasSize.X, 1.0f),
		FMath::Max(AuthoredCanvasSize.Y, 1.0f));
}

FVector2D USafeFitBackgroundWidget::GetValidatedSafeOrigin(const FVector2D& InCanvasSize) const
{
	return FVector2D(
		FMath::Clamp(HardSafeOrigin.X, 0.0f, InCanvasSize.X - 1.0f),
		FMath::Clamp(HardSafeOrigin.Y, 0.0f, InCanvasSize.Y - 1.0f));
}

FVector2D USafeFitBackgroundWidget::GetValidatedSafeSize(const FVector2D& InCanvasSize, const FVector2D& InSafeOrigin) const
{
	return FVector2D(
		FMath::Clamp(HardSafeSize.X, 1.0f, InCanvasSize.X - InSafeOrigin.X),
		FMath::Clamp(HardSafeSize.Y, 1.0f, InCanvasSize.Y - InSafeOrigin.Y));
}
