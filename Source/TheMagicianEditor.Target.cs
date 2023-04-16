// Copyright Fillipe Romero

using UnrealBuildTool;
using System.Collections.Generic;

public class TheMagicianEditorTarget : TargetRules
{
	public TheMagicianEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_2;

		ExtraModuleNames.AddRange( new string[] { "TheMagician" } );
	}
}
