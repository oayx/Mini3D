
/*****************************************************************************
* Author： hannibal
* Description： 点
*****************************************************************************/
#pragma once

#include "Renderer.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL PointRender final : public Renderer
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(PointRender);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(PointRender);
	BEGIN_DERIVED_REFECTION_TYPE(PointRender, Renderer)
		CTORS(DEFAULT_CTOR(PointRender))
	END_REFECTION_TYPE;

private:
	virtual void Awake()override;
	void CreateMaterial(const String& file);

public:
	Material* SetMaterial(const String& file, int index = 0, bool rebuild = true) = delete;
	void SetVertexs(const Vector3v&& vertexs);
	void SetVertexs(const Vector3v&& vertexs, const Color& color);
	void SetVertexs(const Vector3v&& vertexs, float size);
	void SetVertexs(const Vector3v&& vertexs, const Color& color, float size);
	void SetIsSprite();
};//LineRender
DC_END_NAMESPACE