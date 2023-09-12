
/*****************************************************************************
* Author： hannibal
* Date：2020/9/9
* Description：按钮基类
*****************************************************************************/
#pragma once

#include "UIImage.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
enum class UIButtonStatus
{
	Normal,     //正常状态
	Select,     //选择状态
	Deactive,   //禁用状态
};
DECLARE_ENUM_OPERATORS(UIButtonStatus);

class ENGINE_DLL UIButton : public UIImage
{
	friend class UICanvas;
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(UIButton);
	FRIEND_CONSTRUCT_DESTRUCT(UIButton);
	BEGIN_DERIVED_REFECTION_TYPE(UIButton, UIImage)
		CTORS(DEFAULT_CTOR(UIButton))
		ADD_EDITOR_COMPONENT("UI/Button")
	END_DERIVED_REFECTION_TYPE;

protected:
	virtual void Awake()override;
	virtual void OnDrawEditor()override;
			void OnDrawEditor_Button();

	virtual void OnMouseDown(const Vector2& pos, MouseBtnID btn)override;
	virtual void OnMouseUpInside(const Vector2& pos, MouseBtnID btn)override;
	virtual void OnMouseUpOutside(const Vector2& pos, MouseBtnID btn)override;
	virtual void OnMouseClick(const Vector2& pos, MouseBtnID btn)override;

public:
	virtual void SetActive(bool b) { _active = b; if (!b)SetColor(_deactiveColor); base::SetNeedRebuild(); }
	bool GetActive()const { return _active; }

	void SetNormalColor(const Color& color) { _normalColor = color; base::SetNeedRebuild(); }
	void SetPressColor(const Color& color) { _pressColor = color; base::SetNeedRebuild(); }
	void SetDeactiveColor(const Color& color) { _deactiveColor = color; base::SetNeedRebuild(); }

protected:
	bool	_active = true;
	Color	_normalColor = Color::White;
	Color	_pressColor = Color(200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f, 1.0f);
	Color	_deactiveColor = Color(200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f, 128.0f / 255.0f);
};
DC_END_NAMESPACE
