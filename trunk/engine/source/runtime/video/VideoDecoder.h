
/*****************************************************************************************************/
// @author hannibal
// @date   2020/10/04
// @brief  视频解码
/*****************************************************************************************************/
#pragma once

#include "core/image/Image.h"

DC_BEGIN_NAMESPACE
class VideoDecoderPrivate;
/********************************************************************/
class ENGINE_DLL VideoFrame final : public object
{
	DEFAULT_CREATE(VideoFrame);
	FRIEND_CONSTRUCT_DESTRUCT(VideoFrame);
public:
	Image* image = nullptr;
	float present_time = 0;
	bool isLastFrame = false;

	VideoFrame(ColorFormat format, const iSize& size) 
	{
		image = Image::Create(format, size);
		image->Retain();
	}
	~VideoFrame() 
	{ 
		SAFE_RELEASE(image); 
	}
};
class VideoDecoder final : public object
{
	friend class VideoPlayer;
	friend class Application;
	DEFAULT_CREATE(VideoDecoder);
	FRIEND_CONSTRUCT_DESTRUCT(VideoDecoder);
	DISALLOW_COPY_ASSIGN(VideoDecoder);
	BEGIN_REFECTION_TYPE(VideoDecoder)
	END_FINAL_REFECTION_TYPE;
	
	static void Initialize();
	static void Destroy();

private:
	VideoDecoder();
	~VideoDecoder();

	bool OpenFile(const String& path, bool loop, int w = -1, int h = -1);
	void Close();

	VideoFrame* GetFrame();
	void ReleaseFrame(VideoFrame* frame);

	bool IsLoop()const;

	int GetFrameWidth()const;
	int GetFrameHeight()const;

private:
	VideoDecoderPrivate* _private = nullptr;
};
DC_END_NAMESPACE
