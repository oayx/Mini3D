
/*****************************************************************************
* Author： hannibal
* Date：2020/8/19
* Description：
*****************************************************************************/
#pragma once

#include "runtime/component/Component.h"
#include "runtime/input/InputDefine.h"
#include "runtime/renderable/Renderer.h"

DC_BEGIN_NAMESPACE
class UICanvas;
/********************************************************************/
enum class UIEventType
{
	LMouseEnter,
	LMouseLeave,
	LMouseDown,
	LMouseUpInside,
	LMouseUpOutside,
	LMouseClick,

	RMouseEnter,
	RMouseLeave,
	RMouseDown,
	RMouseUpInside,
	RMouseUpOutside,
	RMouseClick,

	MMouseEnter,
	MMouseLeave,
	MMouseDown,
	MMouseUpInside,
	MMouseUpOutside,
	MMouseClick,

	KeyDown,
	KeyUp,

	Max
};
DECLARE_ENUM_OPERATORS(UIEventType);

class ENGINE_DLL UIEventInfo
{
public:
	UIEventType type = UIEventType::LMouseEnter;
	Object*		target = nullptr;

	Vector2		mouse_position = Vector2::zero;
	int			fingerId = 0;

	KeyCode		key = KeyCode::Null;
};

// ui base class
typedef std::function<void(const UIEventInfo& info)> UIEventAction;
class ENGINE_DLL UIView : public Component
{
	typedef List<UIView*> Views;
	friend class UICanvas;
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(UIView);
	DISALLOW_COPY_ASSIGN(UIView);
	FRIEND_CONSTRUCT_DESTRUCT(UIView);
	BEGIN_DERIVED_REFECTION_TYPE(UIView, Component)
	END_REFECTION_TYPE;

protected:
	UIView();
	virtual ~UIView();
	virtual void	Resize();
	virtual void	OnEnable() override { SetNeedRebuild(); base::OnEnable(); }
	virtual void	OnDisable() override { SetNeedRebuild(); base::OnDisable(); }
	virtual void	OnUISize()override { SetNeedRebuild(); base::OnUISize(); }
	virtual bool	FillMesh(VariablePrimitive* primitive, Material* material, int& vxtOffset, int& idxOffset) { return false; }
	virtual String	GetAtlasFile() = 0;

public:
	void			SetSize(float w, float h);
	Size			GetSize()const;
	void			SetColor(const Color& color) { _color = color; this->SetNeedRebuild(); }
	const Color&	GetColor()const { return _color; }

	void			SetEventListener(UIEventType type, UIEventAction action);

	void			SetNeedRebuild();

	bool			GetRaycastTarget()const { return _raycastTarget; }
	void			SetRaycastTarget(bool b) { _raycastTarget = b; }

protected:
	bool			AddChild(UIView* view);
	bool			RemoveChild(UIView* view);

	void			SetParent(UIView* panel);
	UIView*			GetParent()const { return _parent; }

	UICanvas*		GetCanvas();
	UICanvas*		GetRootCanvas();

	float			GetMouseDownTime(MouseBtnID btn)const { return _mouseDownTime[(int)btn]; }

	void			OnDrawEditor_Color();
	void			OnDrawEditor_RaycastTarget();

protected:
	virtual void	OnMouseDown(const Vector2& pos, MouseBtnID btn);
	virtual void	OnMouseUpInside(const Vector2& pos, MouseBtnID btn);
	virtual void	OnMouseUpOutside(const Vector2& pos, MouseBtnID btn);
	virtual void	OnMouseClick(const Vector2& pos, MouseBtnID btn);
	virtual void	OnKeyDown(KeyCode key);
	virtual void	OnKeyUp(KeyCode key);

	virtual bool	HitTest(int x, int y);

protected:
	UIView*			_parent = nullptr;
	Views			_childrens;
	Color			_color = Color::White;
	bool			_raycastTarget = true;
	bool			_isMouseDown[(int)MouseBtnID::Max] = {false};
	float			_mouseDownTime[(int)MouseBtnID::Max] = { 0 };
	UIEventAction	_eventActioin[(int)UIEventType::Max] = { nullptr };
};
DC_END_NAMESPACE
