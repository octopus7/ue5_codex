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
			"CoreUObject",
			"Engine",
			"EngineSettings",
			"EnhancedInput",
			"GameProjectGeneration",
			"HotReload",
			"InputCore",
			"InputEditor",
			"Kismet",
			"Projects",
			"PropertyEditor",
			"Slate",
			"SlateCore",
			"ToolMenus",
			"UnrealEd"
		});
	}
}
