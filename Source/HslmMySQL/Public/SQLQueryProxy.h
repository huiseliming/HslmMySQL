// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UObject/Object.h"
#include "SQLData.h"
#include "SQLQueryProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSQLQueryProxyCompletedSignature, int32, ErrorCode, const FString&, ErrorString, int64, AffectedRowNum, const FSQLResultSet&, ResultSet);

USTRUCT(BlueprintType)
struct HSLMMYSQL_API FSQLQueryProxyBinds
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySQLConnector|Tables|Fields")
	TArray<FSQLData> SQLData;
};

/**
 * 
 */
UCLASS()
class HSLMMYSQL_API USQLQueryProxy : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FSQLQueryProxyCompletedSignature OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FSQLQueryProxyCompletedSignature OnFail;
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", DisplayName = "Hslm SQL Query", AdvancedDisplay = "Host,Port,User,Password,DataBase,SQLString,SQLQueryBind", AutoCreateRefTerm = "Host,User,Password,DataBase,SQLString,SQLQueryBind"), Category = "HTTP Library")
	static USQLQueryProxy* CreateSQLQueryProxyObject(const FString& Host,const int32 Port,const FString& User,const FString&  Password, const FString& DataBase, const FString& SQLString,const TArray<FSQLQueryProxyBinds>& SQLQueryBind);

	FString DataBase;
	FString SQLString;
	TArray<FSQLQueryProxyBinds> SQLQueryBind;
	
};
