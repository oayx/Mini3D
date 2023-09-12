 
/*****************************************************************************
* Author： hannibal
* Date：2020/9/12
* Description：
*****************************************************************************/
#pragma once

#include "def/fighter_def.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class FighterApp : Object
{
public:
	~FighterApp();
	void Start();

private:
	void RegisterEvent();
	void UnRegisterEvent();
	void Update();
	void OnRestart();
	void OnRoleHp(const String& arg1, const String& arg2);
};
DC_END_NAMESPACE
