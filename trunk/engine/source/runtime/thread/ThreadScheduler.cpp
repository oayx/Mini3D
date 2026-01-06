#include "ThreadScheduler.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
ThreadScheduler::Actions* ThreadScheduler::_appendList = Memory::New<ThreadScheduler::Actions>();
ThreadScheduler::Actions* ThreadScheduler::_runList = Memory::New<ThreadScheduler::Actions>();
IMPL_REFECTION_TYPE(ThreadScheduler);
void ThreadScheduler::Destroy()
{
	SAFE_DELETE(_appendList);
	SAFE_DELETE(_runList);
}
void ThreadScheduler::PushAction(const Action& action)
{
	LOCK(GlobalMutex);
	_appendList->Add(action);
}
void ThreadScheduler::Update()
{
	auto runList = _appendList;
	if (runList->Size() > 0)
	{
		{
			LOCK(GlobalMutex);
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
