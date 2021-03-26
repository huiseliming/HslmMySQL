// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SQLDataBase.generated.h"
UENUM(BlueprintType)
enum class ESQLData :uint8
{
	None,
	Integer,
    Float,
    String,
	Binary,
	Date,
};

class HSLMMYSQL_API FSQLDataBase
{
public:
	ESQLData Type;

	static bool CompareEqual(const FSQLDataBase& Lhs, const FSQLDataBase& Rhs);
	
	virtual int32         GetInteger() const { return GetInteger64(); }
	virtual int64         GetInteger64() const { return 0; }
	virtual float	      GetFloat()  const { return GetDouble(); }
	virtual double        GetDouble() const { return 0.0f; }
	virtual FString       GetString() const { return FString(); }
	virtual TArray<uint8> GetBinary() const { return TArray<uint8>(); }
	virtual FDateTime     GetDate() const { return FDateTime(); }
	
protected:
	FSQLDataBase() : Type(ESQLData::None) {}
	virtual ~FSQLDataBase() {}

	virtual FString GetType() const = 0;

	void ErrorMessage(const FString& InType) const;
};

inline bool operator==(const FSQLDataBase& Lhs, const FSQLDataBase& Rhs)
{
	return FSQLDataBase::CompareEqual(Lhs, Rhs);
}

inline bool operator!=(const FSQLDataBase& Lhs, const FSQLDataBase& Rhs)
{
	return !FSQLDataBase::CompareEqual(Lhs, Rhs);
}

class HSLMMYSQL_API FSQLDataInteger : public FSQLDataBase
{
public:
	FSQLDataInteger(int64 InValue) : Value(InValue) {Type = ESQLData::Integer;}

	virtual int64 GetInteger64() const override { return Value; }
	
protected:
	int64 Value;
	virtual FString GetType() const override {return TEXT("Integer");}
};

class HSLMMYSQL_API FSQLDataFloat : public FSQLDataBase
{
public:
	FSQLDataFloat(double InValue) : Value(InValue) {Type = ESQLData::Float;}

	virtual double GetDouble() const override { return Value; }
	
protected:
	double Value;
	virtual FString GetType() const override {return TEXT("Float");}
};

class HSLMMYSQL_API FSQLDataString : public FSQLDataBase
{
public:
	FSQLDataString(FString InValue) : Value(InValue) {Type = ESQLData::String;}

	virtual FString GetString() const override { return Value; }
	
protected:
	FString Value;
	virtual FString GetType() const override {return TEXT("String");}
};

class HSLMMYSQL_API FSQLDataBinary : public FSQLDataBase
{
public:
	FSQLDataBinary(TArray<uint8> InValue) : Value(InValue) {Type = ESQLData::Binary;}

	virtual TArray<uint8> GetBinary() const override { return Value; }
	
protected:
	TArray<uint8> Value;
	virtual FString GetType() const override {return TEXT("Binary");}
};

USTRUCT(BlueprintType)
struct HSLMMYSQL_API FSQLData
{
	GENERATED_USTRUCT_BODY()
	FSQLData(){}
	FSQLData(int32 InInteger) { SQLData = MakeShared<FSQLDataInteger>(InInteger); }
	FSQLData(int64 InInteger64) { SQLData = MakeShared<FSQLDataInteger>(InInteger64); }
	FSQLData(float InFloat){ SQLData = MakeShared<FSQLDataFloat>(InFloat); }
	FSQLData(double InDouble){ SQLData = MakeShared<FSQLDataFloat>(InDouble); }
	FSQLData(FString InString){ SQLData = MakeShared<FSQLDataString>(InString); }
	FSQLData(TArray<uint8> InBinary){ SQLData = MakeShared<FSQLDataBinary>(InBinary); }
	FSQLData(ESQLData Type, int32 InInteger, float InFloat, FString InString, TArray<uint8> InBinary, FDateTime InDate)
	{
		switch (Type)
		{
		case ESQLData::Integer:
			SQLData = MakeShared<FSQLDataInteger>(InInteger);
			break;
		case ESQLData::Float:
			SQLData = MakeShared<FSQLDataFloat>(InFloat);
			break;
		case ESQLData::String:
			SQLData = MakeShared<FSQLDataString>(InString);
			break;
		case ESQLData::Binary:
			SQLData = MakeShared<FSQLDataBinary>(InBinary);
			break;
		case ESQLData::Date:
        case ESQLData::None:
        default:
			break;
           //UE_LOG(LogHslmMySQL, Fatal, TEXT("[%s] tan90"), *UE__FUNC__LINE__);
		}
	}
	ESQLData GetType() const { return SQLData->Type; }
	int32 GetInteger() const { return SQLData->GetInteger(); }
	int64 GetInteger64() const { return SQLData->GetInteger64(); }
	float GetFloat() const { return SQLData->GetFloat(); }
	double GetDouble() const { return SQLData->GetDouble(); }
	FString GetString() const { return SQLData->GetString(); }
	TArray<uint8> GetBinary() const { return SQLData->GetBinary(); }
	FDateTime GetDate() const { return SQLData->GetDate(); }
	
	TSharedPtr<FSQLDataBase> SQLData;
};

USTRUCT(BlueprintType)
struct HSLMMYSQL_API FSQLTableRow
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "MySQLConnector|Tables|Fields")
	TMap<FString, FSQLData> SQLDatas;
};

USTRUCT(BlueprintType)
struct HSLMMYSQL_API FSQLResultSet
{
	GENERATED_USTRUCT_BODY()
	TArray<FSQLTableRow> TableRows;
};

