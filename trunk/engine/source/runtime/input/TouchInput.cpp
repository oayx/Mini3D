#include "TouchInput.h"
#include "Input.h"
#include "MouseInput.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(TouchInput, Object);
TouchInput::Touches TouchInput::_touches;
bool TouchInput::_multiTouchEnabled = true;
bool TouchInput::GetTouch(int index, Touch& touch)
{
	if (index >= _touches.Size())return false;

	Touches::const_iterator it = _touches.begin();
	for (int i = 0; it != _touches.end(); ++i)
	{
		if (i == index)
		{
			touch = *it;
			return true;
		}
	}
	return false;
}
void TouchInput::AddTouch(Touch touch)
{
	if (!_touches.IsEmpty())
	{
		for (const auto& t : _touches)
		{
			if (t.fingerId == touch.fingerId)return;
		}
	}
	else
	{
		_touches.AddLast(touch);
		if (Input::m_simulateMouseWithTouches)
		{
			MouseInput::HandleLBtnDown(touch.position.x, touch.position.y);
			MouseInput::HandleInputPosition(touch.position.x, touch.position.y);
		}
	}
}
void TouchInput::UpdateTouch(Touch touch)
{
	bool is_find = false;
	bool is_first = true;
	for (auto& t : _touches)
	{
		if (t.fingerId == touch.fingerId)
		{
			Touch old_touch = t;
			t = touch;
			switch (touch.phase)
			{
			case TouchPhase::Stationary:
				{
					t.deltaPosition = touch.position - old_touch.position;
					if (is_first && Input::m_simulateMouseWithTouches && old_touch.position != touch.position)
					{
						MouseInput::HandleInputPosition(touch.position.x, touch.position.y);
					}
				}
				break;
			case TouchPhase::Canceled:
			case TouchPhase::Ended:
				{
					if (is_first && Input::m_simulateMouseWithTouches)
					{
						MouseInput::HandleLBtnUp(touch.position.x, touch.position.y);
						MouseInput::HandleInputPosition(touch.position.x, touch.position.y);
					}
				}
				break;
			default:break;
			}
			is_find = true;
			break;
		}
		is_first = false;
	}
	if (!is_find)
	{
		AddTouch(touch);
	}
}
bool TouchInput::GetMultiTouchEnabled() 
{ 
	return _multiTouchEnabled; 
}
void TouchInput::SetMultiTouchEnabled(bool b) 
{ 
	_multiTouchEnabled = b; 
}
void TouchInput::Reset()
{
	for (auto it = _touches.begin(); it != _touches.end(); )
	{
		if (it->phase == TouchPhase::Ended)
		{
			it = _touches.Remove(it);
		}
		else
		{
			if (it->phase == TouchPhase::Began)
			{
				it->phase = TouchPhase::Stationary;
			}
			++it;
		}
	}
}
DC_END_NAMESPACE
