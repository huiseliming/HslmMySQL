#pragma once

#include "CoreMinimal.h"

#include <memory>

struct FMySQLTaskContext;
typedef TSharedPtr<FMySQLTaskContext, ESPMode::ThreadSafe> FMySQLTaskContextSharedPtr;
class UMySQLConnection;

namespace sql
{
	class Connection;
}

class FMySQLRunnable : public FRunnable
{
public:
	FMySQLRunnable() = delete;
	FMySQLRunnable(UMySQLConnection* MySQLConnection); 
	~FMySQLRunnable();
	
	void EnsureCompletion();

	bool EnqueueTaskContext(FMySQLTaskContextSharedPtr SQLContextPtr);

	std::unique_ptr<sql::Connection>& GetSQLConnection();
	TWeakObjectPtr<UMySQLConnection> GetMySQLConnectionWeakPtr();

private:
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

	TQueue<FMySQLTaskContextSharedPtr> TaskContextQueue;
	std::unique_ptr<sql::Connection> SQLConnection;
	TWeakObjectPtr<UMySQLConnection> MySQLConnectionWeakPtr;
	FThreadSafeBool KillSignal;
	FRunnableThread* Thread;
};
