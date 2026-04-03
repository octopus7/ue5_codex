using UnrealBuildTool;

public class CodexHarnessEditor : ModuleRules
{
	public CodexHarnessEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CodexHarness"
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
			"Niagara",
			"NiagaraEditor",
			"Projects",
			"UnrealEd",
			"VoxImporterEditor"
		});
	}
}
