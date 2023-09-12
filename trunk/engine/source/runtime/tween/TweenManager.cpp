#include "TweenManager.h"
#include "Tween.h"
#include "core/time/Time.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(TweenManager);
List<Tween*> TweenManager::_inactiveList;
List<Tween*> TweenManager::_updateList;
bool TweenManager::_autoSimulate = true;
bool TweenManager::_useUnscaledTime = false;
float TweenManager::_speed = 1.0f;
void TweenManager::Destroy()
{
	Stop();
	for (Tween* tween : _updateList)
		SAFE_RELEASE(tween);
	_updateList.Clear();
}

void TweenManager::Update()
{
	if (_autoSimulate)
	{
		//时间缩放
		float dt = _useUnscaledTime ? Time::GetUnscaledDeltaTime() : Time::GetDeltaTime();
		Simulate(dt);
	}
}
void TweenManager::Simulate(float dt)
{
	float delta_time = dt * _speed;

	for (auto& tween : _inactiveList)
	{
		if (!tween->_isActive)tween->Start();
	}

	auto it = _updateList.begin();
	while (it != _updateList.end())
	{
		Tween* tween = *it;
		tween->Update(delta_time);
		if (tween->IsComplete())
		{
			SAFE_RELEASE(tween);
			_updateList.Remove(it++);
			continue;
		}
		++it;
	}
}
void TweenManager::Add(Tween* tween)
{
	CHECK_RETURN_PTR_VOID(tween);
	tween->Retain();
	_updateList.Add(tween);
}
bool TweenManager::AddInactive(Tween* tween)
{
	CHECK_RETURN_PTR_FALSE(tween);
	if (std::find(_inactiveList.begin(), _inactiveList.end(), tween) != _inactiveList.end())
		return false;
	
	tween->Retain();
	_inactiveList.Add(tween);
	return true;
}
bool TweenManager::RemoveInactive(Tween* tween)
{
	CHECK_RETURN_PTR_FALSE(tween);
	for (auto it = _inactiveList.begin(); it != _inactiveList.end(); ++it)
	{
		Tween* t = *it;
		if (t == tween)
		{
			SAFE_RELEASE(tween);
			_inactiveList.Remove(it);
			return true;
		}
	}
	return false;
}
void TweenManager::Stop(Tween* tween)
{
	CHECK_RETURN_PTR_VOID(tween);

	RemoveInactive(tween);

	for (auto it = _updateList.begin(); it != _updateList.end(); ++it)
	{
		Tween* t = *it;
		if (t == tween)
		{
			SAFE_RELEASE(tween);
			_updateList.Remove(it);
			break;
		}
	}
}
void TweenManager::Stop()
{
	for (Tween* tween : _inactiveList)
	{
		SAFE_RELEASE(tween);
	}
	_inactiveList.Clear();

	for (Tween* tween : _updateList)
	{
		tween->Stop();
	}
}
void TweenManager::Stop(Object* target)
{
	CHECK_RETURN_PTR_VOID(target);

	auto it = _inactiveList.begin();
	while (it != _inactiveList.end())
	{
		Tween* tween = *it;
		if (tween->Target() == target)
		{
			SAFE_RELEASE(tween);
			it = _inactiveList.Remove(it);
			continue;
		}
		++it;
	}

	it = _updateList.begin();
	while (it != _updateList.end())
	{
		Tween* tween = *it;
		if (tween->Target() == target)
		{
			SAFE_RELEASE(tween);
			it = _updateList.Remove(it);
			continue;
		}
		++it;
	}
}
bool TweenManager::HasTween(Object* target)
{
	CHECK_RETURN_PTR_FALSE(target);
	for (Tween* tween : _updateList)
	{
		if (tween->Target() == target)
			return true;
	}
	return false;
}
float TweenManager::GetDeltaTime()
{
	return _useUnscaledTime ? Time::GetUnscaledDeltaTime() : Time::GetDeltaTime();
}
DC_END_NAMESPACE
