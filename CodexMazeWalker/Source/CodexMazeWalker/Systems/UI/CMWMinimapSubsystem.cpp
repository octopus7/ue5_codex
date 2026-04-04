#include "Systems/UI/CMWMinimapSubsystem.h"

#include "Systems/Game/CMWGameDataAsset.h"
#include "Systems/Game/CMWGameInstance.h"

void UCMWMinimapSubsystem::NotifyPlayerLocation(const FVector& WorldLocation)
{
	const FIntPoint CurrentTile = WorldToTile(WorldLocation);
	if (CurrentTile == LastTrackedPlayerTile)
	{
		return;
	}

	LastTrackedPlayerTile = CurrentTile;

	for (int32 Y = -1; Y <= 1; ++Y)
	{
		for (int32 X = -1; X <= 1; ++X)
		{
			const bool bIsCenterTile = X == 0 && Y == 0;
			const FLinearColor TileTint = bIsCenterTile
				? FLinearColor(0.35f, 0.8f, 0.45f, 1.0f)
				: FLinearColor(0.22f, 0.22f, 0.22f, 0.85f);
			MarkTileDiscovered(CurrentTile + FIntPoint(X, Y), TileTint);
		}
	}
}

void UCMWMinimapSubsystem::MarkTileDiscovered(const FIntPoint& TileCoord, const FLinearColor& TileColor)
{
	FTileCacheEntry& Entry = TileCache.FindOrAdd(TileCoord);
	if (Entry.bDiscovered && Entry.Color.Equals(TileColor))
	{
		return;
	}

	Entry.bDiscovered = true;
	Entry.Color = TileColor;
	Entry.Revision = ++CurrentRevision;
}

bool UCMWMinimapSubsystem::QuerySnapshot(
	const FVector& CenterWorldLocation,
	int32 VisibleTileRadius,
	int32& InOutRevision,
	FIntPoint& InOutCenterTile,
	TArray<FCMWMinimapTileSnapshot>& OutTiles) const
{
	const FIntPoint CenterTile = WorldToTile(CenterWorldLocation);
	if (InOutRevision == CurrentRevision && InOutCenterTile == CenterTile)
	{
		return false;
	}

	OutTiles.Reset();

	for (int32 Y = -VisibleTileRadius; Y <= VisibleTileRadius; ++Y)
	{
		for (int32 X = -VisibleTileRadius; X <= VisibleTileRadius; ++X)
		{
			const FIntPoint TileCoord = CenterTile + FIntPoint(X, Y);
			const FTileCacheEntry* Entry = TileCache.Find(TileCoord);
			if (!Entry || !Entry->bDiscovered)
			{
				continue;
			}

			FCMWMinimapTileSnapshot Snapshot;
			Snapshot.TileCoord = TileCoord;
			Snapshot.Color = Entry->Color;
			Snapshot.Revision = Entry->Revision;
			OutTiles.Add(Snapshot);
		}
	}

	InOutRevision = CurrentRevision;
	InOutCenterTile = CenterTile;
	return true;
}

float UCMWMinimapSubsystem::GetTileWorldSize() const
{
	if (const UCMWGameInstance* GameInstance = UCMWGameInstance::Get(this))
	{
		if (const UCMWGameDataAsset* GameData = GameInstance->GetGameData())
		{
			return FMath::Max(10.0f, GameData->MinimapTileWorldSize);
		}
	}

	return 400.0f;
}

FIntPoint UCMWMinimapSubsystem::WorldToTile(const FVector& WorldLocation) const
{
	const float TileWorldSize = GetTileWorldSize();
	return FIntPoint(
		FMath::FloorToInt(WorldLocation.X / TileWorldSize),
		FMath::FloorToInt(WorldLocation.Y / TileWorldSize));
}
