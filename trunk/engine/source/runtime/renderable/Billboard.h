
/*****************************************************************************
* Author： hannibal
* Date：2021/5/14
* Description：绕Y轴旋转
*****************************************************************************/
#pragma once

#include "Renderer.h"
#include "core/color/Color.h"

DC_BEGIN_NAMESPACE
class Texture;
class Camera;
/********************************************************************/
class ENGINE_DLL Billboard Final : public Renderer
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(Billboard);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Billboard);
	BEGIN_DERIVED_REFECTION_TYPE(Billboard, Renderer)
		CTORS(DEFAULT_CTOR(Billboard))
		ADD_EDITOR_COMPONENT("Renderer/Billboard")
	END_DERIVED_REFECTION_TYPE;

protected:
	virtual void Awake()override;
	virtual void Update()override;
	virtual void OnPreRender(Camera* camera)override;

public:
	void SetColor(const Color& color);

private:
	bool _isDirty = true;
};//
DC_END_NAMESPACE
