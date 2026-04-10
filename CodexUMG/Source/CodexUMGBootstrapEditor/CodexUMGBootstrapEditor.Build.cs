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
				"InputCore"
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
				"MeshConversion",
				"MeshDescription",
				"Slate",
				"SlateCore",
				"StaticMeshDescription",
				"ToolMenus",
				"UnrealEd",
				"EngineSettings"
			});
	}
}
