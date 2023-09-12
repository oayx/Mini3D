
/*****************************************************************************
* Author： hannibal
* Description：窗口管理
*****************************************************************************/
#pragma once

#include "RenderWindow.h"
#include "runtime/CommonDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class WindowManager Final : public object
{
	friend class Application;
	typedef Vector<RenderWindow*> RenderWindows;
	typedef Map<uint64, RenderWindow*> MapRenderWindows;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(WindowManager);
	BEGIN_REFECTION_TYPE(WindowManager)
	END_FINAL_REFECTION_TYPE;

	static void Initialize();
	static void Destroy();

public:
	static RenderWindow* Create(WindowDesc& info);
	static void AddWindow(TargetDisplay display, RenderWindow* window);
	static void CloseWindow(void* hWnd);

	static RenderWindow* GetWindow(TargetDisplay display) { return _renderWindows[int(display)]; }
	static RenderWindow* GetWindow(void* hWnd);

	static void Update();
	static void Draws();
	static void Resize(WindowResizeDesc& desc);

private:
	static RenderWindows _renderWindows;
	static MapRenderWindows _mapRenderWindows;
};
DC_END_NAMESPACE