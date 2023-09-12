#include "Thread.h"
#include "ThreadScheduler.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
ThreadPools* g_ThreadPool = nullptr;
/********************************************************************/
IMPL_REFECTION_TYPE(Thread);
Thread::Thread()
{
	_handle = DBG_NEW std::thread(&Thread::Run, this);
}
Thread::~Thread()
{
	if (_handle->joinable())
	{
		this->Wait();

		_mutex.lock();
		_isClose = true;
		m_condition.notify_one();
		_mutex.unlock();
		_handle->join();
	}
	SAFE_DELETE(_handle);
}
void Thread::Wait()
{
	std::unique_lock<std::mutex> lock(_mutex);
	m_condition.wait(lock, [this]() {
		return _taskQueue.IsEmpty();
	});
}
int Thread::GetQueueLength()
{
	std::lock_guard<std::mutex> lock(_mutex);
	return _taskQueue.Size();
}
void Thread::AddTask(const Task& task)
{
	std::lock_guard<std::mutex> lock(_mutex);
	_taskQueue.AddLast(task);
	m_condition.notify_one();
}
void Thread::Run()
{
	while (true)
	{
		Task task;
		{
			std::unique_lock<std::mutex> lock(_mutex);
			m_condition.wait(lock, [this] {
				return !_taskQueue.IsEmpty() || _isClose;
			});

			if (_isClose)
			{
				break;
			}
			task = _taskQueue.First();
		}

		if (task.job != nullptr)
		{
			void* result = task.job();
			if (task.complete != nullptr)
			{
				ThreadScheduler::PushAction([=]() {
					task.complete(result);
				});
			}
		}

		{
			std::lock_guard<std::mutex> lock(_mutex);
			_taskQueue.RemoveFirst();
			m_condition.notify_one();
		}

		if (!task.pools)
		{
			this->Wait();//先执行等待，防止卡主线程
			ThreadScheduler::PushAction([this] 
				{
					Thread* thread = this;
					SAFE_DELETE(thread);
				});
		}
	}
}
void Thread::Start(const Task& task)
{
	return g_ThreadPool->AddTask(task);
}
void Thread::Sleep(uint milliseconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
String Thread::CurrentThreadId()
{
	std::thread::id id = std::this_thread::get_id();
	std::stringstream sin;
	sin << id;
	return String(sin.str());
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(ThreadPools, Object);
ThreadPools::ThreadPools(int thread_count)
{
	_threadPools.Resize(thread_count);
	for (uint i = 0; i < _threadPools.Size(); ++i)
	{
		_threadPools[i] = Thread::Create();
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
void ThreadPools::AddTask(const Task& task, int thread_index)
{
	if (task.pools)
	{
		if (thread_index >= 0 && thread_index < _threadPools.Size())
		{
			_threadPools[thread_index]->AddTask(task);
		}
		else
		{
			int min_len = 0x7fffffff;
			int min_index = -1;
			for (int i = 0; i < _threadPools.Size(); ++i)
			{
				int len = _threadPools[i]->GetQueueLength();
				if (min_len > len)
				{
					min_len = len;
					min_index = i;

					if (min_len == 0)
					{
						break;
					}
				}
			}
			if (min_index >= 0 && min_index < _threadPools.Size())
			{
				_threadPools[min_index]->AddTask(task);
			}
		}
	}
	else
	{
		//结束后自行销毁
		Thread* thread = Thread::Create();
		thread->AddTask(task);
	}
}
DC_END_NAMESPACE
