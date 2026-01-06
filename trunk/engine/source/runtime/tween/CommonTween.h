
/*****************************************************************************
* Author： hannibal
* Date：2021/1/8
* Description：通用变换
*****************************************************************************/
#pragma once

#include "Tween.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// callback
class ENGINE_DLL ActionTween final : public Tween
{
	FRIEND_CONSTRUCT_DESTRUCT(ActionTween);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(ActionTween);
	BEGIN_DERIVED_REFECTION_TYPE(ActionTween, Tween)
	END_REFECTION_TYPE;

public:
	/// <summary>
	/// 回调
	/// </summary>
	static ActionTween* Create(Object* target, Action callback, float time);

protected:
	virtual void OnStarted()override {}
	virtual void OnUpdate(float currTime)override;

protected:
	Action _callback = nullptr;
};

/********************************************************************/
// 空变换器，占据变换时间而不做任何变换
class ENGINE_DLL NullTween final : public Tween
{
	FRIEND_CONSTRUCT_DESTRUCT(NullTween);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(NullTween);
	BEGIN_DERIVED_REFECTION_TYPE(NullTween, Tween)
	END_REFECTION_TYPE;

public:
	static NullTween* Create(Object* target, float time);

protected:
	virtual void OnStarted()override {}
	virtual void OnUpdate(float currTime)override {}
};
DC_END_NAMESPACE