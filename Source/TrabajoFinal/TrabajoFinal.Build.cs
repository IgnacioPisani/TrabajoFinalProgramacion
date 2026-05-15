// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TrabajoFinal : ModuleRules
{
	public TrabajoFinal(ReadOnlyTargetRules Target) : base(Target)
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
			"TrabajoFinal",
			"TrabajoFinal/Variant_Platforming",
			"TrabajoFinal/Variant_Platforming/Animation",
			"TrabajoFinal/Variant_Combat",
			"TrabajoFinal/Variant_Combat/AI",
			"TrabajoFinal/Variant_Combat/Animation",
			"TrabajoFinal/Variant_Combat/Gameplay",
			"TrabajoFinal/Variant_Combat/Interfaces",
			"TrabajoFinal/Variant_Combat/UI",
			"TrabajoFinal/Variant_SideScrolling",
			"TrabajoFinal/Variant_SideScrolling/AI",
			"TrabajoFinal/Variant_SideScrolling/Gameplay",
			"TrabajoFinal/Variant_SideScrolling/Interfaces",
			"TrabajoFinal/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
