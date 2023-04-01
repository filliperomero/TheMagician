// Copyright Fillipe Romero

using UnrealBuildTool;
using System.Collections.Generic;

public class TheMagicianTarget : TargetRules
{
	public TheMagicianTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "TheMagician" } );
	}
}
