using UnrealBuildTool;

public class OctoDen : ModuleRules
{
	public OctoDen(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"AssetRegistry",
			"AssetTools",
			"BlueprintGraph",
			"CoreUObject",
			"EditorFramework",
			"EditorSubsystem",
			"Engine",
			"EngineSettings",
			"EnhancedInput",
			"GameProjectGeneration",
			"InputCore",
			"InputEditor",
			"Kismet",
			"KismetCompiler",
			"Projects",
			"PropertyEditor",
			"Slate",
			"SlateCore",
			"ToolMenus",
			"UnrealEd"
		});
	}
}
