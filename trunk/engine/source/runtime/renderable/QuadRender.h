 
/*****************************************************************************
* Author： hannibal
* Date：2009/12/13
* Description： 四边形
*****************************************************************************/
#pragma once

#include "Renderer.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// ClassName：QuadRender
// Description：四边形
//一 使用索引
// 0--------1
// |  -     |
// |     -  |
// 3--------2
// 1 顶点 0 1 2 3
// 2 索引 0 1 2,2 3 0
class ENGINE_DLL QuadRender final : public Renderer
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(QuadRender);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(QuadRender);
	BEGIN_DERIVED_REFECTION_TYPE(QuadRender, Renderer)
		CTORS(DEFAULT_CTOR(QuadRender))
	END_REFECTION_TYPE;

private:
	virtual void Awake()override;
	virtual void Update()override;

public:
	void SetColor(const Color& color);

private:
	bool _isDirty = true;
};//QuadRender
DC_END_NAMESPACE
