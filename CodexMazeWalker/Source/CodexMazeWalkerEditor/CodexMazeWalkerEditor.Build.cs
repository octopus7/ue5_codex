using UnrealBuildTool;

public class CodexMazeWalkerEditor : ModuleRules
{
	public CodexMazeWalkerEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[]
		{
			ModuleDirectory
		});

		PrivateIncludePaths.AddRange(new string[]
		{
			ModuleDirectory
		});

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"CodexMazeWalker",
			"InputCore",
			"EnhancedInput",
			"UMG",
			"UMGEditor",
			"EngineSettings",
			"MeshDescription",
			"MeshConversion",
			"StaticMeshDescription"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"AssetRegistry",
			"AssetTools",
			"BlueprintGraph",
			"InputEditor",
			"Kismet",
			"KismetCompiler",
			"Projects",
			"RenderCore",
			"Slate",
			"SlateCore",
			"UnrealEd"
		});
	}
}
