#include "EInspector_Project_Text.h"
#include "runtime/resources/AssetsManager.h"
#include "runtime/resources/Resources.h"
#include "editor/EditorAppliction.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
void EInspector_Project_Text::OnEnter()
{
	_content = File::ReadAllText(Resource::GetFullDataPath(EditorAppliction::GetInspectorName()));
}
void EInspector_Project_Text::Render(AssetMeta* meta)
{
	DC_PROFILE_FUNCTION;
	base::Render(meta);

	if (!meta)return;
	
	ImGui::TextWrapped("%s",_content.c_str());
}
DC_END_NAMESPACE