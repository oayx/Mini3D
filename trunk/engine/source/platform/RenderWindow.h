
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
	END_DERIVED_REFECTION_TYPE;

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
	bool	IsFullscreen()const { return m_fullscreen; }
	int		GetFPS()const { return m_fps; }
	int		GetAntiAlias()const { return _antiAlias; }
	uint	GetSync()const { return m_sync; }
	TargetDisplay GetTargetDisplay()const { return _targetDisplay; }

protected:
	virtual void OnFocus(bool focused);

protected:
	String	_winName = "";
	Vector2 _position;
	Size	_winSize;
	bool	m_fullscreen = false;
	bool	m_mouseClip = true;

	int		m_fps = 60;
	int		_antiAlias = 4;
	uint	m_sync = 0;

	bool	m_isPaused = false;

	TargetDisplay _targetDisplay = TargetDisplay::Display1;
};
DC_END_NAMESPACE
