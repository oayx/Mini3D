 
#include "AudioListener.h"
#include "AudioManager.h"

#if DC_PLATFORM_MAC || DC_PLATFORM_IOS
#include <OpenAL/OpenAL.h>
#else
#include <AL/al.h>
#endif

DC_BEGIN_NAMESPACE
/********************************************************************/
class AudioListenerPrivate : public object
{
	DEFAULT_CREATE(AudioListenerPrivate);
	FRIEND_CONSTRUCT_DESTRUCT(AudioListenerPrivate);
public:
	AudioListenerPrivate()
	{
		this->SetPosition(Vector3(0, 0, 0));
		this->SetVelocity(Vector3(0, 0, 0));
		this->SetOrientation(Vector3(0, 0, 1), Vector3(0, 1, 0));
		this->SetVolume(1.0f);
	}

	~AudioListenerPrivate()
	{
	}

	void SetVolume(float value)
	{
		alListenerfv(AL_GAIN, (const ALfloat*)&value);
	}

	void SetPosition(const Vector3& pos)
	{
		alListenerfv(AL_POSITION, (const ALfloat*)&pos);
	}

	void SetVelocity(const Vector3& velocity)
	{
		alListenerfv(AL_VELOCITY, (const ALfloat*)&velocity);
	}

	void SetOrientation(const Vector3& forward, const Vector3& up)
	{
		Vector3 orientation[2] = {
			forward,
			up
		};
		alListenerfv(AL_ORIENTATION, (const ALfloat*)&orientation);
	}
};
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(AudioListener, Component);
AudioListener::AudioListener()
{
	_private = AudioListenerPrivate::Create();
	AudioManager::AddListener(this);
}
AudioListener::~AudioListener()
{
	SAFE_DELETE(_private);
}
Object* AudioListener::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	AudioListener* obj = dynamic_cast<AudioListener*>(new_obj);
	if (!obj)return new_obj;

	return obj;
}
void AudioListener::OnEnable()
{
	_private->SetVolume(1.0f);
}
void AudioListener::OnDisable()
{
	_private->SetVolume(0.0f);
}
void AudioListener::OnDestroy()
{
	AudioManager::RemoveListener();
}
void AudioListener::OnTransformChanged()
{
	Vector3 pos = this->GetTransform()->GetPosition();
	Vector3 forward = this->GetTransform()->GetForward();
	Vector3 up = this->GetTransform()->GetUp();

	_private->SetPosition(pos);
	_private->SetOrientation(forward, up);
}
DC_END_NAMESPACE
