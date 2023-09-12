#include "ThreadScheduler.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
ThreadScheduler::Actions* ThreadScheduler::_appendList = DBG_NEW ThreadScheduler::Actions();
ThreadScheduler::Actions* ThreadScheduler::_runList = DBG_NEW ThreadScheduler::Actions();
std::mutex ThreadScheduler::GlobalMutex;
IMPL_REFECTION_TYPE(ThreadScheduler);
void ThreadScheduler::Destroy()
{
	SAFE_DELETE(_appendList);
	SAFE_DELETE(_runList);
}
void ThreadScheduler::PushAction(const Action& action)
{
	thread_lock(GlobalMutex);
	_appendList->Add(action);
}
void ThreadScheduler::Update()
{
	auto runList = _appendList;
	if (runList->Size() > 0)
	{
		{
			thread_lock(GlobalMutex);
			_appendList = _runList;
			_runList = runList;
		}

		for (auto it = runList->begin(); it != runList->end(); ++it)
		{
			(*it)();
		}
		runList->Clear();
	}
}
DC_END_NAMESPACE
