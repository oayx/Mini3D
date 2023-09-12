#include "EditorConfig.h"
#include "runtime/resources/Resources.h"

DC_BEGIN_NAMESPACE
static const String EditorFilePath = "data/settings/EditorConfig.asset";
/********************************************************************/	
bool  EditorConfig::IsLocal = true;
bool  EditorConfig::ShowGrid = true;
bool  EditorConfig::Is3D = true;
float EditorConfig::OperatorSpeed = 0.6f;
int   EditorConfig::LayoutIndex = 0;
float EditorConfig::ProjectIconZoom = 1.0f;
GameViewResolution EditorConfig::GameResolution = GameViewResolution::Free;
void EditorConfig::Load()
{
	String full_path = Resource::GetFullSavePath(EditorFilePath);
	if (File::Exist(full_path))
	{
		SerializeRead transfer(full_path);
		{
			TRANSFER_SIMPLE(IsLocal);
			TRANSFER_SIMPLE(ShowGrid);
			TRANSFER_SIMPLE(Is3D);
			TRANSFER_SIMPLE(OperatorSpeed);
			TRANSFER_SIMPLE(LayoutIndex);
			TRANSFER_SIMPLE(ProjectIconZoom);
			TRANSFER_ENUM(GameResolution);
		}
	}
}
void EditorConfig::Save()
{
	String full_path = Resource::GetFullSavePath(EditorFilePath);
	SerializeWrite transfer(full_path);
	{
		TRANSFER_SIMPLE(IsLocal);
		TRANSFER_SIMPLE(ShowGrid);
		TRANSFER_SIMPLE(Is3D);
		TRANSFER_SIMPLE(OperatorSpeed);
		TRANSFER_SIMPLE(LayoutIndex);
		TRANSFER_SIMPLE(ProjectIconZoom);
		TRANSFER_ENUM(GameResolution);
	}
}
DC_END_NAMESPACE