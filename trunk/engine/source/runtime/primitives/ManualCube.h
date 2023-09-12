
/*****************************************************************************
* FileName： ManualCube.h     Date：2009年12月9日
* Author： hannibal
* Description： 立方体
*****************************************************************************/
#pragma once

#include "ManualPrim.h"

BEGING_NAMESPACE_DC
/********************************************************************/
// ClassName：ManualCube
// Description：创建的box中心点在原点
class ENGINE_DLL ManualCube : public ManualPrim
{
	friend class GameObject;
	typedef ManualPrim base;
	ManualCube(const String &name);
};
END_NAMESPACE_DC