 
/*****************************************************************************
* Author： hannibal
* Date：2009/12/11
* Description： 输入系统
*****************************************************************************/
#pragma once

#include "InputDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// ClassName：Input
// Description：输入系统
class ENGINE_DLL Input final : public Object
{
	friend class Application;
	friend class TouchInput;
	friend class MouseInput;
	friend class KeyboardInput;
	friend class RenderWindow;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Input);
	BEGIN_DERIVED_REFECTION_TYPE(Input, Object)
	END_REFECTION_TYPE;

public:
	static void UpdateMousePosition(float x, float y);
	static void UpdateMouseMove(float x, float y);
	static void SetSimulateMouseWithTouches(bool b) { _simulateMouseWithTouches = b; }

public://mouse
	static bool GetMouseButton(MouseBtnID button);
	static bool GetMouseButtonDown(MouseBtnID button);
	static bool GetMouseButtonUp(MouseBtnID button);
	static bool GetMouseDClick(MouseBtnID button);
	static bool IsMouseMove();
	static Vector2 GetScrollDelta();

public://Keyboard
	static bool GetKey(KeyCode kc);
	static bool GetKeyDown(KeyCode kc);
	static bool GetKeyUp(KeyCode kc);
	static const List<ushort>& GetInputChar();

public://Touch
	static int  GetTouchCount();
	static bool GetTouch(int index, Touch& touch);
	static bool GetMultiTouchEnabled();
	static void SetMultiTouchEnabled(bool b);

private:
	static void Initialize();
	static void Reset();

public:
	inline static Vector3 mousePosition = Vector3::zero;
	inline static Vector3 mousePositionDelta = Vector3::zero;

private:
	inline static Vector3 _oldMousePosition = Vector3::zero;
	inline static bool _simulateMouseWithTouches = true;
};
DC_END_NAMESPACE
