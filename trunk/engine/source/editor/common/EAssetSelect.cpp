#include "EAssetSelect.h"
#include "EditorIcon.h"
#include "runtime/resources/AssetsManager.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
bool EAssetSelect::IsShow = false;	
ResourceType EAssetSelect::_resType = ResourceType::Undefined;
Vector<AssetMeta*> EAssetSelect::_reslist;
std::function<void(const String&)> EAssetSelect::_callback;
void EAssetSelect::Initialize()
{
	//AssetsManager::AddEventListener(RefreshAssetsEvt, nullptr, MakeEventHandler());
}
void EAssetSelect::Destroy()
{
	//AssetsManager::RemoveEventListener(RefreshAssetsEvt, nullptr, MakeEventHandler());
}
void EAssetSelect::Render()
{
	DC_PROFILE_FUNCTION();
	if (!IsShow)return;
	if (_resType == ResourceType::Undefined)return;

	ImGuiViewport* viewport = ImGui::GetWindowViewport();
	if (viewport)ImGui::SetNextWindowPos(ImVec2((viewport->Size.x - 500)*0.5f, (viewport->Size.y - 800)*0.5f));
	ImGui::SetNextWindowSize(ImVec2(500, 800));

	String window_name = "Select Asset";
	switch (_resType)
	{
	case ResourceType::Texture:		window_name = "Select Texture"; break;
	case ResourceType::Material:	window_name = "Select Material"; break;
	case ResourceType::Mesh:		window_name = "Select Mesh"; break;
	case ResourceType::Shader:		window_name = "Select Shader"; break;
	case ResourceType::AudioClip:	window_name = "Select AudioClip"; break;
	case ResourceType::Video:		window_name = "Select Video"; break;
	case ResourceType::Txt:			window_name = "Select Asset"; break;
	case ResourceType::Binary:		window_name = "Select Asset"; break;
	case ResourceType::Font:		window_name = "Select Font"; break;
	case ResourceType::Scene:		window_name = "Select Scene"; break;
	default:  						window_name = "Select Asset"; break;
	}

	bool is_close = false;
	static float icon_zoom = 1.0f;
	ImGui::Begin(window_name.c_str(), &IsShow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);
	{
		static char filter[128] = "";
		ImGui::BeginChild("top panel", ImVec2(0.0f, 40.0f), true, ImGuiWindowFlags_AlwaysAutoResize);
		{//搜索
			const float w = ImGui::CalcTextSize(ICON_FA_TIMES).x + ImGui::GetStyle().ItemSpacing.x * 2.0f;// + 150.0f
			ImGui::SetNextItemWidth(-w);
			ImGui::InputTextWithHint("##filter", "Filter", filter, sizeof(filter));
			ImGui::SameLine();
			if (ImGuiEx::IconButton(ICON_FA_TIMES, "Clear filter"))
			{
				filter[0] = '\0';
			}
		}
		ImGui::EndChild();

		const float ICON_SIZE = ImGui::GetFont()->FontSize + 3;
		ImGui::BeginChild("list view", ImVec2(0, 0));
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + ICON_SIZE + 5.0f, ImGui::GetCursorScreenPos().y));
		if (ImGui::Selectable("None"))
		{
			_callback("");
			is_close = true;
		}
		for (int i = 0; i < _reslist.Size(); ++i)
		{
			AssetMeta* meta = _reslist[i];
			const char* sz_name = meta->GetFullPath().c_str();
			if (filter[0] == '\0' || String::Stristr(sz_name, filter) != nullptr)
			{
				ImGui::Image((ImTextureID)EditorIconCache::GetTexture(meta), ImVec2(ICON_SIZE, ICON_SIZE));
				ImGui::SameLine();
				if (ImGui::Selectable(sz_name))
				{
					_callback(meta->GetFullPath());
					is_close = true;
				}
			}
		}
		ImGui::EndChild();

		if (ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1))
		{
			ImVec2 mouse_pos = ImGui::GetMousePos();
			ImVec2 pos = ImGui::GetWindowPos();
			ImVec2 size = ImGui::GetWindowSize();
			if (!(mouse_pos.x >= pos.x && mouse_pos.y >= pos.y && mouse_pos.x <= pos.x + size.x && mouse_pos.y <= pos.y + size.y))
			{
				is_close = true;
			}
		}
	}
	ImGui::End();

	if (is_close)
	{
		IsShow = false;
	}
}
void EAssetSelect::Search(ResourceType type, std::function<void(const String&)> callback)
{
	if (type == ResourceType::Undefined)return;
	IsShow = true;
	_resType = type;
	_reslist = AssetsManager::GetAssetGroup(type);
	_callback = callback;
}
DC_END_NAMESPACE