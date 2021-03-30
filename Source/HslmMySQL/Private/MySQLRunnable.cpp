#include "MySQLRunnable.h"
#include "HslmDefine.h"
#include "HslmMySQL.h"
#include "MySQLConnection.h"
#include "jdbc/cppconn/connection.h"

FMySQLRunnable::FMySQLRunnable(UMySQLConnection* MySQLConnection)
	: SQLConnection(nullptr)
    , MySQLConnectionWeakPtr(MySQLConnection)
	, KillSignal(false)
{
	Thread = FRunnableThread::Create(this, TEXT("RNGThread") , 0, TPri_BelowNormal);
}

FMySQLRunnable::~FMySQLRunnable()
{
	EnsureCompletion();
}

void FMySQLRunnable::EnsureCompletion()
{
	Stop();
	if (Thread)
	{
		Thread->WaitForCompletion();
		Thread = nullptr;
	}
}

bool FMySQLRunnable::EnqueueTaskContext(FMySQLTaskContextSharedPtr SQLContextPtr)
{
	return TaskContextQueue.Enqueue(SQLContextPtr);
}

std::unique_ptr<sql::Connection>& FMySQLRunnable::GetSQLConnection()
{
	return SQLConnection;
}

TWeakObjectPtr<UMySQLConnection> FMySQLRunnable::GetMySQLConnectionWeakPtr()
{
	return MySQLConnectionWeakPtr;
}

bool FMySQLRunnable::Init()
{
	UE_LOG(LogHslmMySQL, Log, TEXT("[%s] MySQLRunnable Init"), *UE__FUNC__LINE__);
	return true;
}

uint32 FMySQLRunnable::Run()
{
	while (!KillSignal)
	{
		while (!TaskContextQueue.IsEmpty())
		{
			FMySQLTaskContextSharedPtr TaskContext = *TaskContextQueue.Peek();
			TaskContext->Task(this, TaskContext);
			TaskContextQueue.Pop();
		}
		FPlatformProcess::Sleep(1/60);
	}
	return 0;
}

void FMySQLRunnable::Stop()
{
	KillSignal = true;
}

void FMySQLRunnable::Exit()
{
	if(SQLConnection)
	{
		SQLConnection->close();
		SQLConnection = nullptr;
	}
	UE_LOG(LogHslmMySQL, Log, TEXT("[%s] MySQLRunnable Exit"), *UE__FUNC__LINE__);
}
