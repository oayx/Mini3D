
/*****************************************************************************
* FileName�� ManualPlane.h     Date��2009��12��8��
* Author�� hannibal
* Description�� ����Ⱦƽ��
*****************************************************************************/
#pragma once

#include "ManualPrim.h"

BEGING_NAMESPACE_DC
/********************************************************************/
// ClassName��ManualPlane
// Description������һ������=-z,d=0��ƽ��
class ENGINE_DLL ManualPlane : public ManualPrim
{
	friend class GameObject;
	typedef ManualPrim base;

	//- Description��
	// @par��fWidth ��
	// @par��fHeight ��
	// @par��iXSegments x�������
	// @par��iYSegments y�������
	// @par��iUTiles ����ƽ�̴���
	// @par��iVTiles ��
	ManualPlane(const String &name, float fWidth, float fHeight, ushort iXSegments = 10, ushort iYSegments = 10, ushort iUTiles = 1, ushort iVTiles = 1);
};//ManualPlane
END_NAMESPACE_DC