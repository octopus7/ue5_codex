using UnrealBuildTool;

public class PrototypeMeshBuilder : ModuleRules
{
	public PrototypeMeshBuilder(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"ApplicationCore",
				"AssetRegistry",
				"AssetTools",
				"ContentBrowser",
				"Core",
				"CoreUObject",
				"EditorFramework",
				"Engine",
				"GeometryCore",
				"GeometryFramework",
				"InputCore",
				"Json",
				"LevelEditor",
				"MeshConversion",
				"MeshDescription",
				"Projects",
				"Slate",
				"SlateCore",
				"StaticMeshDescription",
				"ToolMenus",
				"UnrealEd"
			});
	}
}
