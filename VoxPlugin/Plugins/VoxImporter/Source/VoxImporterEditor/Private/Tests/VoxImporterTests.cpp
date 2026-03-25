#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "MeshDescription.h"
#include "Math/Color.h"
#include "StaticMeshAttributes.h"
#include "VoxMeshBuilder.h"
#include "VoxParser.h"

namespace
{
	void AppendInt32(TArray<uint8>& Bytes, int32 Value)
	{
		Bytes.Add(static_cast<uint8>(Value & 0xFF));
		Bytes.Add(static_cast<uint8>((Value >> 8) & 0xFF));
		Bytes.Add(static_cast<uint8>((Value >> 16) & 0xFF));
		Bytes.Add(static_cast<uint8>((Value >> 24) & 0xFF));
	}

	void AppendChunk(TArray<uint8>& Bytes, const ANSICHAR* Id, TConstArrayView<uint8> Content)
	{
		Bytes.Append(reinterpret_cast<const uint8*>(Id), 4);
		AppendInt32(Bytes, Content.Num());
		AppendInt32(Bytes, 0);
		Bytes.Append(Content.GetData(), Content.Num());
	}

	TArray<uint8> MakeSingleVoxelFile()
	{
		TArray<uint8> MainChildren;

		TArray<uint8> SizeChunk;
		AppendInt32(SizeChunk, 1);
		AppendInt32(SizeChunk, 1);
		AppendInt32(SizeChunk, 1);
		AppendChunk(MainChildren, "SIZE", SizeChunk);

		TArray<uint8> XyziChunk;
		AppendInt32(XyziChunk, 1);
		XyziChunk.Add(0);
		XyziChunk.Add(0);
		XyziChunk.Add(0);
		XyziChunk.Add(1);
		AppendChunk(MainChildren, "XYZI", XyziChunk);

		TArray<uint8> FileBytes;
		FileBytes.Append(reinterpret_cast<const uint8*>("VOX "), 4);
		AppendInt32(FileBytes, 150);
		FileBytes.Append(reinterpret_cast<const uint8*>("MAIN"), 4);
		AppendInt32(FileBytes, 0);
		AppendInt32(FileBytes, MainChildren.Num());
		FileBytes.Append(MainChildren);
		return FileBytes;
	}

	FBox ComputeVertexBounds(const FMeshDescription& MeshDescription)
	{
		FBox Bounds(EForceInit::ForceInitToZero);
		const TVertexAttributesConstRef<FVector3f> VertexPositions = MeshDescription.VertexAttributes().GetAttributesRef<FVector3f>(MeshAttribute::Vertex::Position);
		for (const FVertexID VertexId : MeshDescription.Vertices().GetElementIDs())
		{
			Bounds += FVector(VertexPositions[VertexId]);
		}
		return Bounds;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVoxParserSingleVoxelTest, "VoxImporter.Parser.SingleVoxel", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FVoxParserSingleVoxelTest::RunTest(const FString& Parameters)
{
	FVoxModelData Model;
	FString ErrorMessage;
	TestTrue(TEXT("Single voxel file parses"), FVoxParser::ParseBytes(MakeSingleVoxelFile(), Model, ErrorMessage));
	TestEqual(TEXT("Voxel count"), Model.Voxels.Num(), 1);
	TestEqual(TEXT("Model size X"), Model.Size.X, 1);
	TestEqual(TEXT("Model size Y"), Model.Size.Y, 1);
	TestEqual(TEXT("Model size Z"), Model.Size.Z, 1);
	TestEqual(TEXT("Palette color index 1 alpha"), static_cast<int32>(Model.GetColor(1).A), 255);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVoxParserRejectsPackTest, "VoxImporter.Parser.RejectsPack", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FVoxParserRejectsPackTest::RunTest(const FString& Parameters)
{
	TArray<uint8> MainChildren;
	TArray<uint8> PackChunk;
	AppendInt32(PackChunk, 2);
	AppendChunk(MainChildren, "PACK", PackChunk);

	TArray<uint8> FileBytes;
	FileBytes.Append(reinterpret_cast<const uint8*>("VOX "), 4);
	AppendInt32(FileBytes, 150);
	FileBytes.Append(reinterpret_cast<const uint8*>("MAIN"), 4);
	AppendInt32(FileBytes, 0);
	AppendInt32(FileBytes, MainChildren.Num());
	FileBytes.Append(MainChildren);

	FVoxModelData Model;
	FString ErrorMessage;
	TestFalse(TEXT("PACK is rejected"), FVoxParser::ParseBytes(FileBytes, Model, ErrorMessage));
	TestTrue(TEXT("Reject reason mentions PACK"), ErrorMessage.Contains(TEXT("PACK")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVoxMeshBuilderCentersBoundsTest, "VoxImporter.MeshBuilder.BoundsCenter", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FVoxMeshBuilderCentersBoundsTest::RunTest(const FString& Parameters)
{
	FVoxModelData Model;
	Model.Size = FIntVector(8, 8, 8);
	Model.Palette.Init(FColor(0, 0, 0, 0), 256);
	Model.Palette[1] = FColor(255, 0, 0, 255);
	Model.Palette[2] = FColor(0, 255, 0, 255);

	FVoxVoxel Voxel;
	Voxel.ColorIndex = 1;
	Voxel.X = 2;
	Voxel.Y = 1;
	Voxel.Z = 0;
	Model.Voxels.Add(Voxel);
	Voxel.X = 3;
	Model.Voxels.Add(Voxel);
	Voxel.X = 4;
	Model.Voxels.Add(Voxel);

	Voxel.ColorIndex = 2;
	Voxel.X = 2;
	Voxel.Y = 2;
	Model.Voxels.Add(Voxel);
	Voxel.X = 3;
	Model.Voxels.Add(Voxel);
	Voxel.X = 4;
	Model.Voxels.Add(Voxel);

	FMeshDescription MeshDescription;
	FString ErrorMessage;
	TestTrue(TEXT("Mesh build succeeds"), FVoxMeshBuilder::BuildMeshDescription(Model, MeshDescription, ErrorMessage));

	const FBox Bounds = ComputeVertexBounds(MeshDescription);
	TestTrue(TEXT("Bounds center is near origin"), Bounds.GetCenter().Equals(FVector::ZeroVector, KINDA_SMALL_NUMBER));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVoxMeshBuilderUsesLinearVertexColorsTest, "VoxImporter.MeshBuilder.LinearVertexColors", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FVoxMeshBuilderUsesLinearVertexColorsTest::RunTest(const FString& Parameters)
{
	FVoxModelData Model;
	Model.Size = FIntVector(1, 1, 1);
	Model.Palette.Init(FColor(0, 0, 0, 0), 256);
	Model.Palette[1] = FColor(68, 68, 68, 255);

	FVoxVoxel Voxel;
	Voxel.X = 0;
	Voxel.Y = 0;
	Voxel.Z = 0;
	Voxel.ColorIndex = 1;
	Model.Voxels.Add(Voxel);

	FMeshDescription MeshDescription;
	FString ErrorMessage;
	TestTrue(TEXT("Mesh build succeeds"), FVoxMeshBuilder::BuildMeshDescription(Model, MeshDescription, ErrorMessage));

	const TVertexInstanceAttributesConstRef<FVector4f> VertexInstanceColors =
		MeshDescription.VertexInstanceAttributes().GetAttributesRef<FVector4f>(MeshAttribute::VertexInstance::Color);
	TestTrue(TEXT("Vertex instance colors exist"), VertexInstanceColors.IsValid());

	const FLinearColor ExpectedLinear = FLinearColor::FromSRGBColor(Model.Palette[1]);
	FVertexInstanceID FirstInstance;
	for (const FVertexInstanceID VertexInstanceId : MeshDescription.VertexInstances().GetElementIDs())
	{
		FirstInstance = VertexInstanceId;
		break;
	}
	const FVector4f StoredColor = VertexInstanceColors[FirstInstance];

	TestTrue(TEXT("Red channel is stored as linear"), FMath::IsNearlyEqual(StoredColor.X, ExpectedLinear.R, KINDA_SMALL_NUMBER));
	TestTrue(TEXT("Green channel is stored as linear"), FMath::IsNearlyEqual(StoredColor.Y, ExpectedLinear.G, KINDA_SMALL_NUMBER));
	TestTrue(TEXT("Blue channel is stored as linear"), FMath::IsNearlyEqual(StoredColor.Z, ExpectedLinear.B, KINDA_SMALL_NUMBER));
	TestTrue(TEXT("Alpha channel is preserved"), FMath::IsNearlyEqual(StoredColor.W, ExpectedLinear.A, KINDA_SMALL_NUMBER));

	return true;
}

#endif
