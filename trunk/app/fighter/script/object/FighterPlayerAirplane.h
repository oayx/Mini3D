 
/*****************************************************************************
* Author： hannibal
* Date：2020/9/12
* Description：飞机
*****************************************************************************/
#pragma once

#include "FighterAirplane.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class FighterPlayerAirplane : public FighterAirplane
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(FighterPlayerAirplane);
	BEGIN_DERIVED_REFECTION_TYPE(FighterPlayerAirplane, FighterAirplane)
	END_DERIVED_REFECTION_TYPE;

protected:
	virtual void Awake()override;
	virtual void Update()override;
	virtual void OnCollisionEnter2D(Collision2D* collisionInfo)override;

	virtual float GetBulletSpeed()override { return 6.0f; }

public:
	virtual void SetHp(int hp)override;
};
DC_END_NAMESPACE
