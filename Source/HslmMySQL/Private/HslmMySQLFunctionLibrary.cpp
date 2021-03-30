// Fill out your copyright notice in the Description page of Project Settings.


#include "HslmMySQLFunctionLibrary.h"

FSQLData UHslmMySQLFunctionLibrary::MakeSQLDataInteger(int32 InInteger)
{
	return FSQLData(InInteger);
}

FSQLData UHslmMySQLFunctionLibrary::MakeSQLDataInteger64(int64 InInteger64)
{
	return FSQLData(InInteger64);
}

FSQLData UHslmMySQLFunctionLibrary::MakeSQLDataDecimal(FString InDecimal)
{
	return FSQLData(InDecimal, ESQLData::Decimal);
}

FSQLData UHslmMySQLFunctionLibrary::MakeSQLDataFloat(float InFloat)
{
	return FSQLData(InFloat);
}

FSQLData UHslmMySQLFunctionLibrary::MakeSQLDataString(FString InString)
{
	return FSQLData(InString);
}

FSQLData UHslmMySQLFunctionLibrary::MakeSQLDataBinary(TArray<uint8> InBinary)
{
	return FSQLData(InBinary);
}

FSQLData UHslmMySQLFunctionLibrary::MakeSQLDataDateTime(FDateTime InDateTime)
{
	return FSQLData(InDateTime);
}

ESQLData UHslmMySQLFunctionLibrary::GetType(const FSQLData& Value)
{
	return Value.GetType();
}

int32 UHslmMySQLFunctionLibrary::ToInteger(const FSQLData& Value)
{
	return Value.GetInteger();
}

int64 UHslmMySQLFunctionLibrary::ToInteger64(const FSQLData& Value)
{
	return Value.GetInteger64();
}

float UHslmMySQLFunctionLibrary::ToFloat(const FSQLData& Value)
{
	return Value.GetFloat();
}

FString UHslmMySQLFunctionLibrary::ToString(const FSQLData& Value)
{
	return Value.GetString();
}

TArray<uint8> UHslmMySQLFunctionLibrary::GetBinary(const FSQLData& Value)
{
	return Value.GetBinary();
}

FDateTime UHslmMySQLFunctionLibrary::ToDateTime(const FSQLData& Value)
{
	return Value.GetDateTime();
}
