 
/*****************************************************************************
* Author： hannibal
* Date：2021/1/5
* Description：粒子发射器形状
*****************************************************************************/
#pragma once

#include "runtime/component/GameObject.h"

DC_BEGIN_NAMESPACE
class Particle;
class ParticleSystem;
/********************************************************************/
class ENGINE_DLL ParticleShape Final : public Object
{
	friend class GameObject;
	friend class ParticleSystem;
	FRIEND_CONSTRUCT_DESTRUCT(ParticleShape);
	DISALLOW_COPY_ASSIGN(ParticleShape);
	BEGIN_DERIVED_REFECTION_TYPE(ParticleShape, Object)
		CTORS(DEFAULT_CTOR(ParticleShape))
	END_DERIVED_REFECTION_TYPE;

	ParticleShape() = default;

public:
	void SetOwner(ParticleSystem* ps) { _particleSystem = ps; }

private:
	void Emit(Particle* particle);
	void EmitBox(Particle* particle);
	void EmitBoxEdge(Particle* particle);
	void EmitCircle(Particle* particle);
	void EmitCircleEdge(Particle* particle);
	void EmitCone(Particle* particle);
	void EmitHemiSphere(Particle* particle);
	void EmitHemiSphereShell(Particle* particle);
	void EmitSphere(Particle* particle);
	void EmitSphereShell(Particle* particle);

private:
	ParticleSystem* _particleSystem = nullptr;
};
DC_END_NAMESPACE
