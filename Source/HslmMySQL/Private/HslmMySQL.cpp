// Copyright Epic Games, Inc. All Rights Reserved.

#include "HslmMySQL.h"

#define LOCTEXT_NAMESPACE "FHslmMySQLModule"

void FHslmMySQLModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogHslmMySQL, Log, TEXT("FHslmPluginModule Loaded"));
	
}

void FHslmMySQLModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
	UE_LOG(LogHslmMySQL, Log, TEXT("FHslmPluginModule Unload"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHslmMySQLModule, HslmMySQL)
DEFINE_LOG_CATEGORY(LogHslmMySQL);
