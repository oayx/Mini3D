#include "EditorConfig.h"
#include "runtime/resources/Resources.h"

DC_BEGIN_NAMESPACE
static const String EditorFilePath = "data/settings/EditorConfig.asset";
/********************************************************************/
void EditorConfig::Load()
{
	String fullPath = Resource::GetFullSavePath(EditorFilePath);
	if (File::Exist(fullPath))
	{
		SerializeRead transfer(fullPath);
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
	String fullPath = Resource::GetFullSavePath(EditorFilePath);
	SerializeWrite transfer(fullPath);
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