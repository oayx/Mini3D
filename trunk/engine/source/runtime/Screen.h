
/*****************************************************************************
* Author： hannibal
* Date：2020/8/19
* Description：screen
*****************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
enum class ScreenOrientation
{
	Unknown = 0,
	//Portrait orientation.
	Portrait = 1,
	//Portrait orientation, upside down.
	PortraitUpsideDown = 2,
	//Landscape orientation, counter-clockwise from the portrait orientation.
	LandscapeLeft = 3,
	Landscape = 3,
	//Landscape orientation, clockwise from the portrait orientation.
	LandscapeRight = 4,
	// Auto-rotates the screen as necessary toward any of the enabled orientations.
	AutoRotation = 5
};
DECLARE_ENUM_OPERATORS(ScreenOrientation);

struct Resolution
{
	int width;
	int height;
	int refreshRate;
};

class ENGINE_DLL Screen final : public object
{
	friend class Application;
	friend class WindowManager;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Screen);
	BEGIN_REFECTION_TYPE(Screen)
	END_FINAL_REFECTION_TYPE;
	
public:
	static void SetResolution(int w, int h, bool fullscreen);
	static float GetWidth() { return _width; }
	static float GetHeight() { return _height; }
	static bool IsFullscreen() { return _fullscreen; }

	static bool ShowCursor(bool show);

	static bool SetOrientation(ScreenOrientation orientation);

private:
	inline static float _width = 0.0f;
	inline static float _height = 0.0f;
	inline static bool _fullscreen = false;
};
DC_END_NAMESPACE
