#include "Misc/AutomationTest.h"

#include "DynamicMesh/DynamicMeshAttributeSet.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "PrototypeCodexMeshBridge.h"
#include "PrototypeMeshGeneration.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPrototypeMeshDslValidationTest,
	"SimpleMeshBuilder.PrototypeMeshBuilder.Validation.RejectsNegativeDimensions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPrototypeMeshDslValidationTest::RunTest(const FString& Parameters)
{
	FPrototypeShapeDsl Dsl;
	Dsl.Version = TEXT("1.0");
	Dsl.MeshName = TEXT("BadMesh");
	Dsl.Units = TEXT("cm");
	Dsl.Pivot = TEXT("base_center");

	FPrototypePrimitiveSpec Primitive;
	Primitive.Type = TEXT("box");
	Primitive.Name = TEXT("invalid_box");
	Primitive.Color = FLinearColor(1.0f, 0.2f, 0.2f, 1.0f);
	Primitive.Width = -100.0;
	Primitive.Depth = 100.0;
	Primitive.Height = 100.0;
	Dsl.Primitives.Add(Primitive);

	FString Error;
	const bool bValid = PrototypeMeshBuilder::ValidateShapeDsl(Dsl, Error);
	TestFalse(TEXT("Validation should reject negative dimensions."), bValid);
	TestFalse(TEXT("Validation should provide an error."), Error.IsEmpty());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPrototypeMeshBoxBuilderTest,
	"SimpleMeshBuilder.PrototypeMeshBuilder.MeshBuild.BoxTriangleCount",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPrototypeMeshBoxBuilderTest::RunTest(const FString& Parameters)
{
	FPrototypeShapeDsl Dsl;
	Dsl.Version = TEXT("1.0");
	Dsl.MeshName = TEXT("UnitBox");
	Dsl.Units = TEXT("cm");
	Dsl.Pivot = TEXT("base_center");

	FPrototypePrimitiveSpec Primitive;
	Primitive.Type = TEXT("box");
	Primitive.Name = TEXT("box");
	Primitive.Color = FLinearColor(0.25f, 0.5f, 0.75f, 1.0f);
	Primitive.Width = 100.0;
	Primitive.Depth = 200.0;
	Primitive.Height = 300.0;
	Primitive.Transform.RotationDeg = FVector(0.0, 90.0, 0.0);
	Dsl.Primitives.Add(Primitive);

	FGeneratedMeshBuffers Buffers;
	FString Error;
	TestTrue(TEXT("BuildMeshBuffers should succeed for a simple box."), PrototypeMeshBuilder::BuildMeshBuffers(Dsl, Buffers, Error));
	TestEqual(TEXT("A box should generate 12 triangles."), Buffers.GetTriangleCount(), 12);
	TestEqual(TEXT("A box triangle soup should generate 36 positions."), Buffers.Positions.Num(), 36);
	TestEqual(TEXT("Generated buffers should include one vertex color per position."), Buffers.Colors.Num(), Buffers.Positions.Num());
	TestTrue(TEXT("Vertex color red should propagate to generated buffers."), FMath::IsNearlyEqual(Buffers.Colors[0].X, 0.25f));
	TestTrue(TEXT("Vertex color green should propagate to generated buffers."), FMath::IsNearlyEqual(Buffers.Colors[0].Y, 0.5f));
	TestTrue(TEXT("Vertex color blue should propagate to generated buffers."), FMath::IsNearlyEqual(Buffers.Colors[0].Z, 0.75f));

	FVector3f MinBounds(FLT_MAX, FLT_MAX, FLT_MAX);
	FVector3f MaxBounds(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (const FVector3f& Position : Buffers.Positions)
	{
		MinBounds.X = FMath::Min(MinBounds.X, Position.X);
		MinBounds.Y = FMath::Min(MinBounds.Y, Position.Y);
		MinBounds.Z = FMath::Min(MinBounds.Z, Position.Z);
		MaxBounds.X = FMath::Max(MaxBounds.X, Position.X);
		MaxBounds.Y = FMath::Max(MaxBounds.Y, Position.Y);
		MaxBounds.Z = FMath::Max(MaxBounds.Z, Position.Z);
	}

	TestTrue(TEXT("Rotated centered box should span equally around the X origin."), FMath::IsNearlyEqual(MinBounds.X, -150.0f));
	TestTrue(TEXT("Centered box should span equally around the Y origin."), FMath::IsNearlyEqual(MinBounds.Y, -50.0f));
	TestTrue(TEXT("Rotated centered box should end at positive X half extent."), FMath::IsNearlyEqual(MaxBounds.X, 150.0f));
	TestTrue(TEXT("Centered box should end at positive Y half extent."), FMath::IsNearlyEqual(MaxBounds.Y, 50.0f));

	UE::Geometry::FDynamicMesh3 DynamicMesh(UE::Geometry::EMeshComponents::None);
	TestTrue(TEXT("BuildDynamicMesh should succeed for generated box buffers."), PrototypeMeshBuilder::BuildDynamicMesh(Buffers, DynamicMesh, Error));
	TestEqual(TEXT("Dynamic mesh should contain 12 triangles."), DynamicMesh.TriangleCount(), 12);
	TestTrue(TEXT("Dynamic mesh should retain primary colors."), DynamicMesh.Attributes() && DynamicMesh.Attributes()->HasPrimaryColors());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPrototypeMeshPayloadParseTest,
	"SimpleMeshBuilder.PrototypeMeshBuilder.Payload.ParsePrimitivePayload",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPrototypeMeshPayloadParseTest::RunTest(const FString& Parameters)
{
	const FString PayloadJson =
		TEXT("{")
		TEXT("\"version\":\"1.0\",")
		TEXT("\"mesh_name\":\"PayloadBox\",")
		TEXT("\"generation_mode\":\"primitive\",")
		TEXT("\"notes\":\"unit test\",")
		TEXT("\"primitive_mesh\":{")
			TEXT("\"units\":\"cm\",")
			TEXT("\"pivot\":\"base_center\",")
			TEXT("\"primitives\":[{")
				TEXT("\"name\":\"body\",")
				TEXT("\"type\":\"box\",")
				TEXT("\"transform\":{")
					TEXT("\"location_cm\":{\"x\":0,\"y\":0,\"z\":0},")
					TEXT("\"rotation_deg\":{\"x\":0,\"y\":0,\"z\":0},")
					TEXT("\"scale\":{\"x\":1,\"y\":1,\"z\":1}")
				TEXT("},")
				TEXT("\"color\":{\"r\":1,\"g\":0.5,\"b\":0.25,\"a\":1},")
				TEXT("\"width\":100,")
				TEXT("\"depth\":100,")
				TEXT("\"height\":100,")
				TEXT("\"radius\":0,")
				TEXT("\"segments\":0,")
				TEXT("\"steps\":0")
			TEXT("}]")
		TEXT("},")
		TEXT("\"voxel_grid\":null")
		TEXT("}");

	FPrototypeMeshPayload Payload;
	FString Error;
	TestTrue(TEXT("Payload JSON should parse."), PrototypeMeshBuilder::ParseMeshPayloadJson(PayloadJson, Payload, Error));
	TestEqual(TEXT("Parsed payload should stay in primitive mode."), Payload.GenerationMode, EPrototypeGenerationMode::Primitive);
	TestEqual(TEXT("Parsed primitive payload should contain one primitive."), Payload.PrimitiveShape.Primitives.Num(), 1);
	TestTrue(TEXT("Parsed primitive payload should validate."), PrototypeMeshBuilder::ValidateShapeDsl(Payload.PrimitiveShape, Error));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPrototypeVoxelMeshBuilderTest,
	"SimpleMeshBuilder.PrototypeMeshBuilder.MeshBuild.VoxelGreedyMesh",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPrototypeVoxelMeshBuilderTest::RunTest(const FString& Parameters)
{
	FPrototypeVoxelGrid Grid;
	Grid.Version = TEXT("1.0");
	Grid.MeshName = TEXT("VoxelPair");
	Grid.Resolution = FIntVector(16, 16, 16);
	Grid.Notes = TEXT("unit test");

	const int32 VoxelCount = Grid.Resolution.X * Grid.Resolution.Y * Grid.Resolution.Z;
	TMap<int32, FString> FilledColors;
	FilledColors.Add(0, TEXT("FF0000"));
	FilledColors.Add(1, TEXT("FF0000"));

	Grid.VoxelsHex.Reserve(VoxelCount * 6);
	for (int32 Index = 0; Index < VoxelCount; ++Index)
	{
		if (const FString* Color = FilledColors.Find(Index))
		{
			Grid.VoxelsHex += *Color;
		}
		else
		{
			Grid.VoxelsHex += TEXT("000000");
		}
	}

	int32 OccupiedVoxelCount = 0;
	FString Error;
	TestTrue(TEXT("Voxel occupancy count should parse."), PrototypeMeshBuilder::CountOccupiedVoxels(Grid, OccupiedVoxelCount, Error));
	TestEqual(TEXT("Two voxels should be counted as occupied."), OccupiedVoxelCount, 2);

	FGeneratedMeshBuffers Buffers;
	TestTrue(TEXT("Greedy voxel mesh build should succeed."), PrototypeMeshBuilder::BuildVoxelMeshBuffers(Grid, Buffers, Error));
	TestEqual(TEXT("Two adjacent voxels of the same color should collapse to a cuboid with 12 triangles."), Buffers.GetTriangleCount(), 12);
	TestTrue(TEXT("Voxel colors should propagate to generated buffers."), FMath::IsNearlyEqual(Buffers.Colors[0].X, 1.0f));
	float MinZ = FLT_MAX;
	for (const FVector3f& Position : Buffers.Positions)
	{
		MinZ = FMath::Min(MinZ, Position.Z);
	}
	TestTrue(TEXT("Voxel buffers should be centered on the base pivot."), FMath::IsNearlyEqual(MinZ, 0.0f));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPrototypeMeshBridgeE2ETest,
	"SimpleMeshBuilder.PrototypeMeshBuilder.Bridge.CodexExecOptional",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPrototypeMeshBridgeE2ETest::RunTest(const FString& Parameters)
{
	if (FPlatformMisc::GetEnvironmentVariable(TEXT("PROTOTYPE_MESH_BRIDGE_E2E")) != TEXT("1"))
	{
		AddInfo(TEXT("Skipping Codex bridge E2E test. Set PROTOTYPE_MESH_BRIDGE_E2E=1 to enable it."));
		return true;
	}

	FPrototypeMeshRequest Request;
	Request.Prompt = TEXT("나무 상자 하나");
	Request.AssetName = TEXT("WoodenBox");
	Request.ContentPath = TEXT("/Game/Generated/PrototypeMeshes");
	Request.Locale = TEXT("ko-KR");
	Request.GenerationMode = EPrototypeGenerationMode::Primitive;
	Request.MaxPrimitiveCount = 32;

	FPrototypeCodexMeshBridge Bridge;
	const FPrototypeBridgeResult Result = Bridge.GenerateDsl(Request);
	TestTrue(FString::Printf(TEXT("Codex bridge should succeed. Error: %s"), *Result.ErrorMessage), Result.bSuccess);
	TestFalse(TEXT("Bridge should return raw payload JSON."), Result.RawPayloadJson.IsEmpty());

	FPrototypeMeshPayload Payload;
	FString Error;
	TestTrue(TEXT("Returned payload JSON should parse."), PrototypeMeshBuilder::ParseMeshPayloadJson(Result.RawPayloadJson, Payload, Error));
	TestEqual(TEXT("E2E request should stay in primitive mode."), Payload.GenerationMode, EPrototypeGenerationMode::Primitive);
	TestTrue(TEXT("Returned primitive payload should validate."), PrototypeMeshBuilder::ValidateShapeDsl(Payload.PrimitiveShape, Error));
	return true;
}

#endif
