 
/*****************************************************************************
* Author： hannibal
* Date：2021/1/5
* Description：粒子发射器
*****************************************************************************/
#pragma once

#include "runtime/component/GameObject.h"

DC_BEGIN_NAMESPACE
class ParticleSystem;
/********************************************************************/
class ENGINE_DLL ParticleEmission Final : public Object
{
	friend class GameObject;
	friend class ParticleSystem;
	FRIEND_CONSTRUCT_DESTRUCT(ParticleEmission);
	DISALLOW_COPY_ASSIGN(ParticleEmission);
	BEGIN_DERIVED_REFECTION_TYPE(ParticleEmission, Object)
		CTORS(DEFAULT_CTOR(ParticleEmission))
	END_DERIVED_REFECTION_TYPE;

	ParticleEmission() = default;

public:
	void SetOwner(ParticleSystem* ps) { _particleSystem = ps; }

private:
	void Simulate(float dt);

private:
	ParticleSystem* _particleSystem = nullptr;
	float			_roundTime = 0.0f;			//回合时间
	int				_roundCreateCount = 0;		//回合创建粒子数量
};
DC_END_NAMESPACE
