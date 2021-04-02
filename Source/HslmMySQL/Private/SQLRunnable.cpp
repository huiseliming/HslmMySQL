#include "SQLRunnable.h"
#include "HslmDefine.h"
#include "HslmMySQL.h"
#include "jdbc/cppconn/connection.h"

FSQLRunnable::FSQLRunnable(const FString& Host, const int32 Port, const FString& User, const FString& Password,
	const FString& DataBase)
{
	URI = "tcp://" + Host + ":" + FString::FromInt(Port) + "/" + DataBase;
	this->User = User;
	this->Password = Password;
	this->DataBase = DataBase;
	Thread = FRunnableThread::Create(this, TEXT("SQLThread") , 0, TPri_BelowNormal);
}

FSQLRunnable::~FSQLRunnable()
{
	EnsureCompletion();
}

void FSQLRunnable::EnsureCompletion()
{
	Stop();
	if (Thread)
	{
		Thread->WaitForCompletion();
		Thread = nullptr;
	}
}

bool FSQLRunnable::EnqueueTaskContext(FSQLTaskContextPtr SQLContextPtr)
{
	return TaskContextQueue.Enqueue(SQLContextPtr);
}

bool FSQLRunnable::Init()
{
	UE_LOG(LogHslmMySQL, Log, TEXT("[%s] MySQLRunnable Init"), *UE__FUNC__LINE__);
	return true;
}

uint32 FSQLRunnable::Run()
{
	while (!KillSignal)
	{
		while (!TaskContextQueue.IsEmpty())
		{
			FSQLTaskContextPtr TaskContext = *TaskContextQueue.Peek();
			TaskContext->Task(this, TaskContext);
			TaskContextQueue.Pop();
		}
		FPlatformProcess::Sleep(1/60);
	}
	return 0;
}

void FSQLRunnable::Stop()
{
	KillSignal = true;
}

void FSQLRunnable::Exit()
{
	if(SQLConnection)
	{
		SQLConnection->close();
		SQLConnection = nullptr;
	}
	UE_LOG(LogHslmMySQL, Log, TEXT("[%s] MySQLRunnable Exit"), *UE__FUNC__LINE__);
}
