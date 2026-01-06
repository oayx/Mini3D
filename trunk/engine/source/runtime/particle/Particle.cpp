#include "Particle.h"
#include "ParticleSystem.h"
#include "runtime/physics/Physics.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Particle, Object);
static int TOTAL_COUNT = 0;
void Particle::Start(ParticleSystem* ps)
{
	this->Idx = ++TOTAL_COUNT;
	_particleSystem = ps;
	this->Play();
	//Debuger::Log("Active Particle:%u, %d", this->Idx, Time::GetFrameCount());
}	
void Particle::Play()
{
	if (this->IsActive)
		return;

	this->IsActive = true;
	this->LifeTime = _particleSystem->Main.StartLifeTime;
	this->Speed = _particleSystem->Main.StartSpeed;
	this->Direction = Vector3::forward;//初始方向受发射器形状影响
	this->Position = _particleSystem->GetTransform()->GetPosition();//初始位置受发射器形状影响
	this->Rotation = _particleSystem->Main.StartRotation;
	this->StartSize = _particleSystem->Main.StartSize;
	this->Scale = this->StartSize;
	this->Force = Vector3::zero;
	this->Gravity = Vector3::zero;
	this->StartColor = _particleSystem->Main.StartColor;
	this->CurrColor = this->StartColor;
}
void Particle::Pause()
{
	if (this->IsActive)
	{
		this->IsActive = false;
	}
}
void Particle::Stop()
{
	if (this->IsActive)
	{
		this->IsActive = false;
		//Debuger::Log("Destroy Particle:%u, %d", this->Idx, Time::GetFrameCount());
	}
}
void Particle::Reset()
{
	this->IsActive = false;
	_particleSystem = nullptr;
}
void Particle::Simulate(float dt)
{
	if (!this->IsActive)
		return;

	if (this->LifeTime <= 0)
	{
		this->Stop();
		return;
	}

	//计算位置
	{
		//计算速度
		float speed = this->Speed;
		Vector3 velocity = this->Direction.Normalize() * speed;
		if (_particleSystem->VelocityOverLifetime.Enabled)
		{
			velocity = velocity * speed;
			velocity += _particleSystem->VelocityOverLifetime.Velocity;
			velocity = velocity * _particleSystem->VelocityOverLifetime.SpeedModifier;
		}

		//受力
		if (_particleSystem->ForceOverLifetime.Enabled)
		{
			this->Force += _particleSystem->ForceOverLifetime.Force * dt;
			velocity += this->Force;
		}

		//受重力
		if (_particleSystem->Main.GravityModifier > 0)
		{
			this->Gravity += Physics::GetGravity() * _particleSystem->Main.GravityModifier * dt;
			velocity += this->Gravity;
		}

		speed = velocity.Lenght();
		velocity = velocity.Normalize();
		//速度限制
		if (_particleSystem->LimitVelocityOverLifetime.Enabled)
		{
			float overflow_speed = speed - _particleSystem->LimitVelocityOverLifetime.Speed;
			if (overflow_speed > 0)
			{
				speed = _particleSystem->LimitVelocityOverLifetime.Speed + Math::Lerp(overflow_speed, 0, _particleSystem->LimitVelocityOverLifetime.Dampen);
			}
		}

		//位置
		this->Position += velocity * speed * dt;
	}

	//计算颜色
	{
		if (_particleSystem->ColorOverLifetime.Enabled)
		{
			this->CurrColor = Color::Lerp(_particleSystem->ColorOverLifetime.TargetColor, StartColor, this->LifeTime / _particleSystem->Main.StartLifeTime);
		}
	}

	//计算大小
	{
		if (_particleSystem->SizeOverLifetime.Enabled)
		{
			this->Scale = Vector3::Lerp(_particleSystem->SizeOverLifetime.TargetSize, StartSize, this->LifeTime / _particleSystem->Main.StartLifeTime);
		}
	}

	this->LifeTime -= dt;
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(ParticlePools, Object);
void ParticlePools::Destroy()
{
	for (auto p : _particles)
	{
		SAFE_DELETE(p);
	}
	_particles.Clear();
}
Particle* ParticlePools::Spawn()
{
	Particle* particle = nullptr;
	if (_particles.IsEmpty())
	{
		particle = Particle::Create();
	}
	else
	{
		particle = _particles.RemoveLast();
	}
	return particle;
}
void ParticlePools::Despawn(Particle* p)
{
	if (p == nullptr)return;
	p->Reset();
	_particles.Add(p);
}
DC_END_NAMESPACE
