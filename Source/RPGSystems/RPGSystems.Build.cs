// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RPGSystems : ModuleRules
{
	public RPGSystems(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput","GameplayAbilities","GameplayTasks","GameplayTags","NetCore", "MotionWarping","Slate",
			"SlateCore", "ModelViewViewModel", "AIModule","NavigationSystem" });
		
		PublicIncludePaths.AddRange(
			new string[] {
				"RPGSystems/Public"
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
				"RPGSystems/Private"
			}
		);
	}
}
