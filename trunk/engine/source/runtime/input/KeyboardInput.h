 
/*****************************************************************************
* Author： hannibal
* Date：2020/2/12
* Description：键盘输入
*****************************************************************************/
#pragma once

#include "runtime/input/InputDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// 键盘输入
class ENGINE_DLL KeyboardInput final : public Object
{
	friend class Input;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(KeyboardInput);
	BEGIN_DERIVED_REFECTION_TYPE(KeyboardInput, Object)
	END_REFECTION_TYPE;

public://消息处理
	static bool HandleKeyDown(WORD w);
	static bool HandleKeyUp(WORD w);

	static void AddInputChar(ushort c);

public:
	static bool GetKey(KeyCode kc);
	static bool GetKeyDown(KeyCode kc);
	static bool GetKeyUp(KeyCode kc);

	static const List<ushort>& GetInputChar() { return _characters; }

private:
	static void Initialize();
	static void Reset();

private:
	inline static bool _mapKeyDown[int(KeyCode::Max)];
	inline static bool _mapKeyUp[int(KeyCode::Max)];
	inline static bool _mapKeyPress[int(KeyCode::Max)];

	inline static List<ushort> _characters;
};//KeyboardInput
DC_END_NAMESPACE
