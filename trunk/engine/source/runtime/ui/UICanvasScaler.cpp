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
Object* UICanvasScaler::Clone(Object* newObj)
{
	base::Clone(newObj);
	UICanvasScaler* obj = dynamic_cast<UICanvasScaler*>(newObj);
	if (!obj)return newObj;

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
		const char* szFlags[] = { "Scale With Screen Size"};
		ImGuiEx::Label("UI Scale Mode");
		static int currentFlags = 0;
		if (ImGui::Combo("##UIScaleMode", &currentFlags, szFlags, ARRAY_SIZE(szFlags)))
		{
		}
	}
	if (_scaleMode == UIScaleMode::ScaleWithScreenSize)
	{
		ImGuiEx::Label("Reference Resolution");
		ImGui::DragInt2("##", _referenceResolution.p, 0.5f, 0, 4096);
		
		{
			const char* szFlags[] = { "Match Width or Height", "Expand", "Shrink" };
			ImGuiEx::Label("Scale Match Mode");
			int currentFlags = int(_scaleMatchMode);
			if (ImGui::Combo("##ScaleMatchMode", &currentFlags, szFlags, ARRAY_SIZE(szFlags)))
			{
				_scaleMatchMode = UIScaleMatchMode(currentFlags);
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

	float orthographicScale = 0.01f;
	Size screenSize(window->GetWidth(), window->GetHeight());
	if (Application::IsEditor())
	{
		orthographicScale = 0.01f;
		screenSize = EMain_GameView::GetWindowSize(screenSize.width, screenSize.height);
	}

	const iSize& size = this->GetReferenceResolution();
	switch (this->GetScaleMatchMode())
	{
	case UIScaleMatchMode::MatchWidthOrHeight:
	{
		if (this->IsMatchWidth())
		{
			float matchHeight = (screenSize.height * size.width) / screenSize.width;
			camera->SetOrthographicSize(size.width * orthographicScale, matchHeight * orthographicScale);
			_realResolution.Set(size.width, matchHeight);
		}
		else
		{
			float matchWidth = (screenSize.width * size.height) / screenSize.height;
			camera->SetOrthographicSize(matchWidth * orthographicScale, size.height * orthographicScale);
			_realResolution.Set(matchWidth, size.height);
		}
	}
	break;
	case UIScaleMatchMode::Expand:
	{
		float scaleScreen = screenSize.width / screenSize.height;
		float scaleResolution = size.width / size.height;
		if (scaleScreen >= scaleResolution)
		{
			float matchWidth = (screenSize.width * size.height) / screenSize.height;
			camera->SetOrthographicSize(matchWidth * orthographicScale, size.height * orthographicScale);
			_realResolution.Set(matchWidth, size.height);
		}
		else
		{
			float matchHeight = (screenSize.height * size.width) / screenSize.width;
			camera->SetOrthographicSize(size.width * orthographicScale, matchHeight * orthographicScale);
			this->_realResolution.Set(size.width, matchHeight);
		}
	}
	break;
	case UIScaleMatchMode::Shrink:
	{
		float scaleScreen = screenSize.width / screenSize.height;
		float scaleResolution = size.width / size.height;
		if (scaleScreen >= scaleResolution)
		{
			float matchHeight = (screenSize.height * size.width) / screenSize.width;
			camera->SetOrthographicSize(size.width * orthographicScale, matchHeight * orthographicScale);
			_realResolution.Set(size.width, matchHeight);
		}
		else
		{
			float matchWidth = (screenSize.width * size.height) / screenSize.height;
			camera->SetOrthographicSize(matchWidth * orthographicScale, size.height * orthographicScale);
			_realResolution.Set(matchWidth, size.height);
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
