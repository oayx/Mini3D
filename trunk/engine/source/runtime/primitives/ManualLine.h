
/*****************************************************************************
* FileName： ManualLine.h     Date：2009年12月13日
* Author： hannibal
* Description： 线
*****************************************************************************/
#pragma once

#include "ManualPrim.h"

BEGING_NAMESPACE_DC
/********************************************************************/
// ClassName：ManualLine
// Description：线
class ENGINE_DLL ManualLine : public ManualPrim
{
	friend class GameObject;
	typedef ManualPrim base;
	ManualLine(const String &name, const VecVector3& lines);
};//ManualLine
END_NAMESPACE_DC