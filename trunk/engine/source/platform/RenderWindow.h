
/*****************************************************************************
* Author： hannibal
* Date：2009/12/8
* Description：
*****************************************************************************/
#pragma once

#include "runtime/CommonDefine.h"
#include "runtime/graphics/RenderDefine.h"

DC_BEGIN_NAMESPACE
class RenderContent;
/********************************************************************/
class RenderWindow : public Object
{
	friend class WindowManager;
	FRIEND_CONSTRUCT_DESTRUCT(RenderWindow);
	DISALLOW_COPY_ASSIGN(RenderWindow);
	BEGIN_DERIVED_REFECTION_TYPE(RenderWindow, Object)
	END_REFECTION_TYPE;

protected:
	RenderWindow() = default;
	virtual ~RenderWindow() = default;

public://window
	virtual bool Create(WindowDesc& info);
	virtual void Resize(WindowResizeDesc& desc);
	virtual int  OnWinMsg(uint uMsg, int64 param1, int64 param2, int64 param3, int64 param4) = 0;

	virtual void Update() {}
	virtual void Draw();

	virtual void* GetHwnd() = 0;

public:
	const	int GetLeft()const { return (int)_position.x; }
	const	int GetTop()const { return (int)_position.y; }
	const	int GetWidth()const { return (int)_winSize.width; }
	const	int GetHeight()const { return (int)_winSize.height; }
	bool	IsFullscreen()const { return _fullscreen; }
	int		GetFPS()const { return _fps; }
	int		GetAntiAlias()const { return _antiAlias; }
	uint	GetSync()const { return _sync; }
	TargetDisplay GetTargetDisplay()const { return _targetDisplay; }

protected:
	virtual void OnFocus(bool focused);

protected:
	String	_winName = "";
	Vector2 _position;
	Size	_winSize;
	bool	_fullscreen = false;
	bool	_mouseClip = true;

	int		_fps = 60;
	int		_antiAlias = 4;
	uint	_sync = 0;

	bool	_isPaused = false;

	TargetDisplay _targetDisplay = TargetDisplay::Display1;
};
DC_END_NAMESPACE
