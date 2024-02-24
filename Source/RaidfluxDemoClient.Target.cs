// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class RaidfluxDemoClientTarget : TargetRules
{
	public RaidfluxDemoClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("RaidfluxDemo");
		bUseLoggingInShipping = true;
	}
}
