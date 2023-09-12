 
/*****************************************************************************
* Author： hannibal
* Date：2020/9/12
* Description：飞机
*****************************************************************************/
#pragma once

#include "FighterActor.h"

DC_BEGIN_NAMESPACE
class Sprite;
class BoxCollider2d;
/********************************************************************/
class FighterAirplane : public FighterActor
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(FighterAirplane);
	BEGIN_DERIVED_REFECTION_TYPE(FighterAirplane, FighterActor)
	END_DERIVED_REFECTION_TYPE;
	
protected:
	virtual void Awake()override;
	virtual void Update()override;
	virtual void OnTransformChanged()override;
	virtual void OnCollisionEnter2D(Collision2D* collisionInfo)override;

	virtual float GetBulletSpeed() = 0;

public:
	virtual void SetCamp(FighterCampType camp)override;
	void SetImage(const String& file);
	void SetFiringRate(float rate) { _firingRate = rate; }

private:
	void HandleFire();

protected:
	Sprite* _airplane = nullptr;
	GameObject* _bulletRoot = nullptr;
	BoxCollider2d* _collider = nullptr;

	float _firingRate = 0.5f;
	float _lastFireTime = 0;
};
DC_END_NAMESPACE
