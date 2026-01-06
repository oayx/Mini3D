#include "ParticleSystem.h"
#include "ParticleEmission.h"
#include "ParticleShape.h"
#include "ParticleRender.h"
#include "Particle.h"
#include "core/time/Timer.h"
#include "runtime/graphics/Material.h"
#include "runtime/resources/AssetsManager.h"
#include "editor/common/EAssetSelect.h"
#include "editor/common/ECommonComponent.h"
#include "editor/main/EMain_Project.h"
#include "editor/EditorAppliction.h"
#include "editor/EditorCursor.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(ParticleSystem, Component);
ParticleSystem::ParticleSystem()
{
	_particleShape = Memory::New<ParticleShape>();
	_particleEmission = Memory::New<ParticleEmission>();
	_particleShape->SetOwner(this);
	_particleEmission->SetOwner(this);
}
ParticleSystem::~ParticleSystem()
{
	Stop();
	SAFE_DELETE(_particleShape);
	SAFE_DELETE(_particleEmission);
}
Object* ParticleSystem::Clone(Object* newObj)
{
	base::Clone(newObj);
	ParticleSystem* obj = dynamic_cast<ParticleSystem*>(newObj);
	if (!obj)return newObj;

	obj->Main = this->Main;
	obj->Emission = this->Emission;
	obj->Shape = this->Shape;
	obj->VelocityOverLifetime = this->VelocityOverLifetime;
	obj->LimitVelocityOverLifetime = this->LimitVelocityOverLifetime;
	obj->ForceOverLifetime = this->ForceOverLifetime;
	obj->ColorOverLifetime = this->ColorOverLifetime;
	obj->SizeOverLifetime = this->SizeOverLifetime;
	obj->TextureSheetAnimation = this->TextureSheetAnimation;

	if (_particleRender && obj->_particleRender)
	{
		_particleRender->Clone(obj->_particleRender);
	}

	return obj;
}
void ParticleSystem::Awake()
{
	base::Awake();
	_particleRender = this->GetGameObject()->AddComponent<ParticleRender>();
}
void ParticleSystem::Start()
{
	base::Start();
	if (!_isActive && this->Main.PlayOnAwake)
	{
		if (this->Main.StartDelay <= 0)
		{
			this->Play();
		}
		else
		{
			Timer::AddOnce(this->Main.StartDelay, CALLBACK_0(ParticleSystem::Play, this));
		}
	}
}
void ParticleSystem::Update()
{
	base::Update();
	//时间缩放
	float dt = this->Main.UseUnscaledTime ? Time::GetUnscaledDeltaTime() : Time::GetDeltaTime();
	Simulate(dt);
}
void ParticleSystem::OnDestroy()
{
	base::OnDestroy();
	if (_particleRender)
	{
		GameObject::Destroy(_particleRender);
		_particleRender = nullptr;
	}
}
void ParticleSystem::OnTransformChanged()
{
	base::OnTransformChanged();

	if (!_isPlaying)return;
	if (this->Main.Space == SimulationSpace::Local)
	{
		//求位置偏差
		Vector3 currentPos = this->GetTransform()->GetPosition();
		Vector3 offsetPos = currentPos - _oldPosition;
		if (offsetPos.SquareSize() > 0)
		{
			for (auto p : _particles)
			{
				p->Position += offsetPos;
			}
			_oldPosition = currentPos;
		}
	}
}
void ParticleSystem::Play()
{
	if (_isPlaying)return;

	_isActive = true;
	_isPlaying = true;
	_lifeTime = this->Main.Duration;
	_oldPosition = this->GetTransform()->GetPosition();
	_random.SetSeed(this->Main.RandomSeed == 0 ? RandomRange(1, INT_MAX) : this->Main.RandomSeed);
	for (auto p : _particles)
	{
		p->Play();
	}
}
void ParticleSystem::Pause()
{
	if (!_isPlaying)return;

	_isPlaying = false;
	for (auto& p : _particles)
	{
		p->Pause();
	}
}
void ParticleSystem::Stop()
{
	_isActive = false;
	_isPlaying = false;
	for (auto& p : _particles)
	{
		p->Stop();
	}
	_particles.Clear();

	//结束操作
	switch (this->Main.StopAction)
	{
	case PStopAction::Disable:
		this->GetGameObject()->SetActive(false);
		break;
	case PStopAction::Destroy:
		GameObject::Destroy(this->GetGameObject());
		break;
	case PStopAction::Callback:
		if (this->Main.StopCallback != nullptr)this->Main.StopCallback();
		break;
	}
}
void ParticleSystem::Simulate(float dt)
{
	if (!_isActive)
		return;

	//先删除过期粒子
	auto it = _particles.begin();
	while (it != _particles.end())
	{
		Particle* p = *it;
		if (!p->IsActive)
		{
			_particles.Remove(it++);
			ParticlePools::Despawn(p);
		}
		else
			++it;
	}

	//时间
	float delta_time = dt * this->Main.SimulationSpeed;

	//更新发射器
	if (_isPlaying)
	{
		_particleEmission->Simulate(delta_time);
	}
	//更新粒子
	for (auto& p : _particles)
	{
		p->Simulate(delta_time);
	}

	//判断是否到期
	if (_isPlaying && !this->Main.Loop)
	{
		_lifeTime -= delta_time;
		if (_lifeTime <= 0)
		{
			_isPlaying = false;
		}
	}

	//粒子全部结束的操作
	if (!_isPlaying && _lifeTime <= 0 && _particles.Size() == 0)
	{
		this->Stop();
	}
}
void ParticleSystem::Emit(int count)
{
	for (int i = 0; i < count && (int)_particles.Size() < this->Main.MaxParticles; ++i)
	{
		Particle* particle = ParticlePools::Spawn();
		particle->Start(this);
		_particles.Add(particle);
		_particleShape->Emit(particle);
	}
}
int ParticleSystem::RandomRange(int min, int max)
{
	return _random.RandomRange(min, max);
}
float ParticleSystem::RandomRange(float min, float max)
{
	return _random.RandomRange(min, max);
}
void ParticleSystem::OnDrawEditor()
{
	base::OnDrawEditor();
	if(ImGui::TreeNodeEx("Base", ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGuiEx::Label("Duration");
		if (ImGui::DragFloat("##Duration", &Main.Duration, 0.01f, 0.0f, MAX_float, "%.3f"))
		{
			if (Main.Duration < 0.0f)Main.Duration = 0.0f;
		}

		ImGuiEx::Label("Loop");
		if (ImGui::Checkbox("##Loop", &Main.Loop))
		{
		}

		ImGuiEx::Label("StartDelay");
		if (ImGui::DragFloat("##StartDelay", &Main.StartDelay, 0.01f, 0.0f, MAX_float, "%.3f"))
		{
		}

		ImGuiEx::Label("StartLifeTime");
		if (ImGui::DragFloat("##StartLifeTime", &Main.StartLifeTime, 0.01f, 0.0001f, MAX_float, "%.3f"))
		{
		}

		ImGuiEx::Label("StartSpeed");
		if (ImGui::DragFloat("##StartSpeed", &Main.StartSpeed, 0.01f, MIN_float, MAX_float, "%.3f"))
		{
		}

		ImGuiEx::Label("StartSize");
		if (ImGui::DragFloat3("##StartSize", (float*)Main.StartSize.p, 0.01f, 0.0f, MAX_float))
		{
		}

		ImGuiEx::Label("StartRotation");
		if (ImGui::DragFloat3("##StartRotation", (float*)Main.StartRotation.p, 0.01f,MIN_float, MAX_float))
		{
		}

		ImGuiEx::Label("StartColor");
		if (ECommonComponent::ShowColor("##StartColor", (float*)Main.StartColor.p, true))
		{
		}

		ImGuiEx::Label("GravityModifier");
		if (ImGui::DragFloat("##GravityModifier", &Main.GravityModifier, 0.01f, MIN_float, MAX_float, "%.3f"))
		{
		}

		ImGuiEx::Label("SimulationSpeed");
		if (ImGui::DragFloat("##SimulationSpeed", &Main.SimulationSpeed, 0.01f, 0, MAX_float, "%.3f"))
		{
		}

		ImGuiEx::Label("UseUnscaledTime");
		if (ImGui::Checkbox("##UseUnscaledTime", &Main.UseUnscaledTime))
		{
		}

		ImGuiEx::Label("PlayOnAwake");
		if (ImGui::Checkbox("##PlayOnAwake", &Main.PlayOnAwake))
		{
		}

		ImGuiEx::Label("MaxParticles");
		if (ImGui::DragInt("##MaxParticles", &Main.MaxParticles, 1, 0, INT_MAX))
		{
		}
		
		ImGuiEx::Label("RandomSeed");
		if (Main.RandomSeed > 0)ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() - 60.0f);
		if (ImGui::DragInt("##RandomSeed", &Main.RandomSeed, 1, 0, INT_MAX))
		{
		}
		if (Main.RandomSeed > 0)
		{
			ImGui::SameLine();
			if (ImGui::SmallButton("Reseed"))
			{
				Main.RandomSeed = RandomRange(1, INT_MAX);
			}
		}

		{
			const char* szFlags[] = { "Local", "World" };
			ImGuiEx::Label("Simulation Space");
			static int currentFlags = (int)Main.Space;
			if (ImGui::Combo("##Space", &currentFlags, szFlags, ARRAY_SIZE(szFlags)))
			{
				Main.Space = SimulationSpace(currentFlags);
			}
		}

		{
			const char* szFlags[] = { "Hierarchy", "Local", "Shape" };
			ImGuiEx::Label("Scaling Mode");
			static int currentFlags = (int)Main.ScalingMode;
			if (ImGui::Combo("##Scaling Mode", &currentFlags, szFlags, ARRAY_SIZE(szFlags)))
			{
				Main.ScalingMode = PScalingMode(currentFlags);
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNodeEx("Emission", ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		ImGuiEx::Label("Enabled");
		if (ImGui::Checkbox("##Enabled", &Emission.Enabled))
		{
		}

		ImGuiEx::Label("RateOverTime");
		if (ImGui::DragInt("##RateOverTime", &Emission.RateOverTime, 1, 0, INT_MAX))
		{
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNodeEx("Shape", ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		ImGuiEx::Label("Enabled");
		if (ImGui::Checkbox("##Enabled", &Shape.Enabled))
		{
		}
		{
			const char* szFlags[] = { "Sphere", "SphereShell", "HemiSphere", "HemiSphereShell", "Cone", "Box", "BoxEdge", "Circle", "CircleEdge" };
			ImGuiEx::Label("Type");
			static int currentFlags = (int)Shape.Type;
			if (ImGui::Combo("##ShapeType", &currentFlags, szFlags, ARRAY_SIZE(szFlags)))
			{
				Shape.Type = PShapeType(currentFlags);
			}
		}
		if (Shape.Type == PShapeType::Sphere || Shape.Type == PShapeType::SphereShell || Shape.Type == PShapeType::HemiSphere
			|| Shape.Type == PShapeType::HemiSphereShell || Shape.Type == PShapeType::Circle || Shape.Type == PShapeType::CircleEdge)
		{
			ImGuiEx::Label("Radius");
			if (ImGui::DragFloat("##Radius", &Shape.Radius, 0.01f, 0, MAX_float, "%.3f"))
			{
			}
		}
		if (Shape.Type == PShapeType::Cone)
		{
			ImGuiEx::Label("Degree");
			if (ImGui::DragFloat("##Degree", &Shape.Degree, 0.01f, 0.0f, 180.0f, "%.3f"))
			{
			}
		}
		ImGuiEx::Label("Position");
		if (ImGui::DragFloat3("##Position", (float*)Shape.Position.p, 0.01f, MIN_float, MAX_float))
		{
		}
		ImGuiEx::Label("Rotation");
		if (ImGui::DragFloat3("##Rotation", (float*)Shape.Rotation.p, 0.01f, MIN_float, MAX_float))
		{
		}
		ImGuiEx::Label("Scale");
		if (ImGui::DragFloat3("##Scale", (float*)Shape.Scale.p, 0.01f, MIN_float, MAX_float))
		{
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNodeEx("Velocity Over Lifetime", ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		ImGuiEx::Label("Enabled");
		if (ImGui::Checkbox("##Enabled", &VelocityOverLifetime.Enabled))
		{
		}

		ImGuiEx::Label("Velocity");
		if (ImGui::DragFloat3("##Velocity", VelocityOverLifetime.Velocity.p, 0.01f, MIN_float, MAX_float))
		{
		}

		ImGuiEx::Label("SpeedModifier");
		if (ImGui::DragFloat("##SpeedModifier", &VelocityOverLifetime.SpeedModifier, 0.01f, MIN_float, MAX_float, "%.3f"))
		{
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNodeEx("Limit Velocity Over Lifetime", ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		ImGuiEx::Label("Enabled");
		if (ImGui::Checkbox("##Enabled", &LimitVelocityOverLifetime.Enabled))
		{
		}

		ImGuiEx::Label("Speed");
		if (ImGui::DragFloat("##Speed", &LimitVelocityOverLifetime.Speed, 0.01f, MIN_float, MAX_float, "%.3f"))
		{
		}

		ImGuiEx::Label("Dampen");
		if (ImGui::DragFloat("##Dampen", &LimitVelocityOverLifetime.Dampen, 0.01f, 0.0f, 1.0f, "%.3f"))
		{
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNodeEx("Force Over Lifetime", ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		ImGuiEx::Label("Enabled");
		if (ImGui::Checkbox("##Enabled", &ForceOverLifetime.Enabled))
		{
		}

		ImGuiEx::Label("Force");
		if (ImGui::DragFloat3("##Force", ForceOverLifetime.Force.p, 0.01f, MIN_float, MAX_float))
		{
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNodeEx("Color Over Lifetime", ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		ImGuiEx::Label("Enabled");
		if (ImGui::Checkbox("##Enabled", &ColorOverLifetime.Enabled))
		{
		}
		ImGuiEx::Label("TargetColor");
		if (ECommonComponent::ShowColor("##TargetColor", ColorOverLifetime.TargetColor.p, true))
		{
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNodeEx("Size Over Lifetime", ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		ImGuiEx::Label("Enabled");
		if (ImGui::Checkbox("##Enabled", &SizeOverLifetime.Enabled))
		{
		}

		ImGuiEx::Label("TargetSize");
		if (ImGui::DragFloat3("##TargetSize", SizeOverLifetime.TargetSize.p, 0.01f, MIN_float, MAX_float))
		{
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNodeEx("Texture Sheet Animation", ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		ImGuiEx::Label("Enabled");
		if (ImGui::Checkbox("##Enabled", &TextureSheetAnimation.Enabled))
		{
		}
		{
			ImGuiEx::Label("Tiles");

			const float width = ImGui::GetContentRegionAvail().x;
			const float charWidth = ImGui::GetFontSize() + 3.0f;
			uint min_v = 1.00f;

			ImGui::TextUnformatted("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.5f - charWidth);
			if (ImGui::DragScalar("##X", ImGuiDataType_U32, &TextureSheetAnimation.Cols, 0.1f, &min_v))
			{
			}

			ImGui::SameLine();
			ImGui::TextUnformatted("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.5f - charWidth);
			if (ImGui::DragScalar("##Y", ImGuiDataType_U32, &TextureSheetAnimation.Rows, 0.1f, &min_v))
			{
			}
		}

		ImGuiEx::Label("StartFrame");
		if (ImGui::InputScalar("##Start Frame", ImGuiDataType_U32, &TextureSheetAnimation.StartFrame))
		{
			TextureSheetAnimation.StartFrame = Math::Clamp<uint>(TextureSheetAnimation.StartFrame, 0, TextureSheetAnimation.Rows * TextureSheetAnimation.Cols - 1);
		}

		ImGuiEx::Label("Cycles");
		float min_v = 1.00f;
		if (ImGui::DragScalar("##Cycles", ImGuiDataType_Float, &TextureSheetAnimation.Cycles, 0.01f, &min_v))
		{
			if (_particleRender)
			{
				_particleRender->CalFrameTime();
			}
		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNodeEx("Renderer", ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		if (_particleRender)_particleRender->OnDrawEditor();
		ImGui::TreePop();
	}
}

/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(ParticleSystem);
template<class TransferFunction> inline
void ParticleSystem::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	if (transfer.Push("Base"))
	{
		Main.Transfer(transfer, ptr);
		transfer.Pop();
	}
	if (transfer.Push("Emission"))
	{
		Emission.Transfer(transfer, ptr);
		transfer.Pop();
	}
	if (transfer.Push("Shape"))
	{
		Shape.Transfer(transfer, ptr);
		transfer.Pop();
	}
	if (transfer.Push("VelocityOverLifetime"))
	{
		VelocityOverLifetime.Transfer(transfer, ptr);
		transfer.Pop();
	}
	if (transfer.Push("LimitVelocityOverLifetime"))
	{
		LimitVelocityOverLifetime.Transfer(transfer, ptr);
		transfer.Pop();
	}
	if (transfer.Push("ForceOverLifetime"))
	{
		ForceOverLifetime.Transfer(transfer, ptr);
		transfer.Pop();
	}
	if (transfer.Push("ColorOverLifetime"))
	{
		ColorOverLifetime.Transfer(transfer, ptr);
		transfer.Pop();
	}
	if (transfer.Push("SizeOverLifetime"))
	{
		SizeOverLifetime.Transfer(transfer, ptr);
		transfer.Pop();
	}
	if (transfer.Push("TextureSheetAnimation"))
	{
		TextureSheetAnimation.Transfer(transfer, ptr);
		transfer.Pop();
	}
	{//Renderer
		if (transfer.IsRead())
		{
			int SortLayer = 0;
			int OrderInLayer = 0;
			TRANSFER_SIMPLE(SortLayer);
			TRANSFER_SIMPLE(OrderInLayer);
			_particleRender->SetSortLayer(SortLayer);
			_particleRender->SetOrderInLayer(OrderInLayer);

			String MaterialFile;
			TRANSFER_SIMPLE(MaterialFile);
			AssetMeta* meta = AssetsManager::GetMeta(MaterialFile);
			if (meta)
			{
				Material* material = _particleRender->SetMaterial(meta->GetFullPath());
				if (material)
				{
					material->SetCastShadow(false);
					material->SetReceiveShadow(false);
				}
			}
		}
		else
		{
			int SortLayer = _particleRender->GetSortLayer();
			int OrderInLayer = _particleRender->GetOrderInLayer();
			TRANSFER_SIMPLE(SortLayer);
			TRANSFER_SIMPLE(OrderInLayer);

			String MaterialFile = _particleRender->GetMaterial()->GetResFile();
			AssetMeta* meta = AssetsManager::GetMetaByFile(MaterialFile);
			if (meta)MaterialFile = meta->GetGUID();
			TRANSFER_SIMPLE(MaterialFile);
		}
	}
}
DC_END_NAMESPACE