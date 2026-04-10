#include "Vox/CodexVoxParser.h"

#include "Misc/FileHelper.h"

namespace
{
	class FCodexVoxBinaryReader
	{
	public:
		explicit FCodexVoxBinaryReader(const TArray<uint8>& InBytes)
			: Bytes(InBytes)
		{
		}

		bool ReadUInt8(uint8& OutValue)
		{
			if (!CanRead(sizeof(uint8)))
			{
				return false;
			}

			OutValue = Bytes[Offset];
			++Offset;
			return true;
		}

		bool ReadInt32(int32& OutValue)
		{
			if (!CanRead(sizeof(int32)))
			{
				return false;
			}

			FMemory::Memcpy(&OutValue, Bytes.GetData() + Offset, sizeof(int32));
			Offset += sizeof(int32);
			return true;
		}

		bool ReadChunkId(FString& OutChunkId)
		{
			if (!CanRead(4))
			{
				return false;
			}

			ANSICHAR RawId[5] = {};
			FMemory::Memcpy(RawId, Bytes.GetData() + Offset, 4);
			Offset += 4;
			OutChunkId = UTF8_TO_TCHAR(RawId);
			return true;
		}

		bool Skip(int32 ByteCount)
		{
			if (!CanRead(ByteCount))
			{
				return false;
			}

			Offset += ByteCount;
			return true;
		}

		int32 GetOffset() const
		{
			return Offset;
		}

		void SetOffset(int32 NewOffset)
		{
			Offset = NewOffset;
		}

	private:
		bool CanRead(int32 ByteCount) const
		{
			return ByteCount >= 0 && Offset + ByteCount <= Bytes.Num();
		}

		const TArray<uint8>& Bytes;
		int32 Offset = 0;
	};

	struct FCodexVoxChunkHeader
	{
		FString Id;
		int32 ContentSize = 0;
		int32 ChildrenSize = 0;
	};

	bool ReadChunkHeader(FCodexVoxBinaryReader& Reader, FCodexVoxChunkHeader& OutHeader)
	{
		return Reader.ReadChunkId(OutHeader.Id)
			&& Reader.ReadInt32(OutHeader.ContentSize)
			&& Reader.ReadInt32(OutHeader.ChildrenSize);
	}
}

bool FCodexVoxParser::LoadModelFromFile(const FString& FilePath, CodexVox::FParsedModel& OutModel, FString& OutError)
{
	TArray<uint8> Bytes;
	if (!FFileHelper::LoadFileToArray(Bytes, *FilePath))
	{
		OutError = FString::Printf(TEXT("Failed to load VOX file '%s'."), *FilePath);
		return false;
	}

	FCodexVoxBinaryReader Reader(Bytes);

	FString Magic;
	int32 Version = 0;
	if (!Reader.ReadChunkId(Magic) || !Reader.ReadInt32(Version))
	{
		OutError = FString::Printf(TEXT("VOX file '%s' is truncated."), *FilePath);
		return false;
	}

	if (Magic != TEXT("VOX "))
	{
		OutError = FString::Printf(TEXT("VOX file '%s' has an invalid magic header."), *FilePath);
		return false;
	}

	FCodexVoxChunkHeader MainHeader;
	if (!ReadChunkHeader(Reader, MainHeader))
	{
		OutError = FString::Printf(TEXT("VOX file '%s' is missing a MAIN chunk."), *FilePath);
		return false;
	}

	if (MainHeader.Id != TEXT("MAIN"))
	{
		OutError = FString::Printf(TEXT("VOX file '%s' does not start with a MAIN chunk."), *FilePath);
		return false;
	}

	if (!Reader.Skip(MainHeader.ContentSize))
	{
		OutError = FString::Printf(TEXT("VOX file '%s' has an invalid MAIN chunk size."), *FilePath);
		return false;
	}

	const int32 MainChildrenEnd = Reader.GetOffset() + MainHeader.ChildrenSize;
	if (MainChildrenEnd > Bytes.Num())
	{
		OutError = FString::Printf(TEXT("VOX file '%s' contains an invalid child chunk size."), *FilePath);
		return false;
	}

	bool bSawSizeChunk = false;
	bool bSawVoxelChunk = false;
	bool bSawPaletteChunk = false;

	OutModel = CodexVox::FParsedModel();
	OutModel.Palette.SetNumZeroed(256);

	while (Reader.GetOffset() < MainChildrenEnd)
	{
		FCodexVoxChunkHeader ChunkHeader;
		if (!ReadChunkHeader(Reader, ChunkHeader))
		{
			OutError = FString::Printf(TEXT("VOX file '%s' ended while reading a chunk header."), *FilePath);
			return false;
		}

		const int32 ContentStart = Reader.GetOffset();
		const int32 ChildStart = ContentStart + ChunkHeader.ContentSize;
		const int32 ChildEnd = ChildStart + ChunkHeader.ChildrenSize;
		if (ChildEnd > MainChildrenEnd)
		{
			OutError = FString::Printf(TEXT("VOX file '%s' contains an invalid chunk size for '%s'."), *FilePath, *ChunkHeader.Id);
			return false;
		}

		if (ChunkHeader.Id == TEXT("PACK"))
		{
			int32 ModelCount = 0;
			if (!Reader.ReadInt32(ModelCount))
			{
				OutError = FString::Printf(TEXT("VOX file '%s' ended while reading PACK."), *FilePath);
				return false;
			}

			if (ModelCount != 1)
			{
				OutError = FString::Printf(TEXT("VOX file '%s' uses PACK=%d, but only a single model is supported."), *FilePath, ModelCount);
				return false;
			}
		}
		else if (ChunkHeader.Id == TEXT("SIZE"))
		{
			if (bSawSizeChunk)
			{
				OutError = FString::Printf(TEXT("VOX file '%s' contains multiple SIZE chunks."), *FilePath);
				return false;
			}

			if (!Reader.ReadInt32(OutModel.Size.X) || !Reader.ReadInt32(OutModel.Size.Y) || !Reader.ReadInt32(OutModel.Size.Z))
			{
				OutError = FString::Printf(TEXT("VOX file '%s' ended while reading SIZE."), *FilePath);
				return false;
			}

			if (OutModel.Size.X != CodexVox::SourceResolution
				|| OutModel.Size.Y != CodexVox::SourceResolution
				|| OutModel.Size.Z != CodexVox::SourceResolution)
			{
				OutError = FString::Printf(
					TEXT("VOX file '%s' has size (%d, %d, %d). Expected %d x %d x %d."),
					*FilePath,
					OutModel.Size.X,
					OutModel.Size.Y,
					OutModel.Size.Z,
					CodexVox::SourceResolution,
					CodexVox::SourceResolution,
					CodexVox::SourceResolution);
				return false;
			}

			bSawSizeChunk = true;
		}
		else if (ChunkHeader.Id == TEXT("XYZI"))
		{
			if (bSawVoxelChunk)
			{
				OutError = FString::Printf(TEXT("VOX file '%s' contains multiple XYZI chunks."), *FilePath);
				return false;
			}

			int32 VoxelCount = 0;
			if (!Reader.ReadInt32(VoxelCount))
			{
				OutError = FString::Printf(TEXT("VOX file '%s' ended while reading XYZI."), *FilePath);
				return false;
			}

			OutModel.Voxels.Reserve(VoxelCount);
			for (int32 VoxelIndex = 0; VoxelIndex < VoxelCount; ++VoxelIndex)
			{
				uint8 X = 0;
				uint8 Y = 0;
				uint8 Z = 0;
				uint8 ColorIndex = 0;
				if (!Reader.ReadUInt8(X) || !Reader.ReadUInt8(Y) || !Reader.ReadUInt8(Z) || !Reader.ReadUInt8(ColorIndex))
				{
					OutError = FString::Printf(TEXT("VOX file '%s' ended while reading voxel %d."), *FilePath, VoxelIndex);
					return false;
				}

				if (ColorIndex == 0)
				{
					OutError = FString::Printf(TEXT("VOX file '%s' contains voxel %d with palette index 0."), *FilePath, VoxelIndex);
					return false;
				}

				OutModel.Voxels.Add({FIntVector(X, Y, Z), ColorIndex});
			}

			bSawVoxelChunk = true;
		}
		else if (ChunkHeader.Id == TEXT("RGBA"))
		{
			if (bSawPaletteChunk)
			{
				OutError = FString::Printf(TEXT("VOX file '%s' contains multiple RGBA chunks."), *FilePath);
				return false;
			}

			if (ChunkHeader.ContentSize != 256 * 4)
			{
				OutError = FString::Printf(TEXT("VOX file '%s' contains an RGBA chunk with invalid size %d."), *FilePath, ChunkHeader.ContentSize);
				return false;
			}

			for (int32 RawIndex = 0; RawIndex < 256; ++RawIndex)
			{
				uint8 R = 0;
				uint8 G = 0;
				uint8 B = 0;
				uint8 A = 0;
				if (!Reader.ReadUInt8(R) || !Reader.ReadUInt8(G) || !Reader.ReadUInt8(B) || !Reader.ReadUInt8(A))
				{
					OutError = FString::Printf(TEXT("VOX file '%s' ended while reading RGBA."), *FilePath);
					return false;
				}

				if (RawIndex < 255)
				{
					OutModel.Palette[RawIndex + 1] = FColor(R, G, B, A);
				}
			}

			OutModel.bHasPalette = true;
			bSawPaletteChunk = true;
		}

		Reader.SetOffset(ChildEnd);
	}

	if (!bSawSizeChunk)
	{
		OutError = FString::Printf(TEXT("VOX file '%s' does not contain a SIZE chunk."), *FilePath);
		return false;
	}

	if (!bSawVoxelChunk)
	{
		OutError = FString::Printf(TEXT("VOX file '%s' does not contain an XYZI chunk."), *FilePath);
		return false;
	}

	if (!bSawPaletteChunk || !OutModel.bHasPalette)
	{
		OutError = FString::Printf(TEXT("VOX file '%s' does not contain an RGBA palette chunk."), *FilePath);
		return false;
	}

	for (const CodexVox::FVoxel& Voxel : OutModel.Voxels)
	{
		if (Voxel.Position.X < 0 || Voxel.Position.X >= OutModel.Size.X
			|| Voxel.Position.Y < 0 || Voxel.Position.Y >= OutModel.Size.Y
			|| Voxel.Position.Z < 0 || Voxel.Position.Z >= OutModel.Size.Z)
		{
			OutError = FString::Printf(
				TEXT("VOX file '%s' contains voxel (%d, %d, %d) outside the declared bounds."),
				*FilePath,
				Voxel.Position.X,
				Voxel.Position.Y,
				Voxel.Position.Z);
			return false;
		}

		if (!OutModel.Palette.IsValidIndex(Voxel.ColorIndex))
		{
			OutError = FString::Printf(TEXT("VOX file '%s' references invalid palette index %u."), *FilePath, Voxel.ColorIndex);
			return false;
		}
	}

	return true;
}
