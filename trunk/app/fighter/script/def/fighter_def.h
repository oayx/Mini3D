 
/*****************************************************************************
* Author： hannibal
* Date：2020/9/12
* Description：地图
*****************************************************************************/
#pragma once

#include "engine_header.h"

DC_BEGIN_NAMESPACE
class FighterId
{
public:
	static const int Layer_Terrain = -10;
	static const int Layer_Airplane = 0;
	static const int Layer_Bullet = 1;
};

static const String Evt_MainRole_Hp = "Evt_MainRole_Hp";
static const String Evt_Restart = "Evt_Restart";

enum class FighterCampType
{
	None = 0,       //不与任何人是一组
	Type_1,
	Type_2,
	Max,
};
DECLARE_ENUM_OPERATORS(FighterCampType);

DC_END_NAMESPACE
