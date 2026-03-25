#include "Misc/AutomationTest.h"

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
	Primitive.Width = 100.0;
	Primitive.Depth = 200.0;
	Primitive.Height = 300.0;
	Dsl.Primitives.Add(Primitive);

	FGeneratedMeshBuffers Buffers;
	FString Error;
	TestTrue(TEXT("BuildMeshBuffers should succeed for a simple box."), PrototypeMeshBuilder::BuildMeshBuffers(Dsl, Buffers, Error));
	TestEqual(TEXT("A box should generate 12 triangles."), Buffers.GetTriangleCount(), 12);
	TestEqual(TEXT("A box triangle soup should generate 36 positions."), Buffers.Positions.Num(), 36);
	TestEqual(TEXT("Generated buffers should include one vertex color per position."), Buffers.Colors.Num(), Buffers.Positions.Num());

	UE::Geometry::FDynamicMesh3 DynamicMesh(UE::Geometry::EMeshComponents::None);
	TestTrue(TEXT("BuildDynamicMesh should succeed for generated box buffers."), PrototypeMeshBuilder::BuildDynamicMesh(Buffers, DynamicMesh, Error));
	TestEqual(TEXT("Dynamic mesh should contain 12 triangles."), DynamicMesh.TriangleCount(), 12);
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
	Request.MaxPrimitiveCount = 32;

	FPrototypeCodexMeshBridge Bridge;
	const FPrototypeBridgeResult Result = Bridge.GenerateDsl(Request);
	TestTrue(FString::Printf(TEXT("Codex bridge should succeed. Error: %s"), *Result.ErrorMessage), Result.bSuccess);
	TestFalse(TEXT("Bridge should return raw DSL JSON."), Result.RawDslJson.IsEmpty());

	FPrototypeShapeDsl Dsl;
	FString Error;
	TestTrue(TEXT("Returned DSL JSON should parse."), PrototypeMeshBuilder::ParseShapeDslJson(Result.RawDslJson, Dsl, Error));
	TestTrue(TEXT("Returned DSL should validate."), PrototypeMeshBuilder::ValidateShapeDsl(Dsl, Error));
	return true;
}

#endif
