 
/*****************************************************************************************************/
// @author hannibal
// @date   2020/08/04
// @brief  声音-资源
/*****************************************************************************************************/
#pragma once

#include "core/stream/DataStream.h"
#include "runtime/resources/Resources.h"

DC_BEGIN_NAMESPACE
#define STREAM_BUFFER_MAX 100
/********************************************************************/
class AudioClipPrivate;
class ENGINE_DLL AudioClip final : public Resource
{
	friend class AudioManager;
	DISALLOW_COPY_ASSIGN(AudioClip);
	FRIEND_CONSTRUCT_DESTRUCT(AudioClip);
	BEGIN_DERIVED_REFECTION_TYPE(AudioClip, Resource)
	END_REFECTION_TYPE;

	AudioClip();
	~AudioClip();

public:
	static  AudioClip* Create(const String& path);
	virtual bool LoadFromFile(const String& file)override ;

	void*	GetBuffer() const;
	bool	IsStream() const { return _stream; }
	void	SetStreamLoop(bool loop);
	float	GetLength()const { return _length; }

private:
	AudioClipPrivate* _private = nullptr;
	int		_channel = 0;
	int		_sampleRate = 0;
	int		_byteRate = 0;
	int		_sampleBits = 0;
	float	_length = 0;		//时长
	int		_sampleCount = 0;
	bool	_stream = false;
};
DC_END_NAMESPACE
/*
WAV文件格式说明表
文件头	偏移地址	字节数	数据类型	内 容
00		H			4		char		"RIFF"标志
04		H			4		long int	文件长度
08		H			4		char		"WAV"标志
0C		H			4		char		"fmt"标志
10		H			4	 				过渡字节（不定）
14		H			2		int			格式类别（10H为PCM形式的声音数据)
16		H			2		int			单声道为1，双声道为2通道数
18		H			2		int			采样率（每秒样本数），表示每个通道的播放速度
1C		H			4		long		波形音频数据传送速率，其值为通道数×每秒数据位数×每样 本的数据位数／8。播放软件利用此值可以估计缓冲区的大小
22		H			2	 				每样本的数据位数，表示每个声道中各个样本的数据位数。如果有多 个声道，对每个声道而言，样本大小都一样。 24H 4 char 数据标记符＂data＂ 28H 4 long int 语音数据的长度
*/