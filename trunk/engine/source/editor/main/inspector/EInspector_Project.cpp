#include "EInspector_Project.h"
#include "EInspector_Project_Font.h"
#include "EInspector_Project_Material.h"
#include "EInspector_Project_Mesh.h"
#include "EInspector_Project_Texture.h"
#include "EInspector_Project_Text.h"
#include "runtime/resources/AssetsManager.h"
#include "runtime/Application.h"
#include "editor/main/EMain_Hierarchy.h"
#include "editor/EditorAppliction.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
EInspector_Project::EInspector_Project()
{
	_inspectors[int(ResourceType::Material)] = DBG_NEW EInspector_Project_Material();
	_inspectors[int(ResourceType::Mesh)] = DBG_NEW EInspector_Project_Mesh();
	_inspectors[int(ResourceType::Texture)] = DBG_NEW EInspector_Project_Texture();
	_inspectors[int(ResourceType::Txt)] = DBG_NEW EInspector_Project_Text();
	_inspectors[int(ResourceType::Font)] = DBG_NEW EInspector_Project_Font();
}
EInspector_Project::~EInspector_Project()
{
	if (_currInspector)
	{
		_currInspector->OnLeave();
		_currInspector = nullptr;
	}
	for (int i = 0; i < (int)ResourceType::Max; ++i)
	{
		SAFE_DELETE(_inspectors[i]);
	}
}
void EInspector_Project::Render()
{
	DC_PROFILE_FUNCTION();
	String select_file = EditorAppliction::GetInspectorName();
	if (select_file.IsEmpty())return;

	AssetMeta* meta = AssetsManager::GetMetaByFile(select_file);
	if (!meta)return;

	if (_currAssetFile != select_file)
	{
		if (_currInspector)_currInspector->OnLeave();
		_currAssetFile = select_file;
		_currInspector = _inspectors[int(meta->GetResType())];
		if (_currInspector)_currInspector->OnEnter();
	}
	
	if (_currInspector)
	{
		_currInspector->Render(meta);
	}
}
/********************************************************************/
void EInspector_Project_Base::Render(AssetMeta* meta)
{
	if (meta)
	{
		ImGui::TextUnformatted(meta->GetFileName().c_str());
		ImGui::SameLine();
		ImGui::TextUnformatted("Import Settings");
		ImGui::NewLine();
		ImGui::Separator();
	}
}
DC_END_NAMESPACE