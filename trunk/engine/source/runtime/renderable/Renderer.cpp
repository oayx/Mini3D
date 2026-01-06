#include "Renderer.h" 
#include "runtime/component/GameObject.h"
#include "runtime/component/Component.inl"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/null/HardwareBuffer.h"
#include "runtime/graphics/null/CGProgram.h"
#include "runtime/ui/GUI.h"
#include "runtime/resources/AssetsManager.h"
#include "runtime/scene/Light.h"
#include "runtime/Application.h"
#include "editor/common/EAssetSelect.h"
#include "editor/main/EMain_Project.h"
#include "editor/main/inspector/EInspector_Project_Material.h"
#include "editor/EditorAppliction.h"
#include "editor/EditorCursor.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Renderer, Component);
Renderer::Renderer()
{
	//默认材质
	Material* mat = Material::GetDefault();
	mat->Retain();
	_materials.Add(mat);
}
Renderer::~Renderer()
{
	for (auto mat : _materials)
	{
		SAFE_RELEASE(mat);
	}
	_materials.Clear();

	for (auto prim : _primitives)
	{
		SAFE_DELETE(prim);
	}
	_primitives.Clear();
}
Object* Renderer::Clone(Object* newObj)
{
	Renderer* obj = dynamic_cast<Renderer*>(newObj);
	if (!obj)return newObj;

	obj->SetSortLayer(_sortLayer);
	obj->SetOrderInLayer(_orderInLayer);

	//材质
	for (int i = 0; i < _materials.Size(); ++i)
	{
		Material* material = _materials[i];
		if(material)obj->SetMaterial(material->GetResFile(), i, true);
	}

	return obj;
}
const Matrix4& Renderer::GetWorldMatrix()
{
	return this->GetTransform()->GetLocalToWorldMatrix();
}
void Renderer::SetPrimitiveType(PrimitiveType type) 
{ 
	_primitiveType = type; 
	for (int i = 0; i < _primitives.Size(); ++i)
	{
		Primitive* primitive = _primitives[i];
		primitive->SetPrimitiveType(type);
	}
}
Material* Renderer::SetMaterial(const String& file)
{
	if (_primitives.IsEmpty())
	{
		SetMaterial(file, 0, true);
	}
	else
	{
		for (int i = 0; i < GetPrimitiveCount(); ++i)
		{
			this->SetMaterial(file, i, false);//设置完成后一次性更新数据
		}
		this->UploadData();
	}
	if (_materials.IsEmpty())
		return nullptr;
	else
		return _materials[0];
}
Material* Renderer::SetMaterial(const String& file, int index, bool rebuild)
{
	if (file.IsEmpty())return nullptr;

	Material* mat = Material::Create(file);
	mat->Retain();
	if (index >= (int)_materials.Size())
	{
		_materials.Add(mat);
	}
	else
	{
		SAFE_RELEASE(_materials[index]);
		_materials[index] = mat;
	}

	//设置材质
	Primitive* primitive = GetPrimitive(index);
	primitive->SetMaterial(mat);

	//修改了材质，重新绑定数据，主要是shader改变后顶点输入语义需要重新设定
	if (rebuild)
	{
		this->UploadData();
	}
	return mat;
}
Material* Renderer::GetMaterial(int index)const
{ 
	if (_materials.IsEmpty())
		return nullptr;
	return _materials[index]; 
}
CGProgram* Renderer::GetProgram(int index)const
{
	Material* mat = GetMaterial(index);
	if (mat == nullptr)return nullptr;
	return mat->GetProgram();
}
Primitive* Renderer::GetPrimitive(int index)
{
	CHECK_RETURN_PTR_NULL(index <= _primitives.Size());

	if (index >= _primitives.Size())
	{
		for (int i = _primitives.Size(); i <= index; ++i)
		{
			Primitive* primitive = Primitive::Create(this, _primitiveType, _vertexLayout);
			_primitives.Add(primitive);

			Material* material = nullptr;
			if (!_materials.IsEmpty())
			{
				if (i >= (int)_materials.Size())
					material = _materials[0];
				else
					material = _materials[i];
			}
			primitive->SetMaterial(material);
		}
	}
	Primitive* prim = _primitives[index];
	return prim;
}
void Renderer::CalLightes()
{
	uint objMask = LayerMask::GetMask(GetGameObject()->GetLayer());
	for (auto material : _materials)
	{
		List<Light*> lightes;
		if(material->GetShader()->EnableLight())
		{
			int count = 0;
			int lightCount = SceneManager::GetLightCount();
			for (int i = 0; i < lightCount; ++i)
			{//先收集方向光
				Light* light = SceneManager::GetLight(i);
				if (light == nullptr || !light->IsEnable() || !light->GetGameObject()->ActiveInHierarchy() || light->mType != LightType::Direction || count >= MAX_DIRECTION_LIGHT)
					continue;

				if (!(objMask & light->GetCullMask()))
					continue;

				lightes.Add(light);
				count++;
			}

			count = 0;
			for (int i = 0; i < lightCount; ++i)
			{//再收集点光源
				Light* light = SceneManager::GetLight(i);
				if (light == nullptr || !light->IsEnable() || !light->GetGameObject()->ActiveInHierarchy() || light->mType != LightType::Point || count >= MAX_POINT_LIGHT)
					continue;

				if (!(objMask & light->GetCullMask()))
					continue;

				//判断点光源形成的球体是否与物体包围盒相交
				Sphere sphere(light->GetTransform()->GetPosition(), light->Range);
				const Aabb& box = GetTransform()->GetBoundingBox();
				if (box.Intersects(sphere) != IntersectResult::Outside)
				{
					lightes.Add(light);
					count++;
				}
			}

			count = 0;
			for (int i = 0; i < lightCount; ++i)
			{//再收集聚光灯
				Light* light = SceneManager::GetLight(i);
				if (light == nullptr || !light->IsEnable() || light->mType != LightType::Spot || count >= MAX_SPOT_LIGHT)
					continue;

				if (!(objMask & light->GetCullMask()))
					continue;

				//判断光源形成的球体是否与物体包围盒相交
				Sphere sphere(light->GetTransform()->GetPosition(), light->Range);
				const Aabb& box = GetTransform()->GetBoundingBox();
				if (box.Intersects(sphere) != IntersectResult::Outside)
				{
					lightes.Add(light);
					count++;
				}

				//TODO:判断方向是否对
			}
		}

		material->SetLightes(lightes);
	}
}
int Renderer::GetRenderQueue(int index)const
{
	Material* mat = GetMaterial(index);
	if (mat == nullptr)
		return (int)eRenderQueue::Geometry;
	return mat->GetRenderQueue();
}
void Renderer::SetRenderQueue(int queue, int index)
{
	Material* mat = GetMaterial(index);
	if (mat == nullptr)return;
	mat->SetRenderQueue(queue);
}
bool Renderer::UploadData()
{
	PrimitiveUploadDesc desc;
	return UploadData(desc);
}
bool Renderer::UploadData(const PrimitiveUploadDesc& desc)
{
	Aabb box;
	for(int i = 0; i < _primitives.Size(); ++i)
	{
		auto primitive = _primitives[i];
		primitive->UploadData(desc);
		box.Merge(primitive->GetBoundingBox());
	}
	this->GetComponent<Transform>()->SetLocalBoundingBox(box);
	return true;
}
void Renderer::ClearData()
{
	for (auto primitive : _primitives)
	{
		primitive->ClearData();
		SAFE_DELETE(primitive);
	}
	_primitives.Clear();
}
void Renderer::AddVertex(const Vector3& pos, int prim)
{
	AssertEx(_vertexLayout == VertexLayout::Variable, "");
	static_cast<VariablePrimitive*>(GetPrimitive(prim))->AddVertex(pos);
}
void Renderer::AddColor(const Color& color, int prim)
{
	AssertEx(_vertexLayout == VertexLayout::Variable, "");
	static_cast<VariablePrimitive*>(GetPrimitive(prim))->AddColor(color);
}
void Renderer::AddNormal(const Vector3& normal, int prim)
{
	AssertEx(_vertexLayout == VertexLayout::Variable, "");
	static_cast<VariablePrimitive*>(GetPrimitive(prim))->AddNormal(normal.Normalize());
}
void Renderer::AddTangent(const Vector3& tangent, int prim)
{
	AssertEx(_vertexLayout == VertexLayout::Variable, "");
	static_cast<VariablePrimitive*>(GetPrimitive(prim))->AddTangent(tangent);
}
void Renderer::AddTexcoord1(const Vector2& texcoord, int prim)
{
	AssertEx(_vertexLayout == VertexLayout::Variable, "");
	static_cast<VariablePrimitive*>(GetPrimitive(prim))->AddTexcoord1(texcoord);
}
void Renderer::AddTexcoord2(const Vector2& texcoord, int prim)
{
	AssertEx(_vertexLayout == VertexLayout::Variable, "");
	static_cast<VariablePrimitive*>(GetPrimitive(prim))->AddTexcoord2(texcoord);
}
void Renderer::SetVertexs(const Vector3v&& vertexs, int prim)
{
	AssertEx(_vertexLayout == VertexLayout::Variable, "");
	static_cast<VariablePrimitive*>(GetPrimitive(prim))->SetVertexs(std::move(vertexs));
}
void Renderer::SetColor(const Color& color, int prim)
{
	AssertEx(_vertexLayout == VertexLayout::Variable, "");
	static_cast<VariablePrimitive*>(GetPrimitive(prim))->SetColor(color);
}
void Renderer::SetColors(const Vector<Color>&& colors, int prim)
{
	AssertEx(_vertexLayout == VertexLayout::Variable, "");
	static_cast<VariablePrimitive*>(GetPrimitive(prim))->SetColors(std::move(colors));
}
void Renderer::SetNormals(const Vector3v&& normals, int prim)
{
	AssertEx(_vertexLayout == VertexLayout::Variable, "");
	static_cast<VariablePrimitive*>(GetPrimitive(prim))->SetNormals(std::move(normals));
}
void Renderer::SetTangents(const Vector3v&& tangents, int prim)
{
	AssertEx(_vertexLayout == VertexLayout::Variable, "");
	static_cast<VariablePrimitive*>(GetPrimitive(prim))->SetTangents(std::move(tangents));
}
void Renderer::SetTexcoords1(const Vector2v&& texcoords, int prim)
{
	AssertEx(_vertexLayout == VertexLayout::Variable, "");
	static_cast<VariablePrimitive*>(GetPrimitive(prim))->SetTexcoords1(std::move(texcoords));
}
void Renderer::SetTexcoords2(const Vector2v&& texcoords, int prim)
{
	AssertEx(_vertexLayout == VertexLayout::Variable, "");
	static_cast<VariablePrimitive*>(GetPrimitive(prim))->SetTexcoords2(std::move(texcoords));
}
void Renderer::SetPointSize(float size, int prim)
{
	GetPrimitive(prim)->SetPointSize(size);
}
void Renderer::AddIndex(uint index, int prim)
{
	GetPrimitive(prim)->AddIndex(index);
}
void Renderer::SetIndices(const Vector<uint>&& indices, int prim)
{
	GetPrimitive(prim)->SetIndices(std::move(indices));
}
void Renderer::SetIndices(uint index_nums, uint *index, int prim)
{
	GetPrimitive(prim)->SetIndices(index_nums, index);
}
void Renderer::AddInstanceTransformRange(const Vector<Matrix4>& mats)
{
	int old_size = _instanceTransform.Size();
	_instanceTransform.Resize(_instanceTransform.Size() + mats.Size());
	Memory::Copy(&_instanceTransform[old_size], mats.Data(), mats.Size() * sizeof(Matrix4));
}
void Renderer::AddInstanceTransformRange(const Matrix4* mats, int size)
{
	int old_size = _instanceTransform.Size();
	_instanceTransform.Resize(_instanceTransform.Size() + size);
	Memory::Copy(&_instanceTransform[old_size], mats, size * sizeof(Matrix4));
}
int Renderer::AddSubPrimitive(uint elem_count, uint vtx_offset, uint idxOffset, uint start_vertex, uint start_index, int prim)
{
	Primitive* primitive = GetPrimitive(prim);
	primitive->AddSubPrimitive(elem_count, vtx_offset, idxOffset, start_vertex, start_index);
	return primitive->GetSubPrimitiveCount() - 1;
}
void Renderer::OnDrawEditor()
{
	base::OnDrawEditor();
	ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::TreeNodeEx("Light", treeFlags))
	{
		OnDrawLight();
		ImGui::TreePop();
	}
	if (ImGui::TreeNodeEx("Material", treeFlags))
	{
		OnDrawMaterial();
		ImGui::TreePop();
	}
	if (ImGui::TreeNodeEx("Shader", treeFlags))
	{
		OnDrawShader();
		ImGui::TreePop();
	}
}
void Renderer::OnDrawMaterial()
{
	Material* material = this->GetMaterial();
	if (!material)return;

	ImGuiEx::Label("File");
	const char* szName = material->GetResName().c_str();
	ImGui::Button(szName, ImVec2(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(ICON_FA_LINK).x - 5, 0));
	if (ImGui::IsItemClicked(0) && material)
	{
		EMain_Project::SetSelectFile(material->GetResFile());
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ProjectAsset"))
		{
			const String& filePath = EditorCursor::GetDragFile();
			if (Resource::GetResourceType(filePath) == ResourceType::Material)
			{
				this->SetMaterial(filePath);
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
			this->SetMaterial(file);
		};

		EAssetSelect::Search(ResourceType::Material, OnSelectAsset);
	}
}
void Renderer::OnDrawShader()
{
	Material* material = this->GetMaterial();
	if (!material)return;
	
	ImVec2 startPos = ImGui::GetCursorScreenPos();
	{
		if (EInspector_Project_Material::RenderMaterial(material, [this] {this->UploadData(); }))
		{
			this->UploadData();
		}
		
		OnDrawOrder();
	}
	bool isInternal = Resource::IsInternal(material->GetResFile());
	if (isInternal)
	{
		ImVec2 end_pos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(ImVec2(startPos.x - 30.0f, startPos.y));
		ImGuiEx::Rect(ImGui::GetContentRegionAvail().x + 50.0f, end_pos.y - startPos.y, 0x10FFFFFF, false);
	}
}
void Renderer::OnDrawLight()
{
	Material* material = this->GetMaterial();
	if (!material)return;

	ImGuiEx::Label("Cast Shadows");
	bool cast_shadows = material->IsCastShadow();
	if (ImGui::Checkbox("##Cast Shadows", &cast_shadows))
	{
		material->SetCastShadow(cast_shadows);
	}

	ImGuiEx::Label("Receive Shadows");
	bool receiveShadows = material->IsReceiveShadow();
	if (ImGui::Checkbox("##Receive Shadows", &receiveShadows))
	{
		material->SetReceiveShadow(receiveShadows);
	}
}
void Renderer::OnDrawOrder()
{
	ImGuiEx::Label("Sort Layer");
	if (ImGui::DragInt("##SortLayer", &_sortLayer, 0.5f, INT_MIN, INT_MAX))
	{
	}

	ImGuiEx::Label("Order In Layer");
	if (ImGui::DragInt("##OrderInLayer", &_orderInLayer, 0.5f, INT_MIN, INT_MAX))
	{
	}
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(Renderer);
template<class TransferFunction> inline
void Renderer::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);
	{
		TRANSFER_SIMPLE(_sortLayer);
		TRANSFER_SIMPLE(_orderInLayer);

		if (transfer.IsRead())
		{
			Vector<String> materials;
			{//先读取包括哪些材质
				if (transfer.Push("Materials"))
				{
					TRANSFER_VECTOR(materials, "Material");
					transfer.Pop();
				}
			}

			for (int i = 0; i < materials.Size(); ++i)
			{
				AssetMeta* meta = AssetsManager::GetMeta(materials[i]);
				if (meta)this->SetMaterial(meta->GetFullPath());
			}
			//单独序列化材质
			for (int i = 0; i < _materials.Size(); ++i)
			{
				Material* material = _materials[i];
				material->Serialize();
			}
		}
		else
		{
			Vector<String> materials; materials.Reserve(_materials.Size());
			for (int i = 0; i < _materials.Size(); ++i)
			{
				Material* material = _materials[i];
				AssetMeta* meta = AssetsManager::GetMetaByFile(material->GetResFile());
				if(meta)materials.Add(meta->GetGUID());
			}
			{//包括哪些材质
				if (transfer.Push("Materials"))
				{
					TRANSFER_VECTOR(materials, "Material");
					transfer.Pop();
				}
			}
			//单独序列化材质
			for (int i = 0; i < _materials.Size(); ++i)
			{
				Material* material = _materials[i];
				material->Deserialize();
			}
		}
	}
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(VirtualRenderer, Renderer);
DC_END_NAMESPACE