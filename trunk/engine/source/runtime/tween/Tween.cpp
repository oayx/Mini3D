#include "Tween.h"
#include "TweenManager.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Tween, Object);
Tween::Tween()
{
	static int64 SHARE_ID = 0;
	_id = ++SHARE_ID;

	TweenManager::AddInactive(this);
}
Tween::~Tween()
{
	for (auto tween : _childrenList)
	{
		SAFE_RELEASE(tween);
	}
	_childrenList.Clear();
}
Tween* Tween::Append(Tween* tween)
{
	CHECK_RETURN_PTR_NULL(!_isActive);
	CHECK_RETURN_PTR_NULL(tween->_root == nullptr);
	CHECK_RETURN_PTR_NULL(tween != this);
	CHECK_RETURN_PTR_NULL(_root != tween);

	TweenManager::RemoveInactive(tween);

	tween->_rootTimeOffset = _transformTime;
	tween->_root = this;
	_childrenList.Add(tween);
	tween->Retain();
	return tween;
}
Tween* Tween::Join(Tween* tween)
{
	CHECK_RETURN_PTR_NULL(!_isActive);
	CHECK_RETURN_PTR_NULL(tween->_root == nullptr);
	CHECK_RETURN_PTR_NULL(tween != this);
	CHECK_RETURN_PTR_NULL(_root != tween);

	TweenManager::RemoveInactive(tween);

	tween->_rootTimeOffset = 0;
	tween->_root = this;
	_childrenList.Add(tween);
	tween->Retain();
	return tween;
}
void Tween::Start(bool auto_manage)
{
	if (_isActive && auto_manage)
		return;
	_isActive = true;
	_isEnded = false;
	_isAllChildrenEnded = false;
	_currTime = Time::GetRealTimeSinceStartup();
	_startTime = Time::GetRealTimeSinceStartup() + _delayTime;
	_endTime = _startTime + _transformTime;
	ResetAllChildren();
	OnStarted();
	if (auto_manage)
	{
		ActiveAllChildren();
		TweenManager::Add(this);
	}

}
void Tween::Stop()
{
	_endTime = 0;
	_isAllChildrenEnded = true;
	_isEnded = true;
	OnCompleted();
}
void Tween::OnStepCompleted()
{
	if (_onStepComplete != nullptr)
	{
		_onStepComplete();
	}
}
void Tween::OnCompleted()
{
	if (_onComplete != nullptr)
	{
		_onComplete();
	}
}
void Tween::Update(float dt)
{
	if (_isPause)return;

	_currTime += dt;
	if (!_isEnded && _currTime >= _startTime)//支持延迟启动
	{//本身update
		if (_onStart != nullptr)
		{
			Action action = _onStart;
			_onStart = nullptr;
			action();
		}

		OnUpdate(_currTime);

		if (_onUpdate != nullptr)
		{
			_onUpdate();
		}

		if (_currTime >= _endTime)
		{
			_isEnded = true;
			OnStepCompleted();
			if(_maxLoopCount == 0)OnCompleted();
		}
	}

	if (!_isAllChildrenEnded)
	{//子节点update
		float timeOffset = _currTime - _startTime;
		int aliveChildrenCount = (int)_childrenList.Size();
		for (Tween* tween : _childrenList)
		{
			if (timeOffset >= tween->_rootTimeOffset)
			{
				if (tween->_startTime == 0)
					tween->Start(false);
				tween->Update(dt);
				if (tween->IsComplete())
					aliveChildrenCount--;
			}
		}
		if (aliveChildrenCount == 0)
			_isAllChildrenEnded = true;
	}

	if (this->IsComplete() && _maxLoopCount != 0)
	{//是否循环执行
		_runLoopCount++;
		if (_maxLoopCount > 0 && _runLoopCount >= _maxLoopCount)
		{
			OnCompleted();
		}
		else
		{
			_isEnded = false;
			_isAllChildrenEnded = false;
			_startTime = _currTime;
			_endTime = _startTime + _transformTime;
			ResetAllChildren();
		}
	}
}
void Tween::ActiveAllChildren()
{
	for (Tween* tween : _childrenList)
	{
		tween->_isActive = true;
		tween->ActiveAllChildren();
	}
}
void Tween::ResetAllChildren()
{
	for (Tween* tween : _childrenList)
	{
		tween->_isEnded = false;
		tween->_startTime = 0;
		tween->ResetAllChildren();
	}
}
DC_END_NAMESPACE