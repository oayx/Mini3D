#include "CommonTween.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(ActionTween, Tween);
ActionTween* ActionTween::Create(Object* target, Action callback, float time)
{
	CHECK_RETURN_PTR_NULL(target);
	ActionTween *tween = Memory::New<ActionTween>();
	tween->AutoRelease();
	tween->_callback = callback;
	tween->_transformTime = time;
	tween->_target = target;
	return tween;
}
void ActionTween::OnUpdate(float currTime)
{
	if (currTime >= _endTime)
	{
		if (_callback != nullptr)_callback();
	}
}

/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(NullTween, Tween);
NullTween* NullTween::Create(Object* target, float time)
{
	CHECK_RETURN_PTR_NULL(target);
	NullTween *tween = Memory::New<NullTween>();
	tween->AutoRelease();
	tween->_transformTime = time;
	tween->_target = target;
	return tween;
}
DC_END_NAMESPACE