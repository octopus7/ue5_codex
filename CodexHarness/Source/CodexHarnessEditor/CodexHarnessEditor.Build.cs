using UnrealBuildTool;

public class CodexHarnessEditor : ModuleRules
{
	public CodexHarnessEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"Engine"
			});

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"AssetRegistry",
				"AssetTools",
				"CodexHarness",
				"MaterialEditor",
				"UnrealEd"
			});
	}
}
