
/*****************************************************************************
* Author： hannibal
* Date：2021/1/8
* Description：补间动画
*****************************************************************************/
#pragma once

#include "Easy.h"
#include "core/time/Time.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL Tween : public Object
{
	typedef List<Tween*> Tweenes;
	friend class TweenManager;
	DISALLOW_COPY_ASSIGN(Tween);
	BEGIN_DERIVED_REFECTION_TYPE(Tween, Object)
	END_DERIVED_REFECTION_TYPE;

protected:
	Tween();
	~Tween();

public:
	//连接一个变换
	Tween* Append(Tween* tween);
	//并排一个变换
	Tween* Join(Tween* tween);

	//暂停
	void Pause() { _isPause = true; }
	//恢复
	void Resume() { _isPause = false; }
	//停止
	void Stop();

	//开始运行起始的时间偏移
	//指定负数跳过前期时间段;指定正数，则延迟执行
	Tween* SetDelay(float time) { _delayTime = time; return this; }

	//运动方式
	Tween* SetEasy(EasyType type) { AssertEx(EnableEasy(),""); _easyType = type; return this; }

	//循环方式
	//count 如果为负数，表示无限循环
	Tween* SetLoop(int count) { _maxLoopCount = count; return this; }

	//反向
	//为true，传入的就是偏移量，即当前坐标 + 传入值 = 目标值
	//为falese，传入的就是目标值，即传入值 = 目标值
	Tween* From(bool is_relative = true) { return this->OnFrom(is_relative); }

	Tween* OnStart(Action callback) { _onStart = callback; return this; }
	Tween* OnUpdate(Action callback) { _onUpdate = callback; return this; }
	Tween* OnStepComplete(Action callback) { _onStepComplete = callback; return this; }
	Tween* OnComplete(Action callback) { _onComplete = callback; return this; }

	//是否正在运行
	bool IsPlaying() { return _startTime != 0 && !this->IsComplete(); }
	//是否完成
	bool IsComplete() { return _isEnded && _isAllChildrenEnded; }

	Object* Target()const { return _target; }
	Tween* Root() { return _root != nullptr ? _root : this; }

	int64 GetId()const { return _id; }

protected:
	//更新
	virtual void OnUpdate(float curr_time) = 0;
	//开始更新变换函数
	virtual void OnStarted() = 0;
	//完成单次变换函数
	virtual void OnStepCompleted();
	//完成变换函数
	virtual void OnCompleted();
	//是否可以SetEasy
	virtual bool EnableEasy() { return false; }
	//反向
	virtual Tween* OnFrom(bool is_relative) { return nullptr; }

	//开始运行变换器
	void Start(bool auto_manage = true);
	void Update(float dt);

	void SetTarget(Object* target) { _target = target; }

	//激活
	void ActiveAllChildren();
	//重置所有子孙变换器
	void ResetAllChildren();

protected:
	int64	 _id = 0;
	Tween*	 _root = nullptr;				//根变换器
	Tweenes  _childrenList;					//子级变换器列表
	Object*  _target = nullptr;				//作用对象
	EasyType _easyType = EasyType::Linear;

	Action	 _onStart = nullptr;			//开始播放，只会执行一次，如果是循环动画，第二次及以后也不会播放
	Action	 _onUpdate = nullptr;
	Action	 _onStepComplete = nullptr;		//完成单步事件，如果是循环动画，会多次执行
	Action	 _onComplete = nullptr;			//完成事件

	float	 _startTime = 0;				//变换开始时间
	float	 _endTime = 0;					//变换结束时间
	float	 _currTime = 0;					//当前时间
	float	 _delayTime = 0;				//延迟执行时间
	float	 _transformTime = 0;			//变换持续时间
	float	 _rootTimeOffset = 0;			//在变换树中的时间偏移

	bool	 _isActive = false;				//是否调用了Start
	bool	 _isPause = false;				//是否暂停
	bool	 _isEnded = false;				//是否已结束自身变换
	bool	 _isAllChildrenEnded = false;	//是否已结束整个变换树

	int		 _runLoopCount = 0;
	int		 _maxLoopCount = 0;				//循环次数：如果为0不循环，小于0无限循环，大于0循环次数
};
DC_END_NAMESPACE