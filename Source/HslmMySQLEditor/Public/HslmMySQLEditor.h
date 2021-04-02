// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"


class FHslmMySQLEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FHslmMySQLEditorModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FHslmMySQLEditorModule>("HslmMySQLEditor");
	}

	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("HslmMySQLEditor");
	}

};

DECLARE_LOG_CATEGORY_EXTERN(LogHslmMySQLEditor, Log, All);
