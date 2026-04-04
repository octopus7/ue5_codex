#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CMWMinimapSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FCMWMinimapTileSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	FIntPoint TileCoord = FIntPoint::ZeroValue;

	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	FLinearColor Color = FLinearColor::Black;

	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	int32 Revision = 0;
};

UCLASS()
class CODEXMAZEWALKER_API UCMWMinimapSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void NotifyPlayerLocation(const FVector& WorldLocation);
	void MarkTileDiscovered(const FIntPoint& TileCoord, const FLinearColor& TileColor);

	bool QuerySnapshot(
		const FVector& CenterWorldLocation,
		int32 VisibleTileRadius,
		int32& InOutRevision,
		FIntPoint& InOutCenterTile,
		TArray<FCMWMinimapTileSnapshot>& OutTiles) const;

	float GetTileWorldSize() const;
	FIntPoint WorldToTile(const FVector& WorldLocation) const;

private:
	struct FTileCacheEntry
	{
		FLinearColor Color = FLinearColor(0.22f, 0.22f, 0.22f, 0.85f);
		bool bDiscovered = false;
		int32 Revision = 0;
	};

	TMap<FIntPoint, FTileCacheEntry> TileCache;
	FIntPoint LastTrackedPlayerTile = FIntPoint(MAX_int32, MAX_int32);
	int32 CurrentRevision = 0;
};
