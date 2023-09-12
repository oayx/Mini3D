#include "UIInputField.h"
#include "UIAtlas.h"
#include "UICanvas.h"
#include "runtime/input/Input.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/null/Texture.h"
#include "editor/common/EAssetSelect.h"
#include "editor/common/ECommonComponent.h"
#include "editor/main/EMain_Project.h"
#include "editor/EditorAppliction.h"
#include "editor/EditorCursor.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(UIInputField, UIImage);
Object* UIInputField::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	UIInputField* obj = dynamic_cast<UIInputField*>(new_obj);
	if (!obj)return new_obj;

	//lable和carat会作为Gameobject的子对象复制

	obj->SetText(_unicodes);
	obj->SetPlaceholderText(_placeholderText);
	obj->SetPlaceholderTextColor(_placeholderTextColor);

	return obj;
}
void UIInputField::Awake()
{
	base::Awake();
	this->SetEventListener(UIEventType::LMouseUpInside, [this](const UIEventInfo& info)
		{
			if (!_isFocus)this->OnFocus();
		});
	this->SetEventListener(UIEventType::LMouseUpOutside, [this](const UIEventInfo& info)
		{
			if (_isFocus)this->OnLostFocus();
		});
}
void UIInputField::Update()
{
	base::Update();
	if (!_isFocus)
		return;
	
	CaretBlink();

	const List<ushort>& chars = Input::GetInputChar();
	if (!chars.IsEmpty())
	{
		for (const auto& c : chars)
		{
			_unicodes.push_back(c);
		}

		//处理特殊字符
		bool has_special_char = true;
		while (has_special_char)
		{
			has_special_char = false;
			for (uint i = 0; i < _unicodes.size(); ++i)
			{
				char c = _unicodes[i];
				switch ((KeyCode)c)
				{
				case KeyCode::Backspace:
				{//backspace
					auto it = _unicodes.begin();
					std::advance(it, i);
					it = _unicodes.erase(it);
					if (i > 0)
					{
						std::advance(it, -1);
						_unicodes.erase(it);
					}
					has_special_char = true;
					break;
				}
				case KeyCode::Tab:
				{//tab
					auto it = _unicodes.begin();
					std::advance(it, i);
					it = _unicodes.erase(it);
					it = _unicodes.insert(it, (char)KeyCode::Space);
					it = _unicodes.insert(it, (char)KeyCode::Space);
					it = _unicodes.insert(it, (char)KeyCode::Space);
					_unicodes.insert(it, (char)KeyCode::Space);
					has_special_char = true;
					break;
				}
				case KeyCode::Return:
				{//TODO:不支持换行
					auto it = _unicodes.begin();
					std::advance(it, i);
					it = _unicodes.erase(it);
					has_special_char = true;
					break;
				}
				case KeyCode::Escape:
				{//esc
					auto it = _unicodes.begin();
					std::advance(it, i);
					it = _unicodes.erase(it);
					has_special_char = true;
					break;
				}
				}
			}
		}
		if (!_unicodes.empty())
			this->SetText(_unicodes);
		else
			this->SetText("");
	}

	this->UpdateCaretPos();

	//判断是否丢失焦点
	if (Input::GetMouseButtonUp(MouseBtnID::Left))
	{
		UICanvas* canvas = this->GetRootCanvas();
		if (canvas == nullptr)
		{
			Debuger::Error("cannot find root canvas");
			return;
		}
		Vector2 ui_pos;
		if (!canvas->ScreenToUIPoint(Input::mousePosition, ui_pos) || !this->HitTest((int)ui_pos.x, (int)ui_pos.y))
		{
			this->OnLostFocus();
		}
	}
}
void UIInputField::SetText(const String& txt)
{
	_unicodes = txt.c_str();
	UILabel* label = GetLabel();
	if (label)
	{
		if (txt.IsEmpty() && !_isFocus)
			label->SetText(_placeholderText);
		else
			label->SetText(txt);
	}

	this->UpdateCaretPos();
}
void UIInputField::SetFont(const String& font)
{
	UILabel* label = GetLabel();
	if (label)
	{
		label->SetFont(font);
	}
}
void UIInputField::SetFontSize(byte size)
{
	UILabel* label = GetLabel();
	if (label)
	{
		label->SetFontSize(size);

		UIImage* caret = GetCaret();
		if (caret)
		{
			caret->SetSize(1, label->GetFontSize());
		}
	}
	this->UpdateCaretPos();
}
void UIInputField::SetFontColor(const Color& color)
{
	UILabel* label = GetLabel();
	if (label)
	{
		label->SetColor(color);
	}
}
void UIInputField::SetContentSize(const Size& size)
{
	UILabel* label = GetLabel();
	if (label)
	{
		label->SetSize(size.width, size.height);
	}
	this->UpdateCaretPos();
}
void UIInputField::SetPlaceholderText(const String& placeholder)
{
	_placeholderText = placeholder;
}
void UIInputField::SetPlaceholderTextColor(const Color& color)
{
	_placeholderTextColor = color;
}
void UIInputField::SetCaretBlinkRate(float rate)
{
	_caretBlinkRate = rate;
}
UILabel* UIInputField::GetLabel()
{
	if (_label) return _label;

	Transform* obj = GetTransform()->Find("Text");
	if (obj)
	{
		UILabel* label = obj->GetGameObject()->GetComponent<UILabel>();
		if (label)
		{
			_label = label;
			label->SetSize(GetSize().width, GetSize().height);
			this->SetText(_unicodes);
			this->SetPlaceholderTextColor(label->GetColor());
			this->SetNeedRebuild();
		}
	}
	return _label;
}
UIImage* UIInputField::GetCaret()
{
	if (_caret)return _caret;

	Transform* obj = GetTransform()->Find("Placeholder");
	if (obj)
	{
		UIImage* caret = obj->GetGameObject()->GetComponent<UIImage>();
		if (caret)
		{
			_caret = caret;
			UILabel* label = GetLabel();
			if (label)
			{
				const Color& color = label->GetColor();
				caret->SetColor(Color(color.r, color.g, color.b, 0));
				caret->SetSize(1, label->GetFontSize());
			}
			else
			{
				caret->SetColor(Color::Clear);
			}

			this->SetNeedRebuild();
		}
	}
	return _caret;
}
void UIInputField::CaretBlink()
{
	if (Time::GetRealTimeSinceStartup() - _caretBlinkTime > _caretBlinkRate)
	{
		SetCaretVisible(!_caretIsShow);
		_caretIsShow = !_caretIsShow;
		_caretBlinkTime = Time::GetRealTimeSinceStartup();
	}
}
void UIInputField::SetCaretVisible(bool b)
{
	UILabel* label = GetLabel();
	UIImage* caret = GetCaret();
	if (label && caret)
	{
		const Color& color = label->GetColor();
		if (!b)
			caret->SetColor(Color(color.r, color.g, color.b, 0));
		else
			caret->SetColor(Color(color.r, color.g, color.b, 1));
	}
}
void UIInputField::UpdateCaretPos()
{
	if (!_isFocus)
		return;
	UILabel* label = GetLabel();		
	UIImage* caret = GetCaret();
	if (!label || !caret)return;
	
	float caret_x = -label->GetSize().width * 0.5f + label->GetContentRealSize().width;
	const Vector3& local_caret_pos = caret->GetTransform()->GetLocalPosition();
	caret->GetTransform()->SetLocalPosition(Vector3(caret_x, local_caret_pos.y, local_caret_pos.z));
}
void UIInputField::OnFocus()
{
	Debuger::Log("OnFocus");

	UILabel* label = GetLabel();
	if (!label)return;

	_isFocus = true;
	if (_unicodes.empty())
	{
		label->SetText("");
	}
	this->UpdateCaretPos();
#if DC_PLATFORM_ANDROID
	java_show_keyboard();
#endif
}
void UIInputField::OnLostFocus()
{
	Debuger::Log("OnLostFocus");

	_isFocus = false;
	SetCaretVisible(false);
	if (_unicodes.empty())
	{
		this->SetText("");
	}
#if DC_PLATFORM_ANDROID
	java_hide_keyboard();
#endif
}
void UIInputField::OnDrawEditor()
{
	Component::OnDrawEditor();

	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
	if(ImGui::TreeNodeEx("Image", base_flags))
	{
		UIImage::OnDrawEditor_Image();
		UIImage::OnDrawEditor_Color();
		UIImage::OnDrawEditor_ImageType();
		UIView::OnDrawEditor_RaycastTarget();

		ImGui::TreePop();
	}
	ImGui::NewLine();
	if (ImGui::TreeNodeEx("Input Field", base_flags))
	{
		{
			ImGuiEx::Label("Text");
			char text_buff[1024 * 16] = { 0 };
			Memory::Copy(text_buff, _unicodes.c_str(), (int)_unicodes.size());
			if (ImGui::InputText("##Text", text_buff, ARRAY_SIZE(text_buff)))
			{
				SetText(text_buff);
			}
		}

		ImGui::TreePop();
	}
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(UIInputField);
template<class TransferFunction> inline
void UIInputField::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_unicodes);

	if (transfer.IsRead())
	{
		this->GetLabel();
		this->GetCaret();
		SetText(_unicodes);
	}
}
DC_END_NAMESPACE