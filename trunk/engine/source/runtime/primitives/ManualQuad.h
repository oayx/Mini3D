
/*****************************************************************************
* FileName�� ManualQuad.h     Date��2009��12��13��
* Author�� hannibal
* Description�� �ı���
*****************************************************************************/
#pragma once

#include "ManualPrim.h"

BEGING_NAMESPACE_DC
/********************************************************************/
// ClassName��ManualQuad
// Description���ı���
//һ ʹ������
// 0--------1
// |  -     |
// |     -  |
// 3--------2
// 1 ���� 0 1 2 3
// 2 ���� 0 1 2,2 3 0
class ENGINE_DLL ManualQuad : public ManualPrim
{
	friend class GameObject;
	typedef ManualPrim base;
	ManualQuad(const String &name);

};//ManualQuad
END_NAMESPACE_DC