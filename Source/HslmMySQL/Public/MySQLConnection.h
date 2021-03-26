// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SQLDataBase.h"
#include "UObject/Object.h"
#include "MySQLConnection.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class HSLMMYSQL_API UMySQLConnection : public UObject
{
	GENERATED_BODY()
	
public:
	
	UMySQLConnection(const FObjectInitializer& ObjectInitializer);
	~UMySQLConnection();
	
	UFUNCTION(BlueprintCallable, Category="Hslm|MySQLConnection")
	bool ConnectToServer(FString Host, int32 Port, FString User, FString Password, FString DataBase = TEXT(""));
	
	UFUNCTION(BlueprintPure, Category="Hslm|MySQLConnection")
	bool IsClosed() const;
	
	UFUNCTION(BlueprintCallable, Category="Hslm|MySQLConnection")
	void CloseConnection();
	
	UFUNCTION(BlueprintCallable, Category="Hslm|MySQLConnection")
	bool Execute(FString InDBName, FString InSQL);

	UFUNCTION(BlueprintCallable, Category="Hslm|MySQLConnection")
	bool ExecuteQuery(FString InDBName, FString InSQL, TArray<FSQLTableRow>& OutResultSet);
	
	UFUNCTION(BlueprintCallable, Category="Hslm|MySQLConnection")
	int32 ExecuteUpdate(FString InDBName, FString InSQL, TArray<FSQLData> InSQLTableRow);

	FString URI;
	
	struct FImpl;
	FImpl* Impl;
	
};
