// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class IFS : ModuleRules
{
	public IFS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
															"UMG", "Slate", "SlateCore", "Niagara",
															"GameplayAbilities", "GameplayTags", "GameplayTasks" });
	}
}
