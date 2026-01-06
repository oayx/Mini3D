#include "EIMGUISetter.h"
#include "core/stream/DataStream.h"
#include "runtime/resources/Resources.h"
#include "runtime/ui/GUI.h"
#include "editor/common/ECommonComponent.h"
#include "editor/EditorDefine.h"
#include "editor/EditorAppliction.h"
 
DC_BEGIN_NAMESPACE
static const String SetterFilePath = "data/settings/EditorStyle.asset";
/********************************************************************/
void EIMGUISetter::InitStyle()
{
	ImGui::StyleColorsDark();
	MemoryDataStream stream;
	if (File::ReadAllBytes(Resource::GetFullSavePath(SetterFilePath), stream))
	{
		if (stream.Size() == sizeof(ImGuiStyle))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			Memory::Copy(&style, stream.Buffer(), stream.Size());
		}
		else
		{
			ResetStyle();
		}
	}
	else
	{
		ResetStyle();
	}
}
void EIMGUISetter::ResetStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	{//font
		style.WindowPadding = ImVec2(8.0f, 8.0f);
		style.FramePadding = ImVec2(8.0f, 3.0f);
		style.CellPadding = ImVec2(4.0f, 2.0f);
		style.ItemSpacing =  ImVec2(6.0f, 3.0f);
		style.ItemInnerSpacing = ImVec2(2.0f, 4.0f);
		style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
		style.IndentSpacing = 21.0;
		style.ScrollbarSize = 16.0;
		style.GrabMinSize = 10.0;
		
		style.WindowBorderSize = 1.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupBorderSize = 1.0f;
		style.FrameBorderSize = 0.0f;
		style.TabBorderSize = 0.0f;
		
		style.WindowRounding = 5.0f;
		style.ChildRounding = 5.0f;
		style.FrameRounding = 5.0f;
		style.PopupRounding = 5.0f;
		style.ScrollbarRounding = 5.0f;
		style.GrabRounding = 5.0f;
		style.LogSliderDeadzone = 4.0f;
		style.TabRounding = 5.0f;
		
		style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_::ImGuiDir_None;
		style.ColorButtonPosition = ImGuiDir_::ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
		style.DisplaySafeAreaPadding = ImVec2(3.0f, 3.0f);
	}
	{//color
		style.Colors[ImGuiCol_Text] = ImVec4(255,255,255,255);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(128, 128, 128, 255);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(41, 41, 41, 255);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(41, 41, 41, 255);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(36, 36, 36, 255);
		style.Colors[ImGuiCol_Border] = ImVec4(61, 61, 61, 255);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(66, 66, 66, 255);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(82, 82, 82, 255);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(94, 94, 94, 255);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(31, 31, 31, 255);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(41, 41, 41, 255);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(41, 41, 41, 255);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(36, 36, 36, 255);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(5, 5, 5, 0);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(79, 79, 79, 255);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(105, 105, 105, 255);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(130, 130, 130, 255);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(130, 130, 130, 255);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(130, 130, 130, 255);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(143, 143, 143, 255);
		style.Colors[ImGuiCol_Button] = ImVec4(69, 69, 69, 255);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(87, 87, 87, 255);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(154, 154, 154, 255);
		style.Colors[ImGuiCol_Header] = ImVec4(89, 89, 89, 255);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(99, 99, 99, 255);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(112, 112, 112, 255);
		style.Colors[ImGuiCol_Separator] = ImVec4(61, 61, 61, 255);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(79, 79, 79, 255);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(87, 87, 87, 255);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(61, 61, 61, 255);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(79, 79, 79, 255);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(94, 94, 94, 255);
		style.Colors[ImGuiCol_Tab] = ImVec4(154, 154, 154, 128);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(154, 154, 154, 255);
		style.Colors[ImGuiCol_TabActive] = ImVec4(154, 154, 154, 255);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(128, 128, 128, 64);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(128, 128, 128, 128);
		style.Colors[ImGuiCol_DockingPreview] = ImVec4(140, 140, 140, 255);
		style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(51, 51, 51, 255);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(156, 156, 156, 255);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(255, 110, 89, 255);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(230, 179, 0, 255);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(255, 153, 0, 255);
		style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(48, 48, 51, 255);
		style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(79, 79, 89, 255);
		style.Colors[ImGuiCol_TableBorderLight] = ImVec4(59, 59, 64, 255);
		style.Colors[ImGuiCol_TableRowBg] = ImVec4(0, 0, 0, 0);
		style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(255, 255, 255, 15);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(66, 150, 250, 89);
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(255, 255, 0, 230);
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(199, 224, 255, 255);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(255, 255, 179, 255);
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(204, 204, 204, 51);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(112, 112, 112, 89);

		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			style.Colors[i] = ImVec4(style.Colors[i].x / 255.0f, style.Colors[i].y / 255.0f, style.Colors[i].z / 255.0f, style.Colors[i].w / 255.0f);
		}
	}
	File::WriteAllBytes(Resource::GetFullSavePath(SetterFilePath), (byte*)&style, sizeof(ImGuiStyle));
}
void EIMGUISetter::InitLayout()
{
	if (!File::Exist(Resource::GetFullSavePath("imgui.ini")))
	{
		SetLayout(LayoutType::Default);
	}
}
void EIMGUISetter::SetLayout(LayoutType type)
{
	String src_file = "";
	String dst_file = "imgui.ini";

	switch (type)
	{
	case LayoutType::Default:src_file = "data/settings/imgui.ini";break;
	case LayoutType::Split2_3:src_file = "data/settings/imgui2_3.ini"; break;
	default: return;
	}

	File::Copy(Resource::GetFullSavePath(src_file), Resource::GetFullSavePath(dst_file));
	EditorConfig::LayoutIndex = (int)type;
}
void EIMGUISetter::Render()
{
	DC_PROFILE_FUNCTION;
	if (!IsShow)return;
	ImGui::SetNextWindowSizeConstraints(ImVec2(400.0f, 600.0f), ImVec2(FLT_MAX, FLT_MAX));
	if (ImGui::Begin(ICON_FA_COG " Style Editor", &IsShow))
	{
		ImGuiStyle& style = ImGui::GetStyle();

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

		if (ImGui::Button("Reset"))
		{
			ResetStyle();
		}

		ECommonComponent::ShowStyleSelector("Colors##Selector");
		ECommonComponent::ShowFontSelector("Fonts##Selector");

		if (ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f"))
			style.GrabRounding = style.FrameRounding;
		{ bool border = (style.WindowBorderSize > 0.0f); if (ImGui::Checkbox("WindowBorder", &border)) { style.WindowBorderSize = border ? 1.0f : 0.0f; } }
		ImGui::SameLine();
		{ bool border = (style.FrameBorderSize > 0.0f);  if (ImGui::Checkbox("FrameBorder", &border)) { style.FrameBorderSize = border ? 1.0f : 0.0f; } }
		ImGui::SameLine();
		{ bool border = (style.PopupBorderSize > 0.0f);  if (ImGui::Checkbox("PopupBorder", &border)) { style.PopupBorderSize = border ? 1.0f : 0.0f; } }

		ImGui::Separator();

		if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Sizes"))
			{
				ImGui::Text("Main");
				ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("CellPadding", (float*)&style.CellPadding, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
				ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
				ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
				ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
				ImGui::Text("Borders");
				ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::Text("Rounding");
				ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("LogSliderDeadzone", &style.LogSliderDeadzone, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
				ImGui::Text("Alignment");
				ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
				int window_menu_button_position = style.WindowMenuButtonPosition + 1;
				if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
					style.WindowMenuButtonPosition = window_menu_button_position - 1;
				ImGui::Combo("ColorButtonPosition", (int*)&style.ColorButtonPosition, "Left\0Right\0");
				ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine(); ECommonComponent::HelpMarker("Alignment applies when a button is larger than its text content.");
				ImGui::SliderFloat2("SelectableTextAlign", (float*)&style.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine(); ECommonComponent::HelpMarker("Alignment applies when a selectable is larger than its text content.");
				ImGui::Text("Safe Area Padding");
				ImGui::SameLine(); ECommonComponent::HelpMarker("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");
				ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Colors"))
			{
				static ImGuiColorEditFlags alpha_flags = 0;
				if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None)) { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
				if (ImGui::RadioButton("Alpha", alpha_flags == ImGuiColorEditFlags_AlphaPreview)) { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
				if (ImGui::RadioButton("Both", alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();
				ECommonComponent::HelpMarker(
					"In the color list:\n"
					"Left-click on color square to open color picker,\n"
					"Right-click to open edit options menu.");

				ImGui::BeginChild("##colors", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
				ImGui::PushItemWidth(-160);
				for (int i = 0; i < ImGuiCol_COUNT; i++)
				{
					const char* name = ImGui::GetStyleColorName(i);
					ImGui::PushID(i);
					ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
					ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
					ImGui::TextUnformatted(name);
					ImGui::PopID();
				}
				ImGui::PopItemWidth();
				ImGui::EndChild();

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

		ImGui::PopItemWidth();
	}
	ImGui::End();
	//关闭时保存
	if (!IsShow)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		File::WriteAllBytes(Resource::GetFullSavePath(SetterFilePath), (byte*)&style, sizeof(ImGuiStyle));
	}
}
DC_END_NAMESPACE