 
/*****************************************************************************
* Author： hannibal
* Date：2020/8/11
* Description： 输入系统
*****************************************************************************/
#pragma once

#include "InputDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// 输入系统
class ENGINE_DLL TouchInput Final : public Object
{
	typedef List<Touch> Touches;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(TouchInput);
	BEGIN_DERIVED_REFECTION_TYPE(TouchInput, Object)
	END_DERIVED_REFECTION_TYPE;

public:
	static int GetTouchCount() { return _touches.Size(); }
	static bool GetTouch(int index, Touch& touch);
	static void AddTouch(Touch touch);
	static void UpdateTouch(Touch touch);
	static bool GetMultiTouchEnabled();
	static void SetMultiTouchEnabled(bool b);

	static void Initialize() {}
	static void Reset();

private:
	static Touches _touches;
	static bool _multiTouchEnabled;
};
DC_END_NAMESPACE
