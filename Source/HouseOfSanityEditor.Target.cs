using UnrealBuildTool;
using System.Collections.Generic;

public class HouseOfSanityEditorTarget : TargetRules
{
	public HouseOfSanityEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;

		ExtraModuleNames.AddRange(new string[] { "HouseOfSanity" });
	}
}
