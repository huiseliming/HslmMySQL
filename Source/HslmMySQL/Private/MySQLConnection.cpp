// Fill out your copyright notice in the Description page of Project Settings.

#include "MySQLConnection.h"
#include <string>
#include <vector>

#include "HslmDefine.h"
#include "MySQLRunnable.h"
#include "HslmMySQL.h"
#include "SQLQuery.h"
#include "mysql/jdbc.h"


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

UMySQLConnection::UMySQLConnection(const FObjectInitializer& ObjectInitializer)
	: UObject(ObjectInitializer)
	, IsConnecting(false)
{
	// Create MySQL worker
	MySQLRunnable = MakeUnique<FMySQLRunnable>(this);
}

bool UMySQLConnection::ConnectToServerWithConnectionInfo(FConnectionInfo ConnectionInfo)
{
	if(IsConnecting)
		return false;
	IsConnecting = true;
	// UE4 的智能指针赋值是否能够释放原来引用的对象（或者说只能赋值一次），有空测试一下
	ConnectionInfoPtr = MakeShared<FConnectionInfo>(ConnectionInfo);
	FMySQLTaskContextSharedPtr MySQLTaskContextSharedPtr = MakeShared<FMySQLTaskContext,ESPMode::ThreadSafe>();
	MySQLTaskContextSharedPtr->Task = [ConnectionInfoPtr = this->ConnectionInfoPtr](FMySQLRunnable* MySQLRunnable, FMySQLTaskContextSharedPtr MySQLTaskContext) -> void
	{
		std::unique_ptr<sql::Connection>& SQLConnection = MySQLRunnable->GetSQLConnection();
		if(SQLConnection)
		{
			try{
				SQLConnection->close();
				AsyncTask(ENamedThreads::GameThread, [MySQLConnectionWeakPtr = MySQLRunnable->GetMySQLConnectionWeakPtr()]{
				    UMySQLConnection* MySQLConnection;
				    if(nullptr != (MySQLConnection = MySQLConnectionWeakPtr.Get())) 
				    { MySQLConnection->OnDisconnected(); }
				});
			}catch (sql::SQLException& SQLException) {
				UE_LOG(LogHslmMySQL, Error, TEXT("[%s] tan90 SQLException: %s, MySQLErrorCode: %d, SQLState: %d"), *UE__FUNC__LINE__, *FString(UTF8_TO_TCHAR(SQLException.what())), SQLException.getErrorCode(), *FString(UTF8_TO_TCHAR(SQLException.getSQLStateCStr())));
			}
			SQLConnection.reset();
		}
    	try
    	{
    		sql::Driver* Driver = sql::mysql::get_driver_instance();
    		const std::string UTF8Host(TCHAR_TO_UTF8(*ConnectionInfoPtr->Host));
    		const std::string UTF8User(TCHAR_TO_UTF8(*ConnectionInfoPtr->User));
    		const std::string UTF8Password(TCHAR_TO_UTF8(*ConnectionInfoPtr->Password));
    		const std::string UTF8DataBase(TCHAR_TO_UTF8(*ConnectionInfoPtr->DataBase));
    		const std::string UTF8URI = "tcp://" + UTF8Host + ":" + std::to_string(ConnectionInfoPtr->Port) + "/" + UTF8DataBase;
    		sql::Connection* Connection = Driver->connect(UTF8URI, UTF8User, UTF8Password);
    		SQLConnection.reset(Connection);
    	}
    	catch (sql::SQLException& SQLException)
    	{
    		UE_LOG(LogHslmMySQL, Error, TEXT("[%s] SQLException: %s, MySQLErrorCode: %d, SQLState: %d"), *UE__FUNC__LINE__, *FString(UTF8_TO_TCHAR(SQLException.what())), SQLException.getErrorCode(), *FString(UTF8_TO_TCHAR(SQLException.getSQLStateCStr())));
    	}
		if (SQLConnection){
			AsyncTask(ENamedThreads::GameThread, [MySQLConnectionWeakPtr = MySQLRunnable->GetMySQLConnectionWeakPtr(), ConnectionInfoPtr]{
			    UMySQLConnection* MySQLConnection;
			    if(nullptr != (MySQLConnection = MySQLConnectionWeakPtr.Get())) 
			    { MySQLConnection->OnConnected(ConnectionInfoPtr); }
			});
		}else{
			AsyncTask(ENamedThreads::GameThread, [MySQLConnectionWeakPtr = MySQLRunnable->GetMySQLConnectionWeakPtr(), ConnectionInfoPtr]{
			    UMySQLConnection* MySQLConnection;
			    if(nullptr != (MySQLConnection = MySQLConnectionWeakPtr.Get())) 
			    { MySQLConnection->OnConnectionFailed(ConnectionInfoPtr); }
			});
		}
	};
	MySQLRunnable->EnqueueTaskContext(MySQLTaskContextSharedPtr);
	return true;
}

bool UMySQLConnection::ConnectToServer(FString Host, FString User, FString Password, FString DataBase, int32 Port)
{
	return ConnectToServerWithConnectionInfo(FConnectionInfo(Host, User, Password, DataBase, Port));
}

bool UMySQLConnection::IsConnected() const
{
	return bIsConnected;
}

void UMySQLConnection::CloseConnection()
{
	FMySQLTaskContextSharedPtr MySQLTaskContextSharedPtr = MakeShared<FMySQLTaskContext,ESPMode::ThreadSafe>();
	MySQLTaskContextSharedPtr->Task = [](FMySQLRunnable* MySQLRunnable, FMySQLTaskContextSharedPtr MySQLTaskContext) -> void
	{
		std::unique_ptr<sql::Connection>& SQLConnection = MySQLRunnable->GetSQLConnection();
		// safe close mysql
		if(SQLConnection)
		{
			SQLConnection->close();
			AsyncTask(ENamedThreads::GameThread, [MySQLConnectionWeakPtr = MySQLRunnable->GetMySQLConnectionWeakPtr()]{
                UMySQLConnection* MySQLConnection;
                if(nullptr != (MySQLConnection = MySQLConnectionWeakPtr.Get())) 
                { MySQLConnection->OnDisconnected(); }
            });
			SQLConnection.reset();
		}
	};
	MySQLRunnable->EnqueueTaskContext(MySQLTaskContextSharedPtr);
}

void UMySQLConnection::ExecuteQuery(USQLQuery* Query)
{
	FMySQLTaskContextSharedPtr MySQLTaskContextSharedPtr = MakeShared<FMySQLTaskContext,ESPMode::ThreadSafe>();
	MySQLTaskContextSharedPtr->QueryObjectWeakPtr = Query;
	MySQLTaskContextSharedPtr->Task = [DBName = Query->DBName, SQL = Query->SQL](FMySQLRunnable* MySQLRunnable, FMySQLTaskContextSharedPtr MySQLTaskContext) -> void
	{
		int32 ErrorCode = 0;
		FString ErrorString;
		int64 AffectedRowNum = 0;
		FSQLResultSet SQLResultSet;
		std::unique_ptr<sql::Connection>& SQLConnection = MySQLRunnable->GetSQLConnection();
		if(SQLConnection)
		{
			try
			{
				const std::string UTF8DBName(TCHAR_TO_UTF8(*DBName));
				const std::string UTF8SQL(TCHAR_TO_UTF8(*SQL));
				if(SQLConnection->getSchema() != UTF8DBName)
					SQLConnection->setSchema(UTF8DBName);
				std::unique_ptr< sql::Statement > Statement(SQLConnection->createStatement());
				std::unique_ptr< sql::ResultSet > ResultSet(Statement->executeQuery(UTF8SQL));
				sql::ResultSetMetaData* ResultSetMetaData = ResultSet->getMetaData();
				const uint32 ColumnCount = ResultSetMetaData->getColumnCount();
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
			catch (sql::SQLException& SQLException)
			{
				if(!SQLConnection->isValid())
				{
					SQLConnection->close();
					AsyncTask(ENamedThreads::GameThread, [MySQLConnectionWeakPtr = MySQLRunnable->GetMySQLConnectionWeakPtr()]{
					    UMySQLConnection* MySQLConnection;
					    if(nullptr != (MySQLConnection = MySQLConnectionWeakPtr.Get())) 
					    { MySQLConnection->OnDisconnected(); }
					});
					SQLConnection.reset();
				}
				ErrorString = FString::Printf(TEXT("SQLException: %s, MySQLErrorCode: %d, SQLState: %d"), *FString(UTF8_TO_TCHAR(SQLException.what())), SQLException.getErrorCode(), *FString(UTF8_TO_TCHAR(SQLException.getSQLStateCStr())));
				UE_LOG(LogHslmMySQL, Error, TEXT("[%s] %s"), *UE__FUNC__LINE__, *ErrorString);
				ErrorCode = -1;
			}
		}else
		{
			ErrorString = TEXT("SQLConnection don't connected");
			UE_LOG(LogHslmMySQL, Error, TEXT("[%s] %s"), *UE__FUNC__LINE__, *ErrorString);
			ErrorCode = -1;
		}
		AsyncTask(ENamedThreads::GameThread, [MySQLTaskContext, ErrorCode, ErrorString, SQLResultSet = MoveTemp(SQLResultSet), AffectedRowNum]{
		    USQLQuery* QueryObject;
		    if(nullptr != (QueryObject = MySQLTaskContext->QueryObjectWeakPtr.Get()))
		    {
		        QueryObject->OnQueryCompleted.Broadcast(ErrorCode, ErrorString, AffectedRowNum, SQLResultSet);
		    }
		});
	};
	MySQLRunnable->EnqueueTaskContext(MySQLTaskContextSharedPtr);
}

void UMySQLConnection::ExecuteUpdate(USQLUpdateQuery* UpdateQuery)
{
	FMySQLTaskContextSharedPtr MySQLTaskContextSharedPtr = MakeShared<FMySQLTaskContext,ESPMode::ThreadSafe>();
	MySQLTaskContextSharedPtr->QueryObjectWeakPtr = UpdateQuery;
	MySQLTaskContextSharedPtr->Task = [DBName = UpdateQuery->DBName, SQL = UpdateQuery->SQL, SQLUpdateBinds = UpdateQuery->SQLUpdateBinds](FMySQLRunnable* MySQLRunnable, FMySQLTaskContextSharedPtr MySQLTaskContext) -> void
	{
		int32 ErrorCode = 0;
		FString ErrorString;
		int64 AffectedRowNum = 0;
		FSQLResultSet SQLResultSet;
		std::unique_ptr<sql::Connection>& SQLConnection = MySQLRunnable->GetSQLConnection();
		if(SQLConnection)
		{
			try
			{
				const std::string UTF8DBName(TCHAR_TO_UTF8(*DBName));
				const std::string UTF8SQL = TCHAR_TO_UTF8(*SQL);
				std::unique_ptr< sql::PreparedStatement > PreparedStatement(SQLConnection->prepareStatement(UTF8SQL));
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
				if(SQLConnection->getSchema() != UTF8DBName)
					SQLConnection->setSchema(UTF8DBName);
				for (int32 i = 0; i < SQLUpdateBinds.Num(); i++)
				{
					for (int32 j = 0; j < SQLUpdateBinds[i].SQLData.Num(); j++)
					{
						switch (SQLUpdateBinds[i].SQLData[j].GetType())
						{
						case ESQLData::Integer:
							PreparedStatement->setInt64(j + 1,SQLUpdateBinds[i].SQLData[j].GetInteger64());
							break;
						case ESQLData::Decimal:
							PreparedStatement->setString(j + 1,std::string(TCHAR_TO_UTF8(*SQLUpdateBinds[i].SQLData[j].GetString())));
							break;
						case ESQLData::Float:
							PreparedStatement->setDouble(j + 1,SQLUpdateBinds[i].SQLData[j].GetDouble());
							break;
						case ESQLData::String:
							PreparedStatement->setString(j + 1, std::string(TCHAR_TO_UTF8(*SQLUpdateBinds[i].SQLData[j].GetString())));
							break;
						case ESQLData::Binary:
							{
								TArray<uint8> Binary = SQLUpdateBinds[i].SQLData[j].GetBinary();
								std::vector<uint8> Bytes;
								Bytes.resize(Binary.Num());
								FMemory::Memcpy(Bytes.data(),Binary.GetData(),Binary.Num());
								BlobDatas.emplace_back(BlobData(std::move(Bytes)));
								PreparedStatement->setBlob(j + 1,BlobDatas[BlobDatas.size()-1].IStream.get());
							}
							break;
						case ESQLData::DateTime:
							PreparedStatement->setString(j + 1, std::string(TCHAR_TO_UTF8(*SQLUpdateBinds[i].SQLData[j].GetString())));
							break;
                        case ESQLData::None:
                        default:
                           UE_LOG(LogHslmMySQL, Fatal, TEXT("[%s] [tan90]"), *UE__FUNC__LINE__);
						}
					}
					AffectedRowNum += PreparedStatement->executeUpdate();
					BlobDatas.clear();
				}
			}
			catch (sql::SQLException& SQLException)
			{
				if(!SQLConnection->isValid())
				{
					SQLConnection->close();
					AsyncTask(ENamedThreads::GameThread, [MySQLConnectionWeakPtr = MySQLRunnable->GetMySQLConnectionWeakPtr()]{
                        UMySQLConnection* MySQLConnection;
                        if(nullptr != (MySQLConnection = MySQLConnectionWeakPtr.Get())) 
                        { MySQLConnection->OnDisconnected(); }
                    });
					SQLConnection.reset();
				}
				ErrorString = FString::Printf(TEXT("SQLException: %s, MySQLErrorCode: %d, SQLState: %d"), *FString(UTF8_TO_TCHAR(SQLException.what())), SQLException.getErrorCode(), *FString(UTF8_TO_TCHAR(SQLException.getSQLStateCStr())));
				UE_LOG(LogHslmMySQL, Error, TEXT("[%s] %s"), *UE__FUNC__LINE__, *ErrorString);
				ErrorCode = -1;
			}
		}else
		{
			ErrorString = TEXT("SQLConnection don't connected");
			UE_LOG(LogHslmMySQL, Error, TEXT("[%s] %s"), *UE__FUNC__LINE__, *ErrorString);
			ErrorCode = -1;
		}
		AsyncTask(ENamedThreads::GameThread, [MySQLTaskContext, ErrorCode, ErrorString, SQLResultSet = MoveTemp(SQLResultSet), AffectedRowNum]{
	        USQLQuery* QueryObject;
	        if(nullptr != (QueryObject = MySQLTaskContext->QueryObjectWeakPtr.Get()))
	        {
	            QueryObject->OnQueryCompleted.Broadcast(ErrorCode, ErrorString, AffectedRowNum, SQLResultSet);
	        }
	    });
	};
	MySQLRunnable->EnqueueTaskContext(MySQLTaskContextSharedPtr);
}

void UMySQLConnection::OnConnected(TSharedPtr<FConnectionInfo> ConnectionInfo)
{
	IsConnecting = false;
	bIsConnected = true;
	check(ConnectionInfo == ConnectionInfoPtr)
	UE_LOG(LogHslmMySQL, Warning, TEXT("[%s] MySQL Host[%s] User[%s] DataBase[%s] Connected"), *UE__FUNC__LINE__, *ConnectionInfo->Host, *ConnectionInfo->User, *ConnectionInfo->DataBase);
}

void UMySQLConnection::OnConnectionFailed(TSharedPtr<FConnectionInfo> ConnectionInfo)
{
	IsConnecting = false;
	bIsConnected = false;
	check(ConnectionInfo == ConnectionInfoPtr)
	ConnectionInfoPtr.Reset();
	UE_LOG(LogHslmMySQL, Warning, TEXT("[%s] MySQL Host[%s] User[%s] DataBase[%s] Connection Failed"), *UE__FUNC__LINE__, *ConnectionInfo->Host, *ConnectionInfo->User, *ConnectionInfo->DataBase);
}

void UMySQLConnection::OnDisconnected()
{
	bIsConnected = false;
	UE_LOG(LogHslmMySQL, Warning, TEXT("[%s] MySQL Host[%s] User[%s] DataBase[%s] Connection Failed"), *UE__FUNC__LINE__, *ConnectionInfoPtr->Host, *ConnectionInfoPtr->User, *ConnectionInfoPtr->DataBase);
	ConnectionInfoPtr.Reset();
}
