 
/*****************************************************************************
* Author： hannibal
* Date：2020/9/12
* Description：飞机
*****************************************************************************/
#pragma once

#include "FighterAirplane.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class FighterEnemyAirplane : public FighterAirplane
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(FighterEnemyAirplane);
	BEGIN_DERIVED_REFECTION_TYPE(FighterEnemyAirplane, FighterAirplane)
	END_DERIVED_REFECTION_TYPE;

protected:
	virtual void Awake()override;
	virtual void Update()override;

	virtual float GetBulletSpeed()override { return -3.0f; }

private:
	bool IsOutScreen();

private:
	float _moveSpeed = 1.5f;
};
DC_END_NAMESPACE
