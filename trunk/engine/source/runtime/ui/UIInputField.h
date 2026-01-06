
/*****************************************************************************
* Author： hannibal
* Date：2020/8/29
* Description：
*****************************************************************************/
#pragma once

#include "UIImage.h"
#include "UILabel.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL UIInputField final : public UIImage
{
	friend class UICanvas;
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(UIInputField);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(UIInputField);
	FRIEND_CONSTRUCT_DESTRUCT(UIInputField);
	BEGIN_DERIVED_REFECTION_TYPE(UIInputField, UIImage)
		CTORS(DEFAULT_CTOR(UIInputField))
		ADD_EDITOR_COMPONENT("UI/InputField")
	END_REFECTION_TYPE;

protected:
	virtual void Awake()override;
	virtual void Update()override;
	virtual void OnDrawEditor()override;

public:
	void		SetText(const String& txt);
	void		SetFont(const String& font);
	void		SetFontSize(byte size);
	void		SetFontColor(const Color& color);
	void		SetContentSize(const Size& size);

	void		SetPlaceholderText(const String& placeholder);
	void		SetPlaceholderTextColor(const Color& color);

	void		SetCaretBlinkRate(float rate);

private:
	UILabel*	GetLabel();
	UIImage*	GetCaret();
	void		CaretBlink();
	void		SetCaretVisible(bool b);
	void		UpdateCaretPos();
	void		OnFocus();
	void		OnLostFocus();

protected:
	UILabel*	_label = nullptr;
	UIImage*	_caret = nullptr;
	bool		_isFocus = false;

	String		_placeholderText = "Enter text...";
	Color		_placeholderTextColor;

	float		_caretBlinkRate = 0.5f;
	float		_caretBlinkTime = 0;
	bool		_caretIsShow = true;

	std::string	_unicodes;
};
DC_END_NAMESPACE
