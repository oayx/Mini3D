 
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
class ENGINE_DLL Input Final : public Object
{
	friend class Application;
	friend class TouchInput;
	friend class MouseInput;
	friend class KeyboardInput;
	friend class RenderWindow;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Input);
	BEGIN_DERIVED_REFECTION_TYPE(Input, Object)
	END_DERIVED_REFECTION_TYPE;

public:
	static void UpdateMousePosition(float x, float y);
	static void UpdateMouseMove(float x, float y);
	static void SetSimulateMouseWithTouches(bool b) { m_simulateMouseWithTouches = b; }

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
	static Vector3 mousePosition;
	static Vector3 mousePositionDelta;

private:
	static Vector3 m_oldMousePosition;
	static bool m_simulateMouseWithTouches;
};
DC_END_NAMESPACE
