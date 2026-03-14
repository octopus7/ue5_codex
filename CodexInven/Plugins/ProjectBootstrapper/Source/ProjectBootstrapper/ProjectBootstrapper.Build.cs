using UnrealBuildTool;

public class ProjectBootstrapper : ModuleRules
{
	public ProjectBootstrapper(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"AssetRegistry",
			"CoreUObject",
			"Engine",
			"EngineSettings",
			"GameProjectGeneration",
			"HotReload",
			"InputCore",
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
