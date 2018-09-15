// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Academia2017 : ModuleRules
{
	public Academia2017(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "PhysX", "APEX", "UMG", "Slate", "SlateCore", "OnlineSubsystem", "OnlineSubsystemUtils" });
	}
}
