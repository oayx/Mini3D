
/*****************************************************************************
* Author： hannibal
* Date：2020/9/16
* Description： 
*****************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL ThreadScheduler final : public object
{
	typedef List<Action> Actions;
	friend class Application;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(ThreadScheduler);
	BEGIN_REFECTION_TYPE(ThreadScheduler)
	END_FINAL_REFECTION_TYPE;
	
private:
	static void Destroy();
	static void Update();

public:
	static void PushAction(const Action& action);
	inline static std::mutex GlobalMutex;

private:
	static Actions* _appendList;
	static Actions* _runList;
};
DC_END_NAMESPACE
