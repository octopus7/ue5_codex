using UnrealBuildTool;
using System.Collections.Generic;

public class CodexTpsEditorTarget : TargetRules
{
    public CodexTpsEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
        ExtraModuleNames.AddRange(new string[] { "CodexTps" });
    }
}

