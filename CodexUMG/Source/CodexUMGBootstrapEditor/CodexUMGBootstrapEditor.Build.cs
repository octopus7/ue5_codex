using UnrealBuildTool;

public class CodexUMGBootstrapEditor : ModuleRules
{
	public CodexUMGBootstrapEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"EnhancedInput",
				"InputCore",
				"UMG"
			});

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"AssetRegistry",
				"AssetTools",
				"CodexUMG",
				"Json",
				"JsonUtilities",
				"InputEditor",
				"KismetCompiler",
				"MaterialEditor",
				"MeshConversion",
				"MeshDescription",
				"Slate",
				"SlateCore",
				"StaticMeshDescription",
				"ToolMenus",
				"UMGEditor",
				"UnrealEd",
				"EngineSettings"
			});
	}
}
