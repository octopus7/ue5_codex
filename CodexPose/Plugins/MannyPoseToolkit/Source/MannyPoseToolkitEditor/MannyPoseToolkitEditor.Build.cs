using UnrealBuildTool;

public class MannyPoseToolkitEditor : ModuleRules
{
    public MannyPoseToolkitEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",
            "ToolMenus",
            "InputCore",
            "Json",
            "JsonUtilities",
            "MannyPoseToolkit"
        });

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "Projects",
            "EditorStyle",
            "LevelEditor",
            "UnrealEd",
            "EditorFramework"
        });
    }
}
