 
/*****************************************************************************
* Author： hannibal
* Date：2020/9/28
* Description：
*****************************************************************************/
#pragma once

#include "runtime/component/GameObject.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class FighterEnemyManager : public Component
{
	friend class GameObject;
	typedef Component base;

protected:
	virtual void Awake()override;
	virtual void Update()override;

private:
	void CreateOne();

private:
	float _createRate = 1.0f;
	float _lastCreateTime = 0;
};
DC_END_NAMESPACE
