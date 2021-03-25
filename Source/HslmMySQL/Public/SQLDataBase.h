// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

enum class ESQLData
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
