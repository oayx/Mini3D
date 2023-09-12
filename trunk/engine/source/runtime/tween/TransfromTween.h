
/*****************************************************************************
* Author： hannibal
* Date：2021/1/8
* Description：Transform变换
*****************************************************************************/
#pragma once

#include "Tween.h"
#include "runtime/CommonDefine.h"

DC_BEGIN_NAMESPACE
class Transform;
/********************************************************************/
// 位移
class ENGINE_DLL MoveTween Final : public Tween
{
	FRIEND_CONSTRUCT_DESTRUCT(MoveTween);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(MoveTween);
	BEGIN_DERIVED_REFECTION_TYPE(MoveTween, Tween)
	END_DERIVED_REFECTION_TYPE;

public:
	/// <summary>
	/// 移动到目标点
	/// </summary>
	/// <param name="target">目标对象</param>
	/// <param name="target_pos">目标位置</param>
	/// <param name="time">变换时长</param>
	/// <returns></returns>
	static MoveTween* To(Transform* target, const Vector3& target_pos, float time, SimulationSpace space = SimulationSpace::Local);
	/// <summary>
	/// 基于当前点移动：按速度
	/// </summary>
	/// <param name="target">目标对象</param>
	/// <param name="speed">速度</param>
	/// <param name="time">变换时长</param>
	/// <returns></returns>
	static MoveTween* Speed(Transform* target, const Vector3& speed, float time, SimulationSpace space = SimulationSpace::Local);

protected:
	virtual void OnStarted()override {}
	virtual void OnUpdate(float curr_time)override;
	virtual bool EnableEasy()override { return true; }
	virtual Tween* OnFrom(bool is_relative)override;

protected:
	Vector3 _startValue = Vector3::zero;
	Vector3 _targetValue = Vector3::zero;
	SimulationSpace _space = SimulationSpace::Local;
};

/********************************************************************/
// 旋转
class ENGINE_DLL RotateTween Final : public Tween
{
	FRIEND_CONSTRUCT_DESTRUCT(RotateTween);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(RotateTween);
	BEGIN_DERIVED_REFECTION_TYPE(RotateTween, Tween)
	END_DERIVED_REFECTION_TYPE;

public:
	/// <summary>
	/// 旋转到指定角度
	/// </summary>
	/// <param name="target">目标对象</param>
	/// <param name="euler">欧拉角</param>
	/// <param name="time">变换时长</param>
	/// <returns></returns>
	static RotateTween* To(Transform* target, const Vector3& euler, float time, SimulationSpace space = SimulationSpace::Local);
	/// <summary>
	/// 按速度
	/// </summary>
	/// <param name="target">目标对象</param>
	/// <param name="euler">欧拉角</param>
	/// <param name="speed">速度：每秒变换角度</param>
	/// <param name="time">变换时长</param>
	/// <returns></returns>
	static RotateTween* Speed(Transform* target, const Vector3& speed, float time, SimulationSpace space = SimulationSpace::Local);

protected:
	virtual void OnStarted()override {}
	virtual void OnUpdate(float curr_time)override;
	virtual bool EnableEasy()override { return true; }
	virtual Tween* OnFrom(bool is_relative)override;

protected:
	Vector3 _startValue = Vector3::zero;
	Vector3 _targetValue = Vector3::zero;
	SimulationSpace _space = SimulationSpace::Local;
};

/********************************************************************/
// 绕点旋转
class ENGINE_DLL RotateAroundTween Final : public Tween
{
	FRIEND_CONSTRUCT_DESTRUCT(RotateAroundTween);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(RotateAroundTween);
	BEGIN_DERIVED_REFECTION_TYPE(RotateAroundTween, Tween)
	END_DERIVED_REFECTION_TYPE;

public:
	/// <summary>
	/// 旋转到指定角度
	/// </summary>
	/// <param name="target">目标对象</param>
	/// <param name="point">点</param>
	/// <param name="axis">经过点的轴</param>
	/// <param name="degree">旋转速度</param>
	/// <returns></returns>
	static RotateAroundTween* Create(Transform* target, const Vector3& point, const Vector3& axis, float degree, SimulationSpace space = SimulationSpace::Local);

protected:
	virtual void OnStarted()override {}
	virtual void OnUpdate(float curr_time)override;
	virtual bool EnableEasy()override { return true; }
	virtual Tween* OnFrom(bool is_relative)override;

protected:
	Vector3 _point = Vector3::zero;
	Vector3 _axis = Vector3::zero;
	float _degree = 0.0f;
	SimulationSpace _space = SimulationSpace::Local;
};

/********************************************************************/
// 缩放
class ENGINE_DLL ScaleTween Final : public Tween
{
	FRIEND_CONSTRUCT_DESTRUCT(ScaleTween);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(ScaleTween);
	BEGIN_DERIVED_REFECTION_TYPE(ScaleTween, Tween)
	END_DERIVED_REFECTION_TYPE;

public:
	/// <summary>
	/// 绝对缩放
	/// </summary>
	/// <param name="target">目标对象</param>
	/// <param name="target_scale">目标缩放值</param>
	/// <param name="time">变换时长</param>
	/// <returns></returns>
	static ScaleTween* To(Transform* target, const Vector3& target_scale, float time);

protected:
	virtual void OnStarted()override {}
	virtual void OnUpdate(float curr_time)override;
	virtual bool EnableEasy()override { return true; }
	virtual Tween* OnFrom(bool is_relative)override;

protected:
	Vector3 _startValue = Vector3::zero;
	Vector3 _targetValue = Vector3::zero;
};

/********************************************************************/
// 镜像
class ENGINE_DLL FlipTween Final : public Tween
{
	FRIEND_CONSTRUCT_DESTRUCT(FlipTween);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(FlipTween);
	BEGIN_DERIVED_REFECTION_TYPE(FlipTween, Tween)
	END_DERIVED_REFECTION_TYPE;

public:
	static FlipTween* Create(Transform* target, bool flipX, bool flipY, bool flipZ);

protected:
	virtual void OnStarted()override;
	virtual void OnUpdate(float curr_time)override {}

protected:
	bool _flipX = false;
	bool _flipY = false;
	bool _flipZ = false;
};

/********************************************************************/
// 放置指定位置
class ENGINE_DLL PlaceTween Final : public Tween
{
	FRIEND_CONSTRUCT_DESTRUCT(PlaceTween);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(PlaceTween);
	BEGIN_DERIVED_REFECTION_TYPE(PlaceTween, Tween)
	END_DERIVED_REFECTION_TYPE;

public:
	static PlaceTween* Create(Transform* target, const Vector3& pos);

protected:
	virtual void OnStarted()override;
	virtual void OnUpdate(float curr_time)override {}

protected:
	Vector3 _position = Vector3::zero;
};
DC_END_NAMESPACE