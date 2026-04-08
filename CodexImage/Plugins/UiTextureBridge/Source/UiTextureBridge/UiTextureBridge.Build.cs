using UnrealBuildTool;

public class UiTextureBridge : ModuleRules
{
	public UiTextureBridge(ReadOnlyTargetRules Target) : base(Target)
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
			"DeveloperSettings",
			"Engine",
			"HTTPServer",
			"Json",
			"JsonUtilities",
			"Projects",
			"Settings",
			"Slate",
			"SlateCore",
			"Sockets",
			"ToolMenus",
			"UnrealEd"
		});
	}
}
