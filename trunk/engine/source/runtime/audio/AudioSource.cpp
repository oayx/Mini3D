 #include "AudioSource.h"
#include "AudioClip.h"
#include "runtime/resources/AssetsManager.h"
#include "runtime/Application.h"
#include "editor/common/EAssetSelect.h"
#include "editor/main/EMain_Project.h"
#include "editor/EditorAppliction.h"
#include "editor/EditorCursor.h"

#if DC_PLATFORM_MAC || DC_PLATFORM_IOS
#include <OpenAL/OpenAL.h>
#else
#include <AL/al.h>
#endif

DC_BEGIN_NAMESPACE
/********************************************************************/
class AudioSourcePrivate : public object
{
	DEFAULT_CREATE(AudioSourcePrivate);
	FRIEND_CONSTRUCT_DESTRUCT(AudioSourcePrivate);
public:
	ALuint m_source = 0;
	bool m_loop = false;
	float m_volume = 1;
	bool m_paused = false;

	AudioSourcePrivate()
	{
		alGenSources(1, &m_source);

		this->SetLoop(m_loop);
		this->SetVolume(m_volume);
		this->SetPitch(1);
		this->SetPosition(Vector3(0, 0, 0));
		this->SetDirection(Vector3(0, 0, 1));
		this->SetVelocity(Vector3(0, 0, 0));

		//alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
		//alSourcef(m_source, AL_ROLLOFF_FACTOR, 1.0);
		//alSourcef(m_source, AL_REFERENCE_DISTANCE, 5);
		//alSourcef(m_source, AL_MAX_DISTANCE, 10);
	}
	~AudioSourcePrivate()
	{
		this->Stop();
		this->ClearBuffers();

		alDeleteSources(1, &m_source);
	}
	void ClearBuffers()
	{
		alSourcei(m_source, AL_BUFFER, 0);
	}
	void SourceBuffer(ALuint buffer)
	{
		alSourcei(m_source, AL_BUFFER, buffer);
	}
	void SetLoop(bool loop)
	{
		alSourcei(m_source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
	}
	void SetPosition(const Vector3& pos)
	{
		alSourcefv(m_source, AL_POSITION, (const ALfloat*)&pos);
	}
	void SetVelocity(const Vector3& velocity)
	{
		alSourcefv(m_source, AL_VELOCITY, (const ALfloat*)&velocity);
	}
	void SetDirection(const Vector3& dir)
	{
		alSourcefv(m_source, AL_DIRECTION, (const ALfloat*)&dir);
	}
	void SetPitch(float pitch)
	{
		alSourcef(m_source, AL_PITCH, pitch);
	}
	void SetVolume(float volume)
	{
		alSourcef(m_source, AL_GAIN, volume);
	}
	void Play()
	{
		m_paused = false;
		alSourcePlay(m_source);
	}
	void Pause()
	{
		if (this->GetState() == AudioSource::State::Playing)
		{
			m_paused = true;
			alSourcePause(m_source);
		}
	}
	void Stop()
	{
		m_paused = false;
		alSourceStop(m_source);
	}
	AudioSource::State GetState() const
	{
		ALint state = 0;
		alGetSourcei(m_source, AL_SOURCE_STATE, &state);

		switch (state)
		{
		case AL_INITIAL:
			return AudioSource::State::Initial;
		case AL_PLAYING:
			return AudioSource::State::Playing;
		case AL_PAUSED:
			return AudioSource::State::Paused;
		case AL_STOPPED:
			return AudioSource::State::Stopped;
		}

		return AudioSource::State::Unknown;
	}
};
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(AudioSource, Component);
AudioSource::AudioSource()
{
	_private = AudioSourcePrivate::Create();
}
AudioSource::~AudioSource()
{
	SAFE_DELETE(_private);
	SAFE_RELEASE(_clip);
}
Object* AudioSource::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	AudioSource* obj = dynamic_cast<AudioSource*>(new_obj);
	if (!obj)return new_obj;

	obj->SetClip(_clip);
	obj->SetLoop(_isLoop);
	obj->SetVolume(_volume);
	obj->SetPitch(_pitch);

	return obj;
}
void AudioSource::SetClip(AudioClip* clip)
{
	this->Stop();
	if (_clip != nullptr)_clip->Release();
	_clip = clip;
	if (_clip)
	{
		_clip->Retain();
		if (!_clip->IsStream())
		{
			ALuint buffer = (ALuint)(size_t)_clip->GetBuffer();
			_private->SourceBuffer(buffer);
		}
		else
		{
			_private->SetLoop(false);
			if (_private->m_loop)
			{
				_clip->SetStreamLoop(true);
			}
		}
	}
}
void AudioSource::SetLoop(bool loop)
{
	_private->m_loop = loop;
	if (_clip && _clip->IsStream())
	{
		_clip->SetStreamLoop(loop);
	}
	else
	{
		_private->SetLoop(loop);
	}
}
void AudioSource::SetVolume(float volume)
{
	_private->m_volume = volume;
	if (_clip && _clip->IsStream())
	{
	}
	else
	{
		_private->SetVolume(volume);
	}
}
void AudioSource::SetPitch(float pitch)
{
	if (_clip && _clip->IsStream())
	{
	}
	else
	{
		_private->SetPitch(pitch);
	}
}
void AudioSource::Play()
{
	if (!IsEnable() || !GetGameObject()->ActiveInHierarchy())
		return;
	if (_clip)
	{
		if (_clip->IsStream())
		{
			//TODO
		}
		else
		{
			_private->Play();
		}
	}
}
void AudioSource::Pause()
{
	_private->Pause();
}
void AudioSource::Stop()
{
	if (_clip)
	{
		_private->Stop();
		_private->ClearBuffers();

		if (_clip->IsStream())
		{
			//TODO
		}
	}
}
void AudioSource::Start()
{
	if (Application::IsPlay())
	{
		if (_playOnAwake)
		{
			this->Play();
		}
	}
}
void AudioSource::OnEnable()
{
	if (Application::IsPlay())
	{
		if (_playOnAwake)
		{
			this->Play();
		}
	}
}
void AudioSource::OnDisable()
{
	if (Application::IsPlay())
	{
		this->Pause();
	}
}
void AudioSource::OnTransformChanged()
{
	Vector3 pos = this->GetTransform()->GetPosition();
	Vector3 forward = this->GetTransform()->GetForward();
	_private->SetPosition(pos);
	_private->SetDirection(forward);
}
AudioSource::State AudioSource::GetState() const
{
	return _private->GetState();
}
void AudioSource::OnDrawEditor()
{
	base::OnDrawEditor();
	{
		ImGuiEx::Label("File");

		const char* sz_name = _clip ? _clip->GetResName().c_str() : "Add Clip";
		ImGui::Button(sz_name, ImVec2(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(ICON_FA_LINK).x - 5, 0));
		if (ImGui::IsItemClicked(0) && !_clip->GetResName().IsEmpty())
		{
			EMain_Project::SetSelectFile(_clip->GetResFile());
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ProjectAsset"))
			{
				const String& file_path = EditorCursor::GetDragFile();
				if (Resource::GetResourceType(file_path) == ResourceType::AudioClip)
				{
					_clip = AudioClip::Create(file_path);
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
				_clip = AudioClip::Create(file);
			};

			EAssetSelect::Search(ResourceType::AudioClip, OnSelectAsset);
		}
	}
	ImGui::NewLine();
	{
		ImGuiEx::Label("Play On Awake");
		ImGui::Checkbox("##PlayOnAwake", &_playOnAwake);

		ImGuiEx::Label("Loop");
		ImGui::Checkbox("##Loop", &_isLoop);

		ImGuiEx::Label("Volume");
		ImGui::DragFloat("##Volume", &_volume, 0.01f, 0.0f, 1.0f);

		ImGuiEx::Label("Pitch");
		ImGui::DragFloat("##Pitch", &_pitch, 0.01f, -3.0f, 3.0f);
	}
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(AudioSource);
template<class TransferFunction> inline
void AudioSource::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	String File = "";
	if (transfer.IsRead())
	{
		TRANSFER_SIMPLE(File);
		AssetMeta* meta = AssetsManager::GetMeta(File);
		if (meta)File = meta->GetFullPath();
	}
	else
	{
		if (_clip)
		{
			File = _clip->GetResFile();
			AssetMeta* meta = AssetsManager::GetMetaByFile(File);
			if (meta)File = meta->GetGUID();
			TRANSFER_SIMPLE(File);
		}
	}
	TRANSFER_SIMPLE(_playOnAwake);
	TRANSFER_SIMPLE(_isLoop);
	TRANSFER_SIMPLE(_volume);
	TRANSFER_SIMPLE(_pitch);

	if (transfer.IsRead())
	{
		if (!File.IsEmpty())SetClip(AudioClip::Create(File));
		SetLoop(_isLoop);
		SetVolume(_volume);
		SetPitch(_pitch);
	}
}
DC_END_NAMESPACE