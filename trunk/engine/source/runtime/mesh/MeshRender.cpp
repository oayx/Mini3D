#include "MeshRender.h"
#include "Mesh.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/null/CGProgram.h"
#include "runtime/graphics/null/GraphicsDevice.h"
#include "runtime/physics/Collider.h"
#include "runtime/resources/AssetsManager.h"
#include "editor/common/EAssetSelect.h"
#include "editor/main/EMain_Project.h"
#include "editor/EditorAppliction.h"
#include "editor/EditorCursor.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(MeshRender, Renderer);
MeshRender::~MeshRender()
{
	SAFE_RELEASE(_mesh);
}
Object* MeshRender::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	MeshRender* obj = dynamic_cast<MeshRender*>(new_obj);
	if (!obj)return new_obj;

	obj->SetMesh(_mesh);

	return obj;
}
void MeshRender::SetMesh(Mesh* mesh)
{
	SAFE_RELEASE(_mesh);
	_mesh = mesh;
	if(_mesh != nullptr)_mesh->Retain();

	this->ClearData();
	if (_mesh != nullptr)
	{
		for (int mesh_index = 0; mesh_index < _mesh->_entities.Size(); ++mesh_index)
		{
			MeshEntity* entity = _mesh->_entities[mesh_index];
			VariablePrimitive* prim = dynamic_cast<VariablePrimitive*>(GetPrimitive(mesh_index));
			if (entity->Vertexs.Size() > 0)		prim->AddVertexRange(entity->Vertexs);
			if (entity->Colors.Size() > 0)		prim->AddColorRange(entity->Colors);
			if (entity->Normals.Size() > 0)		prim->AddNormasRange(entity->Normals);
			if (entity->Tangents.Size() > 0)	prim->AddTangentRange(entity->Tangents);
			if (entity->TexCoords1.Size() > 0)	prim->AddTexcoord1Range(entity->TexCoords1);
			if (entity->TexCoords2.Size() > 0)	prim->AddTexcoord2Range(entity->TexCoords2);
			if (entity->Weights.Size() > 0)		prim->AddWeightRange(entity->Weights);
			if (entity->Indexes.Size() > 0)		prim->AddIndexRange(entity->Indexes);
		}
		this->UploadData();
	}
}
bool MeshRender::CanRemove()
{
	return !GetGameObject()->GetComponent<MeshCollider>();
}
void MeshRender::OnDrawEditor()
{
	ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
	if(ImGui::TreeNodeEx("Mesh", tree_flags))
	{
		ImGuiEx::Label("File");
		int select = -1;
		const char* sz_name = _mesh ? _mesh->GetResName().c_str() : "##Empty mesh";
		ImGui::Button(sz_name, ImVec2(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(ICON_FA_LINK).x - 5, 0));
		if (ImGui::IsItemClicked(0) && _mesh)
		{
			EMain_Project::SetSelectFile(_mesh->GetResFile());
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ProjectAsset"))
			{
				const String& file_path = EditorCursor::GetDragFile();
				if (Resource::GetResourceType(file_path) == ResourceType::Mesh)
				{
					this->SetMesh(Mesh::Create(file_path));
				}
				EditorCursor::EndDrag();
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::SameLine();
		if (ImGui::Selectable(ICON_FA_LINK, false, 0, ImGui::CalcTextSize(ICON_FA_LINK)))
		{
			auto OnSelectAsset = [this](const String& file)
			{
				this->SetMesh(Mesh::Create(file));
			};

			EAssetSelect::Search(ResourceType::Mesh, OnSelectAsset);
		}

		ImGui::TreePop();
	}
	base::OnDrawEditor();
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(MeshRender);
template<class TransferFunction> inline
void MeshRender::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	String MeshFile = "";
	if (transfer.IsRead())
	{
		TRANSFER_SIMPLE(MeshFile);
		AssetMeta* meta = AssetsManager::GetMeta(MeshFile);
		if (meta)
		{
			SetMesh(Mesh::Create(meta->GetFullPath()));
		}
	}
	else
	{
		if (_mesh)
		{
			MeshFile = _mesh->GetResFile();
			AssetMeta* meta = AssetsManager::GetMetaByFile(MeshFile);
			AssertEx(meta, "Cannot find meta file:%s", MeshFile.c_str());
			MeshFile = meta->GetGUID();
		}
		TRANSFER_SIMPLE(MeshFile);
	}
}
DC_END_NAMESPACE