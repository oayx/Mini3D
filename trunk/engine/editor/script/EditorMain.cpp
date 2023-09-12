#include "EditorMain.h"
#include "editor/EditorAppliction.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
bool EditorMain::Start()
{
	Application::SetPlay(false);
	Application::SetPause(false);
	return true;
}

void EditorMain::Release()
{
}
DC_END_NAMESPACE