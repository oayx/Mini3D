#include "KeyboardInput.h"
#include "Input.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(KeyboardInput, Object);
bool KeyboardInput::HandleKeyDown(WORD w)
{
	//Debuger::Log("keycode:%d", w);
	KeyCode kc = (KeyCode)w;
	if (!_mapKeyDown[(int)kc] && !_mapKeyPress[(int)kc])
	{
		_mapKeyDown[(int)kc] = true;
		_mapKeyPress[(int)kc] = true;
	}
	return true;
}
bool KeyboardInput::HandleKeyUp(WORD w)
{
	KeyCode kc = (KeyCode)w;
	_mapKeyUp[(int)kc] = true;
	_mapKeyPress[(int)kc] = false;
	return true;
}
void KeyboardInput::AddInputChar(ushort c)
{
	_characters.Add(c);
}
bool KeyboardInput::GetKey(KeyCode kc)
{
	return _mapKeyPress[(int)kc];
}
bool KeyboardInput::GetKeyDown(KeyCode kc)
{
	return _mapKeyDown[(int)kc];
}
bool KeyboardInput::GetKeyUp(KeyCode kc)
{
	return _mapKeyUp[(int)kc];
}
void KeyboardInput::Initialize()
{
	memset(_mapKeyDown, 0, sizeof(_mapKeyDown));
	memset(_mapKeyUp, 0, sizeof(_mapKeyUp));
	memset(_mapKeyPress, 0, sizeof(_mapKeyPress));
	_characters.Clear();
}
void KeyboardInput::Reset()
{
	memset(_mapKeyDown, 0, sizeof(_mapKeyDown));
	memset(_mapKeyUp, 0, sizeof(_mapKeyUp));
	_characters.Clear();
}
DC_END_NAMESPACE
