#include "PostProcess.h"
#include "runtime/graphics/Graphics.h"
#include "runtime/graphics/Material.h"
#include "runtime/input/Input.h"
#include "runtime/postprocess/DepthOfFieldEffect.h"
#include "runtime/postprocess/MotionBlurEffect.h"
#include "runtime/project/QualitySettings.h"
#include "runtime/renderable/Renderer.h"
#include "runtime/scene/Camera.h"
#include "runtime/Application.h"
#include "editor/EditorAppliction.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(FinalProcess, Object);
IMPL_DERIVED_REFECTION_TYPE(PostEffect, Object);
PostEffect::PostEffect()
{
}
PostEffect::~PostEffect()
{
}
void PostEffect::Render(Camera* camera, RenderTexture* source, RenderTexture* dest)
{
	_camera = camera;

	if (this->Is<DepthOfFieldEffect>() || this->Is<MotionBlurEffect>())
	{
		_camera->AddDepthTextureMode(DepthTextureMode::Depth);
	}

	this->OnRenderImage(source, dest);
}
void PostEffect::OnRenderImage(RenderTexture* source, RenderTexture* dest)
{
	Blit(source, dest);
}
void PostEffect::Blit(Texture* source, RenderTexture* dest)
{
	Blit(source, dest, nullptr, 0);
}
void PostEffect::Blit(Texture* source, RenderTexture* dest, Material* mat)
{
	Blit(source, dest, mat, 0);
}
void PostEffect::Blit(Texture* source, RenderTexture* dest, Material* mat, int pass)
{
	Graphics::Blit(_camera, source, dest, mat, pass);
}
void PostEffect::SetActive(bool b) 
{ 
	_active = b;
	if (_camera)
	{
		if (this->Is<DepthOfFieldEffect>() || this->Is<MotionBlurEffect>())
		{
			_camera->RemoveDepthTextureMode(DepthTextureMode::Depth);
		}
	}
}
void PostEffect::OnDrawEditor() 
{
	ImGuiEx::Label("Priority");
	ImGui::InputInt("Priority", &_priority);
	ImGui::NewLine();
}
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(PostEffect);
template<class TransferFunction> inline
void PostEffect::Transfer(TransferFunction& transfer, void* ptr)
{
	TRANSFER_SIMPLE(_active);
	TRANSFER_SIMPLE(_priority);
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(PostProcess, Component);
PostProcess::~PostProcess()
{
	this->ClearEffect();
	SAFE_RELEASE(_destTexture);
	SAFE_RELEASE(_renderTexture);
	SAFE_DELETE(_finalProcess)
}
Object* PostProcess::Clone(Object* newObj)
{
	base::Clone(newObj);
	PostProcess* obj = dynamic_cast<PostProcess*>(newObj);
	if (!obj)return newObj;

	for (auto effect : _effects)
	{
		if (effect->IsActive())
		{
			PostEffect* new_effect = obj->AddEffect(effect->GetType());
			if(new_effect)effect->Clone(new_effect);
		}
	}

	return obj;
}
void PostProcess::Awake()
{
	base::Awake();
	_finalProcess = Application::GetGraphics()->CreateFinalProcess();
	this->InitEffectTypes();
	this->Resize(0, 0);
}
void PostProcess::Resize(int w, int h)
{ 
	SAFE_RELEASE(_destTexture);
	SAFE_RELEASE(_renderTexture);

	Camera* camera = GetGameObject()->GetComponent<Camera>();
	if (camera)
	{
		TextureDesc desc;
		desc.width = camera->GetRenderTargetWidth(); desc.height = camera->GetRenderTargetHeight(); desc.format = ColorFormat::R8G8B8A8;
		desc.antiAlias = int(QualitySettings::GetMSAAType()); desc.flags = TextureFlag::COLOR_AND_DEPTH;
		_destTexture = Application::GetGraphics()->CreateRenderTexture(desc);
		_destTexture->Retain();

		_renderTexture = Application::GetGraphics()->CreateRenderTexture(desc);
		_renderTexture->Retain();
	}
}
void PostProcess::Render(Camera* camera)
{
	DC_PROFILE_FUNCTION;
	if (_effects.Size() > 0)
	{
		//排序
		List<PostEffect*> sort_effects;
		for (auto effect : _effects)
		{
			if (effect->IsActive())
			{
				auto it = sort_effects.begin();
				for (; it != sort_effects.end(); ++it)
				{
					if (effect->GetPriority() > (*it)->GetPriority())
					{
						sort_effects.Insert(it, effect);
						break;
					}
				}
				if (it == sort_effects.end())
				{
					sort_effects.Add(effect);
				}
			}
		}
		//执行
		bool is_first = true;
		for (auto effect : sort_effects)
		{
			effect->Render(camera, is_first ? _renderTexture : _destTexture, _destTexture);
			is_first = false;
		}
		_finalProcess->Render(camera, _destTexture);
	}
}
bool PostProcess::CanAdd(GameObject* object)
{
	return object->GetComponent<Camera>();
}
PostEffect* PostProcess::AddEffect(const Type* type)
{
	if (!type || !type->Is(typeof(PostEffect)))
		return nullptr;

	//同类型只能添加一次
	for (auto it = _effects.begin(); it != _effects.end(); ++it)
	{
		PostEffect* effect = *it;
		if (effect->GetType() == type)
		{
			effect->SetActive(true);
			return effect;
		}
	}

	//创建实例
	PostEffect* effect = dynamic_cast<PostEffect*>(type->GetConstructor()->Invoke());
	if (!effect)
		return nullptr;

	_effects.Add(effect);
	effect->SetActive(true);
	return effect;
}
PostEffect* PostProcess::GetEffect(const Type* type)
{
	for (auto effect : _effects)
	{
		if (effect->GetType() == type)
			return effect;
	}
	return nullptr;
}
void PostProcess::RemoveEffect(const Type* type)
{
	if (!type || !type->Is(typeof(PostEffect)))
		return;

	for (auto effect : _effects)
	{
		if (effect->GetType() == type)
		{
			effect->SetActive(false);
			break;
		}
	}
}
void PostProcess::ClearEffect()
{
	for (auto it : _effects)
		SAFE_DELETE(it);
	_effects.Clear();
}
bool PostProcess::HasEffect()const
{
	for (auto effect : _effects)
	{
		if (effect->IsActive())
			return true;
	}
	return false;
}
bool PostProcess::HasEffect(const Type* type)const
{
	for (auto effect : _effects)
	{
		if (effect->GetType() == type && effect->IsActive())
			return true;
	}
	return false;
}
void PostProcess::InitEffectTypes()
{
	const auto& types = Type::GetTypes();
	const Type* basetype = typeof(PostEffect);
	for (const auto& obj : types)
	{
		const Type* type = obj.second;
		if (!type->HasFlag(TypeFlag::ShowInEditor))continue;
		if (type != basetype && type->Is(basetype))
		{
			_effectTypes.Add(type);
		}
	}
}
void PostProcess::OnDrawEditor()
{
	base::OnDrawEditor();

	ImGui::Indent(20.0f);
	for (int i = 0; i < _effectTypes.Size(); ++i)
	{
		ImGui::PushID(i);

		const Type* type = _effectTypes[i];
		PostEffect* effect = GetEffect(type);
		bool enable = effect && effect->IsActive();
		if (ImGuiEx::CollapsingHeader(type->GetEditorComponent().c_str(), &enable, ImGuiTreeNodeFlags_None))
		{
			if (effect)effect->OnDrawEditor();
		}

		if (enable != HasEffect(type))
		{
			if (enable)
				AddEffect(type);
			else
				RemoveEffect(type);
		}
		ImGui::PopID();
	}
	ImGui::Unindent(20.0f);
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(PostProcess);
template<class TransferFunction> inline
void PostProcess::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	if (transfer.IsRead())
	{
		Vector<std::string> effects;
		{//先读取包括哪些组件
			if (transfer.Push("Effects"))
			{
				TRANSFER_VECTOR(effects, "Effect");
				transfer.Pop();
			}
		}
		//再依次添加组件
		for (int i = 0; i < effects.Size(); ++i)
		{
			const std::string& eff_name = effects[i];
			PostEffect* effect = AddEffect(Type::GetType(eff_name));
			if (transfer.Push(effect->GetClassId().c_str()))
			{
				effect->Serialize(transfer, ptr);
				transfer.Pop();
			}
		}
	}
	else
	{
		{//先写入组件类型
			Vector<std::string> effects;
			for(auto effect : _effects)
			{
				const std::string& type = effect->GetType()->GetName();
				effects.Add(type);
			}
			if (transfer.Push("Effects"))
			{
				TRANSFER_VECTOR(effects, "Effect");
				transfer.Pop();
			}
		}
		//再写入细节，方便读取
		for (auto effect : _effects)
		{
			if (transfer.Push(effect->GetClassId().c_str()))
			{
				effect->Deserialize(transfer, ptr);
				transfer.Pop();
			}
		}
	}
}
DC_END_NAMESPACE