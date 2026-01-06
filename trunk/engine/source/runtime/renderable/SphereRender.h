
/*****************************************************************************
* Author： hannibal
* Date：2020/2/28
* Description： 球体 
*****************************************************************************/
#pragma once

#include "Renderer.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// ClassName：SphereRender
// Description：球体
class ENGINE_DLL SphereRender final : public Renderer
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(SphereRender);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(SphereRender);
	BEGIN_DERIVED_REFECTION_TYPE(SphereRender, Renderer)
		CTORS(DEFAULT_CTOR(SphereRender))
	END_REFECTION_TYPE;

private:
	virtual void Awake()override;
	virtual void Update()override;

public:
	void SetSphere(ushort segments, ushort rings);
	void SetColor(const Color& color);

private:
	bool _isDirty = true;
};//SphereRender
DC_END_NAMESPACE
