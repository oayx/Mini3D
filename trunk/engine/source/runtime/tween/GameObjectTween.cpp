#include "GameObjectTween.h"
#include "TweenManager.h"
#include "runtime/component/GameObject.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(BlinkTween, Tween);
BlinkTween* BlinkTween::Create(GameObject* target, float time, int count, bool defaultVisible)
{
	CHECK_RETURN_PTR_NULL(target);
	BlinkTween *tween = Memory::New<BlinkTween>();
	tween->AutoRelease();
	tween->_isVisible = defaultVisible;
	tween->_defaultVisible = defaultVisible;
	tween->_switchTime = time;
	tween->_lastSwitchTime = 0;
	tween->_transformTime = count * time;
	tween->SetTarget(target);
	target->SetActive(tween->_isVisible);
	return tween;
}
void BlinkTween::OnStepCompleted()
{
	GameObject* gameobject = dynamic_cast<GameObject*>(_target);
	gameobject->SetActive(true);
	base::OnStepCompleted();
}
void BlinkTween::OnUpdate(float currTime)
{
	GameObject* gameobject = dynamic_cast<GameObject*>(_target);
	if (currTime >= _endTime)
	{
		gameobject->SetActive(_defaultVisible);
		return;
	}

	_lastSwitchTime += TweenManager::GetDeltaTime();
	if (_lastSwitchTime >= _switchTime)
	{
		_lastSwitchTime = 0;
		_isVisible = !_isVisible;
		gameobject->SetActive(_isVisible);
	}
}

/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(VisibleTween, Tween);
VisibleTween* VisibleTween::Create(GameObject* target, bool visible, float time)
{
	CHECK_RETURN_PTR_NULL(target);
	VisibleTween *tween = Memory::New<VisibleTween>();
	tween->AutoRelease();
	tween->_isVisible = visible;
	tween->_transformTime = time;
	tween->_target = target;
	return tween;
}
void VisibleTween::OnUpdate(float currTime)
{
	if (currTime >= _endTime)
	{
		GameObject* gameobject = dynamic_cast<GameObject*>(_target);
		gameobject->SetActive(_isVisible);
	}
}
DC_END_NAMESPACE