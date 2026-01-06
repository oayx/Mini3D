
/*****************************************************************************
* Author： hannibal
* Date：2021/1/8
* Description：变换管理器
*****************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
class Tween;
/********************************************************************/
class ENGINE_DLL TweenManager final : object
{
	friend class Tween;
	friend class Application;
	typedef List<Tween*> Tweenes;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(TweenManager);
	BEGIN_REFECTION_TYPE(TweenManager)
	END_FINAL_REFECTION_TYPE;

private:
	static void Destroy();
	static void Update();

	//添加到队列
	static void Add(Tween* tween);
	static bool AddInactive(Tween* tween);
	static bool RemoveInactive(Tween* tween);

public:
	static void Simulate(float dt);

	//停止指定
	static void Stop(Tween* tween);
	//停止所有变换
	static void Stop();
	//停掉对象上所有的Tween
	static void Stop(Object* target);

	//是否有变换
	static bool HasTween(Object* target);

	//时间偏移
	static float GetDeltaTime();
	static void	 SetAutoSimulate(bool b) { _autoSimulate = b; }
	static void  UseUnscaledTime(bool b) { _useUnscaledTime = b; }
	static void  SetSpeed(float speed) { _speed = speed; }

private:
	inline static Tweenes	_inactiveList;			//未激活列表
	inline static Tweenes	_updateList;			//已激活列表
	inline static bool		_autoSimulate = true;	//是否自动模拟
	inline static bool		_useUnscaledTime = false;//是否受DeltaTime影响，默认true
	inline static float		_speed = 1.0f;			//更新速度
};
DC_END_NAMESPACE
