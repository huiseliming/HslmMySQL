// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "HslmMySQL.h"
#include "HslmDefine.h"
#include "SQLData.generated.h"

UENUM(BlueprintType)
enum class ESQLData :uint8
{
	None,
	Integer,
	Decimal,
    Float,
    String,
	Binary,
	DateTime,
};

class HSLMMYSQL_API FSQLDataBase
{
public:
	ESQLData Type;

	static bool CompareEqual(const FSQLDataBase& Lhs, const FSQLDataBase& Rhs);
	
	virtual int32         GetInteger() const { return 0; }
	virtual int64         GetInteger64() const { return 0; }
	virtual float	      GetFloat()  const { return 0.0f; }
	virtual double        GetDouble() const { return 0.0f; }
	virtual FString       GetString() const { return FString(); }
	virtual TArray<uint8> GetBinary() const { return TArray<uint8>(); }
	virtual TArray<uint8>* GetBinaryPtr() { return nullptr; }
	virtual FDateTime	  GetDateTime() const { return FDateTime(); }

	virtual bool IsNull() const { return true; }
	virtual FString GetType() const = 0;
	
protected:
	FSQLDataBase() : Type(ESQLData::None) {}
	virtual ~FSQLDataBase() {}
};


inline bool operator==(const FSQLDataBase& Lhs, const FSQLDataBase& Rhs)
{
	return FSQLDataBase::CompareEqual(Lhs, Rhs);
}

inline bool operator!=(const FSQLDataBase& Lhs, const FSQLDataBase& Rhs)
{
	return !FSQLDataBase::CompareEqual(Lhs, Rhs);
}

class HSLMMYSQL_API FSQLDataNull : public FSQLDataBase
{
public:
	FSQLDataNull(ESQLData SQLData)
	{
		Type = SQLData;
	}
	virtual bool IsNull() const override { return true; }
	virtual FString GetType() const override {return TEXT("Null");}
};

class HSLMMYSQL_API FSQLDataInteger : public FSQLDataBase
{
public:
	FSQLDataInteger(int64 InValue) : Value(InValue) {Type = ESQLData::Integer;}
	virtual int32 GetInteger() const override { return Value; }
	virtual int64 GetInteger64() const override { return Value; }
	virtual float GetFloat()  const override { return Value; }
	virtual double GetDouble() const override { return Value; }
	virtual FString GetString() const override { return FString::Printf(TEXT("%lld"), Value); }

	virtual bool IsNull() const override { return false; }
	virtual FString GetType() const override {return TEXT("Integer");}
protected:
	int64 Value;
};

class HSLMMYSQL_API FSQLDataDecimal : public FSQLDataBase
{
public:
	FSQLDataDecimal(const FString& InValue) : Value(InValue) {Type = ESQLData::Decimal;}
	virtual int32 GetInteger() const override { return FCString::Atoi(*Value); }
	virtual int64 GetInteger64() const override{ return FCString::Atoi64(*Value); }
	virtual float GetFloat()  const override { return FCString::Atof(*Value); }
	virtual double GetDouble() const override { return FCString::Atod(*Value); }
	virtual FString GetString() const override { return Value; }

	virtual bool IsNull() const override { return false; }
	virtual FString GetType() const override {return TEXT("String");}
protected:
	FString Value;
};

class HSLMMYSQL_API FSQLDataFloat : public FSQLDataBase
{
public:
	FSQLDataFloat(double InValue) : Value(InValue) {Type = ESQLData::Float;}
	virtual int32 GetInteger() const override { return Value; }
	virtual int64 GetInteger64() const override { return Value; }
	virtual float GetFloat()  const override { return Value; }
	virtual double GetDouble() const override { return Value; }
	virtual FString GetString() const override { return FString::SanitizeFloat(Value, 0); }
	
	virtual bool IsNull() const override { return false; }
	virtual FString GetType() const override {return TEXT("Float");}
protected:
	double Value;
};

class HSLMMYSQL_API FSQLDataString : public FSQLDataBase
{
public:
	FSQLDataString(const FString& InValue) : Value(InValue) {Type = ESQLData::String;}
	virtual int32 GetInteger() const override { return FCString::Atoi(*Value); }
	virtual int64 GetInteger64() const override{ return FCString::Atoi64(*Value); }
	virtual float GetFloat()  const override { return FCString::Atof(*Value); }
	virtual double GetDouble() const override { return FCString::Atod(*Value); }
	virtual FString GetString() const override { return Value; }

	virtual bool IsNull() const override { return false; }
	virtual FString GetType() const override {return TEXT("String");}
protected:
	FString Value;
};

class HSLMMYSQL_API FSQLDataBinary : public FSQLDataBase
{
public:
	FSQLDataBinary(const TArray<uint8>& InValue) : Value(InValue) {Type = ESQLData::Binary;}
	FSQLDataBinary(TArray<uint8>&& InValue) : Value(MoveTemp(InValue)) {Type = ESQLData::Binary;}
	virtual TArray<uint8> GetBinary() const override { return Value; }
	virtual TArray<uint8>* GetBinaryPtr() override { return &Value; }
	virtual FString GetString() const override { return FString::FromHexBlob(Value.GetData(), Value.Num()); }

	virtual bool IsNull() const override { return false; }
	virtual FString GetType() const override {return TEXT("Binary");}
protected:
	TArray<uint8> Value;
};

class HSLMMYSQL_API FSQLDataDateTime : public FSQLDataBase
{
public:
	FSQLDataDateTime(FDateTime& InValue) : Value(InValue) { Type = ESQLData::DateTime; }
	virtual FDateTime GetDateTime() const override { return Value; }
	virtual FString GetString() const override { return Value.ToString(TEXT("%Y-%m-%d %H:%M:%S.%s")); }

	virtual bool IsNull() const override { return false; }
	virtual FString GetType() const override {return TEXT("DateTime");}
protected:
	FDateTime Value;
};

USTRUCT(BlueprintType)
struct HSLMMYSQL_API FSQLData
{
	GENERATED_BODY()
	FSQLData(){}
	FSQLData(int32 InInteger) { SQLData = MakeShared<FSQLDataInteger>(InInteger); }
	FSQLData(int64 InInteger64) { SQLData = MakeShared<FSQLDataInteger>(InInteger64); }
	FSQLData(float InFloat){ SQLData = MakeShared<FSQLDataFloat>(InFloat); }
	FSQLData(double InDouble){ SQLData = MakeShared<FSQLDataFloat>(InDouble); }
	FSQLData(FString InString, ESQLData InSQLData = ESQLData::String)
	{
		if (InSQLData == ESQLData::String)
		{
			SQLData = MakeShared<FSQLDataString>(InString);
		}else if(InSQLData == ESQLData::Decimal)
		{
			SQLData = MakeShared<FSQLDataDecimal>(InString);
		}else{
			UE_LOG(LogHslmMySQL, Fatal, TEXT("[%s] [tan90]"), *UE__FUNC__LINE__)
		}
	}
	FSQLData(const TArray<uint8>& InBinary){ SQLData = MakeShared<FSQLDataBinary>(InBinary); }
	FSQLData(TArray<uint8>&& InBinary){ SQLData = MakeShared<FSQLDataBinary>(MoveTemp(InBinary)); }
	FSQLData(FDateTime& InDateTime){ SQLData = MakeShared<FSQLDataDateTime>(InDateTime); }
	
	ESQLData GetType() const { return SQLData->Type; }
	
	int32 GetInteger() const { return SQLData->GetInteger(); }
	int64 GetInteger64() const { return SQLData->GetInteger64(); }
	float GetFloat() const { return SQLData->GetFloat(); }
	double GetDouble() const { return SQLData->GetDouble(); }
	FString GetString() const { return SQLData->GetString(); }
	TArray<uint8> GetBinary() const { return SQLData->GetBinary(); }
	TArray<uint8>* GetBinaryPtr() const { return SQLData->GetBinaryPtr(); }
	FDateTime GetDateTime() const { return SQLData->GetDateTime(); }
	
	TSharedPtr<FSQLDataBase> SQLData;
};

USTRUCT(BlueprintType)
struct HSLMMYSQL_API FSQLTableRow
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "MySQLConnector|Tables|Fields")
	TArray<FSQLData> SQLDatas;
};

USTRUCT(BlueprintType)
struct HSLMMYSQL_API FSQLMateData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySQLConnector|Tables|Fields")
	FString FieldName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySQLConnector|Tables|Fields")
	ESQLData FieldType;
};


USTRUCT(BlueprintType)
struct HSLMMYSQL_API FSQLResultSet
{
	GENERATED_BODY()
	FSQLResultSet() = default;
	FSQLResultSet(const FSQLResultSet&) = default;
	FSQLResultSet(FSQLResultSet&& SQLResultSet) noexcept
	{
		MetaDatas = MoveTemp(SQLResultSet.MetaDatas);
		TableRows = MoveTemp(SQLResultSet.TableRows);
	}
	FSQLResultSet& operator=(const FSQLResultSet&) = default;
	FSQLResultSet& operator=(FSQLResultSet&& SQLResultSet) noexcept
	{
		if(&SQLResultSet != this)
		{
			MetaDatas = MoveTemp(SQLResultSet.MetaDatas);
			TableRows = MoveTemp(SQLResultSet.TableRows);
		}
		return *this;
	}
	UPROPERTY(BlueprintReadWrite, Category = "MySQLConnector|Tables|Fields")
	TArray<FSQLMateData> MetaDatas;
	UPROPERTY(BlueprintReadWrite, Category = "MySQLConnector|Tables|Fields")
	TArray<FSQLTableRow> TableRows;
};

