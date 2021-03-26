// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SQLDataBase.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HslmMySQLFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class HSLMMYSQL_API UHslmMySQLFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure, meta = (Keywords = "construct build", NativeMakeFunc, DisplayName="Make SQLData (Integer)"), Category = "Hslm|MySQLFunctionLibrary|SQLData")
    static FSQLData MakeSQLDataInteger(int32 InInteger);
	UFUNCTION(BlueprintPure, meta = (Keywords = "construct build", NativeMakeFunc, DisplayName="Make SQLData (Integer64)"), Category = "Hslm|MySQLFunctionLibrary|SQLData")
	static FSQLData MakeSQLDataInteger64(int64 InInteger64);
	UFUNCTION(BlueprintPure, meta = (Keywords = "construct build", NativeMakeFunc, DisplayName="Make SQLData (Float)"), Category = "Hslm|MySQLFunctionLibrary|SQLData")
	static FSQLData MakeSQLDataFloat(float InFloat);
	// UFUNCTION(BlueprintPure, meta = (Keywords = "construct build", NativeMakeFunc, DisplayName="Make SQLData (Double)"), Category = "Hslm|MySQLFunctionLibrary|SQLData")
	// static FSQLData MakeSQLDataDouble(double InDouble);
	UFUNCTION(BlueprintPure, meta = (Keywords = "construct build", NativeMakeFunc, DisplayName="Make SQLData (String)"), Category = "Hslm|MySQLFunctionLibrary|SQLData")
	static FSQLData MakeSQLDataString(FString InString);
	UFUNCTION(BlueprintPure, meta = (Keywords = "construct build", NativeMakeFunc, DisplayName="Make SQLData (Binary)"), Category = "Hslm|MySQLFunctionLibrary|SQLData")
	static FSQLData MakeSQLDataBinary(TArray<uint8> InBinary);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " Get Type", CompactNodeTitle = "->", BlueprintAutocast), Category = "Hslm|MySQLFunctionLibrary")
	static ESQLData GetType( UPARAM(ref) const FSQLData& Value );
	
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Convert To Integer", CompactNodeTitle = "->", BlueprintAutocast), Category = "Hslm|MySQLFunctionLibrary")
	static int32 ToInteger( UPARAM(ref) const FSQLData& Value );
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Convert To Integer64", CompactNodeTitle = "->", BlueprintAutocast), Category = "Hslm|MySQLFunctionLibrary")
	static int64 ToInteger64( UPARAM(ref) const FSQLData& Value );
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Convert To Float", CompactNodeTitle = "->", BlueprintAutocast), Category = "Hslm|MySQLFunctionLibrary")
	static float ToFloat( UPARAM(ref) const FSQLData& Value );
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Convert To String", CompactNodeTitle = "->", BlueprintAutocast), Category = "Hslm|MySQLFunctionLibrary")
	static FString ToString( UPARAM(ref) const FSQLData& Value );
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Convert To Binary", CompactNodeTitle = "->", BlueprintAutocast), Category = "Hslm|MySQLFunctionLibrary")
	static TArray<uint8> GetBinary( UPARAM(ref) const FSQLData& Value );
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Convert To Date", CompactNodeTitle = "->", BlueprintAutocast), Category = "Hslm|MySQLFunctionLibrary")
	static FDateTime ToDate( UPARAM(ref) const FSQLData& Value );
	
};
