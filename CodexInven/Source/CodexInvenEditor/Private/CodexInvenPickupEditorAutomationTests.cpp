#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "CodexInvenPickupData.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "MeshDescription.h"
#include "StaticMeshAttributes.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCodexInvenGeneratedPickupAssetsAutomationTest,
	"CodexInven.Pickups.GeneratedAssets",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCodexInvenGeneratedPickupAssetsAutomationTest::RunTest(const FString& Parameters)
{
	for (const ECodexInvenPickupType PickupType : CodexInvenPickupData::GetAllPickupTypes())
	{
		const FCodexInvenPickupDefinition& Definition = CodexInvenPickupData::GetPickupDefinitionChecked(PickupType);

		UStaticMesh* const Mesh = Definition.WorldMesh.LoadSynchronous();
		TestNotNull(TEXT("Generated pickup mesh exists"), Mesh);
		if (Mesh != nullptr)
		{
			TestEqual(TEXT("Generated pickup mesh uses one material slot"), Mesh->GetStaticMaterials().Num(), 1);
			TestTrue(TEXT("Generated pickup mesh keeps a valid mesh description"), Mesh->IsMeshDescriptionValid(0));
			if (Mesh->IsMeshDescriptionValid(0))
			{
				FMeshDescription* const MeshDescription = Mesh->GetMeshDescription(0);
				TestNotNull(TEXT("Mesh description can be read"), MeshDescription);
				if (MeshDescription != nullptr)
				{
					FStaticMeshAttributes Attributes(*MeshDescription);
					const TVertexInstanceAttributesConstRef<FVector4f> VertexColors = Attributes.GetVertexInstanceColors();
					bool bHasNonWhiteVertexColor = false;
					for (const FVertexInstanceID VertexInstanceID : MeshDescription->VertexInstances().GetElementIDs())
					{
						if (!VertexColors.Get(VertexInstanceID).Equals(FVector4f::One()))
						{
							bHasNonWhiteVertexColor = true;
							break;
						}
					}

					TestTrue(TEXT("Generated pickup mesh stores vertex colors"), bHasNonWhiteVertexColor);
				}
			}
		}

		UTexture2D* const IconTexture = Definition.InventoryIcon.LoadSynchronous();
		TestNotNull(TEXT("Generated pickup icon exists"), IconTexture);
		if (IconTexture != nullptr)
		{
			const FIntPoint ImportedSize = IconTexture->GetImportedSize();
			TestEqual(TEXT("Generated pickup icon width is 64"), ImportedSize.X, 64);
			TestEqual(TEXT("Generated pickup icon height is 64"), ImportedSize.Y, 64);
		}
	}

	return true;
}

#endif
