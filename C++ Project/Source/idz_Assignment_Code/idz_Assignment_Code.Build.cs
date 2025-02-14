// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class idz_Assignment_Code : ModuleRules
{
	public idz_Assignment_Code(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
