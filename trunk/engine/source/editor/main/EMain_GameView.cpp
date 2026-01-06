#include "EMain_GameView.h"
#include "EMain_Hierarchy.h"
#include "runtime/component/Component.inl"
#include "runtime/graphics/Material.h"
#include "runtime/project/QualitySettings.h"
#include "runtime/scene/Camera.h"
#include "runtime/ui/GUI.h"
#include "runtime/ui/UICanvasScaler.h"
#include "runtime/Application.h"
#include "runtime/Screen.h"
#include "inspector/EInspector_Hierarchy.h"
#include "inspector/EInspector_Project.h"
#include "inspector/EInspector_ProjectSettings_Layers.h"
#include "editor/EditorAppliction.h"
#include "editor/main/EMain_Tools.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(EMain_GameView, EWindowBase);
void EMain_GameView::Update()
{
	if (_oldViewPort != _viewPort)
	{
		Resize();
		_oldViewPort = _viewPort;
		_isRTDirty = true;
	}

	if (!_renderTexture || _isRTDirty)
	{
		CreateRenderTexture();
		_isRTDirty = false;
	}

}
void EMain_GameView::Render()
{
	DC_PROFILE_FUNCTION;
	if (!IsShow)return;

	ImGui::SetNextWindowSizeConstraints(ImVec2(400.0f, 400.0f), ImVec2(FLT_MAX, FLT_MAX));
	ImGui::Begin(ICON_FA_GAMEPAD" Game   ", &IsShow, ImGuiWindowFlags_MenuBar);
	{
		ShowToolbar();
		const ImVec2& pos = ImGui::GetCursorScreenPos();
		const ImVec2& size = ImGui::GetContentRegionAvail();

		if (_renderTexture)
		{
			Size screenSize((float)_renderTexture->GetWidth(), (float)_renderTexture->GetHeight());
			Size imageSize = GetWindowSize(size.x, size.y);
			Vector2 image_pos((size.x - imageSize.width) * 0.5f, (size.y - imageSize.height) * 0.5f);
			float uv_x = 0.0f, uv_y = 0.0f;
			if (EditorConfig::GameResolution == GameViewResolution::Free)
			{
				float offsetX = (screenSize.width - imageSize.width) * 0.5f;
				float offsetY = (screenSize.height - imageSize.height) * 0.5f;
				offsetX = offsetX < 0.0f ? 0.0f : offsetX;
				offsetY = offsetY < 0.0f ? 0.0f : offsetY;
				uv_x = offsetX / screenSize.width;
				uv_y = offsetY / screenSize.height;
			}
			_viewPort.Set(pos.x + image_pos.x, pos.y + image_pos.y, imageSize.width, imageSize.height);
			ImGui::SetCursorScreenPos(ImVec2(_viewPort.x, _viewPort.y));
			ImGui::Image(_renderTexture, ImVec2(_viewPort.width, _viewPort.height), ImVec2(uv_x, uv_y), ImVec2(1.0f - uv_x, 1.0f - uv_y));
		}

		//焦点
		if (ImGui::IsItemClicked(1) && ImGui::IsWindowHovered(1))
		{
			ImGui::SetWindowFocus();
		}
	}

	_isFocus = ImGui::IsWindowFocused();
	ImGui::End();
}
void EMain_GameView::Resize()
{
	Size imageSize = GetWindowSize(_viewPort.width, _viewPort.height);
	for (int i = 0; i < SceneManager::GetCameraCount(); ++i)
	{
		Camera* camera = SceneManager::GetCamera(i);
		if (camera && !camera->GetGameObject()->HasFlag(GameObjectFlag::EditorCamera))
		{
			if (camera->IsOrthographic() && camera->GetComponent<UICanvasScaler>())
			{
				camera->GetComponent<UICanvasScaler>()->Refresh();
			}
			else
			{
				camera->SetAspect(imageSize.width / imageSize.height);
			}
		}
	}
}
Size EMain_GameView::GetWindowSize(float width, float height)
{
	Size imageSize(width, height);
	float viewScale = width / height;
	switch (EditorConfig::GameResolution)
	{
	case GameViewResolution::Scale_5_4:
	{
		float scale = 5.0f / 4.0f;
		if (viewScale < scale)imageSize.height = imageSize.width / scale;
		else if (viewScale > scale)imageSize.width = imageSize.height * scale;
		break;
	}
	case GameViewResolution::Scale_4_3:
	{
		float scale = 4.0f / 3.0f;
		if (viewScale < scale)imageSize.height = imageSize.width / scale;
		else if (viewScale > scale)imageSize.width = imageSize.height * scale;
		break;
	}
	case GameViewResolution::Scale_3_2:
	{
		float scale = 3.0f / 2.0f;
		if (viewScale < scale)imageSize.height = imageSize.width / scale;
		else if (viewScale > scale)imageSize.width = imageSize.height * scale;
		break;
	}
	case GameViewResolution::Scale_16_9:
	{
		float scale = 16.0f / 9.0f;
		if (viewScale < scale)imageSize.height = imageSize.width / scale;
		else if (viewScale > scale)imageSize.width = imageSize.height * scale;
		break;
	}
	case GameViewResolution::Scale_9_16:
	{
		float scale = 9.0f / 16.0f;
		if (viewScale < scale)imageSize.height = imageSize.width / scale;
		else if (viewScale > scale)imageSize.width = imageSize.height * scale;
		break;
	}
	}
	return imageSize;
}
void EMain_GameView::ShowToolbar()
{
	if (ImGui::BeginMenuBar())
	{
		{
			const char* szFlags[] = { "Free Aspect", "5:4", "4:3", "3:2", "16:9", "9:16" };
			static int currentIndex = (int)EditorConfig::GameResolution;
			ImGui::SetNextItemWidth(150.0f);
			if (ImGui::Combo("##Shadow Type", &currentIndex, szFlags, ARRAY_SIZE(szFlags)))
			{
				EditorConfig::GameResolution = (GameViewResolution)currentIndex;
				Resize();
			}
		}

		ImGui::EndMenuBar();
	}
}
void EMain_GameView::CreateRenderTexture()
{
	SAFE_RELEASE(_renderTexture);
	int antiAlias = _isEnableMSAA ? int(QualitySettings::GetMSAAType()) : 1;
	int size = Math::Clamp(Math::Max<int>(_viewPort.width, _viewPort.height), 512, 2048);
	TextureDesc desc;
	desc.width = size; desc.height = size; desc.antiAlias = antiAlias; desc.flags = TextureFlag::COLOR_AND_DEPTH;
	_renderTexture = Application::GetGraphics()->CreateRenderTexture(desc);
	_renderTexture->Retain();
}
DC_END_NAMESPACE