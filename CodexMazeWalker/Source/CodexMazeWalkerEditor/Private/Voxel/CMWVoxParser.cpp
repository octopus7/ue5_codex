#include "Voxel/CMWVoxParser.h"

#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"

namespace
{
	struct FCMWVoxEntry
	{
		uint8 X = 0;
		uint8 Y = 0;
		uint8 Z = 0;
		uint8 ColorIndex = 0;
	};

	uint32 ReadUInt32(const TArray<uint8>& Bytes, int32& Offset)
	{
		const uint32 Value =
			static_cast<uint32>(Bytes[Offset]) |
			(static_cast<uint32>(Bytes[Offset + 1]) << 8) |
			(static_cast<uint32>(Bytes[Offset + 2]) << 16) |
			(static_cast<uint32>(Bytes[Offset + 3]) << 24);
		Offset += 4;
		return Value;
	}

	FString ReadChunkId(const TArray<uint8>& Bytes, int32& Offset)
	{
		const FString ChunkId = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(Bytes.GetData() + Offset))).Left(4);
		Offset += 4;
		return ChunkId;
	}

	TArray<FColor> BuildFallbackPalette()
	{
		TArray<FColor> Palette;
		Palette.SetNumZeroed(256);
		Palette[0] = FColor::Transparent;

		for (int32 Index = 1; Index < 256; ++Index)
		{
			FColor Color = FLinearColor::MakeFromHSV8(static_cast<uint8>((Index * 17) % 255), 180, 255).ToFColor(true);
			Color.A = 255;
			Palette[Index] = Color;
		}

		return Palette;
	}

	int32 ToIndex(const FIntVector& Dimensions, int32 X, int32 Y, int32 Z)
	{
		return X + Dimensions.X * (Y + Dimensions.Y * Z);
	}
}

bool FCMWVoxParser::ParseFile(const FString& SourceFilePath, FCMWVoxModelData& OutModelData, FString& OutError)
{
	TArray<uint8> FileBytes;
	if (!FFileHelper::LoadFileToArray(FileBytes, *SourceFilePath))
	{
		OutError = FString::Printf(TEXT("Failed to load VOX file: %s"), *SourceFilePath);
		return false;
	}

	if (FileBytes.Num() < 8)
	{
		OutError = TEXT("VOX file was too small.");
		return false;
	}

	int32 Offset = 0;
	const FString Magic = ReadChunkId(FileBytes, Offset);
	if (Magic != TEXT("VOX "))
	{
		OutError = TEXT("VOX file did not begin with 'VOX '.");
		return false;
	}

	ReadUInt32(FileBytes, Offset);

	FIntVector Dimensions = FIntVector::ZeroValue;
	TArray<FCMWVoxEntry> VoxelEntries;
	TArray<FColor> Palette = BuildFallbackPalette();

	while (Offset + 12 <= FileBytes.Num())
	{
		const FString ChunkId = ReadChunkId(FileBytes, Offset);
		const int32 ContentSize = static_cast<int32>(ReadUInt32(FileBytes, Offset));
		const int32 ChildrenSize = static_cast<int32>(ReadUInt32(FileBytes, Offset));
		const int32 ContentOffset = Offset;

		if (ChunkId == TEXT("SIZE") && ContentSize >= 12)
		{
			Dimensions.X = static_cast<int32>(ReadUInt32(FileBytes, Offset));
			Dimensions.Y = static_cast<int32>(ReadUInt32(FileBytes, Offset));
			Dimensions.Z = static_cast<int32>(ReadUInt32(FileBytes, Offset));
		}
		else if (ChunkId == TEXT("XYZI") && ContentSize >= 4)
		{
			const uint32 VoxelCount = ReadUInt32(FileBytes, Offset);
			VoxelEntries.Reserve(static_cast<int32>(VoxelCount));
			for (uint32 VoxelIndex = 0; VoxelIndex < VoxelCount && Offset + 4 <= FileBytes.Num(); ++VoxelIndex)
			{
				FCMWVoxEntry& Entry = VoxelEntries.AddDefaulted_GetRef();
				Entry.X = FileBytes[Offset++];
				Entry.Y = FileBytes[Offset++];
				Entry.Z = FileBytes[Offset++];
				Entry.ColorIndex = FileBytes[Offset++];
			}
		}
		else if (ChunkId == TEXT("RGBA") && ContentSize >= 1024)
		{
			Palette.SetNumZeroed(256);
			Palette[0] = FColor::Transparent;
			for (int32 ColorIndex = 1; ColorIndex < 256 && Offset + 4 <= FileBytes.Num(); ++ColorIndex)
			{
				FColor Color;
				Color.R = FileBytes[Offset++];
				Color.G = FileBytes[Offset++];
				Color.B = FileBytes[Offset++];
				Color.A = FileBytes[Offset++];
				Palette[ColorIndex] = Color;
			}
		}

		Offset = ContentOffset + ContentSize + ChildrenSize;
	}

	if (Dimensions.X <= 0 || Dimensions.Y <= 0 || Dimensions.Z <= 0)
	{
		OutError = TEXT("VOX file did not contain a valid SIZE chunk.");
		return false;
	}

	if (Dimensions.X > 32 || Dimensions.Y > 32 || Dimensions.Z > 32)
	{
		OutError = FString::Printf(TEXT("VOX dimensions %s exceed the supported 32x32x32 limit."), *Dimensions.ToString());
		return false;
	}

	OutModelData.Dimensions = Dimensions;
	OutModelData.Voxels.SetNumZeroed(Dimensions.X * Dimensions.Y * Dimensions.Z);

	for (const FCMWVoxEntry& Entry : VoxelEntries)
	{
		if (Entry.X >= Dimensions.X || Entry.Y >= Dimensions.Y || Entry.Z >= Dimensions.Z)
		{
			continue;
		}

		const int32 LinearIndex = ToIndex(Dimensions, Entry.X, Entry.Y, Entry.Z);
		OutModelData.Voxels[LinearIndex] = Palette.IsValidIndex(Entry.ColorIndex) ? Palette[Entry.ColorIndex] : FColor::White;
		OutModelData.Voxels[LinearIndex].A = 255;
	}

	return true;
}
