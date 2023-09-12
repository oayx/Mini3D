
/*****************************************************************************
* FileName： ManualQuad.h     Date：2009年12月13日
* Author： hannibal
* Description： 四边形
*****************************************************************************/
#pragma once

#include "ManualPrim.h"

BEGING_NAMESPACE_DC
/********************************************************************/
// ClassName：ManualQuad
// Description：四边形
//一 使用索引
// 0--------1
// |  -     |
// |     -  |
// 3--------2
// 1 顶点 0 1 2 3
// 2 索引 0 1 2,2 3 0
class ENGINE_DLL ManualQuad : public ManualPrim
{
	friend class GameObject;
	typedef ManualPrim base;
	ManualQuad(const String &name);

};//ManualQuad
END_NAMESPACE_DC