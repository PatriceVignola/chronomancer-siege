// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class Academia2017Target : TargetRules
{
	public Academia2017Target(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		ExtraModuleNames.Add("Academia2017");
	}
}
