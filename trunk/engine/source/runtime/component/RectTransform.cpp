#include "RectTransform.h"
#include "runtime/component/Component.inl"
#include "runtime/component/GameObject.h"
#include "runtime/scene/LayerMask.h"
#include "runtime/ui/GUI.h"
#include "runtime/ui/UICanvas.h"
#include "runtime/ui/UIView.h"
#include "runtime/Application.h"
#include "editor/EditorAppliction.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(RectTransform, Transform);
RectTransform::RectTransform()
{
}
RectTransform::~RectTransform()
{
}	
void RectTransform::OnEnable()
{
	base::OnEnable();
	this->SetNeedRebuild();
}
void RectTransform::OnDisable()
{
	base::OnDisable();
	this->SetNeedRebuild();
}
void RectTransform::OnTransformChanged()
{
	base::OnTransformChanged();
	this->SetNeedRebuild();
}
void RectTransform::UpdateTransform(const Vector3& offset)
{
	if (_isTransfromDirty)
	{
		Vector3 anchor_position = GetParentArchor();
		if (_parentNode != nullptr)
		{
			RectTransform* rect_t = dynamic_cast<RectTransform*>(_parentNode);
			if (rect_t != nullptr)
			{
				const Size& parent_size = rect_t->GetSize();
				switch (_stretchMode)
				{
				case StretchMode::Horizontal:
					_size.Set(parent_size.width - (_stretchValue.x + _stretchValue.y), _originalSize.height);
					break;
				case StretchMode::Vertical:
					_size.Set(_originalSize.width, parent_size.height - (_stretchValue.z + _stretchValue.w));
					break;
				case StretchMode::All:
					_size.Set(parent_size.width - (_stretchValue.x + _stretchValue.y), parent_size.height - (_stretchValue.z + _stretchValue.w));
					break;
				default:
					_size = _originalSize;
					break;
				}
			}
			else
			{
				_size = _originalSize;
			}
		}
		else
		{
			_size = _originalSize;
		}
		base::UpdateTransform(anchor_position);
	}
}
Object* RectTransform::Clone(Object* new_obj)
{
	base::Clone(new_obj);

	RectTransform* obj = dynamic_cast<RectTransform*>(new_obj);
	if (!obj)return new_obj;

	obj->SetSize(_originalSize.width, _originalSize.height);
	obj->SetAlige(_alige);
	obj->SetStretchMode(_stretchMode);
	obj->_stretchValue = _stretchValue;

	return obj;
}
void RectTransform::SetAlige(AligeType alige) 
{
	_alige = alige;
	this->SetTransformDirty();
	this->SetNeedRebuild();
}
void RectTransform::SetStretchMode(StretchMode mode)
{
	_stretchMode = mode;
	this->SetTransformDirty();
	this->SetNeedRebuild();
}
void RectTransform::SetSize(float w, float h)
{
	_originalSize.Set(w, h);
	this->SetTransformDirty();
	this->SetNeedRebuild();
	GetGameObject()->PostComponentAction(ComponentAction::UISize, false, false);
}
const Size& RectTransform::GetSize()
{
	UpdateTransform();
	return _size;
}
void RectTransform::SetPosition(const Vector3& pos)
{
	Vector3 archor_pos = GetParentArchor();
	Vector3 position = pos - archor_pos * 0.01f;
	base::SetPosition(position);
}
Vector3 RectTransform::GetParentArchor()
{
	Vector3 anchor_position = Vector3::zero;
	if (_parentNode != nullptr)
	{
		RectTransform* rect_t = dynamic_cast<RectTransform*>(_parentNode);
		if (rect_t != nullptr)
		{
			const Size& parent_size = rect_t->GetSize();
			switch (_alige)
			{
			case AligeType::Right:
				anchor_position.x = parent_size.width * 0.5f;
				break;
			case AligeType::RightBottom:
				anchor_position.x = parent_size.width * 0.5f;
				anchor_position.y = -parent_size.height * 0.5f;
				break;
			case AligeType::Bottom:
				anchor_position.y = -parent_size.height * 0.5f;
				break;
			case AligeType::LeftBottom:
				anchor_position.x = -parent_size.width * 0.5f;
				anchor_position.y = -parent_size.height * 0.5f;
				break;
			case AligeType::Left:
				anchor_position.x = -parent_size.width * 0.5f;
				break;
			case AligeType::LeftTop:
				anchor_position.x = -parent_size.width * 0.5f;
				anchor_position.y = parent_size.height * 0.5f;
				break;
			case AligeType::Top:
				anchor_position.y = parent_size.height * 0.5f;
				break;
			case AligeType::RightTop:
				anchor_position.x = parent_size.width * 0.5f;
				anchor_position.y = parent_size.height * 0.5f;
				break;
			default:break;
			}
		}
	}
	return anchor_position;
}
void RectTransform::SetNeedRebuild()
{
	UIView* view = this->GetComponent<UIView>();
	if (view != nullptr)view->SetNeedRebuild();
}
void RectTransform::OnDrawEditor()
{
	UICanvas* canvas = GetComponent<UICanvas>();
	bool can_modify = !(canvas && (canvas->GetRenderMode() != UIRenderMode::None));
	ImVec2 mask_start_pos = ImGui::GetCursorScreenPos();

	float fullWidth = ImGui::GetContentRegionAvail().x;
	float itemWidth = fullWidth * 0.65f;

	const ImVec2 start_pos = ImVec2(ImGui::GetCursorScreenPos().x + fullWidth - itemWidth, ImGui::GetCursorScreenPos().y);
	const float width = itemWidth;
	const float reset_width = ImGui::CalcTextSize(ICON_FA_TRASH_ALT).x + 5.0f;

	const ImVec4 NORMAL_COLOR(1, 1, 1, 1); const ImVec4 SELECT_COLOR(1, 0, 0, 1);
	ImVec4 color = _alige == AligeType::LeftTop ? SELECT_COLOR : NORMAL_COLOR;
	{
		if (ImGui::ImageButton((ImTextureID)GUITextureCache::Get("internal/texture/editor/arrow_top_left.png"), ImVec2(15.0f, 15.0f), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), color))
		{
			Vector3 pos = GetLocalPosition() + GetParentArchor();
			SetAlige(AligeType::LeftTop);
			UpdateTransform();
			SetLocalPosition(pos - GetParentArchor());
		}
		ImGui::SameLine();
		color = _alige == AligeType::Top ? SELECT_COLOR : NORMAL_COLOR;
		if (ImGui::ImageButton((ImTextureID)GUITextureCache::Get("internal/texture/editor/arrow_top.png"), ImVec2(15.0f, 15.0f), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), color))
		{
			Vector3 pos = GetLocalPosition() + GetParentArchor();
			SetAlige(AligeType::Top);
			UpdateTransform();
			SetLocalPosition(pos - GetParentArchor());
		}
		ImGui::SameLine();
		color = _alige == AligeType::RightTop ? SELECT_COLOR : NORMAL_COLOR;
		if (ImGui::ImageButton((ImTextureID)GUITextureCache::Get("internal/texture/editor/arrow_top_right.png"), ImVec2(15.0f, 15.0f), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), color))
		{
			Vector3 pos = GetLocalPosition() + GetParentArchor();
			SetAlige(AligeType::RightTop);
			UpdateTransform();
			Vector3 anchor_position = GetParentArchor();
			SetLocalPosition(pos - GetParentArchor());
		}

		ImGui::SameLine();
		ImGui::SetCursorScreenPos(ImVec2(start_pos.x, ImGui::GetCursorScreenPos().y));
		ImGui::TextUnformatted(_stretchMode == StretchMode::Horizontal || _stretchMode == StretchMode::All ? "Left" : "Pos X");

		ImGui::SameLine();
		ImGui::SetCursorScreenPos(ImVec2(start_pos.x + (width - reset_width) * 0.33f, ImGui::GetCursorScreenPos().y));
		ImGui::TextUnformatted(_stretchMode == StretchMode::Vertical || _stretchMode == StretchMode::All ? "Top" : "Pos Y");

		ImGui::SameLine();
		ImGui::SetCursorScreenPos(ImVec2(start_pos.x + (width - reset_width) * 0.66f, ImGui::GetCursorScreenPos().y));
		ImGui::TextUnformatted("Pos Z");
	}
	{
		color = _alige == AligeType::Left ? SELECT_COLOR : NORMAL_COLOR;
		if (ImGui::ImageButton((ImTextureID)GUITextureCache::Get("internal/texture/editor/arrow_left.png"), ImVec2(15.0f, 15.0f), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), color))
		{
			Vector3 pos = GetLocalPosition() + GetParentArchor();
			SetAlige(AligeType::Left);
			UpdateTransform();
			Vector3 anchor_position = GetParentArchor();
			SetLocalPosition(pos - GetParentArchor());
		}
		ImGui::SameLine();
		color = _alige == AligeType::Middle ? SELECT_COLOR : NORMAL_COLOR;
		if (ImGui::ImageButton((ImTextureID)GUITextureCache::Get("internal/texture/editor/cellCursor.png"), ImVec2(15.0f, 15.0f), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), color))
		{
			Vector3 pos = GetLocalPosition() + GetParentArchor();
			SetAlige(AligeType::Middle);
			UpdateTransform();
			Vector3 anchor_position = GetParentArchor();
			SetLocalPosition(pos - GetParentArchor());
		}
		ImGui::SameLine();
		color = _alige == AligeType::Right ? SELECT_COLOR : NORMAL_COLOR;
		if (ImGui::ImageButton((ImTextureID)GUITextureCache::Get("internal/texture/editor/arrow_right.png"), ImVec2(15.0f, 15.0f), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), color))
		{
			Vector3 pos = GetLocalPosition() + GetParentArchor();
			SetAlige(AligeType::Right);
			UpdateTransform();
			Vector3 anchor_position = GetParentArchor();
			SetLocalPosition(pos - GetParentArchor());
		}

		ImGui::SameLine();
		ImGui::SetCursorScreenPos(ImVec2(start_pos.x, ImGui::GetCursorScreenPos().y));
		ImGui::SetNextItemWidth(width - reset_width);
		Vector3 pos = this->GetLocalPosition();
		if (_stretchMode == StretchMode::Horizontal || _stretchMode == StretchMode::All)pos = Vector3(_stretchValue.x, pos.y, pos.z);
		if (_stretchMode == StretchMode::Vertical || _stretchMode == StretchMode::All)pos = Vector3(pos.x, _stretchValue.z, pos.z);
		if (ImGui::DragFloat3("##Positin", (float*)pos.ptr(), 0.1f, -10000.0f, 10000.0f))
		{
			if (_stretchMode == StretchMode::Horizontal || _stretchMode == StretchMode::All)
			{
				float old_value = _stretchValue.x;
				_stretchValue.x = pos.x;
				this->SetLocalPosition(Vector3(this->GetLocalPosition().x - (old_value - _stretchValue.x)*0.5f, this->GetLocalPosition().y, pos.z));
				this->SetTransformDirty();
			}
			if (_stretchMode == StretchMode::Vertical || _stretchMode == StretchMode::All)
			{
				float old_value = _stretchValue.z;
				_stretchValue.z = pos.y;
				this->SetLocalPosition(Vector3(this->GetLocalPosition().x, this->GetLocalPosition().y + (old_value - _stretchValue.z)*0.5f, pos.z));
				this->SetTransformDirty();
			}
			if(_stretchMode == StretchMode::Null)
				this->SetLocalPosition(pos);
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(reset_width);
		if (ImGui::Selectable(ICON_FA_UNDO"##Reset Positin"))
		{
			this->SetLocalPosition(Vector3::zero);
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Reset Positin");
	}
	{
		color = _alige == AligeType::LeftBottom ? SELECT_COLOR : NORMAL_COLOR;
		if (ImGui::ImageButton((ImTextureID)GUITextureCache::Get("internal/texture/editor/arrow_bottom_left.png"), ImVec2(15.0f, 15.0f), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), color))
		{
			Vector3 pos = GetLocalPosition() + GetParentArchor();
			SetAlige(AligeType::LeftBottom);
			UpdateTransform();
			Vector3 anchor_position = GetParentArchor();
			SetLocalPosition(pos - GetParentArchor());
		}
		ImGui::SameLine();
		color = _alige == AligeType::Bottom ? SELECT_COLOR : NORMAL_COLOR;
		if (ImGui::ImageButton((ImTextureID)GUITextureCache::Get("internal/texture/editor/arrow_bottom.png"), ImVec2(15.0f, 15.0f), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), color))
		{
			Vector3 pos = GetLocalPosition() + GetParentArchor();
			SetAlige(AligeType::Bottom);
			UpdateTransform();
			Vector3 anchor_position = GetParentArchor();
			SetLocalPosition(pos - GetParentArchor());
		}
		ImGui::SameLine();
		color = _alige == AligeType::RightBottom ? SELECT_COLOR : NORMAL_COLOR;
		if (ImGui::ImageButton((ImTextureID)GUITextureCache::Get("internal/texture/editor/arrow_bottom_right.png"), ImVec2(15.0f, 15.0f), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), color))
		{
			Vector3 pos = GetLocalPosition() + GetParentArchor();
			SetAlige(AligeType::RightBottom);
			UpdateTransform();
			Vector3 anchor_position = GetParentArchor();
			SetLocalPosition(pos - GetParentArchor());
		}

		ImGui::SameLine();
		ImGui::SetCursorScreenPos(ImVec2(start_pos.x, ImGui::GetCursorScreenPos().y));
		ImGui::TextUnformatted(_stretchMode == StretchMode::Horizontal || _stretchMode == StretchMode::All ? "Right" : "Width");

		ImGui::SameLine();
		ImGui::SetCursorScreenPos(ImVec2(start_pos.x + (width - reset_width) * 0.33f, ImGui::GetCursorScreenPos().y));
		ImGui::TextUnformatted(_stretchMode == StretchMode::Vertical || _stretchMode == StretchMode::All ? "Bottom" : "Height");
	}
	{
		color = _stretchMode == StretchMode::Horizontal ? SELECT_COLOR : NORMAL_COLOR;
		if (ImGui::ImageButton((ImTextureID)GUITextureCache::Get("internal/texture/editor/arrow_h.png"), ImVec2(15.0f, 15.0f), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), color))
		{
			SetStretchMode(_stretchMode == StretchMode::Horizontal ? StretchMode::Null : StretchMode::Horizontal);
			if (_stretchMode == StretchMode::Horizontal)
			{
				if (_parentNode != nullptr)
				{
					RectTransform* parent_t = dynamic_cast<RectTransform*>(_parentNode);
					if (parent_t != nullptr)
					{
						const Size& parent_size = parent_t->GetSize();
						_stretchValue.x = parent_size.width * 0.5f + (_localPosition.x - _originalSize.width * 0.5f);
						_stretchValue.y = parent_size.width * 0.5f - (_localPosition.x + _originalSize.width * 0.5f);
					}
				}
			}
			else
			{
				if (_parentNode != nullptr)
				{
					RectTransform* parent_t = dynamic_cast<RectTransform*>(_parentNode);
					if (parent_t != nullptr)
					{
						const Size& parent_size = parent_t->GetSize();
						_originalSize.width = parent_size.width - (_stretchValue.x + _stretchValue.y);
					}
				}
			}
		}
		ImGui::SameLine();
		color = _stretchMode == StretchMode::All ? SELECT_COLOR : NORMAL_COLOR;
		if (ImGui::ImageButton((ImTextureID)GUITextureCache::Get("internal/texture/editor/arrow_all.png"), ImVec2(15.0f, 15.0f), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), color))
		{
			SetStretchMode(_stretchMode == StretchMode::All ? StretchMode::Null : StretchMode::All);
			if (_stretchMode == StretchMode::All)
			{
				if (_parentNode != nullptr)
				{
					RectTransform* parent_t = dynamic_cast<RectTransform*>(_parentNode);
					if (parent_t != nullptr)
					{
						const Size& parent_size = parent_t->GetSize();
						_stretchValue.x = parent_size.width * 0.5f + (_localPosition.x - _originalSize.width * 0.5f);
						_stretchValue.y = parent_size.width * 0.5f - (_localPosition.x + _originalSize.width * 0.5f);
						_stretchValue.z = parent_size.height * 0.5f - (_localPosition.y + _originalSize.height * 0.5f);
						_stretchValue.w = parent_size.height * 0.5f + (_localPosition.y - _originalSize.height * 0.5f);
					}
				}
			}
			else
			{
				if (_parentNode != nullptr)
				{
					RectTransform* parent_t = dynamic_cast<RectTransform*>(_parentNode);
					if (parent_t != nullptr)
					{
						const Size& parent_size = parent_t->GetSize();
						_originalSize.width = parent_size.width - (_stretchValue.x + _stretchValue.y);
						_originalSize.height = parent_size.height - (_stretchValue.z + _stretchValue.w);
					}
				}
			}
		}
		ImGui::SameLine();
		color = _stretchMode == StretchMode::Vertical ? SELECT_COLOR : NORMAL_COLOR;
		if (ImGui::ImageButton((ImTextureID)GUITextureCache::Get("internal/texture/editor/arrow_v.png"), ImVec2(15.0f, 15.0f), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), color))
		{
			SetStretchMode(_stretchMode == StretchMode::Vertical ? StretchMode::Null : StretchMode::Vertical);
			if (_stretchMode == StretchMode::Vertical)
			{
				if (_parentNode != nullptr)
				{
					RectTransform* parent_t = dynamic_cast<RectTransform*>(_parentNode);
					if (parent_t != nullptr)
					{
						const Size& parent_size = parent_t->GetSize();
						_stretchValue.z = parent_size.height * 0.5f - (_localPosition.y + _originalSize.height * 0.5f);
						_stretchValue.w = parent_size.height * 0.5f + (_localPosition.y - _originalSize.height * 0.5f);
					}
				}
			}
			else
			{
				if (_parentNode != nullptr)
				{
					RectTransform* parent_t = dynamic_cast<RectTransform*>(_parentNode);
					if (parent_t != nullptr)
					{
						const Size& parent_size = parent_t->GetSize();
						_originalSize.height = parent_size.height - (_stretchValue.z + _stretchValue.w);
					}
				}
			}
		}

		ImGui::SameLine();
		ImGui::SetCursorScreenPos(ImVec2(start_pos.x, ImGui::GetCursorScreenPos().y));
		ImGui::SetNextItemWidth((width - reset_width) * 0.66f);
		Size size = _originalSize;
		if (_stretchMode == StretchMode::Horizontal || _stretchMode == StretchMode::All)size = Size(_stretchValue.y, size.height);
		if (_stretchMode == StretchMode::Vertical || _stretchMode == StretchMode::All)size = Size(size.width, _stretchValue.w);
		if (ImGui::DragFloat2("##Size", (float*)size.p, 0.1f, -10000.0f, 10000.0f, "%.3f"))
		{
			if (_stretchMode == StretchMode::Horizontal || _stretchMode == StretchMode::All)
			{
				float old_value = _stretchValue.y;
				_stretchValue.y = size.width;
				this->SetLocalPosition(this->GetLocalPosition() + Vector3((old_value - _stretchValue.y)*0.5f, 0.0f, 0.0f));
			}
			if (_stretchMode == StretchMode::Vertical || _stretchMode == StretchMode::All)
			{
				float old_value = _stretchValue.w;
				_stretchValue.w = size.height;
				this->SetLocalPosition(this->GetLocalPosition() - Vector3(0.0f, (old_value - _stretchValue.w)*0.5f, 0.0f));
			}
			if (_stretchMode == StretchMode::Null)
				this->SetSize(size.width, size.height);
		}
	}

	ImGui::NewLine();
	base::OnDrawRotateScale(width, reset_width);

	{
		const float full_width = ImGui::GetContentRegionAvail().x;
		const float offset = 1.0f;
		if (ImGui::Button(ICON_FA_ANGLE_UP"##Position up", ImVec2(full_width, 0)))
		{
			SetLocalPosition(this->_localPosition + Vector3(0.0f, offset, 0.0f));
		}
		if (ImGui::Button(ICON_FA_ANGLE_LEFT"##Position left", ImVec2(full_width * 0.322f, 0)))
		{
			SetLocalPosition(this->_localPosition + Vector3(-offset, 0.0f, 0.0f));
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_ANGLE_DOWN"##Position down", ImVec2(full_width * 0.322f, 0)))
		{
			SetLocalPosition(this->_localPosition + Vector3(0.0f, -offset, 0.0f));
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_ANGLE_RIGHT"##Position right", ImVec2(full_width * 0.322f, 0)))
		{
			SetLocalPosition(this->_localPosition + Vector3(offset, 0.0f, 0.0f));
		}
	}

	if (!can_modify)
	{
		ImVec2 end_pos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(ImVec2(mask_start_pos.x - 30.0f, mask_start_pos.y));
		ImGuiEx::Rect(ImGui::GetContentRegionAvail().x + 50.0f, end_pos.y - mask_start_pos.y, 0x20FFFFFF, false);
	}
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(RectTransform);
template<class TransferFunction> inline
void RectTransform::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_ENUM(_alige);
	TRANSFER_ENUM(_stretchMode);
	TRANSFER_SIMPLE(_stretchValue);
	TRANSFER_SIMPLE(_originalSize);
	if (transfer.IsRead())
	{
		SetSize(_originalSize.width, _originalSize.height);
	}
}
DC_END_NAMESPACE