
/*****************************************************************************
* Author： hannibal
* Date：2020/9/9
* Description：image 动画
*****************************************************************************/
#pragma once

#include "UIImage.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL UIImageAnimation final : UIImage
{
	typedef Vector<std::pair<String, String>> Atlases;
	friend class UICanvas;
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(UIImageAnimation);
	FRIEND_CONSTRUCT_DESTRUCT(UIImageAnimation);
	BEGIN_DERIVED_REFECTION_TYPE(UIImageAnimation, UIImage)
		CTORS(DEFAULT_CTOR(UIImageAnimation))
	END_REFECTION_TYPE;

protected:
	virtual void Start()override;
	virtual void OnEnable()override;
	virtual void OnDisable()override;
	virtual void Update()override;

public:
	void		Simulate(float detal_time);
	void		Play();
	void		Rewind();//重新播放
	void		Stop();
	void		Pause();
	void		Resume();

	void		AddImage(const String& file, const String& name) { _atlases.Add({ file, name }); }
	void		OnComplete(Action fun) { _completeFun = fun; }
	void		SetNativeSize(bool b) { _setNativeSize = b; }
	void		SetFPS(uint fps) { _fPS = fps; }
	uint		GetFPS()const { return _fPS; }
	bool		IsPlaying()const { return _isPlaying; }
	void		SetLoop(bool b) { _isLoop = b; }
	bool		IsLoop()const { return _isLoop; }
	void		SetFoward(bool b) { _isFoward = b; }
	bool		IsFoward()const { return _isFoward; }
	void		SetSelfUpdate(bool b) { _isSelfUpdate = b; }
	bool		IsSelfUpdate()const { return _isSelfUpdate; }
	int			GetCurFrame()const { return _curFrame; }
	int			GetFrameCount()const { return _atlases.Size(); }

private:
	void		SetFrame(int frame);
	void		OnStop();

protected:
	Atlases		_atlases;
	bool		_setNativeSize = true;		
	Action		_completeFun = nullptr;	//完成回调，针对非循环

	uint		_fPS = 12;					//帧数
	bool		_isAutoPlay = true;		//是否自动播放
	bool		_isPlaying = false;        //是否正在播放
	bool		_isLoop = true;			//是否循环播放
	bool		_isFoward = true;			//是否是正常顺序播放
	bool		_isSelfUpdate = true;		//      
	int			_curFrame = 0;				//当前帧数
	float		_deltaTime = 0;
};
DC_END_NAMESPACE
