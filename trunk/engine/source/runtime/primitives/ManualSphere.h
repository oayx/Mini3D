
/*****************************************************************************
* FileName： ManualSphere.h     Date：2020年2月28日
* Author： hannibal
* Description： 球体
*****************************************************************************/
#pragma once

#include "ManualPrim.h"

BEGING_NAMESPACE_DC
/********************************************************************/
// ClassName：ManualSphere
// Description：球体
class ENGINE_DLL ManualSphere : public ManualPrim
{
	friend class GameObject;
	typedef ManualPrim base;
	ManualSphere(const String &name, ushort segments = 10, ushort rings = 10);
};//ManualSphere
END_NAMESPACE_DC