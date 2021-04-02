// Copyright Epic Games, Inc. All Rights Reserved.
namespace UnrealBuildTool.Rules
{
	public class HslmMySQLEditor : ModuleRules
	{
		public HslmMySQLEditor(ReadOnlyTargetRules Target) : base(Target)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{ 
					"Core", 
					"CoreUObject", 
					"Engine",
					"BlueprintGraph",
					"HslmMySQL"
				}
			);
		}
	}
}