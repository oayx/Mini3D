 
/*****************************************************************************
* Author： hannibal
* Date：2020/2/12
* Description：鼠标输入
*****************************************************************************/
#pragma once

#include "runtime/input/InputDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// 鼠标输入
class ENGINE_DLL MouseInput final : public Object
{
	friend class Input;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(MouseInput);
	BEGIN_DERIVED_REFECTION_TYPE(MouseInput, Object)
	END_REFECTION_TYPE;

public://消息处理
	static bool HandleLBtnDown(float x_pos, float y_pos);
	static bool HandleRBtnDown(float x_pos, float y_pos);
	static bool HandleMBtnDown(float x_pos, float y_pos);
	static bool HandleLBtnUp(float x_pos, float y_pos);
	static bool HandleRBtnUp(float x_pos, float y_pos);
	static bool HandleMBtnUp(float x_pos, float y_pos);

	static bool HandleLDClick(float x_pos, float y_pos);
	static bool HandleRDClick(float x_pos, float y_pos);
	static bool HandleMDClick(float x_pos, float y_pos);

	static bool HandleInputPosition(float x_pos, float y_pos);
	static bool HandleInputMove(float x_offset, float y_offset);
	static bool HandleInputWheel(float x, float y);

public:
	static bool GetMouseButton(MouseBtnID button);
	static bool GetMouseButtonDown(MouseBtnID button);
	static bool GetMouseButtonUp(MouseBtnID button);
	static bool GetMouseDClick(MouseBtnID button);

	static bool IsMouseMove() { return _isMouseMove; }
	static Vector2 GetScrollDelta() { return _mouseScrollDelta; }

private:
	static void Initialize();
	static void Reset();

private:
	inline static bool _isMouseMove = false;
	inline static Vector2 _mouseScrollDelta = Vector2::zero;
	inline static bool _mapMouseBtnDown[int(MouseBtnID::Max)] = { false };
	inline static bool _mapMouseBtnUp[int(MouseBtnID::Max)] = { false };
	inline static bool _mapMouseBtnPress[int(MouseBtnID::Max)] = { false };
	inline static bool _mapMouseDClick[int(MouseBtnID::Max)] = { false };
};//MouseInput
DC_END_NAMESPACE
