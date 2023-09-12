#include "MouseInput.h"
#include "Input.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(MouseInput, Object);
bool MouseInput::m_isMouseMove = false;
Vector2 MouseInput::m_mouseScrollDelta = Vector2::zero;
bool MouseInput::m_mapMouseBtnDown[int(MouseBtnID::Max)] = { false };
bool MouseInput::m_mapMouseBtnUp[int(MouseBtnID::Max)] = { false };
bool MouseInput::m_mapMouseBtnPress[int(MouseBtnID::Max)] = { false };
bool MouseInput::m_mapMouseDClick[int(MouseBtnID::Max)] = { false };
bool MouseInput::HandleLBtnDown(float x_pos, float y_pos)
{
	//Debuger::Log("HandleLBtnDown:%d,%d",x_pos,y_pos);
	MouseBtnID btn = MouseBtnID::Left;
	m_mapMouseBtnDown[(int)btn] = true;
	m_mapMouseBtnPress[(int)btn] = true;
	return true;
}
bool MouseInput::HandleRBtnDown(float x_pos, float y_pos)
{
	MouseBtnID btn = MouseBtnID::Right;
	m_mapMouseBtnDown[(int)btn] = true;
	m_mapMouseBtnPress[(int)btn] = true;
	return true;
}
bool MouseInput::HandleMBtnDown(float x_pos, float y_pos)
{
	MouseBtnID btn = MouseBtnID::Mid;
	m_mapMouseBtnDown[(int)btn] = true;
	m_mapMouseBtnPress[(int)btn] = true;
	return true;
}
bool MouseInput::HandleLBtnUp(float x_pos, float y_pos)
{
	MouseBtnID btn = MouseBtnID::Left;
	m_mapMouseBtnUp[(int)btn] = true;
	m_mapMouseBtnPress[(int)btn] = false;
	return true;
}
bool MouseInput::HandleRBtnUp(float x_pos, float y_pos)
{
	MouseBtnID btn = MouseBtnID::Right;
	m_mapMouseBtnUp[(int)btn] = true;
	m_mapMouseBtnPress[(int)btn] = false;
	return true;
}
bool MouseInput::HandleMBtnUp(float x_pos, float y_pos)
{
	MouseBtnID btn = MouseBtnID::Mid;
	m_mapMouseBtnUp[(int)btn] = true;
	m_mapMouseBtnPress[(int)btn] = false;
	return true;
}
bool MouseInput::HandleLDClick(float x_pos, float y_pos)
{
	MouseBtnID btn = MouseBtnID::Left;
	m_mapMouseDClick[(int)btn] = true;
	return true;
}
bool MouseInput::HandleRDClick(float x_pos, float y_pos)
{
	MouseBtnID btn = MouseBtnID::Right;
	m_mapMouseDClick[(int)btn] = true;
	return true;
}
bool MouseInput::HandleMDClick(float x_pos, float y_pos)
{
	MouseBtnID btn = MouseBtnID::Mid;
	m_mapMouseDClick[(int)btn] = true;
	return true;
}
bool MouseInput::HandleInputPosition(float x_pos, float y_pos)
{
	Input::UpdateMousePosition(x_pos, y_pos);
	return true;
}
bool MouseInput::HandleInputMove(float x_offset, float y_offset)
{
	m_isMouseMove = true;
	Input::UpdateMouseMove(x_offset, y_offset);
	return true;
}
bool MouseInput::HandleInputWheel(float x, float y)
{
	m_mouseScrollDelta = Vector2(x, y);
	return true;
}
bool MouseInput::GetMouseButton(MouseBtnID btn)
{
	return m_mapMouseBtnPress[(int)btn];
}
bool MouseInput::GetMouseButtonDown(MouseBtnID btn)
{
	return m_mapMouseBtnDown[(int)btn];
}
bool MouseInput::GetMouseButtonUp(MouseBtnID btn)
{
	return m_mapMouseBtnUp[(int)btn];
}
bool MouseInput::GetMouseDClick(MouseBtnID btn)
{
	return m_mapMouseDClick[(int)btn];
}
void MouseInput::Initialize()
{
	m_isMouseMove = false;
	m_mouseScrollDelta = Vector2::zero;
	memset(m_mapMouseBtnDown, 0, sizeof(m_mapMouseBtnDown));
	memset(m_mapMouseBtnUp, 0, sizeof(m_mapMouseBtnUp));
	memset(m_mapMouseBtnPress, 0, sizeof(m_mapMouseBtnPress));
	memset(m_mapMouseDClick, 0, sizeof(m_mapMouseDClick));
}
void MouseInput::Reset()
{
	m_isMouseMove = false;
	m_mouseScrollDelta = Vector2::zero;
	memset(m_mapMouseBtnDown, 0, sizeof(m_mapMouseBtnDown));
	memset(m_mapMouseBtnUp, 0, sizeof(m_mapMouseBtnUp));
	memset(m_mapMouseDClick, 0, sizeof(m_mapMouseDClick));
}
DC_END_NAMESPACE
