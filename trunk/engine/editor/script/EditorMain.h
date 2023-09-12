 
/*****************************************************************************
* Author： hannibal
* Date：2021/3/21
* Description：
*****************************************************************************/
#pragma once

#include "engine_header.h"

DC_BEGIN_NAMESPACE
class Transform;
class TestPanel;
/********************************************************************/
class EditorMain : Object
{
public:
	bool Start();
	void Release();
};
DC_END_NAMESPACE
