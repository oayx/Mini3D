#include "Thread.h"
#include "ThreadScheduler.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
ThreadPools* g_ThreadPool = nullptr;
/********************************************************************/
IMPL_REFECTION_TYPE(Thread);
Thread::Thread()
	: _name("jobthread_unknow")
{
	_handle = Memory::New<std::thread>(&Thread::Run, this);
}
Thread::Thread(const String& name)
	: _name(name)
{
	_handle = Memory::New<std::thread>(&Thread::Run, this);
}
Thread::~Thread()
{
	if (_handle != nullptr)
	{
		try
		{
			if (_handle->joinable())
			{
				this->Wait();

				_mutex.lock();
				_isClose = true;
				_condition.notify_one();
				_mutex.unlock();
				_handle->join();
			}
		}
		catch (...)
		{
		}
		SAFE_DELETE(_handle);
	}
}
Thread* Thread::Start(const Task& task)
{
	if (g_ThreadPool == nullptr)
		return nullptr;
	return g_ThreadPool->AddTask(task);
}
Thread* Thread::Start(const Task::Job& job)
{
	if (g_ThreadPool == nullptr)
		return nullptr;
	Task task;
	task.job = job;
	return g_ThreadPool->AddTask(task);
}
void Thread::Sleep(uint milliseconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
uint64 Thread::CurrentThreadId()
{
	std::thread::id id = std::this_thread::get_id();
	return std::hash<std::thread::id>()(id);
}
void Thread::Wait()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_condition.wait(lock, [this]() {
		return _taskQueue.IsEmpty();
		});
}
int Thread::GetTaskCount()
{
	std::lock_guard<std::mutex> lock(_mutex);
	return _taskQueue.Size();
}
void Thread::AddTask(const Task& task)
{
	MyAssert(!_isClose);
	std::lock_guard<std::mutex> lock(_mutex);
	_taskQueue.AddLast(task);
	_condition.notify_one();//唤醒
}
void Thread::Run()
{
	DC_PROFILE_THREAD(_name.c_str());

	std::thread::id id = std::this_thread::get_id();
	_threadId = std::hash<std::thread::id>()(id);

	while (true)
	{
		Task task;
		{
			std::unique_lock<std::mutex> lock(_mutex);
			_condition.wait(lock, [this] {//等待唤醒，条件是队列不为空或关闭线程
				return !_taskQueue.IsEmpty() || _isClose;
			});

			if (_isClose)
			{//线程结束
				break;
			}
			task = _taskQueue.First();
		}

		if (task.job != nullptr)
		{
			task.job();
			if (task.complete != nullptr)
			{
				ThreadScheduler::PushAction([=]() {
					task.complete();
				});
			}
		}

		{
			std::lock_guard<std::mutex> lock(_mutex);
			_taskQueue.RemoveFirst();
			_condition.notify_one();
		}

		if (!task.pools)
		{
			_isClose = true;
			this->Wait();//先执行等待，防止卡主线程
			ThreadScheduler::PushAction([this] 
				{
					Thread* thread = this;
					SAFE_DELETE(thread);
				});
		}
	}
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(ThreadPools, Object);
ThreadPools::ThreadPools(int thread_count)
{
	_threadPools.Resize(thread_count);
	for (uint i = 0; i < _threadPools.Size(); ++i)
	{
		_threadPools[i] = Thread::Create(String::Format("jobthread_{}", i));
	}
}
ThreadPools::~ThreadPools()
{
	for (uint i = 0; i < _threadPools.Size(); ++i)
	{
		SAFE_DELETE(_threadPools[i]);
	}
	_threadPools.Clear();
}
void ThreadPools::Initialize(int thread_count)
{
	AssertEx(g_ThreadPool == nullptr, "");
	g_ThreadPool = ThreadPools::Create(thread_count);
}
void ThreadPools::Destroy()
{
	SAFE_DELETE(g_ThreadPool);
}
void ThreadPools::WaitAll()
{
	for (auto& i : _threadPools)
	{
		i->Wait();
	}
}
Thread* ThreadPools::AddTask(const Task& task, int thread_index)
{
	if (task.pools)
	{
		if (thread_index >= 0 && thread_index < _threadPools.Size())
		{//使用指定的线程
			_threadPools[thread_index]->AddTask(task);
			return _threadPools[thread_index];
		}
		else
		{//取一个压力最小的thread
			int minLen = 0x7fffffff;
			int minIndex = -1;
			for (int i = 0; i < _threadPools.Size(); ++i)
			{
				int len = _threadPools[i]->GetTaskCount();
				if (minLen > len)
				{
					minLen = len;
					minIndex = i;

					if (minLen == 0)
					{
						break;
					}
				}
			}
			if (minIndex >= 0 && minIndex < _threadPools.Size())
			{
				_threadPools[minIndex]->AddTask(task);
				return _threadPools[minIndex];
			}
		}
	}
	else
	{
		//结束后自行销毁
		Thread* thread = Thread::Create();
		thread->AddTask(task);
		return thread;
	}
	//不应该到这里来
	MyAssert(false);
	return nullptr;
}
DC_END_NAMESPACE
