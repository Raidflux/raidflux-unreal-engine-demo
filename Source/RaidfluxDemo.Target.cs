// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class RaidfluxDemoTarget : TargetRules
{
	public RaidfluxDemoTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("RaidfluxDemo");
		bUseLoggingInShipping = true;
	}
}
