 
/*****************************************************************************************************/
// @author hannibal
// @date   2015/04/29
// @brief  事件，需要继承IEvent，并实现接口函数
/*****************************************************************************************************/
#pragma once

#include "EventController.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
//派发事件
class ENGINE_DLL EventDispatcher Final : public Object
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(EventDispatcher);
	BEGIN_DERIVED_REFECTION_TYPE(EventDispatcher, Object)
	END_DERIVED_REFECTION_TYPE;

public:
	static bool AddEventListener(const String& type, Object *object, const EventHandler &selector);
	static bool AddEventListener(const String& type, Object *object, const EventHandler1 &selector);
	static bool AddEventListener(const String& type, Object *object, const EventHandler2 &selector);
	static bool AddEventListener(const String& type, Object *object, const EventHandler3 &selector);
	static bool AddEventListener(const String& type, Object *object, const EventHandler4 &selector);
	static bool RemoveEventListener(const String& type, Object *object, const EventHandler &selector);
	static bool RemoveEventListener(const String& type, Object *object, const EventHandler1 &selector);
	static bool RemoveEventListener(const String& type, Object *object, const EventHandler2 &selector);
	static bool RemoveEventListener(const String& type, Object *object, const EventHandler3 &selector);
	static bool RemoveEventListener(const String& type, Object *object, const EventHandler4 &selector);
	static void TriggerEvent(const String& type);
	static void TriggerEvent(const String& type, const String& arg1);
	static void TriggerEvent(const String& type, const String& arg1, const String& arg2);
	static void TriggerEvent(const String& type, const String& arg1, const String& arg2, const String& arg3);
	static void TriggerEvent(const String& type, const String& arg1, const String& arg2, const String& arg3, const String& arg4);

	static void Clear();

private:
	static EventController _eventController;
};
DC_END_NAMESPACE
