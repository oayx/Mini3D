
/*****************************************************************************
* FileName： ManualPlane.h     Date：2009年12月8日
* Author： hannibal
* Description： 可渲染平面
*****************************************************************************/
#pragma once

#include "ManualPrim.h"

BEGING_NAMESPACE_DC
/********************************************************************/
// ClassName：ManualPlane
// Description：创建一个法线=-z,d=0的平面
class ENGINE_DLL ManualPlane : public ManualPrim
{
	friend class GameObject;
	typedef ManualPrim base;

	//- Description：
	// @par：fWidth 宽
	// @par：fHeight 高
	// @par：iXSegments x方向段数
	// @par：iYSegments y方向段数
	// @par：iUTiles 纹理平铺次数
	// @par：iVTiles 宽
	ManualPlane(const String &name, float fWidth, float fHeight, ushort iXSegments = 10, ushort iYSegments = 10, ushort iUTiles = 1, ushort iVTiles = 1);
};//ManualPlane
END_NAMESPACE_DC