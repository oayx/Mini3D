
/*****************************************************************************************************/
// @author hannibal
// @date   2020/10/04
// @brief  视频
/*****************************************************************************************************/
#pragma once

#include "core/time/IntervalTime.h"
#include "runtime/component/GameObject.h"

DC_BEGIN_NAMESPACE
class Texture;
class VideoDecoder;
/********************************************************************/
class ENGINE_DLL VideoPlayer final : public Component
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(VideoPlayer);
	DISALLOW_COPY_ASSIGN(VideoPlayer);
	BEGIN_DERIVED_REFECTION_TYPE(VideoPlayer, Component)
		CTORS(DEFAULT_CTOR(VideoPlayer))
		ADD_EDITOR_COMPONENT("VideoPlayer")
	END_REFECTION_TYPE;

protected:
	VideoPlayer();
	~VideoPlayer();
	virtual void Update()override;

public:
	bool Play(const String& file, bool loop);
	bool Replay();
	bool Pause();
	bool Resume();
	void Stop();
	void SetFPS(int fps);
	void SetSize(int w, int h) { _frameSize.Set((float)w, (float)h); }//需要在play之前设置
	void OnComplete(const Action& fun) { _onComplete = fun; }

public:
	Texture* GetTexture()const { return _frameTexture; }

protected:
	VideoDecoder* _videoDecoder = nullptr;
	IntervalTime  _intervalTime;
	Texture*	  _frameTexture = nullptr;
	byte*		  _imageData = nullptr;
	Action		  _onComplete = nullptr;

	int			  _fps = 30;
	bool		  _isPlaying = false;
	String		  _filePath = "";
	Size		  _frameSize;//输出帧大小
};
DC_END_NAMESPACE
