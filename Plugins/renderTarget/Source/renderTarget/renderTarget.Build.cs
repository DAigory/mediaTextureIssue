// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class renderTarget : ModuleRules
{
    public renderTarget(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				 "Core", "CoreUObject", "Engine"				
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Engine",
				"ApplicationCore",
				"Core",
				"CoreUObject",
				"Engine",									
				"RenderCore",
				"RHI",
				"D3D11RHI",							
				"MediaAssets",
				"DeveloperSettings",
				"ShadersSendSomething",
			}
			);


		PrivateIncludePaths.AddRange(new string[]
		{
				"ShadersSendSomething/Private"
		});

		DynamicallyLoadedModuleNames.AddRange(new string[]
		 {
				"Media",
		 });

		PrivateIncludePathModuleNames.AddRange(new string[]
		{
				"Media",
		});

		var EngineDir = Path.GetFullPath(Target.RelativeEnginePath);
			
		PrivateDependencyModuleNames.Add("WmfMedia");
		PrivateIncludePaths.Add(Path.Combine(EngineDir, "Plugins/Media/WmfMedia/Source/WmfMedia/Private/"));
		PrivateIncludePaths.Add(Path.Combine(EngineDir, "Plugins/Media/WmfMedia/Source/WmfMedia/Private/Wmf/"));

		AddEngineThirdPartyPrivateStaticDependencies(Target, "DX11");
		AddEngineThirdPartyPrivateStaticDependencies(Target, "DX9");

		PublicSystemLibraries.Add("mfplat.lib");


	}
}
