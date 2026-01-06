#include "Input.h"
#include "MouseInput.h"
#include "TouchInput.h"
#include "KeyboardInput.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Input, Object);
void Input::UpdateMousePosition(float x, float y)
{
	_oldMousePosition.x = mousePosition.x;
	_oldMousePosition.y = mousePosition.y;
	mousePosition.x = (float)x;
	mousePosition.y = (float)y;
}
void Input::UpdateMouseMove(float x, float y)
{
	mousePositionDelta.x = x;
	mousePositionDelta.y = y;
}
bool Input::GetMouseButton(MouseBtnID button)
{
	return MouseInput::GetMouseButton(button);
}
bool Input::GetMouseButtonDown(MouseBtnID button)
{
	return MouseInput::GetMouseButtonDown(button);
}
bool Input::GetMouseButtonUp(MouseBtnID button)
{
	return MouseInput::GetMouseButtonUp(button);
}
bool Input::GetMouseDClick(MouseBtnID button)
{
	return MouseInput::GetMouseDClick(button);
}
bool Input::IsMouseMove()
{
	return MouseInput::IsMouseMove();
}
Vector2 Input::GetScrollDelta()
{ 
	return MouseInput::GetScrollDelta();
}
bool Input::GetKey(KeyCode kc)
{
	return KeyboardInput::GetKey(kc);
}
bool Input::GetKeyDown(KeyCode kc)
{
	return KeyboardInput::GetKeyDown(kc);
}
bool Input::GetKeyUp(KeyCode kc)
{
	return KeyboardInput::GetKeyUp(kc);
}
const List<ushort>& Input::GetInputChar()
{ 
	return KeyboardInput::GetInputChar(); 
}
int Input::GetTouchCount()
{
	return TouchInput::GetTouchCount();
}
bool Input::GetTouch(int index, Touch& touch)
{
	return TouchInput::GetTouch(index, touch);
}
bool Input::GetMultiTouchEnabled()
{
	return TouchInput::GetMultiTouchEnabled();
}
void Input::SetMultiTouchEnabled(bool b)
{
	TouchInput::SetMultiTouchEnabled(b);
}
void Input::Initialize()
{
	MouseInput::Initialize();
	KeyboardInput::Initialize();
	TouchInput::Initialize();
}
void Input::Reset()
{
	MouseInput::Reset();
	KeyboardInput::Reset();
	TouchInput::Reset();
}
DC_END_NAMESPACE
