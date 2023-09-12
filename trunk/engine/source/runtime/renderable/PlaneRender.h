
/*****************************************************************************
* Author： hannibal 
* Date：2009/12/8
* Description： 可渲染平面
*****************************************************************************/
#pragma once

#include "Renderer.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// ClassName：PlaneRender
// Description：创建一个法线=-z,d=0的平面
class ENGINE_DLL PlaneRender Final : public Renderer
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(PlaneRender);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(PlaneRender);
	BEGIN_DERIVED_REFECTION_TYPE(PlaneRender, Renderer)
		CTORS(DEFAULT_CTOR(PlaneRender))
	END_DERIVED_REFECTION_TYPE;

private:
	virtual void Awake()override;
	virtual void Update()override;

public:
	//- Description：
	// @par：fWidth 宽
	// @par：fHeight 高
	// @par：iXSegments x方向段数
	// @par：iYSegments y方向段数
	// @par：iUTiles 纹理平铺次数
	// @par：iVTiles 宽
	void SetPlane(float fWidth, float fHeight, ushort iXSegments = 10, ushort iYSegments = 10, ushort iUTiles = 1, ushort iVTiles = 1);
	void SetColor(const Color& color);

private:
	bool _isDirty = true;
};//PlaneRender
DC_END_NAMESPACE
