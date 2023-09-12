
/*****************************************************************************
* Author： hannibal
* Date：2020/8/8
* Description：
*****************************************************************************/
#pragma once

#include "runtime/graphics/RenderDefine.h"
#include "runtime/scene/SceneUtils.h"

DC_BEGIN_NAMESPACE

enum class WindowFlag
{
	None		= 0x00,
	FullScreen	= 0x01,			//是否全屏模式
	MaxSize		= 0x02,
	Resizable	= 0x08,			//是否可以调整窗口大小
	MouseClip	= 0x10,			//限制鼠标不超出窗口
};
DECLARE_ENUM_OPERATORS(WindowFlag);

//窗口创建
struct WindowDesc
{
	String name = "";			//窗口名称
	void* hInstance = nullptr;
	void* hWnd = nullptr;		//句柄
	int x = -1;					//窗口模式如果小于0，则居中
	int y = -1;
	int width = 0;				//大小
	int  height = 0;
	byte fps = 60;				//0不限制
	byte antiAlias = 4;			//MSAA，0或者1不开启，支持2/4/8重采样
	uint sync = 0;				//垂直同步，0不开启
	WindowFlag flags = WindowFlag::None;
	TargetDisplay display = TargetDisplay::Display1;//窗口编号
};

//改变创建大小
struct WindowResizeDesc
{
	void* hWnd = nullptr;
	int width = 0;
	int height = 0;
	bool fullscreen = false;
	bool fullscreen_valid = true;//fullscreen标记是否有效，如果无效，使用之前设定
	WindowResizeDesc() {}
	WindowResizeDesc(void* hwnd, int w, int h, bool full, bool valid) :hWnd(hwnd), width(w), height(h), fullscreen(full), fullscreen_valid(valid) {}
};

//开始渲染
struct RenderFrameDesc
{
	int face = 0;
	ViewPortDesc view_port;
	void* target_buffer = nullptr;
	void* depth_stencil_buffer = nullptr;
	ClearFlag clear_flag = ClearFlag::DonotClear;
	Color clear_color = Color::Clear;
};
DC_END_NAMESPACE