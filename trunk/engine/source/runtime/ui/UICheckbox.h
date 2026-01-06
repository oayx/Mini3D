
/*****************************************************************************
* Author： hannibal
* Date：2020/9/9
* Description：checkbox
*****************************************************************************/
#pragma once

#include "UIButton.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL UICheckbox final : public UIButton
{
	friend class UICanvas;
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(UICheckbox);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(UICheckbox);
	FRIEND_CONSTRUCT_DESTRUCT(UICheckbox);
	BEGIN_DERIVED_REFECTION_TYPE(UICheckbox, UIButton)
		CTORS(DEFAULT_CTOR(UICheckbox))
		ADD_EDITOR_COMPONENT("UI/Checkbox")
	END_REFECTION_TYPE;

protected:
	virtual void OnMouseClick(const Vector2& pos, MouseBtnID btn)override;
	virtual void OnDrawEditor()override;

public:
	void		SetChecked(bool b);
	bool		IsChecked()const { return _isChecked; }

	void		SetNormalAtlas(const String& file, const String& name);
	void		SetSelectAtlas(const String& file, const String& name);

protected:
	bool		_isChecked = false;

	String		_normalAtlasFile = "internal/atlas/ui.png";
	String		_normalAtlasName = "UISprite.png";
	String		_selectAtlasFile = "internal/atlas/ui.png";
	String		_selectAtlasName = "Check.png";
};
DC_END_NAMESPACE
