#include "CommonTween.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(ActionTween, Tween);
ActionTween* ActionTween::Create(Object* target, Action callback, float time)
{
	CHECK_RETURN_PTR_NULL(target);
	ActionTween *tween = DBG_NEW ActionTween();
	tween->AutoRelease();
	tween->_callback = callback;
	tween->_transformTime = time;
	tween->_target = target;
	return tween;
}
void ActionTween::OnUpdate(float curr_time)
{
	if (curr_time >= _endTime)
	{
		if (_callback != nullptr)_callback();
	}
}

/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(NullTween, Tween);
NullTween* NullTween::Create(Object* target, float time)
{
	CHECK_RETURN_PTR_NULL(target);
	NullTween *tween = DBG_NEW NullTween();
	tween->AutoRelease();
	tween->_transformTime = time;
	tween->_target = target;
	return tween;
}
DC_END_NAMESPACE