 
/*****************************************************************************
* Author： hannibal
* Date：2009/12/9
* Description： 立方体
*****************************************************************************/
#pragma once

#include "Renderer.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// ClassName：CubeRender
// Description：创建的box中心点在原点
class ENGINE_DLL CubeRender final : public Renderer
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(CubeRender);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(CubeRender);
	BEGIN_DERIVED_REFECTION_TYPE(CubeRender, Renderer)
		CTORS(DEFAULT_CTOR(CubeRender))
	END_REFECTION_TYPE;

private:
	virtual void Awake()override;
	virtual void Update()override;

public:
	void SetColor(const Color& color);

private:
	bool _isDirty = true;
};
DC_END_NAMESPACE
