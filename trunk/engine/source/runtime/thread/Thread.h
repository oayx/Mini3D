
/*****************************************************************************
* Author： hannibal
* Date：2020/2/16
* Description： 
*****************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// thread task
struct Task
{
	typedef std::function<void()> Job;
	typedef std::function<void()> Callback;

	bool	pools = true;	//使用对象池管理
	Job		job = nullptr;
	Callback complete = nullptr;
};

/********************************************************************/
class ENGINE_DLL Thread final : public object
{
	friend class ThreadPools;
	DEFAULT_CREATE(Thread);
	FRIEND_CONSTRUCT_DESTRUCT(Thread);
	DISALLOW_COPY_ASSIGN(Thread);
	BEGIN_REFECTION_TYPE(Thread)
	END_FINAL_REFECTION_TYPE;

public:
	//start a task
	static Thread* Start(const Task& task);
	static Thread* Start(const Task::Job& job);
	static void Sleep(uint milliseconds);
	static uint64 CurrentThreadId();

public:
	void   Wait();
	int	   GetTaskCount();
	uint64 ThreadId()const { return _threadId; }

private:
	Thread();
	Thread(const String& name);
	~Thread();

	void AddTask(const Task& task);
	void Run();

private:
	String			_name;
	bool			_isClose = false;
	uint64			_threadId = 0;
	std::thread*	_handle = nullptr;
	List<Task>		_taskQueue;
	std::mutex		_mutex;
	std::condition_variable _condition;
};

/********************************************************************/
// thread pools
class ThreadPools final : public Object
{
	friend class Thread;
	friend class Application;
	DEFAULT_CREATE(ThreadPools);
	FRIEND_CONSTRUCT_DESTRUCT(ThreadPools);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(ThreadPools);
	BEGIN_DERIVED_REFECTION_TYPE(ThreadPools, Object)
	END_REFECTION_TYPE;

private:
	ThreadPools(int thread_count);
	~ThreadPools();
	static void Initialize(int thread_count);
	static void Destroy();

	Thread* AddTask(const Task& task, int thread_index = -1);
	void WaitAll();

	int  GetThreadCount() const { return _threadPools.Size(); }

private:
	Vector<Thread*> _threadPools;
};
DC_END_NAMESPACE
