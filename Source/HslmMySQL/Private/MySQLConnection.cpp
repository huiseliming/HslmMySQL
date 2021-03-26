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
		Connection = Driver->connect(UTF8URI, UTF8User, UTF8Password);
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
	return !(Impl->Connection && !Impl->Connection->isClosed());
}

void UMySQLConnection::CloseConnection()
{
	if(Impl->Connection != nullptr)
	{
		Impl->Connection->close();
		Impl->Connection.reset();
	}
}

bool UMySQLConnection::Execute(FString InDBName, FString InSQL)
{
	bool ReturnValue;
	if(IsClosed())
	{
		return false;
	}
	try
	{
		const std::string UTF8DBName = TCHAR_TO_UTF8(*InDBName);
		if(Impl->Connection->getSchema() != UTF8DBName)
			Impl->Connection->setSchema(UTF8DBName);
		const std::string UTF8SQL = TCHAR_TO_UTF8(*InSQL);
		std::unique_ptr< sql::Statement > Statement(Impl->Connection->createStatement());
		ReturnValue = Statement->execute(UTF8SQL);
	}
	catch (sql::SQLException& SQLException)
	{
		UE_LOG(LogHslmMySQL, Error, TEXT("[%s] SQLException: %s, MySQLErrorCode: %d, SQLState: %d"), *UE__FUNC__LINE__, *FString(UTF8_TO_TCHAR(SQLException.what())), SQLException.getErrorCode(), *FString(UTF8_TO_TCHAR(SQLException.getSQLStateCStr())));
		return false;
	}
	return ReturnValue;
}

bool UMySQLConnection::ExecuteQuery(FString InDBName, FString InSQL, TArray<FSQLTableRow>& OutResultSet)
{
	if(IsClosed())
	{
		return false;
	}
	OutResultSet.Empty();
	try
	{
		const std::string UTF8SQL = TCHAR_TO_UTF8(*InSQL);
		const std::string UTF8DBName = TCHAR_TO_UTF8(*InDBName);
		if(Impl->Connection->getSchema() != UTF8DBName)
			Impl->Connection->setSchema(UTF8DBName);
		std::unique_ptr< sql::Statement > Statement(Impl->Connection->createStatement());
		std::unique_ptr< sql::ResultSet > ResultSet(Statement->executeQuery(UTF8SQL));
		sql::ResultSetMetaData* ResultSetMetaData = ResultSet->getMetaData();
		const uint32 ColumnCount = ResultSetMetaData->getColumnCount();
		while (ResultSet->next())
		{
			for (uint32 i = 1; i < ColumnCount + 1; ++i)
			{
				FSQLTableRow SQLTableRow;
				std::string UTF8ColumnName = ResultSetMetaData->getColumnName(i);
				FString ColumnName(UTF8_TO_TCHAR(UTF8ColumnName.c_str()));
				const int32 ColumnType = ResultSetMetaData->getColumnType(i);
				switch (ColumnType)
				{
					case sql::DataType::UNKNOWN:
					case sql::DataType::BIT:
					case sql::DataType::TINYINT:
					case sql::DataType::SMALLINT:
					case sql::DataType::MEDIUMINT:
					case sql::DataType::INTEGER:
					case sql::DataType::BIGINT:
						SQLTableRow.SQLDatas.Add(ColumnName, FSQLData(ResultSet->getInt64(i)));
						break;
					case sql::DataType::REAL:
					case sql::DataType::DOUBLE:
					case sql::DataType::DECIMAL:
					case sql::DataType::NUMERIC:
						SQLTableRow.SQLDatas.Add(ColumnName, FSQLData(static_cast<double>(ResultSet->getDouble(i))));
						break;
					case sql::DataType::CHAR:
					case sql::DataType::VARCHAR:
					case sql::DataType::LONGVARCHAR:
					case sql::DataType::JSON:
						{
							std::string UTF8String = ResultSet->getString(i);
							SQLTableRow.SQLDatas.Add(ColumnName, FSQLData(FString(UTF8_TO_TCHAR(UTF8String.c_str()))));
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
							TArray<uint8> BlobArray;
							BlobArray.SetNum(BlobSize);
							InputStream->read(reinterpret_cast<char*>(BlobArray.GetData()), BlobSize);
							SQLTableRow.SQLDatas.Add(ColumnName, FSQLData(BlobArray));
						}
						break;
					case sql::DataType::TIMESTAMP:
					case sql::DataType::DATE:
					case sql::DataType::TIME:
					case sql::DataType::YEAR:
					case sql::DataType::GEOMETRY:
					case sql::DataType::ENUM:
					case sql::DataType::SET:
					case sql::DataType::SQLNULL:
					default:
						UE_LOG(LogHslmMySQL, Fatal, TEXT("[%s] tan90"), *UE__FUNC__LINE__);
				}
				OutResultSet.Add(SQLTableRow);
			}
		}
	}
	catch (sql::SQLException& SQLException)
	{
		UE_LOG(LogHslmMySQL, Error, TEXT("[%s] SQLException: %s, MySQLErrorCode: %d, SQLState: %d"), *UE__FUNC__LINE__, *FString(UTF8_TO_TCHAR(SQLException.what())), SQLException.getErrorCode(), *FString(UTF8_TO_TCHAR(SQLException.getSQLStateCStr())));
		return false;
	}
	return true;
}

int32 UMySQLConnection::ExecuteUpdate(FString InDBName, FString InSQL, TArray<FSQLData> InSQLTableRow)
{
	int32 ReturnValue = -1;
	if(IsClosed())
	{
		return false;
	}
	try
	{
		const std::string UTF8SQL = TCHAR_TO_UTF8(*InSQL);
		std::unique_ptr< sql::PreparedStatement > PreparedStatement(Impl->Connection->prepareStatement(UTF8SQL));
		struct BlobData
		{
			class StreamBufferData : public std::streambuf
			{
			public:
				StreamBufferData(char *in_data, size_t in_size)
				{
					setg(in_data, in_data, in_data + in_size);
				}
			};
			BlobData(std::vector<uint8>&& Binary)
			{
				Bytes = std::move(Binary);
				StreamBuffer = std::make_unique<StreamBufferData>(reinterpret_cast<char *>(Bytes.data()),Bytes.size());
				IStream = std::make_unique<std::istream>(StreamBuffer.get());
			}
			BlobData(BlobData&& Other)
			{
				Bytes = std::move(Other.Bytes);
				StreamBuffer = std::move(Other.StreamBuffer);
				IStream = std::move(Other.IStream);
			}
	
			~BlobData()
			{
				IStream.reset();
				StreamBuffer.reset();
			}
			std::vector<uint8> Bytes;
			std::unique_ptr<StreamBufferData> StreamBuffer;
			std::unique_ptr<std::istream> IStream;
			
			BlobData() = default;
		};
		std::vector<BlobData> BlobDatas;
		
		for (int32 i = 0; i < InSQLTableRow.Num(); i++)
		{
			switch (InSQLTableRow[i].GetType())
			{
				case ESQLData::Integer:
					PreparedStatement->setInt64(i + 1,InSQLTableRow[i].GetInteger64());
					break;
				case ESQLData::Float:
					PreparedStatement->setDouble(i + 1,InSQLTableRow[i].GetDouble());
					break;
				case ESQLData::String:
					{
						FString String;
						std::string UTF8String = TCHAR_TO_UTF8(*(InSQLTableRow[i].GetString()));
						PreparedStatement->setString(i + 1,UTF8String);
					}
					break;
				case ESQLData::Binary:
					{
						TArray<uint8> Binary = InSQLTableRow[i].GetBinary();
						std::vector<uint8> Bytes;
						Bytes.resize(Binary.Num());
						FMemory::Memcpy(Bytes.data(),Binary.GetData(),Binary.Num());
						BlobDatas.emplace_back(BlobData(std::move(Bytes)));
						PreparedStatement->setBlob(i + 1,BlobDatas[BlobDatas.size()-1].IStream.get());
					}
					break;
				case ESQLData::Date:
				case ESQLData::None:
				default:
                   UE_LOG(LogHslmMySQL, Fatal, TEXT("[%s] tan90"), *UE__FUNC__LINE__);
			}
		}
		ReturnValue = PreparedStatement->executeUpdate();
	}
	catch (sql::SQLException& SQLException)
	{
		UE_LOG(LogHslmMySQL, Error, TEXT("[%s] SQLException: %s, MySQLErrorCode: %d, SQLState: %d"), *UE__FUNC__LINE__, *FString(UTF8_TO_TCHAR(SQLException.what())), SQLException.getErrorCode(), *FString(UTF8_TO_TCHAR(SQLException.getSQLStateCStr())));
		return -1;
	}
	return ReturnValue;
}
