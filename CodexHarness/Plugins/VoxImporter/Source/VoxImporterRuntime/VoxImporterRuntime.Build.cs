using UnrealBuildTool;

public class VoxImporterRuntime : ModuleRules
{
	public VoxImporterRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"GeometryCore",
				"MeshConversion",
				"MeshDescription",
				"StaticMeshDescription"
			});
	}
}
