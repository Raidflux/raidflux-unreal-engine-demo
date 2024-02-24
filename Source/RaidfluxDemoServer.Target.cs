// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class RaidfluxDemoServerTarget : TargetRules
{
	public RaidfluxDemoServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("RaidfluxDemo");
		bUseLoggingInShipping = true;
	}
}
