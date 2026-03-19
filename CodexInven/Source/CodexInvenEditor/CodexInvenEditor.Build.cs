using System.IO;
using UnrealBuildTool;

public class CodexInvenEditor : ModuleRules
{
	public CodexInvenEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "..", "CodexInven"));

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"CodexInven",
				"Core",
				"CoreUObject",
				"Engine"
			});

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"AssetRegistry",
				"AssetTools",
				"GeometryFramework",
				"GeometryScriptingCore",
				"GeometryScriptingEditor",
				"MeshDescription",
				"Slate",
				"SlateCore",
				"StaticMeshDescription",
				"UMG",
				"UMGEditor",
				"UnrealEd"
			});
	}
}
