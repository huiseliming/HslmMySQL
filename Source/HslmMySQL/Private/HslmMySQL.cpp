// Copyright Epic Games, Inc. All Rights Reserved.

#include "HslmMySQL.h"
#include "SQLConnection.h"

#define LOCTEXT_NAMESPACE "FHslmMySQLModule"

void FHslmMySQLModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogHslmMySQL, Log, TEXT("FHslmMySQLModule Loaded"));
	
}

void FHslmMySQLModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	for (auto& SQLConnection : SQLConnections)
		SQLConnection.Value->RemoveFromRoot();
	SQLConnections.Empty();
	UE_LOG(LogHslmMySQL, Log, TEXT("FHslmMySQLModule Unload"));
}

USQLConnection* FHslmMySQLModule::GetConnection(FString Host, int32 Port, FString User, FString Password, FString DataBase)
{
	const FString ConnectionName = FString::Printf(TEXT("%s:%d-%s"), *Host, Port, *User);
	USQLConnection*& SQLConnection = SQLConnections.FindOrAdd(ConnectionName, nullptr);
	if (SQLConnection)
	{
		return SQLConnection;
	}
	SQLConnection = NewObject<USQLConnection>();
	SQLConnection->AddToRoot();
	SQLConnection->Initialize(Host, Port, User, Password, DataBase);
	return SQLConnection;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHslmMySQLModule, HslmMySQL)
DEFINE_LOG_CATEGORY(LogHslmMySQL);
