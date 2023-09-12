 
/*****************************************************************************
* Author： hannibal
* Date：2021/8/9
* Description：日志显示
*****************************************************************************/
#pragma once

#include "editor/EWindowBase.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class EConsole : public EWindowBase
{
	friend class EMain_Menu;

public:
	static void Render();

	static void AddLogMessage(LogMsgType type, const char* msg) { _allLogs.Add(std::pair(type, std::string(msg))); }

private:
	static bool IsShow;
	static Vector<std::pair<LogMsgType, std::string>> _allLogs;
};
DC_END_NAMESPACE