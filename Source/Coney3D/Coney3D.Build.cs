// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Coney3D : ModuleRules
{
	public Coney3D(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore","EnhancedInput", "Niagara","InputCore",
			"HeadMountedDisplay", "ApplicationCore", "Slate", "SlateCore", "EngineSettings" });

        PrivateDependencyModuleNames.AddRange(System.Array.Empty<string>());

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
