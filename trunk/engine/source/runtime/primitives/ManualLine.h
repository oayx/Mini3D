
/*****************************************************************************
* FileName�� ManualLine.h     Date��2009��12��13��
* Author�� hannibal
* Description�� ��
*****************************************************************************/
#pragma once

#include "ManualPrim.h"

BEGING_NAMESPACE_DC
/********************************************************************/
// ClassName��ManualLine
// Description����
class ENGINE_DLL ManualLine : public ManualPrim
{
	friend class GameObject;
	typedef ManualPrim base;
	ManualLine(const String &name, const VecVector3& lines);
};//ManualLine
END_NAMESPACE_DC