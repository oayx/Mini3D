
/*****************************************************************************
* Author： hannibal
* Date：2010/1/14
* Description：2D精灵 
*****************************************************************************/
#pragma once

#include "Renderer.h"
#include "core/color/Color.h"

DC_BEGIN_NAMESPACE
class Texture;
/********************************************************************/
class ENGINE_DLL Sprite final : public Renderer
{
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(Sprite);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Sprite);
	FRIEND_CONSTRUCT_DESTRUCT(Sprite);
	BEGIN_DERIVED_REFECTION_TYPE(Sprite, Renderer)
		CTORS(DEFAULT_CTOR(Sprite))
		ADD_EDITOR_COMPONENT("Renderer/Sprite")
	END_REFECTION_TYPE;

protected:
	virtual void Awake()override;
	virtual void Update()override;
	virtual void OnDrawEditor()override;

public:
	Material* SetMaterial(const String& file, int index = 0, bool rebuild = true) = delete;
	void SetColor(const Color& color);
	void SetTexture(Texture* tex);
	Size GetSize();

private:
	void CreateMaterial();

private:	
	bool	_isDirty = true;
	Color	_color = Color::White;
};//
DC_END_NAMESPACE
