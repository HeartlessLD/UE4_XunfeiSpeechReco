// Copyright Epic Games, Inc. All Rights Reserved.

#include "XunFeiSpeechReco.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FXunFeiSpeechRecoModule"

void* FXunFeiSpeechRecoModule::XunDllHandler = nullptr;

PRAGMA_DISABLE_OPTIMIZATION
void FXunFeiSpeechRecoModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FString BaseDir = FPaths::ProjectPluginsDir() + TEXT("XunfeiSpeechReco/ThirdParty/libs/");
	FString FileName = (PLATFORM_WINDOWS && PLATFORM_64BITS) ? TEXT("msc_x64.dll") : TEXT("msc.dll");
	FString FilePath = BaseDir + FileName;
	FPlatformProcess::PushDllDirectory(*FilePath);
	XunDllHandler = FPlatformProcess::GetDllHandle(*FilePath);
	FPlatformProcess::PopDllDirectory(*FilePath);
}
PRAGMA_ENABLE_OPTIMIZATION
void FXunFeiSpeechRecoModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if (XunDllHandler)
	{
		FPlatformProcess::FreeDllHandle(XunDllHandler);
		XunDllHandler = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FXunFeiSpeechRecoModule, XunFeiSpeechReco)