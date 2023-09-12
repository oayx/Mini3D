#include "ParticleRender.h"
#include "ParticleSystem.h"
#include "Particle.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/Pass.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/graphics/TextureUnit.h"
#include "runtime/scene/Camera.h"
#include "runtime/scene/SceneManager.h"
#include "editor/common/EAssetSelect.h"
#include "editor/main/EMain_Project.h"
#include "editor/EditorAppliction.h"
#include "editor/EditorCursor.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(ParticleRender, Renderer);
void ParticleRender::Awake()
{
	base::Awake();
	this->SetMaterial("internal/material/Default-Particle.material");
	_particleSystem = GetGameObject()->GetComponent<ParticleSystem>();
}
void ParticleRender::Start()
{
	base::Start();
	if (_particleSystem && _particleSystem->TextureSheetAnimation.Enabled)
	{
		_currFrame = _particleSystem->TextureSheetAnimation.StartFrame;
		this->CalFrameTime();
	}
}
void ParticleRender::Update()
{
	base::Update();
	if (_particleSystem)
	{
		if (_particleSystem->TextureSheetAnimation.Enabled && Time::GetRealTimeSinceStartup() - _lastFrameTime >= _frameTime)
		{
			_lastFrameTime = Time::GetRealTimeSinceStartup();
			_currFrame++;
			_currFrame = _currFrame % (_particleSystem->TextureSheetAnimation.Cols * _particleSystem->TextureSheetAnimation.Rows);
		}
	}
}
void ParticleRender::OnPreRender(Camera* camera)
{
	base::OnPreRender(camera);
	this->Refresh(camera);
}
void ParticleRender::OnAddComponent(Component* com)
{
	base::OnAddComponent(com);
	if (com && com->Is<ParticleSystem>())
	{
		_particleSystem = dynamic_cast<ParticleSystem*>(com);
	}
}
void ParticleRender::OnRemoveComponent(Component* com)
{
	base::OnRemoveComponent(com);
	if (com && com->Is<ParticleSystem>())
	{
		_particleSystem = nullptr;
	}
}
void ParticleRender::CalFrameTime()
{
	if (_particleSystem)
	{
		int frame_count = _particleSystem->TextureSheetAnimation.Cols * _particleSystem->TextureSheetAnimation.Rows;
		if (frame_count > 0)
		{
			_frameTime = _particleSystem->Main.StartLifeTime/ frame_count;
			_frameTime /= _particleSystem->TextureSheetAnimation.Cycles;
		}
	}
}
void ParticleRender::Refresh(Camera* camera)
{
	this->ClearData();
	if (camera && _particleSystem)
	{
		VariablePrimitive* primitive = static_cast<VariablePrimitive*>(this->GetPrimitive(0));
		Material* material = this->GetMaterial(0);
		int vxt_offset = 0, idx_offset = 0;
		const List<Particle*>& particles = _particleSystem->_particles;
		for (const auto& particle : particles)
		{
			this->FillMesh(particle, camera, primitive, vxt_offset, idx_offset);
		}
	}
	this->UploadData();
}
bool ParticleRender::FillMesh(Particle* particle, Camera* camera, VariablePrimitive* primitive, int& vxt_offset, int& idx_offset)
{
	if(!_particleSystem)return false;
	//缩放
	Vector3 scale = particle->Scale;
	switch (_particleSystem->Main.ScalingMode)
	{
		case PScalingMode::Local:
		{
			scale = scale * _particleSystem->GetTransform()->GetLocalScale();
			break;
		}
		case PScalingMode::Hierarchy:
		{
			scale = scale * _particleSystem->GetTransform()->GetScale();
			break;
		}
		case PScalingMode::Shape:
		{
			Material* mat = GetMaterial(0);
			if (mat == nullptr)break;
			Texture* texture = mat->GetTexture();
			if (texture == nullptr)break;
			scale = scale * (texture->GetHeight() * 0.005f);//w/100/2
			break;
		}
	}
	//变换矩阵
	Matrix4 mat_world = Matrix4(particle->Position, camera->GetTransform()->GetRotation(), scale);

	//position
	primitive->AddVertex(mat_world.TransformPoint(Vector3(-0.5f, 0.5f, 0)));
	primitive->AddVertex(mat_world.TransformPoint(Vector3(0.5f, 0.5f, 0)));
	primitive->AddVertex(mat_world.TransformPoint(Vector3(0.5f, -0.5f, 0)));
	primitive->AddVertex(mat_world.TransformPoint(Vector3(-0.5f, -0.5f, 0)));

	//uv
	Vector2 uv1 = { 0,0 }, uv2 = { 1,0 }, uv3 = { 1,1 }, uv4 = { 0,1 };
	if (_particleSystem->TextureSheetAnimation.Enabled)
	{
		int row = _currFrame / _particleSystem->TextureSheetAnimation.Cols;
		int col = _currFrame % _particleSystem->TextureSheetAnimation.Cols;
		float u_scale = (float)col / _particleSystem->TextureSheetAnimation.Cols;
		float v_scale = (float)row / _particleSystem->TextureSheetAnimation.Rows;
		float u_tile = 1.0f / _particleSystem->TextureSheetAnimation.Cols;
		float v_tile = 1.0f / _particleSystem->TextureSheetAnimation.Rows;
		uv1 = Vector2(u_scale, v_scale);
		uv2 = Vector2(u_scale + u_tile, v_scale);
		uv3 = Vector2(u_scale + u_tile, v_scale + v_tile);
		uv4 = Vector2(u_scale, v_scale + v_tile);
	}
	primitive->AddTexcoord1(uv1);
	primitive->AddTexcoord1(uv2);
	primitive->AddTexcoord1(uv3);
	primitive->AddTexcoord1(uv4);

	//color
	primitive->AddColor(particle->CurrColor);
	primitive->AddColor(particle->CurrColor);
	primitive->AddColor(particle->CurrColor);
	primitive->AddColor(particle->CurrColor);

	//index
	primitive->AddIndex(vxt_offset + 0);
	primitive->AddIndex(vxt_offset + 1);
	primitive->AddIndex(vxt_offset + 2);
	primitive->AddIndex(vxt_offset + 0);
	primitive->AddIndex(vxt_offset + 2);
	primitive->AddIndex(vxt_offset + 3);

	//submesh
	vxt_offset += 4;
	idx_offset += 6;

	return true;
}
void ParticleRender::OnDrawEditor()
{
	ImGuiEx::Label("Enabled");
	bool enable = this->IsEnable();
	if (ImGui::Checkbox("##Enabled", &enable))
	{
		this->SetEnable(enable);
	}

	ImGuiEx::Label("Material");
	Material* material = this->GetMaterial();
	ImGui::Button(material->GetResName().c_str(), ImVec2(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(ICON_FA_LINK).x - 5, 0));
	if (ImGui::IsItemClicked(0) && !material->GetResName().IsEmpty())
	{
		EMain_Project::SetSelectFile(material->GetResFile());
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ProjectAsset"))
		{
			const String& file_path = EditorCursor::GetDragFile();
			if (Resource::GetResourceType(file_path) == ResourceType::Material)
			{
				material = this->SetMaterial(file_path);
				if (material)
				{
					material->SetCastShadow(false);
					material->SetReceiveShadow(false);
				}
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
			Material* material = this->SetMaterial(file);
			if (material)
			{
				material->SetCastShadow(false);
				material->SetReceiveShadow(false);
			}
		};

		EAssetSelect::Search(ResourceType::Material, OnSelectAsset);
	}

	{
		ImGui::NewLine();

		ImGuiEx::Label("Sort Layer");
		int sort_layer = this->GetSortLayer();
		if (ImGui::DragInt("##SortLayer", &sort_layer, 0.5f, INT_MIN, INT_MAX))
		{
			this->SetSortLayer(sort_layer);
		}

		ImGuiEx::Label("Order In Layer");
		int order_in_layer = this->GetOrderInLayer();
		if (ImGui::DragInt("##OrderInLayer", &order_in_layer, 0.5f, INT_MIN, INT_MAX))
		{
			this->SetOrderInLayer(order_in_layer);
		}
	}
}
DC_END_NAMESPACE
