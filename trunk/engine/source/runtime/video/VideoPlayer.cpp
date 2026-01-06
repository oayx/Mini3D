
#include "VideoPlayer.h"
#include "VideoDecoder.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/input/Input.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(VideoPlayer, Component);
VideoPlayer::VideoPlayer()
{
	_intervalTime.Init(1.0f / _fps, true);
}
VideoPlayer::~VideoPlayer()
{
	this->Stop();
}
void VideoPlayer::Update()
{
	DC_PROFILE_FUNCTION;
	base::Update();
	if (_isPlaying)
	{
		if (_intervalTime.Update(Time::GetDeltaTime()))
		{
			VideoFrame* frame = _videoDecoder->GetFrame();
			bool isLastFrame = false;
			if (frame != nullptr)
			{
				ColorConverter::FromRGB8toRGBA8(frame->image->Data(), frame->image->GetWidth()*frame->image->GetHeight(), _imageData);
				TextureLockDesc lock_desc;
				byte* data = _frameTexture->Lock(lock_desc);
				::memcpy(data, _imageData, frame->image->GetWidth() * frame->image->GetHeight() * 4);
				_frameTexture->Unlock(lock_desc);
				isLastFrame = frame->isLastFrame;
			}
			_videoDecoder->ReleaseFrame(frame);
			if (isLastFrame && !_videoDecoder->IsLoop())
			{
				this->Stop();
				if (_onComplete != nullptr)_onComplete();
			}
		}
	}
}
bool VideoPlayer::Play(const String& file, bool loop)
{
	DC_PROFILE_FUNCTION;
	if (_isPlaying)
		return false;

	this->Stop();
	_videoDecoder = VideoDecoder::Create();
	bool ret = _videoDecoder->OpenFile(file, loop, (int)_frameSize.width, (int)_frameSize.height);
	if (!ret)
		return false;

	TextureDesc desc;
	desc.width = _videoDecoder->GetFrameWidth(); desc.height = _videoDecoder->GetFrameHeight(); desc.format = ColorFormat::R8G8B8A8; 
	desc.usage = GPUResourceUsage::Dynamic; desc.flags = TextureFlag::sRGB;
	_imageData = Memory::NewArray<byte>(_videoDecoder->GetFrameWidth() * _videoDecoder->GetFrameHeight() * 4);
	_frameTexture = Texture::CreateFromMemroy(_imageData, desc);
	_frameTexture->Retain();

	_isPlaying = true;
	_filePath = file;
	return true;
}
bool VideoPlayer::Replay()
{
	if (_filePath.IsEmpty() || _videoDecoder == nullptr)
		return false;

	if (!_isPlaying)
		return Play(_filePath, _videoDecoder->IsLoop());
	
	//TODO
	return false;
}
bool VideoPlayer::Pause()
{
	if (_videoDecoder == nullptr)return false;
	if (!_isPlaying)return false;

	_isPlaying = false;
	return true;
}
bool VideoPlayer::Resume()
{
	if (_videoDecoder == nullptr)return false;
	if (_isPlaying)return false;

	_isPlaying = true;
	return true;
}
void VideoPlayer::Stop()
{
	_isPlaying = false;
	_filePath = "";
	SAFE_DELETE(_videoDecoder);
	SAFE_RELEASE(_frameTexture);
	if (_imageData)
	{
		Memory::DeleteArray(_imageData);
		_imageData = nullptr;
	}
}
void VideoPlayer::SetFPS(int fps)
{
	_fps = fps;
	_intervalTime.Init(1.0f / _fps, true);
}
DC_END_NAMESPACE
