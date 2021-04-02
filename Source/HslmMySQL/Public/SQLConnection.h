// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SQLRunnable.h"
#include "SQLConnection.generated.h"

class USQLQueryProxy;

/**
 * 
 */
UCLASS()
class HSLMMYSQL_API USQLConnection : public UObject
{
	GENERATED_BODY()
public:
	friend class FSQLRunnable;	
public:
	~USQLConnection();
	
	void Initialize(const FString& Host, const int32 Port, const FString& User, const FString& Password, const FString& DataBase);
	
	UFUNCTION(BlueprintCallable, Category="Hslm|MySQLConnection")
    void ExecuteSQL(USQLQueryProxy* Query);
	
	TUniquePtr<FSQLRunnable> SQLRunnable;
};
