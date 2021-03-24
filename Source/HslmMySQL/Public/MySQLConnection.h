// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MySQLResultSet.h"
#include "UObject/Object.h"
#include "MySQLConnection.generated.h"

/**
 * 
 */
UCLASS()
class HSLMMYSQL_API UMySQLConnection : public UObject
{
	GENERATED_BODY()
	
public:
	
	UMySQLConnection(const FObjectInitializer& ObjectInitializer);
	~UMySQLConnection();

	bool ConnectToServer(FString Host, int32 Port, FString User, FString Password, FString DataBase = TEXT(""));
	
	bool IsClosed() const;
	
	void CloseConnection();
	
	bool ExecuteQuery(FString InSQL, FMySQLResultSet& OutResultSet);
	bool ExecuteUpdate(FString InSQL, TArray<FJsonObject> InValues);

	FString URI;
	
	struct FImpl;
	FImpl* Impl;
};
