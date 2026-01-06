#include "Animation.h"
#include "AnimationNode.h"
#include "AnimationClip.h"
#include "AnimationState.h"
#include "AnimationBlender.h"
#include "Mesh.h"
#include "Bone.h"
#include "editor/EditorDefine.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Bone, Object);
IMPL_DERIVED_REFECTION_TYPE(Animation, Component);
Animation::Animation()
{
	_animationBlender = AnimationBlender::Create(this);
}
Animation::~Animation()
{
	SAFE_DELETE(_currentState);
	SAFE_DELETE(_oldState);
	SAFE_DELETE(_animationBlender);
	_animationClips.Clear();
	_boneTransformes.Clear();
}
Object* Animation::Clone(Object* newObj)
{
	base::Clone(newObj);
	Animation* obj = dynamic_cast<Animation*>(newObj);
	if (!obj)return newObj;

	obj->SetPlayOnAwake(_playOnAwake);
	obj->SetAutoSimulate(_autoSimulate);
	obj->SetUseUnscaledTime(_useUnscaledTime);
	obj->SetSpeed(_speed);
	obj->SetDefaultName(_defaultName);

	obj->SetMesh(Mesh::Create(_meshFile));

	return obj;
}
void Animation::Start()
{
	base::Start();
	if (!_currentState || !_currentState->IsPlaying())
	{
		if (_playOnAwake)
		{
			if (_defaultName.IsEmpty())
				Play();
			else
				Play(_defaultName);
		}
		Simulate(0.0f);
	}
}
void Animation::Update()
{
	if (_autoSimulate)
	{
		//时间缩放
		float dt = _useUnscaledTime ? Time::GetUnscaledDeltaTime() : Time::GetDeltaTime();
		Simulate(dt);
	}
}
void Animation::Simulate(float dt)
{
	if (_animationClips.IsEmpty() || _currentState == nullptr)
		return;

	DC_PROFILE_FUNCTION;
	float delta_time = dt * _speed;
	if (_currentState->IsPlaying())
	{
		//更新动画
		_currentState->Update(delta_time);

		//动画过渡
		_animationBlender->Update(delta_time);

		//取最终变换矩阵
		Matrix4 mat;
		_currentState->TransfromBone(mat, _boneTransformes);
	}
}
bool Animation::Play()
{
	if (_currentState == nullptr || _animationClips.IsEmpty())
		return false;

	auto clip = _animationClips.begin();
	if (_currentState->IsPlaying() && _currentState->GetName() == clip->first)
		return false;

	this->Stop();
	return this->PlayImpl(clip->first, clip->second->IsLoop());
}
bool Animation::Play(const String& name)
{
	auto clip = this->GetClip(name);
	if (!clip)return false;
	return this->Play(name, clip->IsLoop());
}
bool Animation::Play(const String& name, bool loop)
{
	if (_currentState == nullptr)
		return false;

	if (_currentState->IsPlaying() && _currentState->GetName() == name)
		return false;

	this->Stop();
	return this->PlayImpl(name, loop);
}
bool Animation::Play(const VecString& names)
{
	if (_currentState == nullptr || names.IsEmpty())
		return false;

	this->Stop();
	_playSet = names;
	if (!this->PlayImpl(_playSet.First(), false))
		return false;
	_playSet.Remove(_playSet.begin());
	return true;
}
bool Animation::PlayCross(const String& name, float fade_length, bool loop)
{
	if (_currentState == nullptr)
		return false;

	if (_currentState->IsPlaying() && _currentState->GetName() == name)
		return false;

	if (_currentState->_isPlaying)
		this->SwapState();

	this->Stop();
	if(!this->PlayImpl(name, loop))
		return false;
	_animationBlender->SetLength(fade_length);
	return true;
}
bool Animation::PlayCross(const VecString& names, float fade_length)
{
	if (_currentState == nullptr || names.IsEmpty())
		return false;

	if (_currentState->_isPlaying)
		this->SwapState();

	this->Stop();
	_playSet = names;
	if (!this->PlayImpl(_playSet.First(), false))
		return false;
	_playSet.Remove(_playSet.begin());
	_animationBlender->SetLength(fade_length);
	return true;
}
void Animation::Stop()
{
	_playSet.Clear();
	_animationBlender->Stop();
	if(_currentState != nullptr)_currentState->Stop();
	if (_oldState != nullptr)_oldState->Stop();
}
const String& Animation::GetName()const
{
	if (_currentState == nullptr || !_currentState->IsPlaying())
		return String::Empty;
	return _currentState->GetName();
}
uint Animation::GetFrames()const
{
	if (_currentState == nullptr || !_currentState->IsPlaying())
		return 0;
	return _currentState->GetFrames();
}
uint Animation::GetFPS()const
{
	if (_currentState == nullptr || !_currentState->IsPlaying())
		return 0;
	return _currentState->GetFPS();
}
float Animation::GetDuration()const
{
	if (_currentState == nullptr || !_currentState->IsPlaying())
		return 0.0f;
	return _currentState->GetDuration();
}
bool Animation::IsLoop()const
{
	if (_currentState == nullptr)
		return false;
	return _currentState->IsLoop();
}
bool Animation::IsPlaying()const
{
	if (_currentState == nullptr)
		return false;
	return _currentState->IsPlaying();
}
float Animation::GetPosition()const 
{
	if (_currentState == nullptr)
		return 0.0f;
	return _currentState->GetPosition(); 
}
AnimationClip* Animation::GetClip(const String& name)
{
	auto it = _animationClips.Find(name);
	if (it == _animationClips.end())
		return nullptr;
	
	return it->second;
}
void Animation::SetMesh(Mesh* mesh)
{
	if (_currentState != nullptr)
	{
		_currentState->Stop();
	}
	SAFE_DELETE(_currentState);
	SAFE_DELETE(_oldState);
	_boneTransformes.Clear();
	_animationClips.Clear();

	if (!mesh)
	{
		_meshFile = "";
		return;
	}

	_meshFile = mesh->GetResFile();

	_currentState = AnimationState::Create(this);
	_oldState = AnimationState::Create(this);

	//拷贝骨骼
	_boneTransformes.Resize(mesh->GetNumBones());
	_currentState->SetBoneKeyFrames(mesh->GetNumBones());
	_oldState->SetBoneKeyFrames(mesh->GetNumBones());
	//for (auto bone : mesh->_bones)
	//{
	//	_boneTransformes[bone->Id] = bone->FinalTransform;
	//}

	//拷贝动画剪辑
	for (const auto& clip : mesh->_animationClips)
	{
		AssertEx(_animationClips.Add(clip.first, clip.second),"");
	}
}
bool Animation::PlayImpl(const String& name, bool loop)
{
	if (_currentState == nullptr)return false;
	if (_currentState->GetName() == name)return false;

	auto it = _animationClips.Find(name);
	if (it == _animationClips.end())
	{
		Debuger::Error("Cannot find animation:%s", name.c_str());
		return false;
	}
	Debuger::Log("%s", name.c_str());

	_currentState->Play(it->second, loop);
	return true;
}
void Animation::OnPlayComplete(AnimationState* state)
{
	if (_onComplete != nullptr)_onComplete();

	if (!state->IsLoop())
	{
		if (_playSet.IsEmpty())
		{
			this->Stop();
		}
		else if(_currentState != nullptr)
		{
			if (_animationBlender->GetLength() > 0.0f)
			{
				this->SwapState();
				_animationBlender->Reset();
			}
			if (!this->PlayImpl(_playSet.First(), false))
			{
				this->Stop();
			}
			_playSet.Remove(_playSet.begin());
		}
	}
}
void Animation::SwapState()
{
	AnimationState* temp_state = _currentState;
	_currentState = _oldState;
	_oldState = temp_state;
}
void Animation::OnDrawEditor()
{
	base::OnDrawEditor();

	ImGuiEx::Label("Animations");
	if (ImGui::BeginCombo("##AnimationClips", _defaultName.c_str()))
	{
		for (const auto& obj : _animationClips)
		{
			if (ImGui::Selectable(obj.first.c_str()))
			{
				_defaultName = obj.first;
			}
		}
		ImGui::EndCombo();
	}

	ImGuiEx::Label("Play On Awake");
	ImGui::Checkbox("##PlayOnAwake", &_playOnAwake);

	ImGuiEx::Label("Auto Simulate");
	ImGui::Checkbox("##AutoSimulate", &_autoSimulate);

	ImGuiEx::Label("Use Unscaled Time");
	ImGui::Checkbox("##UseUnscaledTime", &_useUnscaledTime);

	float minValue = 0.0f;
	ImGuiEx::Label("Speed");
	ImGui::DragScalar("##Speed", ImGuiDataType_Float, &_speed, 0.01f, &minValue, nullptr, "%.2f");
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(Animation);
template<class TransferFunction> inline
void Animation::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_playOnAwake);
	TRANSFER_SIMPLE(_autoSimulate);
	TRANSFER_SIMPLE(_useUnscaledTime);
	TRANSFER_SIMPLE(_speed);
	TRANSFER_SIMPLE(_defaultName);
}
DC_END_NAMESPACE