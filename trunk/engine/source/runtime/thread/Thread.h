
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
	typedef std::function<void*()> Job;
	typedef std::function<void(void*)> Callback;

	bool	 pools = true;	//使用对象池管理
	Job		 job = nullptr;
	Callback complete = nullptr;
};

/********************************************************************/
class ENGINE_DLL Thread Final : public object
{
	friend class ThreadPools;
	DEFAULT_CREATE(Thread);
	FRIEND_CONSTRUCT_DESTRUCT(Thread);
	DISALLOW_COPY_ASSIGN(Thread);
	BEGIN_REFECTION_TYPE(Thread)
	END_FINAL_REFECTION_TYPE;

public:
	void AddTask(const Task& task);
	void Wait();
	int  GetQueueLength();

public:
	//start a task
	static void Start(const Task& task);
	static void Sleep(uint milliseconds);
	static String CurrentThreadId();

private:
	Thread();
	~Thread();
	void Run();

private:
	bool			_isClose = false;
	std::thread*	_handle = nullptr;
	List<Task>		_taskQueue;
	std::mutex		_mutex;
	std::condition_variable m_condition;
};

/********************************************************************/
// thread pools
class ThreadPools Final : public Object
{
	friend class Thread;
	friend class Application;
	DEFAULT_CREATE(ThreadPools);
	FRIEND_CONSTRUCT_DESTRUCT(ThreadPools);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(ThreadPools);
	BEGIN_DERIVED_REFECTION_TYPE(ThreadPools, Object)
	END_DERIVED_REFECTION_TYPE;

private:
	ThreadPools(int thread_count);
	~ThreadPools();
	static void Initialize(int thread_count);
	static void Destroy();

	void AddTask(const Task& task, int thread_index = -1);
	void WaitAll();

	int  GetThreadCount() const { return _threadPools.Size(); }

private:
	Vector<Thread*> _threadPools;
};
DC_END_NAMESPACE
