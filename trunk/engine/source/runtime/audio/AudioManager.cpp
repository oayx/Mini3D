 
#include "AudioManager.h"
#include "AudioListener.h"
#include "AudioClip.h"
#include "runtime/component/GameObject.h"

#if DC_PLATFORM_MAC || DC_PLATFORM_IOS
#include <OpenAL/OpenAL.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

DC_BEGIN_NAMESPACE
/********************************************************************/
static AudioManager* g_AudioManager = nullptr;
static ALCdevice* g_device = nullptr;
static ALCcontext* g_context = nullptr;
AudioListener* AudioManager::m_listener = nullptr;
AudioManager::AudioClips AudioManager::_audioClips;
IMPL_REFECTION_TYPE(AudioManager);
void AudioManager::Initialize()
{
	String name;
	if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATE_ALL_EXT"))
	{
		name = alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);
	}
	if (name.IsEmpty())
	{
		name = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
	}
	Debuger::Log("open al device specifier: %s", name.c_str());

	//TODO:在某些windows声卡驱动下，如果不插入耳机，又没有其它外接扬声器，会导致OpenAL无法初始化
	g_device = alcOpenDevice(nullptr);
	if (g_device == nullptr)
	{
		Debuger::Error("open al device open failed");
		return;
	}

	g_context = alcCreateContext(g_device, nullptr);
	if (g_context == nullptr)
	{
		Debuger::Error("open al context create failed");
		return;
	}

	if (alcMakeContextCurrent(g_context) == ALC_FALSE)
	{
		alcDestroyContext(g_context);
		g_context = nullptr;
		alcCloseDevice(g_device);
		g_device = nullptr;
		Debuger::Error("open al context set failed");
		return;
	}

	Debuger::Log("open al init success");
}
void AudioManager::Destroy()
{
	alcMakeContextCurrent(nullptr);
	if (g_context)
	{
		alcDestroyContext(g_context);
		g_context = nullptr;
	}
	if (g_device)
	{
		alcCloseDevice(g_device);
		g_device = nullptr;
	}

	m_listener = nullptr;
	DestroyAudioClip();
}
bool AudioManager::AddListener(AudioListener* listener)
{
	if (m_listener != nullptr)
	{
		Debuger::Error("AudioManager::AddListener - already existed listener");
		return false;
	}
	m_listener = listener;
	return true;
}
bool AudioManager::AddAudioClip(const String& name, AudioClip* clip)
{
	if (clip != nullptr && _audioClips.Find(name) == _audioClips.end())
	{
		clip->Retain();
		AssertEx(_audioClips.Add(name, clip), "");
		return true;
	}
	Debuger::Error("Already have clip:%s", name.c_str());
	return false;
}
AudioClip* AudioManager::GetAudioClip(const String& name)
{
	AudioClips::const_iterator it = _audioClips.Find(name);
	if (it != _audioClips.end())
	{
		return it->second;
	}
	return nullptr;
}
void AudioManager::DestroyAudioClip(const String& name)
{
	AudioClips::iterator it = _audioClips.Find(name);
	if (it != _audioClips.end())
	{
		SAFE_RELEASE(it->second);
		_audioClips.Remove(it);
	}
}
void AudioManager::DestroyAudioClip()
{
	for (auto obj : _audioClips)
	{
		SAFE_RELEASE(obj.second);
	}
	_audioClips.Clear();
}
DC_END_NAMESPACE
