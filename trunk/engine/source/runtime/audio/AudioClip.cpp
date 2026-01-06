 
#include "AudioClip.h"
#include "AudioManager.h"
#if DC_PLATFORM_MAC || DC_PLATFORM_IOS
#include <OpenAL/OpenAL.h>
#else
#include <AL/al.h>
#endif

DC_BEGIN_NAMESPACE
/********************************************************************/
struct WaveHeader
{
	char riff[4];
	int size;
	char wave[4];
	char fmt[4];
	int wave_size;
	short format;		//格式类别(值为1，表示数据PCMμ律编码的数据)
	short channel;		//单声道为1，双声道为2通道数
	int sampleRate;		//采样率（每秒样本数），表示每个通道的播放速度
	int byteRate;		//波形音频数据传送速率，其值为通道数×每秒数据位数×每样 本的数据位数／8。播放软件利用此值可以估计缓冲区的大小
	short blockAlign;	//数据的调整数（按字节计算）
	short sampleBits;	//样本数据位数
};

class AudioClipPrivate : public object
{
	DEFAULT_CREATE(AudioClipPrivate);
	FRIEND_CONSTRUCT_DESTRUCT(AudioClipPrivate);
public:
	ALuint _buffer = 0;
	std::mutex _mutex;
	bool _isStreamLoop = false;

	AudioClipPrivate()
	{
	}

	~AudioClipPrivate()
	{
		if (_buffer)
		{
			alDeleteBuffers(1, &_buffer);
			_buffer = 0;
		}
	}
	static void BufferData(ALuint buffer, int channel, int sampleBits, const void* data, int size, int frequency)
	{
		ALenum format = 0;

		if (channel == 1)
		{
			if (sampleBits == 8)
			{
				format = AL_FORMAT_MONO8;
			}
			else if (sampleBits == 16)
			{
				format = AL_FORMAT_MONO16;
			}
			else
			{
				Debuger::Error("audio data sample bits error: %d", sampleBits);
			}
		}
		else if (channel == 2)
		{
			if (sampleBits == 8)
			{
				format = AL_FORMAT_STEREO8;
			}
			else if (sampleBits == 16)
			{
				format = AL_FORMAT_STEREO16;
			}
			else
			{
				Debuger::Error("audio data sample bits error: %d", sampleBits);
			}
		}
		else
		{
			Debuger::Error("audio data channel error: %d", channel);
		}

		alBufferData(buffer, format, data, size, frequency);
	}
	void BufferData(int channel, int sampleBits, const void* data, int size, int frequency)
	{
		if (_buffer == 0)
		{
			alGenBuffers(1, &_buffer);
		}

		BufferData(_buffer, channel, sampleBits, data, size, frequency);
	}
};
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(AudioClip, Resource);
AudioClip* AudioClip::Create(const String& path)
{
	AudioClip* clip = AudioManager::GetAudioClip(path);
	if (clip)return clip;

	clip = Memory::New<AudioClip>();
	clip->AutoRelease();
	clip->Load(path);
	AudioManager::AddAudioClip(path, clip);
	return clip;
}
bool AudioClip::LoadFromFile(const String& file)
{
	String fullPath = Resource::GetFullDataPath(file);
	MemoryDataStream stream;
	if (File::ReadAllBytes(fullPath, stream))
	{
		WaveHeader wav;
		stream.Read(&wav, sizeof(wav));

		if (memcmp(wav.riff, "RIFF", 4) != 0 ||
			memcmp(wav.wave, "WAVE", 4) != 0 ||
			memcmp(wav.fmt, "fmt ", 4) != 0 ||
			wav.format != 1)
		{
			assert(!"wav file format error");
		}

		if (wav.wave_size == 18)
		{
			stream.Read<short>();
		}
		else if (wav.wave_size != 16)
		{
			assert(!"wav file format error");
		}

		char chunk[4];
		stream.Read(chunk, sizeof(chunk));

		while (memcmp(chunk, "data", 4) != 0)
		{
			int size = stream.Read<int>();
			stream.Read(nullptr, size);
			stream.Read(chunk, sizeof(chunk));
		}

		if (memcmp(chunk, "data", 4) == 0)
		{
			int size = stream.Read<int>();
			int bytes_per_sample = wav.sampleBits / 8 * wav.channel;
			int byteRate = wav.sampleRate * bytes_per_sample;
			assert(byteRate == wav.byteRate);
			float length = size / (float)byteRate;
			int sample_count = size / bytes_per_sample;

			this->_channel = wav.channel;
			this->_sampleRate = wav.sampleRate;
			this->_byteRate = wav.byteRate;
			this->_sampleBits = wav.sampleBits;
			this->_length = length;
			this->_sampleCount = sample_count;
			this->_private->BufferData(this->_channel, this->_sampleBits, stream.CurrBuffer(), size, this->_sampleRate);
			return true;
		}
	}
	else
	{
		Debuger::Error("wav file not exist: %s", file.c_str());
	}
	return false;
}
AudioClip::AudioClip()
	: base(ResourceType::AudioClip)
{
	_private = AudioClipPrivate::Create();
}
AudioClip::~AudioClip()
{
	SAFE_DELETE(_private);
}
void* AudioClip::GetBuffer() const
{
	return (void*)(size_t)_private->_buffer;
}
void AudioClip::SetStreamLoop(bool loop)
{
	_private->_mutex.lock();
	_private->_isStreamLoop = loop;
	_private->_mutex.unlock();
}
DC_END_NAMESPACE
