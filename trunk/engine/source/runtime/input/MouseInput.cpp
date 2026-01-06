#include "MouseInput.h"
#include "Input.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(MouseInput, Object);
bool MouseInput::HandleLBtnDown(float x_pos, float y_pos)
{
	//Debuger::Log("HandleLBtnDown:%d,%d",x_pos,y_pos);
	MouseBtnID btn = MouseBtnID::Left;
	_mapMouseBtnDown[(int)btn] = true;
	_mapMouseBtnPress[(int)btn] = true;
	return true;
}
bool MouseInput::HandleRBtnDown(float x_pos, float y_pos)
{
	MouseBtnID btn = MouseBtnID::Right;
	_mapMouseBtnDown[(int)btn] = true;
	_mapMouseBtnPress[(int)btn] = true;
	return true;
}
bool MouseInput::HandleMBtnDown(float x_pos, float y_pos)
{
	MouseBtnID btn = MouseBtnID::Mid;
	_mapMouseBtnDown[(int)btn] = true;
	_mapMouseBtnPress[(int)btn] = true;
	return true;
}
bool MouseInput::HandleLBtnUp(float x_pos, float y_pos)
{
	MouseBtnID btn = MouseBtnID::Left;
	_mapMouseBtnUp[(int)btn] = true;
	_mapMouseBtnPress[(int)btn] = false;
	return true;
}
bool MouseInput::HandleRBtnUp(float x_pos, float y_pos)
{
	MouseBtnID btn = MouseBtnID::Right;
	_mapMouseBtnUp[(int)btn] = true;
	_mapMouseBtnPress[(int)btn] = false;
	return true;
}
bool MouseInput::HandleMBtnUp(float x_pos, float y_pos)
{
	MouseBtnID btn = MouseBtnID::Mid;
	_mapMouseBtnUp[(int)btn] = true;
	_mapMouseBtnPress[(int)btn] = false;
	return true;
}
bool MouseInput::HandleLDClick(float x_pos, float y_pos)
{
	MouseBtnID btn = MouseBtnID::Left;
	_mapMouseDClick[(int)btn] = true;
	return true;
}
bool MouseInput::HandleRDClick(float x_pos, float y_pos)
{
	MouseBtnID btn = MouseBtnID::Right;
	_mapMouseDClick[(int)btn] = true;
	return true;
}
bool MouseInput::HandleMDClick(float x_pos, float y_pos)
{
	MouseBtnID btn = MouseBtnID::Mid;
	_mapMouseDClick[(int)btn] = true;
	return true;
}
bool MouseInput::HandleInputPosition(float x_pos, float y_pos)
{
	Input::UpdateMousePosition(x_pos, y_pos);
	return true;
}
bool MouseInput::HandleInputMove(float x_offset, float y_offset)
{
	_isMouseMove = true;
	Input::UpdateMouseMove(x_offset, y_offset);
	return true;
}
bool MouseInput::HandleInputWheel(float x, float y)
{
	_mouseScrollDelta = Vector2(x, y);
	return true;
}
bool MouseInput::GetMouseButton(MouseBtnID btn)
{
	return _mapMouseBtnPress[(int)btn];
}
bool MouseInput::GetMouseButtonDown(MouseBtnID btn)
{
	return _mapMouseBtnDown[(int)btn];
}
bool MouseInput::GetMouseButtonUp(MouseBtnID btn)
{
	return _mapMouseBtnUp[(int)btn];
}
bool MouseInput::GetMouseDClick(MouseBtnID btn)
{
	return _mapMouseDClick[(int)btn];
}
void MouseInput::Initialize()
{
	_isMouseMove = false;
	_mouseScrollDelta = Vector2::zero;
	memset(_mapMouseBtnDown, 0, sizeof(_mapMouseBtnDown));
	memset(_mapMouseBtnUp, 0, sizeof(_mapMouseBtnUp));
	memset(_mapMouseBtnPress, 0, sizeof(_mapMouseBtnPress));
	memset(_mapMouseDClick, 0, sizeof(_mapMouseDClick));
}
void MouseInput::Reset()
{
	_isMouseMove = false;
	_mouseScrollDelta = Vector2::zero;
	memset(_mapMouseBtnDown, 0, sizeof(_mapMouseBtnDown));
	memset(_mapMouseBtnUp, 0, sizeof(_mapMouseBtnUp));
	memset(_mapMouseDClick, 0, sizeof(_mapMouseDClick));
}
DC_END_NAMESPACE
