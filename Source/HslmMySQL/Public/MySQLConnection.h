// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MySQLRunnable.h"
#include "UObject/Object.h"
#include "MySQLConnection.generated.h"

class UMySQLConnection;
class USQLQuery;
class USQLUpdateQuery;

USTRUCT(BlueprintType)
struct FConnectionInfo
{
	GENERATED_BODY()
	FConnectionInfo()
        : Host(TEXT("127.0.0.1"))
        , User(TEXT("User"))
        , Password(TEXT("Password"))
        , DataBase(TEXT(""))
        , Port(3306)
	{}
	FConnectionInfo(const FConnectionInfo&) = default;
	FConnectionInfo(FString Host, FString User, FString Password, FString DataBase = TEXT(""), int32 Port = 3306)
    : Host(Host)
    , User(User)
    , Password(Password)
    , DataBase(DataBase)
	, Port(Port)
	{}
	UPROPERTY(EditAnywhere, BlueprintReadWrite ,Category="Hslm|MySQLConnection")
	FString Host;
	UPROPERTY(EditAnywhere, BlueprintReadWrite ,Category="Hslm|MySQLConnection")
	FString User;
	UPROPERTY(EditAnywhere, BlueprintReadWrite ,Category="Hslm|MySQLConnection")
	FString Password;
	UPROPERTY(EditAnywhere, BlueprintReadWrite ,Category="Hslm|MySQLConnection")
	FString DataBase;
	UPROPERTY(EditAnywhere, BlueprintReadWrite ,Category="Hslm|MySQLConnection")
	int32 Port;
};

struct FMySQLTaskContext
{
	TWeakObjectPtr<USQLQuery> QueryObjectWeakPtr;
	TFunction<void(FMySQLRunnable*, FMySQLTaskContextSharedPtr)> Task;
};

/**
 * 
 */
UCLASS(BlueprintType)
class HSLMMYSQL_API UMySQLConnection : public UObject
{
	GENERATED_BODY()

public:
	friend class FMySQLRunnable;	
public:
	
	UMySQLConnection(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName="Connect To Server (ConnectionInfo)"), Category="Hslm|MySQLConnection")
	bool ConnectToServerWithConnectionInfo(FConnectionInfo ConnectionInfo);
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName="Connect To Server (Host, User, Password, DataBase, Port)"), Category="Hslm|MySQLConnection")
	bool ConnectToServer(FString Host, FString User, FString Password, FString DataBase = TEXT(""), int32 Port = 3306);
	
	UFUNCTION(BlueprintPure, Category="Hslm|MySQLConnection")
	bool IsConnected() const;
	
	UFUNCTION(BlueprintCallable, Category="Hslm|MySQLConnection")
	void CloseConnection();
	
	UFUNCTION(BlueprintCallable, Category="Hslm|MySQLConnection")
	void ExecuteQuery(USQLQuery* Query);
	
	UFUNCTION(BlueprintCallable, Category="Hslm|MySQLConnection")
    void ExecuteUpdate(USQLUpdateQuery* UpdateQuery);
    
	// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMySQLConnectedResultSignature, FConnectionInfo, ConnectionInfo);
	// UPROPERTY(BlueprintAssignable, Category="Hslm|MySQLConnection")
	// FMySQLConnectedResultSignature OnConnected;
	//
	// UPROPERTY(BlueprintAssignable, Category="Hslm|MySQLConnection")
	// FMySQLConnectedResultSignature OnConnectionFailed;

private:
	void OnConnected(TSharedPtr<FConnectionInfo> ConnectionInfo);

	void OnConnectionFailed(TSharedPtr<FConnectionInfo> ConnectionInfo);

	void OnDisconnected();
	
	bool bIsConnected;
	
	TSharedPtr<FConnectionInfo> ConnectionInfoPtr;
	TUniquePtr<FMySQLRunnable> MySQLRunnable;
	bool IsConnecting;
};
