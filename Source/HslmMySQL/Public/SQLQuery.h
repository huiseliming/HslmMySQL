// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SQLData.h"
#include "UObject/Object.h"
#include "SQLQuery.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FMySQLQueryCompletedSignature, int32, HasError, const FString&, ErrorString, int64, AffectedRowNum, const FSQLResultSet&, ResultSet);

USTRUCT(BlueprintType)
struct HSLMMYSQL_API FSQLUpdateBinds
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySQLConnector|Tables|Fields")
	TArray<FSQLData> SQLData;
};


/**
 * 
 */
UCLASS(BlueprintType)
class HSLMMYSQL_API USQLQuery : public UObject
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite ,Category="Hslm|MySQLConnection")
	FString DBName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite ,Category="Hslm|MySQLConnection")
	FString SQL;
	UPROPERTY(BlueprintAssignable, Category="Hslm|MySQLConnection")
	FMySQLQueryCompletedSignature OnQueryCompleted;
};

/**
* 
*/
UCLASS()
class HSLMMYSQL_API USQLUpdateQuery : public USQLQuery
{
	GENERATED_BODY()
	public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite ,Category="Hslm|MySQLConnection")
	TArray<FSQLUpdateBinds> SQLUpdateBinds;
};
