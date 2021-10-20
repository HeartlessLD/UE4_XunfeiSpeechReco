// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class XunFeiSpeechReco : ModuleRules
{
	private string ModulePath
    {
		get { return ModuleDirectory; }
    }

	private string ThirdPartyPath
    {
		get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty")); }
    }
	public XunFeiSpeechReco(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
				Path.Combine(ThirdPartyPath, "include")
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Projects",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

		string libPath = Path.Combine(ThirdPartyPath, "libs");
		//添加语法等相关资源
		RuntimeDependencies.Add(Path.Combine(ThirdPartyPath, "res/..."));
		if (Target.Platform == UnrealTargetPlatform.Win64)
        {
			//PublicLibraryPaths.Add(libPath);
			PublicAdditionalLibraries.Add(Path.Combine(libPath, "msc_x64.lib"));
			PublicDelayLoadDLLs.Add("msc_x64.dll");
			
			RuntimeDependencies.Add(Path.Combine(libPath, "msc_x64.dll"));

		}
		else if(Target.Platform == UnrealTargetPlatform.Win32)
        {
			PublicAdditionalLibraries.Add(Path.Combine(libPath, "msc.lib"));
			PublicDelayLoadDLLs.Add("msc.dll");
			RuntimeDependencies.Add(Path.Combine(libPath, "msc.dll"));
		}
	}
}
