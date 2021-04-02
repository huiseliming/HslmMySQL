// Fill out your copyright notice in the Description page of Project Settings.


#include "SQLConnection.h"

#include <vector>

#include "HslmMySQL.h"
#include "HslmDefine.h"
#include "SQLQueryProxy.h"
#include "mysql/jdbc.h"

struct FSQLQueryProxyPackage
{
	int32 ErrorCode;
	FString ErrorString;
	int64 AffectedRowNum;
	FSQLResultSet ResultSet;
};
static ESQLData ConvertSQLType(uint32 InDataType)
{
	switch (InDataType)
	{
	case sql::DataType::UNKNOWN:
		return ESQLData::None;
	case sql::DataType::BIT:
    case sql::DataType::TINYINT:
    case sql::DataType::SMALLINT:
    case sql::DataType::MEDIUMINT:
    case sql::DataType::INTEGER:
    case sql::DataType::BIGINT:
        return ESQLData::Integer;
	case sql::DataType::REAL:
    case sql::DataType::DOUBLE:
        return ESQLData::Float;
	case sql::DataType::DECIMAL:
    case sql::DataType::NUMERIC:
        return ESQLData::Decimal;
	case sql::DataType::CHAR:
    case sql::DataType::VARCHAR:
    case sql::DataType::LONGVARCHAR:
        return ESQLData::String;
	case sql::DataType::BINARY:
    case sql::DataType::VARBINARY:
    case sql::DataType::LONGVARBINARY:
        return ESQLData::Binary;
	case sql::DataType::TIMESTAMP:
    case sql::DataType::DATE:
    case sql::DataType::TIME:
    case sql::DataType::YEAR:
        return ESQLData::DateTime;
	case sql::DataType::GEOMETRY:
    case sql::DataType::ENUM:
    case sql::DataType::SET:
    case sql::DataType::SQLNULL:
    case sql::DataType::JSON:
    default:
        return ESQLData::None;
	}
}

static void Query(sql::Connection* Connection, std::string SQL, FSQLQueryProxyPackage& SQLQueryProxyPackage)
{
	std::unique_ptr< sql::Statement > Statement(Connection->createStatement());
	std::unique_ptr< sql::ResultSet > ResultSet(Statement->executeQuery(SQL));
	sql::ResultSetMetaData* ResultSetMetaData = ResultSet->getMetaData();
	const uint32 ColumnCount = ResultSetMetaData->getColumnCount();
	FSQLResultSet& SQLResultSet = SQLQueryProxyPackage.ResultSet;
	for (uint32 i = 1; i < ColumnCount + 1; ++i)
	{
		std::string UTF8ColumnName = ResultSetMetaData->getColumnName(i);
		const int32 ColumnType = ResultSetMetaData->getColumnType(i);
		SQLResultSet.MetaDatas.Add(FSQLMateData{FString(UTF8_TO_TCHAR(UTF8ColumnName.c_str())),ConvertSQLType(ColumnType)});
	}
	while (ResultSet->next())
	{
		int32 RowIndex = SQLResultSet.TableRows.Add(FSQLTableRow());
		for (uint32 i = 1; i < ColumnCount + 1; ++i)
		{
			switch (SQLResultSet.MetaDatas[i-1].FieldType)
			{
			case ESQLData::Integer:
				SQLResultSet.TableRows[RowIndex].SQLDatas.Add(FSQLData(ResultSet->getInt64(i)));
				break;
			case ESQLData::Decimal:
				SQLResultSet.TableRows[RowIndex].SQLDatas.Add(FSQLData(UTF8_TO_TCHAR(ResultSet->getString(i).c_str()), ESQLData::Decimal));
				break;
			case ESQLData::Float:
				SQLResultSet.TableRows[RowIndex].SQLDatas.Add(FSQLData(static_cast<double>(ResultSet->getDouble(i))));
				break;
			case ESQLData::String:
				SQLResultSet.TableRows[RowIndex].SQLDatas.Add(FSQLData(UTF8_TO_TCHAR(ResultSet->getString(i).c_str())));
				break;
			case ESQLData::Binary:
				{
					// Large amounts of data can cause performance problems?
					std::istream* InputStream = ResultSet->getBlob(i);
					InputStream->seekg(0, std::ios::end);
					uint32 BlobSize = InputStream->tellg();
					InputStream->seekg(0, std::ios::beg);
					TArray<uint8> BlobArray;
					BlobArray.SetNum(BlobSize);
					InputStream->read(reinterpret_cast<char*>(BlobArray.GetData()), BlobSize);
					SQLResultSet.TableRows[RowIndex].SQLDatas.Add(FSQLData(MoveTemp(BlobArray)));
				}
				break;
			case ESQLData::DateTime:
				{
					FDateTime DateTime;
					FDateTime::Parse(FString(UTF8_TO_TCHAR(ResultSet->getString(i).c_str())), DateTime);
					SQLResultSet.TableRows[RowIndex].SQLDatas.Add(FSQLData(DateTime));
				}
				break;
			case ESQLData::None:
			default:
                UE_LOG(LogHslmMySQL, Fatal, TEXT("[%s] [tan90]"), *UE__FUNC__LINE__);
				break;
			}
		}
	}
}
	
static void QueryWithBinds(sql::Connection* Connection, std::string SQL, TArray<FSQLQueryProxyBinds>& QueryBind,  FSQLQueryProxyPackage& SQLQueryProxyPackage)
{
	FSQLResultSet& SQLResultSet = SQLQueryProxyPackage.ResultSet;
	std::unique_ptr< sql::PreparedStatement > PreparedStatement(Connection->prepareStatement(SQL));
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
	for (int32 i = 0; i < QueryBind.Num(); i++)
	{
		for (int32 j = 0; j < QueryBind[i].SQLData.Num(); j++)
		{
			switch (QueryBind[i].SQLData[j].GetType())
			{
			case ESQLData::Integer:
				PreparedStatement->setInt64(j + 1,QueryBind[i].SQLData[j].GetInteger64());
				break;
			case ESQLData::Decimal:
				PreparedStatement->setString(j + 1,std::string(TCHAR_TO_UTF8(*QueryBind[i].SQLData[j].GetString())));
				break;
			case ESQLData::Float:
				PreparedStatement->setDouble(j + 1,QueryBind[i].SQLData[j].GetDouble());
				break;
			case ESQLData::String:
				PreparedStatement->setString(j + 1, std::string(TCHAR_TO_UTF8(*QueryBind[i].SQLData[j].GetString())));
				break;
			case ESQLData::Binary:
				{
					TArray<uint8> Binary = QueryBind[i].SQLData[j].GetBinary();
					std::vector<uint8> Bytes;
					Bytes.resize(Binary.Num());
					FMemory::Memcpy(Bytes.data(),Binary.GetData(),Binary.Num());
					BlobDatas.emplace_back(BlobData(std::move(Bytes)));
					PreparedStatement->setBlob(j + 1,BlobDatas[BlobDatas.size()-1].IStream.get());
				}
				break;
			case ESQLData::DateTime:
				PreparedStatement->setString(j + 1, std::string(TCHAR_TO_UTF8(*QueryBind[i].SQLData[j].GetString())));
				break;
            case ESQLData::None:
            default:
               UE_LOG(LogHslmMySQL, Fatal, TEXT("[%s] [tan90]"), *UE__FUNC__LINE__);
			}
		}
		SQLQueryProxyPackage.AffectedRowNum += PreparedStatement->executeUpdate();
	}
}

USQLConnection::~USQLConnection()
{
}

void USQLConnection::Initialize(const FString& Host, const int32 Port, const FString& User, const FString& Password, const FString& DataBase)
{
	SQLRunnable = MakeUnique<FSQLRunnable>(Host, Port ,User, Password,DataBase);
}

void USQLConnection::ExecuteSQL(USQLQueryProxy* SQLQueryProxy)
{
	FSQLTaskContextPtr SQLTaskContextPtr = MakeShared<FSQLTaskContext>();
	SQLTaskContextPtr->SQLQueryProxy = SQLQueryProxy;
	SQLTaskContextPtr->Task = [](FSQLRunnable* SQLRunnable, FSQLTaskContextPtr SQLTaskContextPtr) -> void
	{
		TUniquePtr<FSQLQueryProxyPackage> SQLQueryProxyPackage(new FSQLQueryProxyPackage{0, {}, 0, {}});
		try
		{
			if(SQLRunnable->SQLConnection == nullptr)
			{
					sql::Driver* Driver = sql::mysql::get_driver_instance();
					const std::string UTF8User(TCHAR_TO_UTF8(*SQLRunnable->User));
					const std::string UTF8Password(TCHAR_TO_UTF8(*SQLRunnable->Password));
					const std::string UTF8URI(TCHAR_TO_UTF8(*SQLRunnable->URI));
					sql::Connection* Connection = Driver->connect(UTF8URI, UTF8User, UTF8Password);
					SQLRunnable->SQLConnection.Reset(Connection);
			}
			if(!SQLRunnable->SQLConnection->isValid())
			{
				SQLRunnable->SQLConnection->reconnect();
			}
			if(SQLRunnable->SQLConnection->isValid())
			{
				const std::string UTF8DataBase(TCHAR_TO_UTF8(*SQLRunnable->DataBase));
				if(SQLRunnable->SQLConnection->getSchema() != UTF8DataBase)
					SQLRunnable->SQLConnection->setSchema(UTF8DataBase);
				if (SQLTaskContextPtr->SQLQueryProxy->SQLQueryBind.Num() == 0)
				{
					Query(SQLRunnable->SQLConnection.Get(), TCHAR_TO_UTF8(*SQLTaskContextPtr->SQLQueryProxy->SQLString), *SQLQueryProxyPackage.Get());
				}else{
					QueryWithBinds(SQLRunnable->SQLConnection.Get(), TCHAR_TO_UTF8(*SQLTaskContextPtr->SQLQueryProxy->SQLString), SQLTaskContextPtr->SQLQueryProxy->SQLQueryBind, *SQLQueryProxyPackage.Get());
				}
			}else
			{
				SQLQueryProxyPackage->ErrorCode =  -1;
				SQLQueryProxyPackage->ErrorString = FString::Printf(TEXT("SQLConnection is valid"));
			}
		}
		catch (sql::SQLException& SQLException)
		{
			SQLQueryProxyPackage->ErrorCode =  SQLException.getErrorCode();
			SQLQueryProxyPackage->ErrorString = FString::Printf(TEXT("SQLException: %s, SQLErrorCode: %d, SQLState: %d"), *FString(UTF8_TO_TCHAR(SQLException.what())), SQLException.getErrorCode(), *FString(UTF8_TO_TCHAR(SQLException.getSQLStateCStr())));
			UE_LOG(LogHslmMySQL, Error, TEXT("[%s] %s"), *UE__FUNC__LINE__, *SQLQueryProxyPackage->ErrorString);
		}
		AsyncTask(ENamedThreads::GameThread, [SQLQueryProxyPackage = MoveTemp(SQLQueryProxyPackage), SQLTaskContextPtr]{
		    USQLQueryProxy* SQLQueryProxy;
		    if(nullptr != (SQLQueryProxy = SQLTaskContextPtr->SQLQueryProxy.Get()))
		    {
			    if (SQLQueryProxyPackage->ErrorCode == 0)
			    {
			    	SQLQueryProxy->OnSuccess.Broadcast(SQLQueryProxyPackage->ErrorCode, SQLQueryProxyPackage->ErrorString, SQLQueryProxyPackage->AffectedRowNum, SQLQueryProxyPackage->ResultSet);
			    }else
			    {
			    	SQLQueryProxy->OnFail.Broadcast(SQLQueryProxyPackage->ErrorCode, SQLQueryProxyPackage->ErrorString, SQLQueryProxyPackage->AffectedRowNum, SQLQueryProxyPackage->ResultSet);
			    }
		    }
		});
	};
	SQLRunnable->EnqueueTaskContext(SQLTaskContextPtr);
}

