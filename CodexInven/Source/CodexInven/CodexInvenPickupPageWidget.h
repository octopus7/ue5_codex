#pragma once

#include "SafeFitBackgroundWidget.h"
#include "CodexInvenPickupPageWidget.generated.h"

class UCanvasPanel;

UCLASS()
class CODEXINVEN_API UCodexInvenPickupPageWidget : public USafeFitBackgroundWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCanvasPanel> ContentCanvasPanel = nullptr;
};
