// Copyright Epic Games, Inc. All Rights Reserved.

#include "HslmMySQLEditor.h"

#define LOCTEXT_NAMESPACE "FHslmMySQLEditorModule"

void FHslmMySQLEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogHslmMySQLEditor, Log, TEXT("FHslmMySQLEditorModule Loaded"));
	
}

void FHslmMySQLEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
	UE_LOG(LogHslmMySQLEditor, Log, TEXT("FHslmMySQLEditorModule Unload"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHslmMySQLEditorModule, HslmMySQLEditor)
DEFINE_LOG_CATEGORY(LogHslmMySQLEditor);