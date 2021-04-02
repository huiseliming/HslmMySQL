// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
class USQLConnection;

class FHslmMySQLModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FHslmMySQLModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FHslmMySQLModule>("HslmMySQL");
	}

	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("HslmMySQL");
	}

	USQLConnection* GetConnection(FString Host, int32 Port, FString User, FString Password, FString DataBase);
	
	TMap<FString, USQLConnection*> SQLConnections;
};

DECLARE_LOG_CATEGORY_EXTERN(LogHslmMySQL, Log, All);
