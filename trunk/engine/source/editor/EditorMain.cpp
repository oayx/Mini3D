#include "EditorMain.h"
#include "core/stream/DataStream.h"
#include "runtime/component/GameObject.h"
#include "runtime/resources/Resources.h"
#include "runtime/ui/GUI.h"
#include "runtime/Application.h"
#include "editor/common/EAssetSelect.h"
#include "editor/file/EOpenProject.h"
#include "editor/help/EIMGUISetter.h"
#include "editor/main/EMain_GameView.h"
#include "editor/main/EMain_Hierarchy.h"
#include "editor/main/EMain_Inspector.h"
#include "editor/main/EMain_Menu.h"
#include "editor/main/EMain_Project.h"
#include "editor/main/EMain_SceneView.h"
#include "editor/main/EMain_Tools.h"
#include "editor/window/EConsole.h"
#include "editor/window/ELightSetting.h"
 
DC_BEGIN_NAMESPACE
static const ImWchar ICONS_RANGES[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
static ImFont* AddFontFromFile(const String& file, float size, bool has_icons, const ImWchar* font_ranges)
{
	ImGuiIO& io = ImGui::GetIO();
	ImFontConfig iconsConfig;
	iconsConfig.FontDataOwnedByAtlas = false;
	ImFont* font = io.Fonts->AddFontFromFileTTF(Resource::GetFullDataPath(file).c_str(), size, &iconsConfig, font_ranges);
	if (font && has_icons)
	{
		iconsConfig.MergeMode = true;
		iconsConfig.GlyphMinAdvanceX = size;
		io.Fonts->AddFontFromFileTTF(Resource::GetFullDataPath("internal/font/fa-regular-400.ttf").c_str(), size * 0.75f, &iconsConfig, ICONS_RANGES);
		io.Fonts->AddFontFromFileTTF(Resource::GetFullDataPath("internal/font/fa-solid-900.ttf").c_str(), size * 0.75f, &iconsConfig, ICONS_RANGES);
	}
	return font;
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(EditorMain, Object);
void EditorMain::Initialize()
{
	//配置文件
	EIMGUISetter::InitLayout();

	//样式
	EIMGUISetter::InitStyle();
	
	ImGuiIO& io = ImGui::GetIO();
	//字体
	{
		const float FONT_SIZE = 18.0f;
		//默认字体
		_defaultFont = AddFontFromFile("internal/font/msyh.ttf", FONT_SIZE, true, io.Fonts->GetGlyphRangesChineseFull());//默认加载所有中文字，内存会大
		//大字体
		_bigFont = AddFontFromFile("internal/font/msyh.ttf", 24.0f, false, nullptr);
		//大图标
		_bigIconFont = AddFontFromFile("internal/font/fa-solid-900.ttf", FONT_SIZE * 1.0f, true, ICONS_RANGES);
	}

	//
	{
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;			// Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;				// Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;				// Enable Multi-Viewport / Platform Windows

		io.BackendPlatformName = "imgui_win32";
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;			// We can honor GetMouseCursor() values (optional)
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;			// We can honor io.WantSetMousePos requests (optional, rarely used)
		io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;		// We can create multi-viewports on the Platform side (optional)
		io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport;	// We can set io.MouseHoveredViewport correctly (optional, not easy)
	}

	EAssetSelect::Initialize();
	EMain_Inspector::Initialize();
}
void EditorMain::Destroy()
{
	_defaultFont = nullptr;
	_bigIconFont = nullptr;
	EAssetSelect::Destroy();
	EMain_Inspector::Destroy();
}
void EditorMain::Render()
{
	DC_PROFILE_FUNCTION;
	ImGui::PushFont(_defaultFont);

	//创建菜单
	EMain_Menu::Render();
	CreateDock();
	EMain_Hierarchy::Render();
	EMain_Inspector::Render();
	EMain_Project::Render();
	EMain_GameView::Render();
	EMain_SceneView::Render();

	{//File
		EOpenProject::Render();
	}
	{//Tools
	}
	{//window
		EConsole::Render();
		ELightSetting::Render();
	}
	{//help
		EIMGUISetter::Render();
	}

	{//其他临时性的
		EAssetSelect::Render();
	}

	ImGui::PopFont();
}
void EditorMain::Resize(const WindowResizeDesc& desc)
{
}
void EditorMain::CreateDock()
{
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	windowFlags |= ImGuiWindowFlags_NoBackground;

	bool p_open = true;
	ImGui::Begin("DockSpace", &p_open, windowFlags);
	ImGui::PopStyleVar();
	ImGui::PopStyleVar(2);

	static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);

	if (ImGui::BeginMenuBar())
	{
		EMain_Tools::Render();

		ImGui::EndMenuBar();
	}

	ImGui::End();
}
DC_END_NAMESPACE