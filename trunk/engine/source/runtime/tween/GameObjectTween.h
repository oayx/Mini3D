
/*****************************************************************************
* Author： hannibal
* Date：2021/1/8
* Description：GameObject变换
*****************************************************************************/
#pragma once

#include "Tween.h"

DC_BEGIN_NAMESPACE
class GameObject;
/********************************************************************/
// callback
class ENGINE_DLL BlinkTween Final : public Tween
{
	FRIEND_CONSTRUCT_DESTRUCT(BlinkTween);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(BlinkTween);
	BEGIN_DERIVED_REFECTION_TYPE(BlinkTween, Tween)
	END_DERIVED_REFECTION_TYPE;

public:
	/// <summary>
	/// 构建器
	/// </summary>
	/// <param name="target">目标对象</param>
	/// <param name="time">变换间隔：一次显示或隐藏的时长</param>
	/// <param name="count">变换次数，显示和隐藏分别算一次变换</param>
	/// <param name="defaultVisible">默认是否可见，变换结束后，会回到默认状态</param>
	/// <returns></returns>
	static BlinkTween* Create(GameObject* target, float time, int count, bool defaultVisible = true);

protected:
	virtual void OnStarted()override {}
	virtual void OnStepCompleted()override;
	virtual void OnUpdate(float curr_time)override;

protected:
	bool	_isVisible = false;		//变换完成后目标可视状态
	bool	_defaultVisible = false;	//默认是否可见
	float	_switchTime = 0.0f;		//变换时间
	float	_lastSwitchTime = 0.0f;
};

/********************************************************************/
// 可视变换器
class ENGINE_DLL VisibleTween Final : public Tween
{
	FRIEND_CONSTRUCT_DESTRUCT(VisibleTween);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(VisibleTween);
	BEGIN_DERIVED_REFECTION_TYPE(VisibleTween, Tween)
	END_DERIVED_REFECTION_TYPE;

public:
	/// <summary>
	/// 构建器
	/// </summary>
	/// <param name="target">目标对象</param>
	/// <param name="time">显示/隐藏</param>
	/// <param name="time">时间到后生效</param>
	/// <returns></returns>
	static VisibleTween* Create(GameObject* target, bool visible, float time);

protected:
	virtual void OnStarted()override {}
	virtual void OnUpdate(float curr_time)override;

protected:
	bool _isVisible = false;//变换完成后目标可视状态
};
DC_END_NAMESPACE