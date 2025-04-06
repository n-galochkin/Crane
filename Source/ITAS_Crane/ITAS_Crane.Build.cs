// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class ITAS_Crane : ModuleRules
{
	public ITAS_Crane(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "GeometryCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "GeometryFramework", "GeometryScriptingCore" });
	}
}
