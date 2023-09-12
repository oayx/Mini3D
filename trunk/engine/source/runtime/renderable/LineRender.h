
/*****************************************************************************
* Author： hannibal
* Date：2009/12/13 
* Description： 线
*****************************************************************************/
#pragma once

#include "Renderer.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL LineRender Final : public Renderer
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(LineRender);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(LineRender);
	BEGIN_DERIVED_REFECTION_TYPE(LineRender, Renderer)
		CTORS(DEFAULT_CTOR(LineRender))
	END_DERIVED_REFECTION_TYPE;

private:
	virtual void Awake()override;
	void CreateMaterial();

public:
	void Begin();
	void AddVertex(const Vector3& pos, const Color& color);
	void End();
	void SetVertexs(const Vector3v&& vertexs);
	void SetVertexs(const Vector3v&& vertexs, const Color& color);
	Material* SetMaterial(const String& file, int index = 0, bool rebuild = true) = delete;
	//线的粗细
	void SetWidth(float width);
	void SetPrimitiveType(PrimitiveType type);
};//LineRender
DC_END_NAMESPACE
