using UnrealBuildTool;

public class CodexInvenMediumEditor : ModuleRules
{
	public CodexInvenMediumEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CodexInvenMedium"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"AssetRegistry",
			"AssetTools",
			"CoreUObject",
			"Engine",
			"EngineSettings",
			"EnhancedInput",
			"InputCore",
			"InputEditor",
			"Kismet",
			"Projects",
			"UnrealEd"
		});
	}
}
