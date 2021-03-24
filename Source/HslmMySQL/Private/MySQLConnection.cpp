// Fill out your copyright notice in the Description page of Project Settings.

#include "MySQLConnection.h"
#include "mysql/jdbc.h"
#include <string>
#include <vector>

#include "HslmMySQL.h"
#include "HslmDefine.h"

struct UMySQLConnection::FImpl
{
	std::unique_ptr<sql::Connection> Connection;
};

UMySQLConnection::UMySQLConnection(const FObjectInitializer& ObjectInitializer)
	: UObject(ObjectInitializer)
	, Impl(new FImpl)
{
	
}

UMySQLConnection::~UMySQLConnection()
{
	CloseConnection();
	delete Impl;
}

bool UMySQLConnection::ConnectToServer(FString Host, int32 Port, FString User, FString Password, FString DataBase)
{
	sql::Connection* Connection = nullptr;
	CloseConnection();
	try
	{
		sql::Driver* Driver = sql::mysql::get_driver_instance();
		const std::string UTF8Host(TCHAR_TO_UTF8(*Host));
		const std::string UTF8User(TCHAR_TO_UTF8(*User));
		const std::string UTF8Password(TCHAR_TO_UTF8(*Password));
		const std::string UTF8DataBase(TCHAR_TO_UTF8(*DataBase));
		const std::string UTF8URI = "tcp://" + UTF8Host + ":" + std::to_string(Port) + "/" + UTF8DataBase;
		Connection = Driver->connect(UTF8URI, UTF8DataBase, UTF8Password);
	}
	catch (sql::SQLException& SQLException)
	{
		UE_LOG(LogHslmMySQL, Error, TEXT("[%s] SQLException: %s, MySQLErrorCode: %d, SQLState: %d"), *UE__FUNC__LINE__, *FString(UTF8_TO_TCHAR(SQLException.what())), SQLException.getErrorCode(), *FString(UTF8_TO_TCHAR(SQLException.getSQLStateCStr())));
		return false;
	}
	Impl->Connection.reset(Connection);
	return true;
}

bool UMySQLConnection::IsClosed() const
{
	return Impl->Connection && !Impl->Connection->isClosed();
}

void UMySQLConnection::CloseConnection()
{
	if(Impl->Connection != nullptr)
	{
		Impl->Connection->close();
		Impl->Connection.reset();
	}
}

bool UMySQLConnection::ExecuteQuery(FString InSQL, FMySQLResultSet& OutResultSet)
{
	OutResultSet.Data.Empty();
	if(IsClosed())
	{
		return false;
	}
	try
	{
		std::unique_ptr< sql::Statement > Statement(Impl->Connection->createStatement());
		std::unique_ptr< sql::ResultSet > ResultSet(Statement->executeQuery("SELECT 'Welcome to Connector/C++' AS _message"));
		sql::ResultSetMetaData* ResultSetMetaData(ResultSet->getMetaData());
		const uint32 ColumnCount = ResultSetMetaData->getColumnCount();
		while (ResultSet->next())
		{
			FJsonObject RowObject;
			for (uint32 i = 0; i < ColumnCount; ++i)
			{
				const int32 ColumnType = ResultSetMetaData->getColumnType(i);
				std::string UTF8ColumnName = ResultSetMetaData->getColumnName(i);
				FString ColumnName(UTF8_TO_TCHAR(UTF8ColumnName.c_str()));
				switch (ColumnType)
				{
				case sql::DataType::UNKNOWN:
                   case sql::DataType::BIT:
                       UE_LOG(LogHslmMySQL, Fatal, TEXT("[%s] sql::DataType no convert impl "), *UE__FUNC__LINE__);
					break;
				case sql::DataType::TINYINT:
                case sql::DataType::SMALLINT:
                case sql::DataType::MEDIUMINT:
                case sql::DataType::INTEGER:
                    RowObject.SetNumberField(ColumnName,ResultSet->getInt(i));
					break;
				case sql::DataType::BIGINT:
					RowObject.SetNumberField(ColumnName,ResultSet->getInt64(i));
					break;
				case sql::DataType::REAL:
                case sql::DataType::DOUBLE:
                    RowObject.SetNumberField(ColumnName,ResultSet->getDouble(i));
					break;
				case sql::DataType::DECIMAL:
                case sql::DataType::NUMERIC:
					UE_LOG(LogHslmMySQL, Fatal, TEXT("[%s] sql::DataType no convert impl "), *UE__FUNC__LINE__);
					break;
				case sql::DataType::CHAR:
                case sql::DataType::VARCHAR:
                case sql::DataType::LONGVARCHAR:
					{
	                    std::string UTF8String = ResultSet->getString(i);
						RowObject.SetStringField(ColumnName, FString(UTF8_TO_TCHAR(UTF8String.c_str())));
					}
					break;
				case sql::DataType::BINARY:
                case sql::DataType::VARBINARY:
                case sql::DataType::LONGVARBINARY:
					{
						// Large amounts of data can cause performance problems
	                    std::istream* InputStream = ResultSet->getBlob(i);
						InputStream->seekg(0, std::ios::end);
						uint32 BlobSize = InputStream->tellg();
						InputStream->seekg(0, std::ios::beg);
						std::vector<char> Blob(BlobSize);
						InputStream->read(Blob.data(), BlobSize);
						TArray<TSharedPtr<FJsonValue>> BlobArray;
						for (int j = 0; j < Blob.size(); ++j)
						{
							BlobArray.Add(TSharedPtr<FJsonValue>(new FJsonValueNumber(uint8(Blob[j]))));
						}
						RowObject.SetArrayField(ColumnName, BlobArray);
					}
					break;
				case sql::DataType::TIMESTAMP:
				case sql::DataType::DATE:
				case sql::DataType::TIME:
				case sql::DataType::YEAR:
				case sql::DataType::GEOMETRY:
				case sql::DataType::ENUM:
				case sql::DataType::SET:
					UE_LOG(LogHslmMySQL, Fatal, TEXT("[%s] sql::DataType no convert impl "), *UE__FUNC__LINE__);
					break;
				case sql::DataType::SQLNULL:
					break;
				case sql::DataType::JSON:
					{
						std::string UTF8String = ResultSet->getString(i);
						RowObject.SetStringField(ColumnName, FString(UTF8_TO_TCHAR(UTF8String.c_str())));
					}
					break;
				default:
					UE_LOG(LogHslmMySQL, Fatal, TEXT("[%s] tan90"), *UE__FUNC__LINE__);
				break;
				}
			}
			OutResultSet.Data.Add(RowObject);
		}
	}
	catch (sql::SQLException& SQLException)
	{
		UE_LOG(LogHslmMySQL, Error, TEXT("[%s] SQLException: %s, MySQLErrorCode: %d, SQLState: %d"), *UE__FUNC__LINE__, *FString(UTF8_TO_TCHAR(SQLException.what())), SQLException.getErrorCode(), *FString(UTF8_TO_TCHAR(SQLException.getSQLStateCStr())));
		return false;
	}
	return true;
}

bool UMySQLConnection::ExecuteUpdate(FString SQL, TArray<FJsonObject> InValues)
{
	
	return true;
}
