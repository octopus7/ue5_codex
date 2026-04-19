// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CodexPose : ModuleRules
{
	public CodexPose(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"CodexPose",
			"CodexPose/Variant_Platforming",
			"CodexPose/Variant_Platforming/Animation",
			"CodexPose/Variant_Combat",
			"CodexPose/Variant_Combat/AI",
			"CodexPose/Variant_Combat/Animation",
			"CodexPose/Variant_Combat/Gameplay",
			"CodexPose/Variant_Combat/Interfaces",
			"CodexPose/Variant_Combat/UI",
			"CodexPose/Variant_SideScrolling",
			"CodexPose/Variant_SideScrolling/AI",
			"CodexPose/Variant_SideScrolling/Gameplay",
			"CodexPose/Variant_SideScrolling/Interfaces",
			"CodexPose/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
