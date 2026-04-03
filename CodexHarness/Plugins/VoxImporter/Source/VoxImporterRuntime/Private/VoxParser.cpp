#include "VoxParser.h"

#include "Containers/ArrayView.h"
#include "Misc/FileHelper.h"

namespace
{
	struct FVoxChunkHeader
	{
		FString Id;
		int32 ContentSize = 0;
		int32 ChildrenSize = 0;
	};

	class FVoxByteReader
	{
	public:
		explicit FVoxByteReader(TConstArrayView<uint8> InBytes)
			: Bytes(InBytes)
		{
		}

		bool ReadInt32(int32& OutValue)
		{
			if (!CanRead(4))
			{
				return false;
			}

			OutValue =
				static_cast<int32>(Bytes[Offset]) |
				(static_cast<int32>(Bytes[Offset + 1]) << 8) |
				(static_cast<int32>(Bytes[Offset + 2]) << 16) |
				(static_cast<int32>(Bytes[Offset + 3]) << 24);
			Offset += 4;
			return true;
		}

		bool ReadUInt8(uint8& OutValue)
		{
			if (!CanRead(1))
			{
				return false;
			}

			OutValue = Bytes[Offset++];
			return true;
		}

		bool ReadChunkHeader(FVoxChunkHeader& OutHeader)
		{
			if (!CanRead(12))
			{
				return false;
			}

			OutHeader.Id.Reset();
			OutHeader.Id.Reserve(4);
			for (int32 Index = 0; Index < 4; ++Index)
			{
				OutHeader.Id.AppendChar(static_cast<TCHAR>(Bytes[Offset + Index]));
			}
			Offset += 4;

			return ReadInt32(OutHeader.ContentSize) && ReadInt32(OutHeader.ChildrenSize);
		}

		bool Seek(int32 NewOffset)
		{
			if (NewOffset < 0 || NewOffset > Bytes.Num())
			{
				return false;
			}

			Offset = NewOffset;
			return true;
		}

		int32 Tell() const
		{
			return Offset;
		}

		int32 Num() const
		{
			return Bytes.Num();
		}

	private:
		bool CanRead(int32 ByteCount) const
		{
			return ByteCount >= 0 && Offset + ByteCount <= Bytes.Num();
		}

		TConstArrayView<uint8> Bytes;
		int32 Offset = 0;
	};

	constexpr uint32 GVoxDefaultPalette[256] =
	{
		0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff, 0xff00ffff, 0xffffccff, 0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff, 0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff,
		0xff6699ff, 0xff3399ff, 0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff, 0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff, 0xff0033ff, 0xffff00ff,
		0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff, 0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc, 0xff66ffcc, 0xff33ffcc, 0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
		0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc, 0xff0099cc, 0xffff66cc, 0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc, 0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc,
		0xff6633cc, 0xff3333cc, 0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc, 0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99, 0xff00ff99, 0xffffcc99,
		0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99, 0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999, 0xff669999, 0xff339999, 0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
		0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399, 0xff003399, 0xffff0099, 0xffcc0099, 0xff990099, 0xff660099, 0xff330099, 0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66,
		0xff66ff66, 0xff33ff66, 0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66, 0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966, 0xff009966, 0xffff6666,
		0xffcc6666, 0xff996666, 0xff666666, 0xff336666, 0xff006666, 0xffff3366, 0xffcc3366, 0xff993366, 0xff663366, 0xff333366, 0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
		0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33, 0xff00ff33, 0xffffcc33, 0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33, 0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933,
		0xff669933, 0xff339933, 0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633, 0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333, 0xff003333, 0xffff0033,
		0xffcc0033, 0xff990033, 0xff660033, 0xff330033, 0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00, 0xff66ff00, 0xff33ff00, 0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
		0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900, 0xff009900, 0xffff6600, 0xffcc6600, 0xff996600, 0xff666600, 0xff336600, 0xff006600, 0xffff3300, 0xffcc3300, 0xff993300,
		0xff663300, 0xff333300, 0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000, 0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077, 0xff000055, 0xff000044,
		0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00, 0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700, 0xff005500, 0xff004400, 0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
		0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000, 0xffeeeeee, 0xffdddddd, 0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777, 0xff555555, 0xff444444, 0xff222222, 0xff111111
	};

	FColor UnpackPaletteColor(uint32 PackedColor)
	{
		return FColor(
			static_cast<uint8>(PackedColor & 0xFF),
			static_cast<uint8>((PackedColor >> 8) & 0xFF),
			static_cast<uint8>((PackedColor >> 16) & 0xFF),
			static_cast<uint8>((PackedColor >> 24) & 0xFF));
	}

	void InitializeDefaultPalette(FVoxModelData& Model)
	{
		Model.Palette.SetNum(256);
		for (int32 Index = 0; Index < 256; ++Index)
		{
			Model.Palette[Index] = UnpackPaletteColor(GVoxDefaultPalette[Index]);
		}
	}
}

bool FVoxParser::ParseFile(const FString& Filename, FVoxModelData& OutModel, FString& OutError)
{
	TArray<uint8> Bytes;
	if (!FFileHelper::LoadFileToArray(Bytes, *Filename))
	{
		OutError = FString::Printf(TEXT("Failed to read .vox file: %s"), *Filename);
		return false;
	}

	return ParseBytes(Bytes, OutModel, OutError);
}

bool FVoxParser::ParseBytes(TConstArrayView<uint8> Bytes, FVoxModelData& OutModel, FString& OutError)
{
	OutModel.Reset();
	InitializeDefaultPalette(OutModel);

	FVoxByteReader Reader(Bytes);
	if (Reader.Num() < 8)
	{
		OutError = TEXT("Invalid .vox file: file is too small.");
		return false;
	}

	FString Magic;
	Magic.Reserve(4);
	for (int32 Index = 0; Index < 4; ++Index)
	{
		uint8 Byte = 0;
		if (!Reader.ReadUInt8(Byte))
		{
			OutError = TEXT("Invalid .vox file: truncated magic header.");
			return false;
		}
		Magic.AppendChar(static_cast<TCHAR>(Byte));
	}

	if (Magic != TEXT("VOX "))
	{
		OutError = TEXT("Invalid .vox file: missing VOX header.");
		return false;
	}

	int32 Version = 0;
	if (!Reader.ReadInt32(Version))
	{
		OutError = TEXT("Invalid .vox file: missing version.");
		return false;
	}

	if (Version != 150)
	{
		OutError = FString::Printf(TEXT("Unsupported .vox version: %d. Expected version 150."), Version);
		return false;
	}

	FVoxChunkHeader MainChunk;
	if (!Reader.ReadChunkHeader(MainChunk))
	{
		OutError = TEXT("Invalid .vox file: missing MAIN chunk.");
		return false;
	}

	if (MainChunk.Id != TEXT("MAIN"))
	{
		OutError = TEXT("Invalid .vox file: expected MAIN chunk.");
		return false;
	}

	if (MainChunk.ContentSize != 0)
	{
		OutError = TEXT("Invalid .vox file: MAIN chunk content must be empty.");
		return false;
	}

	const int32 MainChildrenEnd = Reader.Tell() + MainChunk.ChildrenSize;
	if (MainChildrenEnd > Reader.Num())
	{
		OutError = TEXT("Invalid .vox file: MAIN chunk children extend past file size.");
		return false;
	}

	bool bSawSize = false;
	bool bSawXyzi = false;

	while (Reader.Tell() < MainChildrenEnd)
	{
		FVoxChunkHeader ChunkHeader;
		if (!Reader.ReadChunkHeader(ChunkHeader))
		{
			OutError = TEXT("Invalid .vox file: truncated chunk header.");
			return false;
		}

		const int32 ChunkContentStart = Reader.Tell();
		const int32 ChunkContentEnd = ChunkContentStart + ChunkHeader.ContentSize;
		const int32 ChunkEnd = ChunkContentEnd + ChunkHeader.ChildrenSize;

		if (ChunkEnd > MainChildrenEnd || ChunkContentEnd > MainChildrenEnd)
		{
			OutError = FString::Printf(TEXT("Invalid .vox file: chunk '%s' exceeds MAIN bounds."), *ChunkHeader.Id);
			return false;
		}

		if (ChunkHeader.Id == TEXT("PACK"))
		{
			OutError = TEXT("Unsupported .vox content: PACK/multi-model files are not supported in v1.");
			return false;
		}

		if (ChunkHeader.Id == TEXT("nTRN") || ChunkHeader.Id == TEXT("nGRP") || ChunkHeader.Id == TEXT("nSHP"))
		{
			OutError = FString::Printf(TEXT("Unsupported .vox content: scene graph chunk '%s' is not supported in v1."), *ChunkHeader.Id);
			return false;
		}

		if (ChunkHeader.Id != TEXT("SIZE") && ChunkHeader.Id != TEXT("XYZI") && ChunkHeader.Id != TEXT("RGBA"))
		{
			OutError = FString::Printf(TEXT("Unsupported .vox content: chunk '%s' is not supported in v1."), *ChunkHeader.Id);
			return false;
		}

		if (ChunkHeader.Id == TEXT("SIZE"))
		{
			if (bSawSize)
			{
				OutError = TEXT("Unsupported .vox content: multiple SIZE chunks indicate multiple models.");
				return false;
			}

			if (ChunkHeader.ContentSize != 12)
			{
				OutError = TEXT("Invalid .vox file: SIZE chunk must be exactly 12 bytes.");
				return false;
			}

			int32 SizeX = 0;
			int32 SizeY = 0;
			int32 SizeZ = 0;
			if (!Reader.ReadInt32(SizeX) || !Reader.ReadInt32(SizeY) || !Reader.ReadInt32(SizeZ))
			{
				OutError = TEXT("Invalid .vox file: truncated SIZE chunk.");
				return false;
			}

			if (SizeX <= 0 || SizeY <= 0 || SizeZ <= 0 || SizeX > 256 || SizeY > 256 || SizeZ > 256)
			{
				OutError = FString::Printf(TEXT("Invalid .vox SIZE: %d x %d x %d. Supported range is 1..256 per axis."), SizeX, SizeY, SizeZ);
				return false;
			}

			OutModel.Size = FIntVector(SizeX, SizeY, SizeZ);
			bSawSize = true;
		}
		else if (ChunkHeader.Id == TEXT("XYZI"))
		{
			if (!bSawSize)
			{
				OutError = TEXT("Invalid .vox file: XYZI chunk appeared before SIZE.");
				return false;
			}

			if (bSawXyzi)
			{
				OutError = TEXT("Unsupported .vox content: multiple XYZI chunks indicate multiple models.");
				return false;
			}

			int32 VoxelCount = 0;
			if (!Reader.ReadInt32(VoxelCount))
			{
				OutError = TEXT("Invalid .vox file: truncated XYZI voxel count.");
				return false;
			}

			if (VoxelCount < 0 || ChunkHeader.ContentSize != 4 + (VoxelCount * 4))
			{
				OutError = TEXT("Invalid .vox file: XYZI chunk size does not match voxel count.");
				return false;
			}

			OutModel.Voxels.Reset(VoxelCount);
			for (int32 VoxelIndex = 0; VoxelIndex < VoxelCount; ++VoxelIndex)
			{
				uint8 X = 0;
				uint8 Y = 0;
				uint8 Z = 0;
				uint8 ColorIndex = 0;

				if (!Reader.ReadUInt8(X) || !Reader.ReadUInt8(Y) || !Reader.ReadUInt8(Z) || !Reader.ReadUInt8(ColorIndex))
				{
					OutError = TEXT("Invalid .vox file: truncated voxel data.");
					return false;
				}

				if (X >= OutModel.Size.X || Y >= OutModel.Size.Y || Z >= OutModel.Size.Z)
				{
					OutError = FString::Printf(TEXT("Invalid .vox file: voxel coordinate (%d, %d, %d) is outside SIZE bounds."), X, Y, Z);
					return false;
				}

				if (ColorIndex == 0)
				{
					OutError = TEXT("Invalid .vox file: voxel color index 0 is reserved.");
					return false;
				}

				FVoxVoxel& Voxel = OutModel.Voxels.AddDefaulted_GetRef();
				Voxel.X = X;
				Voxel.Y = Y;
				Voxel.Z = Z;
				Voxel.ColorIndex = ColorIndex;
			}

			bSawXyzi = true;
		}
		else if (ChunkHeader.Id == TEXT("RGBA"))
		{
			if (ChunkHeader.ContentSize != 1024)
			{
				OutError = TEXT("Invalid .vox file: RGBA chunk must be exactly 1024 bytes.");
				return false;
			}

			OutModel.Palette.SetNum(256);
			OutModel.Palette[0] = FColor(0, 0, 0, 0);

			for (int32 PaletteIndex = 0; PaletteIndex < 256; ++PaletteIndex)
			{
				uint8 R = 0;
				uint8 G = 0;
				uint8 B = 0;
				uint8 A = 0;
				if (!Reader.ReadUInt8(R) || !Reader.ReadUInt8(G) || !Reader.ReadUInt8(B) || !Reader.ReadUInt8(A))
				{
					OutError = TEXT("Invalid .vox file: truncated RGBA palette.");
					return false;
				}

				if (PaletteIndex <= 254)
				{
					OutModel.Palette[PaletteIndex + 1] = FColor(R, G, B, A);
				}
			}
		}

		if (!Reader.Seek(ChunkEnd))
		{
			OutError = FString::Printf(TEXT("Invalid .vox file: failed to advance after chunk '%s'."), *ChunkHeader.Id);
			return false;
		}
	}

	if (!bSawSize || !bSawXyzi)
	{
		OutError = TEXT("Invalid .vox file: both SIZE and XYZI chunks are required.");
		return false;
	}

	return true;
}
