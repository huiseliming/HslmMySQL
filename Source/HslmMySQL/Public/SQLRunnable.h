#pragma once

#include "CoreMinimal.h"

class FSQLRunnable;
class USQLQueryProxy;
struct FSQLTaskContext;
typedef TSharedPtr<FSQLTaskContext> FSQLTaskContextPtr;
namespace sql
{
	class Connection;
}

struct FSQLTaskContext
{
	TWeakObjectPtr<USQLQueryProxy> SQLQueryProxy;
	TFunction<void(FSQLRunnable*, FSQLTaskContextPtr)> Task;
};

class FSQLRunnable : public FRunnable
{
public:
	FSQLRunnable() = delete;
	FSQLRunnable(const FString& Host, const int32 Port, const FString& User, const FString& Password, const FString& DataBase);
	~FSQLRunnable();
	
	void EnsureCompletion();

	bool EnqueueTaskContext(FSQLTaskContextPtr SQLContextPtr);

	FString URI;
	FString User;
	FString Password;
	FString DataBase;
	TUniquePtr<sql::Connection> SQLConnection;

private:
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

	TQueue<FSQLTaskContextPtr> TaskContextQueue;
	FThreadSafeBool KillSignal;
	FRunnableThread* Thread;
};
