 
/*****************************************************************************************************/
// @author hannibal
// @date   2020/08/04
// @brief  声音-管理器
/*****************************************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
class AudioClip;
class AudioListener;
/********************************************************************/
class AudioManager Final : public object
{
	friend class AudioClip;
	friend class AudioListener;
	friend class Application;
	typedef Map<String, AudioClip*> AudioClips;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(AudioManager);
	BEGIN_REFECTION_TYPE(AudioManager)
	END_FINAL_REFECTION_TYPE;
	
private:
	static void Initialize();
	static void Destroy();

	static bool AddListener(AudioListener* listener);
	static void RemoveListener() { m_listener = nullptr; }
	static AudioListener* GetListener() { return m_listener; }

	static bool AddAudioClip(const String& name, AudioClip* clip);
	static AudioClip* GetAudioClip(const String& name);
	static void DestroyAudioClip(const String& name);
	static void DestroyAudioClip();

private:
	//销毁交给gameobject实现，这里只是保留引用
	static AudioListener* m_listener;
	static AudioClips _audioClips;
};
DC_END_NAMESPACE
