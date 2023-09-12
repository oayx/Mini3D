#include "UICanvasScaler.h"
#include "UICanvas.h"
#include "runtime/component/Component.inl"
#include "runtime/scene/Camera.h"
#include "runtime/Application.h"
#include "runtime/Screen.h"
#include "platform/WindowManager.h"
#include "editor/common/EAssetSelect.h"
#include "editor/common/ECommonComponent.h"
#include "editor/main/EMain_GameView.h"
#include "editor/main/EMain_Project.h"
#include "editor/EditorAppliction.h"
#include "editor/EditorCursor.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(UICanvasScaler, Component);
Object* UICanvasScaler::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	UICanvasScaler* obj = dynamic_cast<UICanvasScaler*>(new_obj);
	if (!obj)return new_obj;

	obj->SetReferenceResolution(_referenceResolution);
	obj->SetScaleMatchMode(_scaleMatchMode);
	obj->SetMatchWidth(_matchWidth);

	return obj;
}
bool UICanvasScaler::CanRemove()
{
	UICanvas* canvas = this->GetComponent<UICanvas>();
	return !canvas;
}
void UICanvasScaler::OnDrawEditor()
{
	base::OnDrawEditor();
	{
		const char* sz_flags[] = { "Scale With Screen Size"};
		ImGuiEx::Label("UI Scale Mode");
		static int current_flags = 0;
		if (ImGui::Combo("##UIScaleMode", &current_flags, sz_flags, ARRAY_SIZE(sz_flags)))
		{
		}
	}
	if (_scaleMode == UIScaleMode::ScaleWithScreenSize)
	{
		ImGuiEx::Label("Reference Resolution");
		ImGui::DragInt2("##", _referenceResolution.p, 0.5f, 0, 4096);
		
		{
			const char* sz_flags[] = { "Match Width or Height", "Expand", "Shrink" };
			ImGuiEx::Label("Scale Match Mode");
			int current_flags = int(_scaleMatchMode);
			if (ImGui::Combo("##ScaleMatchMode", &current_flags, sz_flags, ARRAY_SIZE(sz_flags)))
			{
				_scaleMatchMode = UIScaleMatchMode(current_flags);
			}
		}
		if (_scaleMatchMode == UIScaleMatchMode::MatchWidthOrHeight)
		{
			ImGuiEx::Label("Match Width");
			ImGui::Checkbox("##Match Width", &_matchWidth);
		}
	}
}
void UICanvasScaler::Refresh()
{
	UICanvas* canvas = this->GetComponent<UICanvas>();
	if (!canvas)return;

	Camera* camera = this->GetComponent<Camera>();
	if (!camera)return;

	RenderWindow* window = WindowManager::GetWindow(camera->GetTargetDisplay());
	if (!window)return;

	float orthographic_scale = 0.01f;
	Size screen_size(window->GetWidth(), window->GetHeight());
	if (Application::IsEditor())
	{
		orthographic_scale = 0.01f;
		screen_size = EMain_GameView::GetWindowSize(screen_size.width, screen_size.height);
	}

	const iSize& size = this->GetReferenceResolution();
	switch (this->GetScaleMatchMode())
	{
	case UIScaleMatchMode::MatchWidthOrHeight:
	{
		if (this->IsMatchWidth())
		{
			float match_height = (screen_size.height * size.width) / screen_size.width;
			camera->SetOrthographicSize(size.width * orthographic_scale, match_height * orthographic_scale);
			_realResolution.Set(size.width, match_height);
		}
		else
		{
			float match_width = (screen_size.width * size.height) / screen_size.height;
			camera->SetOrthographicSize(match_width * orthographic_scale, size.height * orthographic_scale);
			_realResolution.Set(match_width, size.height);
		}
	}
	break;
	case UIScaleMatchMode::Expand:
	{
		float scale_screen = screen_size.width / screen_size.height;
		float scale_resolution = size.width / size.height;
		if (scale_screen >= scale_resolution)
		{
			float match_width = (screen_size.width * size.height) / screen_size.height;
			camera->SetOrthographicSize(match_width * orthographic_scale, size.height * orthographic_scale);
			_realResolution.Set(match_width, size.height);
		}
		else
		{
			float match_height = (screen_size.height * size.width) / screen_size.width;
			camera->SetOrthographicSize(size.width * orthographic_scale, match_height * orthographic_scale);
			this->_realResolution.Set(size.width, match_height);
		}
	}
	break;
	case UIScaleMatchMode::Shrink:
	{
		float scale_screen = screen_size.width / screen_size.height;
		float scale_resolution = size.width / size.height;
		if (scale_screen >= scale_resolution)
		{
			float match_height = (screen_size.height * size.width) / screen_size.width;
			camera->SetOrthographicSize(size.width * orthographic_scale, match_height * orthographic_scale);
			_realResolution.Set(size.width, match_height);
		}
		else
		{
			float match_width = (screen_size.width * size.height) / screen_size.height;
			camera->SetOrthographicSize(match_width * orthographic_scale, size.height * orthographic_scale);
			_realResolution.Set(match_width, size.height);
		}
	}
	break;
	}

	//修改canvas所在节点大小
	RectTransform* transform = canvas->GetGameObject()->GetRectTransform();
	if (transform != nullptr)
	{
		transform->SetSize(_realResolution.width, _realResolution.height);
	}

	canvas->SetNeedRebuild(true);
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(UICanvasScaler);
template<class TransferFunction> inline
void UICanvasScaler::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_referenceResolution);
	TRANSFER_ENUM(_scaleMode);
	TRANSFER_ENUM(_scaleMatchMode);
	TRANSFER_SIMPLE(_matchWidth);
	if (transfer.IsRead())
	{
		this->SetReferenceResolution(_referenceResolution);
		this->SetScaleMatchMode(_scaleMatchMode);
		this->SetMatchWidth(_matchWidth);
	}
}
DC_END_NAMESPACE
