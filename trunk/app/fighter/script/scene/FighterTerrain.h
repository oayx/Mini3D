 
/*****************************************************************************
* Author： hannibal
* Date：2020/9/12
* Description：地图
*****************************************************************************/
#pragma once

#include "def/fighter_def.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class FighterTerrain : public Component
{
	friend class GameObject;
	typedef Component base;
	typedef List<GameObject*> Terrains;
	FRIEND_CONSTRUCT_DESTRUCT(FighterTerrain);
	FighterTerrain();
	~FighterTerrain();
protected:
	virtual void Awake()override;
	virtual void Update()override;

private:
	Terrains _terrains;
	float _moveSpeed = -3.0f;
	Size _terrainTileSize;
};
DC_END_NAMESPACE
