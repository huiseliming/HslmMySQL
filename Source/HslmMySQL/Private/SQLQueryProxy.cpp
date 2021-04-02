// Fill out your copyright notice in the Description page of Project Settings.


#include "SQLQueryProxy.h"
#include "HslmMySQL.h"
#include "SQLConnection.h"

USQLQueryProxy* USQLQueryProxy::CreateSQLQueryProxyObject(const FString& Host,const int32 Port,const FString& User,const FString&  Password, const FString& DataBase, const FString& SQLString,const TArray<FSQLQueryProxyBinds>& SQLQueryBind)
{
	auto Connection = FHslmMySQLModule::Get().GetConnection(Host, Port, User, Password, DataBase);
	auto Proxy = NewObject<USQLQueryProxy>();
	Proxy->SetFlags(RF_StrongRefOnFrame);
	Proxy->DataBase = DataBase;
	Proxy->SQLString = SQLString;
	Proxy->SQLQueryBind = SQLQueryBind;
	Connection->ExecuteSQL(Proxy);
	return Proxy;
}
