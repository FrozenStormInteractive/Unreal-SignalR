// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using System;
using UnrealBuildTool;

public class SignalR : ModuleRules
{
	public SignalR(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "ApplicationCore",
            "Json",
        }
        );

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "HTTP",
            "WebSockets",
            "Engine",
        }
        );
    }
}
