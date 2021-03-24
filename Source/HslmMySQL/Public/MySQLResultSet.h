// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Json.h"
#include "MySQLResultSet.generated.h"

UENUM()
namespace EMySQLData
{
	enum Type
	{
		Undefined = 0,

        /* Column types as defined in protobuf (mysqlx_resultset.proto)*/
        SInt     = 1, /**< 64-bit signed integer number type*/
        UInt     = 2, /**< 64-bit unsigned integer number type*/
        Double   = 5, /**< Floating point double number type*/
        Float    = 6, /**< Floating point float number type*/
        Bytes    = 7, /**< Bytes array type*/
        Time     = 10,/**< Time type*/
        Datetime = 12,/**< Datetime type*/
        Set      = 15,/**< Set type*/
        Enum     = 16,/**< Enum type*/
        Bit      = 17,/**< Bit type*/
        Decimal  = 18,/**< Decimal type*/

        /* Column types from DevAPI (no number constants assigned, just names)*/
        Bool     = 19,/**< Bool type*/
        Json     = 20,/**< JSON type*/
        String   = 21,/**< String type*/
        Geometry = 22,/**< Geometry type*/
        Timestamp= 23,/**< Timestamp type*/

        Null     = 100, /**< NULL value*/
        Expr     = 101  /**< Expression type*/
    };
}

USTRUCT(BlueprintType)
struct FMySQLResultSet
{
	GENERATED_USTRUCT_BODY()

	TArray<EMySQLData::Type> ColumnDataTypes;
	TArray<FString> ColumnNames;
	TArray<FJsonObject> Data;
};