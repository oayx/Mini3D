
/*****************************************************************************
* Author： hannibal
* Date：2020/9/4
* Description：
*****************************************************************************/
#pragma once

#include "UIView.h"
#include "runtime/font/Font.h"

DC_BEGIN_NAMESPACE
class Texture;
/********************************************************************/
enum class UIFontStyle
{
	Normal = 0x0,
	Bold = 0x1,
	Italic = 0x2,
	BoldAndItalic = 0x3,
};
DECLARE_ENUM_OPERATORS(UIFontStyle);

enum class UITextAnchor
{
	UpperLeft = 0,
	UpperCenter = 1,
	UpperRight = 2,
	MiddleLeft = 3,
	MiddleCenter = 4,
	MiddleRight = 5,
	LowerLeft = 6,
	LowerCenter = 7,
	LowerRight = 8
};
DECLARE_ENUM_OPERATORS(UITextAnchor);

enum class UIHorizontalWrapMode
{
	Wrap = 0,		//自动换行：水平方向不超过ContentSize，如果高度足够，自动换行；高度不够，裁剪后面的不显示
	Overflow = 1,	//不会换行：如果水平方向超过ContentSize，继续显示
};
DECLARE_ENUM_OPERATORS(UIHorizontalWrapMode);

enum class UIVerticalWrapMode
{
	Truncate = 0,	//换行后，不会超出ContentSize
	Overflow = 1,	//换行后，超过ContentSize，继续换行
};
DECLARE_ENUM_OPERATORS(UIVerticalWrapMode);
/********************************************************************/
class ENGINE_DLL UILabel final : public UIView
{
	friend class UICanvas;
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(UILabel);
	DISALLOW_COPY_ASSIGN(UILabel);
	FRIEND_CONSTRUCT_DESTRUCT(UILabel);
	BEGIN_DERIVED_REFECTION_TYPE(UILabel, UIView)
		CTORS(DEFAULT_CTOR(UILabel))
		ADD_EDITOR_COMPONENT("UI/Label")
	END_REFECTION_TYPE;

protected:
	UILabel();
	~UILabel();

public:
	virtual void	OnDrawEditor()override;
	virtual bool	FillMesh(VariablePrimitive* primitive, Material* material, int& vxtOffset, int& idxOffset)override;
	virtual String	GetAtlasFile()override;

public:
	void			SetText(const String& txt) { _utf8Text = txt; this->SetNeedRebuild();}
	void			SetAlignment(UITextAnchor align) { _alignment = align; this->SetNeedRebuild();}
	void			SetFont(const String& font) { _font = font; this->SetNeedRebuild();}
	void			SetFontStyle(UIFontStyle style) { _fontStyle = style; this->SetNeedRebuild();}
	void			SetFontSize(ushort size) { _fontSize = size; this->SetNeedRebuild();}
	void			SetWordSpace(int space) { _wordSpace = space; this->SetNeedRebuild();}
	void			SetLineSpace(float space) { _lineSpace = space; this->SetNeedRebuild();}

	void			SetHorizontalOverflow(UIHorizontalWrapMode wrap) { _horizontalOverflow = wrap; this->SetNeedRebuild();}
	void			SetVerticalOverflow(UIVerticalWrapMode wrap) { _verticalOverflow = wrap; this->SetNeedRebuild();}

	const String&	GetText()const { return _utf8Text; }
	const String&	GetFont()const { return _font; }
	byte			GetFontSize()const { return _fontSize; }
	const Size&		GetContentRealSize()const { return _contentRealSize; }

protected:
	String			_utf8Text = "";
	UITextAnchor	_alignment = UITextAnchor::MiddleLeft;
	String			_font = "";
	UIFontStyle		_fontStyle = UIFontStyle::Normal;
	ushort			_fontSize = DEFAULT_FONT_SIZE;
	int				_wordSpace = 0;		//字间距
	float			_lineSpace = 1.0f;		//行间距
	Size			_contentRealSize;		//内容真实大小

	UIHorizontalWrapMode _horizontalOverflow = UIHorizontalWrapMode::Wrap;
	UIVerticalWrapMode	 _verticalOverflow = UIVerticalWrapMode::Truncate;
};
/********************************************************************/
//阴影
class ENGINE_DLL UITextShadow final : public Component
{
	friend class UILabel;
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(UITextShadow);
	FRIEND_CONSTRUCT_DESTRUCT(UITextShadow);
	BEGIN_DERIVED_REFECTION_TYPE(UITextShadow, Component)
		CTORS(DEFAULT_CTOR(UITextShadow))
		ADD_EDITOR_COMPONENT("UI/Shadow")
	END_REFECTION_TYPE;

protected:
	UITextShadow() : color(0.0f, 0.0f, 0.0f, 0.5f) {}
	virtual void Awake()override;
	virtual void OnEnable() override { SetNeedRebuild(); base::OnEnable(); }
	virtual void OnDisable() override { SetNeedRebuild(); base::OnDisable(); }
	virtual bool CanAdd(GameObject* object)override;
	virtual void OnDestroy()override;
	virtual void OnDrawEditor()override;

public:
	void SetColor(const Color& color) { this->color = color; SetNeedRebuild(); }
	void SetDistance(float x, float y) { distanceX = x; distanceY = y; SetNeedRebuild(); }

private:
	void SetNeedRebuild();

private:
	Color color;
	float distanceX = 1.0f;
	float distanceY = -1.0f;
};
/********************************************************************/
//描边
class ENGINE_DLL UITextOutline final : public Component
{
	friend class UILabel;
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(UITextOutline);
	FRIEND_CONSTRUCT_DESTRUCT(UITextOutline);
	BEGIN_DERIVED_REFECTION_TYPE(UITextOutline, Component)
		CTORS(DEFAULT_CTOR(UITextOutline))
		ADD_EDITOR_COMPONENT("UI/Outline")
	END_REFECTION_TYPE;

protected:
	UITextOutline() : color(0.0f, 0.0f, 0.0f, 0.5f) {}
	virtual void Awake()override;
	virtual void OnEnable() override { SetNeedRebuild(); base::OnEnable(); }
	virtual void OnDisable() override { SetNeedRebuild(); base::OnDisable(); }
	virtual bool CanAdd(GameObject* object)override;
	virtual void OnDestroy()override;
	virtual void OnDrawEditor()override;

public:
	void SetColor(const Color& color) { this->color = color; SetNeedRebuild(); }
	void SetDistance(float x, float y) { distanceX = x; distanceY = y; SetNeedRebuild(); }

private:
	void SetNeedRebuild();

private:
	Color color;
	float distanceX = 1.0f;
	float distanceY = 1.0f;
};
DC_END_NAMESPACE
