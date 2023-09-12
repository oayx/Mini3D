 
/*****************************************************************************
* Author： hannibal
* Date：2020/9/16
* Description：子弹
*****************************************************************************/
#pragma once

#include "def/fighter_def.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class FighterActor : public Component
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(FighterActor);
	BEGIN_DERIVED_REFECTION_TYPE(FighterActor, Component)
	END_DERIVED_REFECTION_TYPE;
	
public:
	virtual void SetCamp(FighterCampType camp) { _camp = camp; }
	FighterCampType GetCamp()const { return _camp; }

	virtual void SetHp(int hp) { _hp = hp; _maxHp = hp; }
	int  GetHp()const { return _hp; }
	int  GetMaxHp()const { return _maxHp; }

protected:
	FighterCampType _camp = FighterCampType::None;
	int _hp = 1;
	int _maxHp = 1;
};
DC_END_NAMESPACE
