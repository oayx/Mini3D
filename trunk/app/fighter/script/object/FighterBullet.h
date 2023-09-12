 
/*****************************************************************************
* Author： hannibal
* Date：2020/9/16
* Description：子弹
*****************************************************************************/
#pragma once

#include "FighterActor.h"

DC_BEGIN_NAMESPACE
class Sprite;
class BoxCollider2d;
/********************************************************************/
class FighterBullet : public FighterActor
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(FighterBullet);
	BEGIN_DERIVED_REFECTION_TYPE(FighterBullet, FighterActor)
	END_DERIVED_REFECTION_TYPE;
	
protected:
	virtual void Awake()override;
	virtual void Update()override;
	virtual void OnTransformChanged()override;
	virtual void OnCollisionEnter2D(Collision2D* collisionInfo)override;

public:
	static uint SharedId() { ++_bulletId; return _bulletId; }

	virtual void SetCamp(FighterCampType camp)override;
	void SetImage(const String& file);
	void SetSpeed(float speed) { _speed = speed; }

private:
	bool IsOutScreen();

protected:
	static uint		_bulletId;
	Sprite*			_bullet = nullptr;
	BoxCollider2d*	_collider = nullptr;
	float			_speed = 1;
};
DC_END_NAMESPACE
