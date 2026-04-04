#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Systems/UI/CMWMinimapSubsystem.h"
#include "CMWMinimapWidget.generated.h"

UCLASS(Abstract, Blueprintable)
class CODEXMAZEWALKER_API UCMWMinimapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual int32 NativePaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;

protected:
	void RefreshFromSubsystem();

	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	TArray<FCMWMinimapTileSnapshot> CachedTiles;

	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	FIntPoint CachedCenterTile = FIntPoint(MAX_int32, MAX_int32);

	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	int32 VisibleTileRadius = 6;

	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	FLinearColor PlayerMarkerColor = FLinearColor(0.95f, 0.15f, 0.15f, 1.0f);

	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	FLinearColor BackgroundColor = FLinearColor(0.03f, 0.03f, 0.03f, 0.85f);

private:
	FTimerHandle RefreshTimerHandle;
	int32 CachedRevision = INDEX_NONE;
};
