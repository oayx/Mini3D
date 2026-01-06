#include "EConsole.h"
#include "core/stream/DataStream.h"
#include "runtime/ui/GUI.h"
#include "editor/EditorDefine.h"
#include "editor/common/ECommonComponent.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
void EConsole::Render()
{
	DC_PROFILE_FUNCTION;
	if (!IsShow)return;
	ImGui::SetNextWindowSizeConstraints(ImVec2(400.0f, 400.0f), ImVec2(FLT_MAX, FLT_MAX));
	if (ImGui::Begin(ICON_FA_EDIT " Console", &IsShow, ImGuiWindowFlags_NoCollapse))
	{
		uint logMask = 0;
		//工具栏
		{
			const char* labels[] = { "Info", "Warning", "Error" };
			static bool select[] = { true, true, true };
			for (uint i = 0; i < ARRAY_SIZE(labels); ++i)
			{
				if (i > 0) ImGui::SameLine();
				if (ImGui::Checkbox(labels[i], &select[i]))
				{
				}
			}
			if (select[0])
			{
				logMask |= 1 << (int)LogMsgType::Debug;
				logMask |= 1 << (int)LogMsgType::Log;
			}
			if (select[1])
			{
				logMask |= 1 << (int)LogMsgType::Warning;
			}
			if (select[2])
			{
				logMask |= 1 << (int)LogMsgType::Error;
				logMask |= 1 << (int)LogMsgType::Exception;
			}

			//ImGui::SameLine();
			//ImGui::Spacing();

			//static bool auto_scroll = true;
			//ImGui::SameLine();
			//if (ImGui::Checkbox("AutoScroll", &auto_scroll))
			//{
			//}

			ImGui::SameLine();
			if (ImGui::Button("Clear"))
			{
				_allLogs.Clear();
			}
		}

		static int selectLogIndex = -1;

		ImGui::BeginChild("Logs", ImVec2(0, 0), true);
		{
			float height = ImGui::GetContentRegionAvail().y;
			//日志列表
			ImGui::BeginChild("LogList", ImVec2(0, height * 0.5f));
			{
				if (ImGui::BeginPopupContextWindow())
				{
					if (ImGui::Selectable("Clear")) {}
					ImGui::EndPopup();
				}

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
				for (int i = 0; i < _allLogs.Size(); i++)
				{
					ImGui::PushID(i);

					const std::pair<LogMsgType, std::string>& msg = _allLogs[i];
					if (logMask & (1 << (int)msg.first))
					{
						ImVec4 color(1.0f, 0.0f, 0.0f, 1.0f);
						bool hasColor = false;
						const char* label = NO_ICON;
						switch (msg.first)
						{
						case LogMsgType::Debug:
						case LogMsgType::Log:
							label = ICON_FA_INFO_CIRCLE;
							break;
						case LogMsgType::Warning:
						{
							color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
							hasColor = true;
							label = ICON_FA_QUESTION_CIRCLE;
							break;
						}
						case LogMsgType::Error:
						case LogMsgType::Exception:
						{
							color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
							hasColor = true;
							label = ICON_FA_TIMES_CIRCLE;
							break;
						}
						default:break;
						}
						if (hasColor)
							ImGui::PushStyleColor(ImGuiCol_Text, color);

						if (ImGui::Selectable(label))
							selectLogIndex = i;
						if (hasColor)
							ImGui::PopStyleColor();

						ImGui::SameLine();
						ImGui::Selectable(msg.second.c_str());
					}
					ImGui::PopID();
				}
				ImGui::PopStyleVar();
			}
			ImGui::EndChild();

			//单条详情
			ImGui::Separator();
			ImGui::BeginChild("LogDetail", ImVec2(0, height * 0.5f - 10.0f));
			{
				if (selectLogIndex >= 0 && selectLogIndex < _allLogs.Size())
				{
					ImGui::TextWrapped("%s", _allLogs[selectLogIndex].second.c_str());
				}
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();
	}
	ImGui::End();
}
DC_END_NAMESPACE