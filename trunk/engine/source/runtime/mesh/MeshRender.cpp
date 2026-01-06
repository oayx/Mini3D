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
Object* MeshRender::Clone(Object* newObj)
{
	base::Clone(newObj);
	MeshRender* obj = dynamic_cast<MeshRender*>(newObj);
	if (!obj)return newObj;

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
		for (int meshIndex = 0; meshIndex < _mesh->_entities.Size(); ++meshIndex)
		{
			MeshEntity* entity = _mesh->_entities[meshIndex];
			VariablePrimitive* prim = dynamic_cast<VariablePrimitive*>(GetPrimitive(meshIndex));
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
	ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
	if(ImGui::TreeNodeEx("Mesh", treeFlags))
	{
		ImGuiEx::Label("File");
		const char* szName = _mesh ? _mesh->GetResName().c_str() : "##Empty mesh";
		ImGui::Button(szName, ImVec2(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(ICON_FA_LINK).x - 5, 0));
		if (ImGui::IsItemClicked(0) && _mesh)
		{
			EMain_Project::SetSelectFile(_mesh->GetResFile());
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ProjectAsset"))
			{
				const String& filePath = EditorCursor::GetDragFile();
				if (Resource::GetResourceType(filePath) == ResourceType::Mesh)
				{
					this->SetMesh(Mesh::Create(filePath));
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