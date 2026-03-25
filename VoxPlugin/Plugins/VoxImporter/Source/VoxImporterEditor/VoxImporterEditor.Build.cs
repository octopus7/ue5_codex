using UnrealBuildTool;

public class VoxImporterEditor : ModuleRules
{
	public VoxImporterEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"UnrealEd",
				"VoxImporterRuntime"
			});

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"AssetTools",
				"AssetRegistry",
				"ContentBrowser",
				"MeshDescription",
				"MeshReductionInterface",
				"MeshUtilitiesCommon",
				"Slate",
				"SlateCore",
				"StaticMeshDescription",
				"ToolMenus"
			});
	}
}
