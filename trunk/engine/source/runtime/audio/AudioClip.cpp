 
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
	int sample_rate;	//采样率（每秒样本数），表示每个通道的播放速度
	int byte_rate;		//波形音频数据传送速率，其值为通道数×每秒数据位数×每样 本的数据位数／8。播放软件利用此值可以估计缓冲区的大小
	short block_align;	//数据的调整数（按字节计算）
	short sample_bits;	//样本数据位数
};

class AudioClipPrivate : public object
{
	DEFAULT_CREATE(AudioClipPrivate);
	FRIEND_CONSTRUCT_DESTRUCT(AudioClipPrivate);
public:
	ALuint m_buffer = 0;
	std::mutex _mutex;
	bool m_stream_loop = false;

	AudioClipPrivate()
	{
	}

	~AudioClipPrivate()
	{
		if (m_buffer)
		{
			alDeleteBuffers(1, &m_buffer);
			m_buffer = 0;
		}
	}
	static void BufferData(ALuint buffer, int channel, int sample_bits, const void* data, int size, int frequency)
	{
		ALenum format = 0;

		if (channel == 1)
		{
			if (sample_bits == 8)
			{
				format = AL_FORMAT_MONO8;
			}
			else if (sample_bits == 16)
			{
				format = AL_FORMAT_MONO16;
			}
			else
			{
				Debuger::Error("audio data sample bits error: %d", sample_bits);
			}
		}
		else if (channel == 2)
		{
			if (sample_bits == 8)
			{
				format = AL_FORMAT_STEREO8;
			}
			else if (sample_bits == 16)
			{
				format = AL_FORMAT_STEREO16;
			}
			else
			{
				Debuger::Error("audio data sample bits error: %d", sample_bits);
			}
		}
		else
		{
			Debuger::Error("audio data channel error: %d", channel);
		}

		alBufferData(buffer, format, data, size, frequency);
	}
	void BufferData(int channel, int sample_bits, const void* data, int size, int frequency)
	{
		if (m_buffer == 0)
		{
			alGenBuffers(1, &m_buffer);
		}

		BufferData(m_buffer, channel, sample_bits, data, size, frequency);
	}
};
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(AudioClip, Resource);
AudioClip* AudioClip::Create(const String& path)
{
	AudioClip* clip = AudioManager::GetAudioClip(path);
	if (clip)return clip;

	clip = DBG_NEW AudioClip();
	clip->AutoRelease();
	clip->Load(path);
	AudioManager::AddAudioClip(path, clip);
	return clip;
}
bool AudioClip::LoadFromFile(const String& file)
{
	String full_path = Resource::GetFullDataPath(file);
	MemoryDataStream stream;
	if (File::ReadAllBytes(full_path, stream))
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
			MemoryDataStream buffer(size);
			buffer.Write(stream.data(), stream.Tell(), size);

			int bytes_per_sample = wav.sample_bits / 8 * wav.channel;
			int byte_rate = wav.sample_rate * bytes_per_sample;
			assert(byte_rate == wav.byte_rate);
			float length = size / (float)byte_rate;
			int sample_count = size / bytes_per_sample;

			this->m_channel = wav.channel;
			this->m_sample_rate = wav.sample_rate;
			this->m_byte_rate = wav.byte_rate;
			this->m_sample_bits = wav.sample_bits;
			this->m_samples = buffer;
			this->m_length = length;
			this->m_sample_count = sample_count;
			this->_private->BufferData(this->m_channel, this->m_sample_bits, this->m_samples.data(), this->m_samples.Size(), this->m_sample_rate);
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
	: base(ResourceType::AudioClip), m_samples(0)
{
	_private = AudioClipPrivate::Create();
}
AudioClip::~AudioClip()
{
	SAFE_DELETE(_private);
}
void* AudioClip::GetBuffer() const
{
	return (void*)(size_t)_private->m_buffer;
}
void AudioClip::SetStreamLoop(bool loop)
{
	_private->_mutex.lock();
	_private->m_stream_loop = loop;
	_private->_mutex.unlock();
}
DC_END_NAMESPACE
