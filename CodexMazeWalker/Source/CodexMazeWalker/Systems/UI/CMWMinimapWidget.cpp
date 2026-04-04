#include "Systems/UI/CMWMinimapWidget.h"

#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"
#include "Systems/Game/CMWGameDataAsset.h"
#include "Systems/Game/CMWGameInstance.h"

void UCMWMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	float RefreshInterval = 0.2f;
	if (const UCMWGameInstance* GameInstance = UCMWGameInstance::Get(this))
	{
		if (const UCMWGameDataAsset* GameData = GameInstance->GetGameData())
		{
			VisibleTileRadius = FMath::Max(1, GameData->MinimapVisibleTileRadius);
			RefreshInterval = FMath::Max(0.05f, GameData->MinimapRefreshIntervalSeconds);
		}
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(RefreshTimerHandle, this, &ThisClass::RefreshFromSubsystem, RefreshInterval, true, 0.0f);
	}
}

void UCMWMinimapWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RefreshTimerHandle);
	}

	Super::NativeDestruct();
}

int32 UCMWMinimapWidget::NativePaint(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	const FSlateBrush* WhiteBrush = FCoreStyle::Get().GetBrush(TEXT("WhiteBrush"));
	const FVector2D LocalSize = AllottedGeometry.GetLocalSize();

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(FVector2f(LocalSize), FSlateLayoutTransform(FVector2f::ZeroVector)),
		WhiteBrush,
		ESlateDrawEffect::None,
		BackgroundColor);

	const int32 VisibleDiameter = VisibleTileRadius * 2 + 1;
	const float TileSize = FMath::Min(LocalSize.X, LocalSize.Y) / static_cast<float>(VisibleDiameter);
	const FVector2D GridSize(TileSize * VisibleDiameter, TileSize * VisibleDiameter);
	const FVector2D GridOrigin = (LocalSize - GridSize) * 0.5f;

	for (const FCMWMinimapTileSnapshot& Tile : CachedTiles)
	{
		const FIntPoint RelativeTile = Tile.TileCoord - CachedCenterTile;
		if (FMath::Abs(RelativeTile.X) > VisibleTileRadius || FMath::Abs(RelativeTile.Y) > VisibleTileRadius)
		{
			continue;
		}

		const FVector2D TilePosition(
			GridOrigin.X + (RelativeTile.X + VisibleTileRadius) * TileSize,
			GridOrigin.Y + (-RelativeTile.Y + VisibleTileRadius) * TileSize);

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId + 1,
			AllottedGeometry.ToPaintGeometry(FVector2f(FVector2D(TileSize, TileSize)), FSlateLayoutTransform(FVector2f(TilePosition))),
			WhiteBrush,
			ESlateDrawEffect::None,
			Tile.Color);
	}

	const float MarkerSize = TileSize * 0.45f;
	const FVector2D MarkerPosition = GridOrigin + FVector2D(VisibleTileRadius * TileSize, VisibleTileRadius * TileSize) + FVector2D((TileSize - MarkerSize) * 0.5f, (TileSize - MarkerSize) * 0.5f);
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 2,
		AllottedGeometry.ToPaintGeometry(FVector2f(FVector2D(MarkerSize, MarkerSize)), FSlateLayoutTransform(FVector2f(MarkerPosition))),
		WhiteBrush,
		ESlateDrawEffect::None,
		PlayerMarkerColor);

	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId + 3, InWidgetStyle, bParentEnabled);
}

void UCMWMinimapWidget::RefreshFromSubsystem()
{
	UWorld* World = GetWorld();
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!World || !OwningPawn)
	{
		return;
	}

	UCMWMinimapSubsystem* MinimapSubsystem = World->GetSubsystem<UCMWMinimapSubsystem>();
	if (!MinimapSubsystem)
	{
		return;
	}

	TArray<FCMWMinimapTileSnapshot> NewTiles;
	FIntPoint NewCenterTile = CachedCenterTile;
	int32 NewRevision = CachedRevision;
	if (MinimapSubsystem->QuerySnapshot(OwningPawn->GetActorLocation(), VisibleTileRadius, NewRevision, NewCenterTile, NewTiles))
	{
		CachedTiles = MoveTemp(NewTiles);
		CachedCenterTile = NewCenterTile;
		CachedRevision = NewRevision;
		Invalidate(EInvalidateWidgetReason::Paint);
	}
}
