// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using System.IO;

namespace UnrealBuildTool.Rules
{
    public class ShadersSendSomething : ModuleRules
    {
        public ShadersSendSomething(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

            PrivateIncludePaths.AddRange(new string[]
            {
                "ShadersSendSomething/Private"
            });

            PrivateDependencyModuleNames.AddRange(
                new string[] {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "Renderer",
                    "RenderCore",
                    "RHI",
                    "Projects",
                    "MediaUtils",
                    "MediaAssets",
                    "D3D11RHI"
                });

          
            var EngineDir = Path.GetFullPath(Target.RelativeEnginePath);            
            PrivateDependencyModuleNames.Add("WmfMedia");
            PrivateIncludePaths.Add(Path.Combine(EngineDir, "Plugins/Media/WmfMedia/Source/WmfMedia/Private/"));

        }
    }
}
