 
/*****************************************************************************
* Author： hannibal
* Date：2021/1/5
* Description：粒子
*****************************************************************************/
#pragma once

#include "core/Object.h"
#include "core/color/Color.h"

DC_BEGIN_NAMESPACE
class ParticleSystem;
/********************************************************************/
class Particle final : public Object
{
	friend class ParticlePools;
	friend class ParticleSystem;
	friend class ParticleEmission;
	friend class ParticleRender;
	friend class ParticleShape;
	DEFAULT_CREATE(Particle);
	FRIEND_CONSTRUCT_DESTRUCT(Particle);
	DISALLOW_COPY_ASSIGN(Particle);
	BEGIN_DERIVED_REFECTION_TYPE(Particle, Object)
	END_REFECTION_TYPE;

	using base::base;

private:
	void Start(ParticleSystem* ps);
	void Play();
	void Pause();
	void Stop();
	void Reset();

	void Simulate(float dt);

private:
	uint		Idx = 0;
	bool		IsActive = false;
	float		LifeTime = 0;		//剩余时间
	float		Speed = 1;			//速度
	Vector3		Direction;			//方向
	Vector3		Position;			//当前位置
	Vector3		Rotation;			//当前旋转
	Vector3		StartSize;
	Vector3		Scale;				//当前缩放
	Vector3		Force;				//受力
	Vector3		Gravity;			//受重力
	Color		StartColor;
	Color		CurrColor;			//当前颜色

private:
	ParticleSystem* _particleSystem = nullptr;
};

/********************************************************************/
//粒子对象池
class ParticlePools final : public Object
{
	friend class Particle;
	friend class Application;
	typedef List<Particle*> Particles;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(ParticlePools);
	BEGIN_DERIVED_REFECTION_TYPE(ParticlePools, Object)
	END_REFECTION_TYPE;

private:
	~ParticlePools() {}
	static void Destroy();

public:
	static Particle* Spawn();
	static void Despawn(Particle* p);

private:
	inline static Particles _particles;
};
DC_END_NAMESPACE
