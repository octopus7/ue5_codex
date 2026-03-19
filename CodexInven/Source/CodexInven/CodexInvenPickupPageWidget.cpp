#include "CodexInvenPickupPageWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/Texture2D.h"
#include "SafeFitBackgroundWidget.h"

void UCodexInvenPickupPageWidget::SetBackgroundTexture(UTexture2D* InBackgroundTexture)
{
	if (BackgroundTexture == InBackgroundTexture)
	{
		return;
	}

	BackgroundTexture = InBackgroundTexture;
	RefreshBackgroundState();
}

UTexture2D* UCodexInvenPickupPageWidget::GetBackgroundTexture() const
{
	return BackgroundTexture;
}

void UCodexInvenPickupPageWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetIsFocusable(false);
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	BuildWidgetTreeIfNeeded();
	RefreshBackgroundState();
}

void UCodexInvenPickupPageWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	BuildWidgetTreeIfNeeded();
	RefreshBackgroundState();
}

void UCodexInvenPickupPageWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	BuildWidgetTreeIfNeeded();
	RefreshBackgroundState();
}

void UCodexInvenPickupPageWidget::BuildWidgetTreeIfNeeded()
{
	if (WidgetTree == nullptr)
	{
		WidgetTree = NewObject<UWidgetTree>(this, TEXT("PickupPageWidgetTree"));
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

	if (BackgroundWidget == nullptr)
	{
		BackgroundWidget = Cast<USafeFitBackgroundWidget>(WidgetTree->FindWidget(TEXT("BackgroundWidget")));
	}

	if (BackgroundWidget == nullptr && RootCanvasPanel != nullptr)
	{
		BackgroundWidget = WidgetTree->ConstructWidget<USafeFitBackgroundWidget>(USafeFitBackgroundWidget::StaticClass(), TEXT("BackgroundWidget"));
		BackgroundWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (UCanvasPanelSlot* const BackgroundSlot = RootCanvasPanel->AddChildToCanvas(BackgroundWidget))
		{
			BackgroundSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
			BackgroundSlot->SetOffsets(FMargin(0.0f));
		}
	}

	if (ContentCanvasPanel == nullptr)
	{
		ContentCanvasPanel = Cast<UCanvasPanel>(WidgetTree->FindWidget(TEXT("ContentCanvasPanel")));
	}

	if (ContentCanvasPanel == nullptr && RootCanvasPanel != nullptr)
	{
		ContentCanvasPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("ContentCanvasPanel"));
		ContentCanvasPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (UCanvasPanelSlot* const ContentSlot = RootCanvasPanel->AddChildToCanvas(ContentCanvasPanel))
		{
			ContentSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
			ContentSlot->SetOffsets(FMargin(0.0f));
		}
	}
}

void UCodexInvenPickupPageWidget::RefreshBackgroundState()
{
	if (BackgroundWidget == nullptr)
	{
		return;
	}

	BackgroundWidget->SetBackgroundTexture(BackgroundTexture);
}
