
/*****************************************************************************
* FileName�� ManualCube.h     Date��2009��12��9��
* Author�� hannibal
* Description�� ������
*****************************************************************************/
#pragma once

#include "ManualPrim.h"

BEGING_NAMESPACE_DC
/********************************************************************/
// ClassName��ManualCube
// Description��������box���ĵ���ԭ��
class ENGINE_DLL ManualCube : public ManualPrim
{
	friend class GameObject;
	typedef ManualPrim base;
	ManualCube(const String &name);
};
END_NAMESPACE_DC