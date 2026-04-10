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
				"InputEditor",
				"KismetCompiler",
				"Slate",
				"SlateCore",
				"ToolMenus",
				"UnrealEd",
				"EngineSettings"
			});
	}
}
