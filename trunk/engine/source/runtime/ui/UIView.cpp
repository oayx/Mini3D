#include "UIView.h"
#include "UICanvas.h"
#include "UIAtlas.h"
#include "runtime/component/Component.inl"
#include "runtime/scene/LayerMask.h"
#include "editor/common/ECommonComponent.h"
#include "editor/EditorAppliction.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(UIView, Component);
UIView::UIView()
{
	for (int i = 0; i < (int)MouseBtnID::Max; ++i)
	{
		_isMouseDown[i] = false;
		_mouseDownTime[i] = 0;
	}
	for (int i = 0; i < (int)UIEventType::Max; ++i)
	{
		_eventActioin[i] = nullptr;
	}
}
UIView::~UIView()
{ 
	for (auto child : _childrens)
	{
		SAFE_DELETE(child);
	}
	_childrens.Clear();
}
Object* UIView::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	UIView* obj = dynamic_cast<UIView*>(new_obj);
	if (!obj)return new_obj;

	obj->SetColor(_color);
	obj->SetRaycastTarget(_raycastTarget);

	return obj;
}
void UIView::Resize()
{
	for (auto& child : _childrens)
	{
		child->Resize();
	}
}
void UIView::SetSize(float w, float h)
{ 
	RectTransform* transform = dynamic_cast<RectTransform*>(GetTransform());
	CHECK_RETURN_PTR_VOID(transform);
	if(transform != nullptr)
		transform->SetSize(w, h);
	this->SetNeedRebuild();
}
Size UIView::GetSize()const
{
	RectTransform* transform = dynamic_cast<RectTransform*>(GetTransform());
	if (transform != nullptr)
		return transform->GetSize();
	AssertEx(false, "cannot find RectTransform");
	return Size();
}
void UIView::SetEventListener(UIEventType type, UIEventAction action)
{
	_eventActioin[(int)type] = action;
}
void UIView::SetNeedRebuild() 
{ 
	UICanvas* canvas = this->GetCanvas();
	if (canvas != nullptr)
	{
		canvas->SetNeedRebuild(true);
	}
	if (GetGameObject())
	{
		GetGameObject()->GetTransform()->SetTransformDirty(false, false);
	}
}
bool UIView::AddChild(UIView* view)
{
	if (view == nullptr)return false;
	for (const auto& child : _childrens)
	{
		if (child->GetInstanceId() == view->GetInstanceId())
		{
			return false;
		}
	}

	this->SetNeedRebuild();
	_childrens.Add(view);
	view->SetParent(this);
	return true;
}
bool UIView::RemoveChild(UIView* view)
{
	if (view == nullptr)return false;
	for (auto it = _childrens.begin(); it != _childrens.end(); ++it)
	{
		UIView* child = *it;
		if (child->GetInstanceId() == view->GetInstanceId())
		{
			this->SetNeedRebuild();
			child->SetParent(nullptr);
			_childrens.Remove(it);
			return true;
		}
	}
	return false;
}
void UIView::SetParent(UIView* panel)
{
	UIView* old_parent = _parent;
	_parent = panel;
	this->SetNeedRebuild();
	if (old_parent != nullptr)
	{
		old_parent->RemoveChild(this);
	}
}
UICanvas* UIView::GetCanvas()
{
	UICanvas* root_canvas = this->GetComponent<UICanvas>();
	if (root_canvas == nullptr)
	{//向上递归
		Transform* parent = this->GetTransform()->GetParent();
		while (parent != nullptr)
		{
			UICanvas* canvas = parent->GetComponent<UICanvas>();
			if (canvas != nullptr)
			{
				root_canvas = canvas;
				break;
			}
			parent = parent->GetParent();
		}
	}
	return root_canvas;
}
UICanvas* UIView::GetRootCanvas()
{
	UICanvas* root_canvas = this->GetComponent<UICanvas>();
	Transform* parent = this->GetTransform()->GetParent();
	while (parent != nullptr)
	{//向上递归到最顶级
		UICanvas* canvas = parent->GetComponent<UICanvas>();
		if (canvas != nullptr) root_canvas = canvas;
		parent = parent->GetParent();
	}
	return root_canvas;
}
void UIView::OnMouseDown(const Vector2& pos, MouseBtnID btn)
{
	//Debuger::Log("OnMouseDown:%s", this->GetName().c_str());
	_isMouseDown[(int)btn] = true;
	_mouseDownTime[(int)btn] = Time::GetRealTimeSinceStartup();

	UIEventType event_type = UIEventType::LMouseDown;
	switch (btn)
	{
	case MouseBtnID::Left: event_type = UIEventType::LMouseDown; break;
	case MouseBtnID::Right: event_type = UIEventType::RMouseDown; break;
	case MouseBtnID::Mid: event_type = UIEventType::MMouseDown; break;
	default: break;
	}
	UIEventInfo info = { event_type, this, pos, (int)btn, KeyCode::Null };
	if(_eventActioin[(int)event_type])
		_eventActioin[(int)event_type](info);

	this->SetNeedRebuild();
}
void UIView::OnMouseUpInside(const Vector2& pos, MouseBtnID btn)
{
	//Debuger::Log("OnMouseUpInside:%s", this->GetName().c_str());
	_isMouseDown[(int)btn] = false;

	UIEventType event_type = UIEventType::LMouseDown;
	switch (btn)
	{
	case MouseBtnID::Left: event_type = UIEventType::LMouseUpInside; break;
	case MouseBtnID::Right: event_type = UIEventType::RMouseUpInside; break;
	case MouseBtnID::Mid: event_type = UIEventType::MMouseUpInside; break;
	default: break;
	}
	UIEventInfo info = { event_type, this, pos, (int)btn, KeyCode::Null };
	if (_eventActioin[(int)event_type])
		_eventActioin[(int)event_type](info);

	this->SetNeedRebuild();
}
void UIView::OnMouseUpOutside(const Vector2& pos, MouseBtnID btn)
{
	//Debuger::Log("OnMouseUpOutside:%s", this->GetName().c_str());
	_isMouseDown[(int)btn] = false;

	UIEventType event_type = UIEventType::LMouseDown;
	switch (btn)
	{
	case MouseBtnID::Left: event_type = UIEventType::LMouseUpOutside; break;
	case MouseBtnID::Right: event_type = UIEventType::RMouseUpOutside; break;
	case MouseBtnID::Mid: event_type = UIEventType::MMouseUpOutside; break;
	default: break;
	}
	UIEventInfo info = { event_type, this, pos, (int)btn, KeyCode::Null };
	if (_eventActioin[(int)event_type])
		_eventActioin[(int)event_type](info);

	this->SetNeedRebuild();
}
void UIView::OnMouseClick(const Vector2& pos, MouseBtnID btn)
{
	//Debuger::Log("OnMouseClick:%s", this->GetName().c_str());
	_isMouseDown[(int)btn] = false;

	UIEventType event_type = UIEventType::LMouseDown;
	switch (btn)
	{
	case MouseBtnID::Left: event_type = UIEventType::LMouseClick; break;
	case MouseBtnID::Right: event_type = UIEventType::RMouseClick; break;
	case MouseBtnID::Mid: event_type = UIEventType::MMouseClick; break;
	default: break;
	}
	UIEventInfo info = { event_type, this, pos, (int)btn, KeyCode::Null };
	if (_eventActioin[(int)event_type])
		_eventActioin[(int)event_type](info);

	this->SetNeedRebuild();
}
void UIView::OnKeyDown(KeyCode key)
{
	this->SetNeedRebuild();
}
void UIView::OnKeyUp(KeyCode key)
{
	this->SetNeedRebuild();
}
bool UIView::HitTest(int x, int y)
{
	const Vector3& pos = GetTransform()->GetPosition() * 100.0f;
	const Size& size = GetSize();
	Rect rect(pos.x - size.width * 0.5f, pos.y - size.height * 0.5f, size.width, size.height);
	if (rect.Contains(Point((float)x, (float)y)))
	{
		return true;
	}
	return false;
}
void UIView::OnDrawEditor_Color()
{
	ImGuiEx::Label("Color");
	Color color = _color;
	if (ECommonComponent::ShowColor("Color", color.p, false))
	{
		SetColor(color);
	}
}
void UIView::OnDrawEditor_RaycastTarget()
{
	ImGuiEx::Label("Raycast Target");
	if (ImGui::Checkbox("##RaycastTarget", &_raycastTarget))
	{
		SetRaycastTarget(_raycastTarget);
	}
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(UIView);
template<class TransferFunction> inline
void UIView::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);
	TRANSFER_SIMPLE(_color);
	TRANSFER_SIMPLE(_raycastTarget);
}
DC_END_NAMESPACE