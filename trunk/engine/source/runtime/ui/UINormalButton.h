
/*****************************************************************************
* Author： hannibal
* Date：2020/9/9
* Description：三态按钮
*****************************************************************************/
#pragma once

#include "UIButton.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL UINormalButton Final : public UIButton
{
	friend class UICanvas;
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(UINormalButton);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(UINormalButton);
	FRIEND_CONSTRUCT_DESTRUCT(UINormalButton);
	BEGIN_DERIVED_REFECTION_TYPE(UINormalButton, UIButton)
		CTORS(DEFAULT_CTOR(UINormalButton))
		ADD_EDITOR_COMPONENT("UI/NormalButton")
	END_DERIVED_REFECTION_TYPE;

public:
	virtual void	OnDrawEditor()override;

	void			SetStatus(UIButtonStatus status);
	virtual void	SetActive(bool b)override;

	void			SetNormalAtlas(const String& file, const String& name);
	void			SetSelectAtlas(const String& file, const String& name);
	void			SetDeactiveAtlas(const String& file, const String& name);

protected:
	virtual void	OnMouseDown(const Vector2& pos, MouseBtnID btn)override;
	virtual void	OnMouseUpInside(const Vector2& pos, MouseBtnID btn)override;
	virtual void	OnMouseUpOutside(const Vector2& pos, MouseBtnID btn)override;

protected:
	UIButtonStatus	_status = UIButtonStatus::Normal;

	String			_normalAtlasFile = "";
	String			_normalAtlasName = "";
	String			_selectAtlasFile = "";
	String			_selectAtlasName = "";
	String			_deactiveAtlasFile = "";
	String			_deactiveAtlasName = "";
};
DC_END_NAMESPACE
