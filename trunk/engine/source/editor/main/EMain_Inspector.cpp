#include "EMain_Inspector.h"
#include "EMain_Hierarchy.h"
#include "inspector/EInspector_Hierarchy.h"
#include "inspector/EInspector_Project.h"
#include "inspector/EInspector_ProjectSettings_Layers.h"
#include "inspector/EInspector_ProjectSettings_Physics.h"
#include "inspector/EInspector_ProjectSettings_Quality.h"
#include "runtime/ui/GUI.h"
#include "editor/EditorAppliction.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(EMain_Inspector, EWindowBase);
bool EMain_Inspector::IsShow = true;
InspectorType EMain_Inspector::_inspectorType = InspectorType::None;
EInspector_Base* EMain_Inspector::_inspectors[(int)InspectorType::Max] = { nullptr };
void EMain_Inspector::Initialize()
{
	_inspectors[int(InspectorType::Hierarchy)] = DBG_NEW EInspector_Hierarchy();
	_inspectors[int(InspectorType::Project)] = DBG_NEW EInspector_Project();
	_inspectors[int(InspectorType::ProjectSettings_Layers)] = DBG_NEW EInspector_ProjectSettings_Layers();
	_inspectors[int(InspectorType::ProjectSettings_Physics)] = DBG_NEW EInspector_ProjectSettings_Physics();
	_inspectors[int(InspectorType::ProjectSettings_Quality)] = DBG_NEW EInspector_ProjectSettings_Quality();
}
void EMain_Inspector::Destroy()
{
	for (int i = 0 ; i < (int)InspectorType::Max; ++i)
	{
		SAFE_DELETE(_inspectors[i]);
	}
}
void EMain_Inspector::Render()
{
	DC_PROFILE_FUNCTION();
	if (!IsShow)return;

	ImGui::SetNextWindowSizeConstraints(ImVec2(400.0f, 400.0f), ImVec2(FLT_MAX, FLT_MAX));
	ImGui::Begin(ICON_FA_INFO_CIRCLE" Inspector", &IsShow, ImGuiWindowFlags_NoCollapse);

	if (_inspectorType != EditorAppliction::GetInspectorType())
	{
		if(_inspectorType != InspectorType::None)_inspectors[int(_inspectorType)]->OnLeave();
		_inspectorType = EditorAppliction::GetInspectorType();
		if (_inspectorType != InspectorType::None)_inspectors[int(_inspectorType)]->OnEnter();
	}

	if (_inspectorType != InspectorType::None)_inspectors[int(_inspectorType)]->Render();

	ImGui::End();
}
DC_END_NAMESPACE