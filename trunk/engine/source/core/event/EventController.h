 
/*****************************************************************************************************/
// @author hannibal
// @date   2015/04/29
// @brief  事件
/*****************************************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
typedef void (Object::*EventHandler)();
#define MakeEventHandler(_SELECTOR) static_cast<EventHandler>(&_SELECTOR)
typedef void (Object::*EventHandler1)(const String&);
#define MakeEventHandler1(_SELECTOR) static_cast<EventHandler1>(&_SELECTOR)
typedef void (Object::*EventHandler2)(const String&, const String&);
#define MakeEventHandler2(_SELECTOR) static_cast<EventHandler2>(&_SELECTOR)
typedef void (Object::*EventHandler3)(const String&, const String&, const String&);
#define MakeEventHandler3(_SELECTOR) static_cast<EventHandler3>(&_SELECTOR)
typedef void (Object::*EventHandler4)(const String&, const String&, const String&, const String&);
#define MakeEventHandler4(_SELECTOR) static_cast<EventHandler4>(&_SELECTOR)
/********************************************************************/
//派发事件
class ENGINE_DLL EventController : public Object
{
	struct EventItem
	{
		Object	*object = nullptr;
		bool	is_notify = false;
		EventHandler handler = nullptr;
		EventHandler1 handler1 = nullptr;
		EventHandler2 handler2 = nullptr;
		EventHandler3 handler3 = nullptr;
		EventHandler4 handler4 = nullptr;
		EventItem(Object* obj, const EventHandler& fun) :object(obj), handler(fun), is_notify(false) {}
		EventItem(Object* obj, const EventHandler1& fun) :object(obj), handler1(fun), is_notify(false) {}
		EventItem(Object* obj, const EventHandler2& fun) :object(obj), handler2(fun), is_notify(false) {}
		EventItem(Object* obj, const EventHandler3& fun) :object(obj), handler3(fun), is_notify(false) {}
		EventItem(Object* obj, const EventHandler4& fun) :object(obj), handler4(fun), is_notify(false) {}
	};

	typedef std::multimap<String, EventItem> map_event_type;
	typedef map_event_type::iterator map_it;
	typedef std::pair<map_it, map_it> map_range_type;
	DISALLOW_COPY_ASSIGN(EventController);
	BEGIN_DERIVED_REFECTION_TYPE(EventController, Object)
	END_DERIVED_REFECTION_TYPE;

public:
	EventController(){ _dispatch_count = 0, _is_dispatching = false; }
	virtual ~EventController(){}

public:
	bool AddEventListener(const String& type, Object *object, const EventHandler &selector);
	bool AddEventListener(const String& type, Object *object, const EventHandler1 &selector);
	bool AddEventListener(const String& type, Object *object, const EventHandler2 &selector);
	bool AddEventListener(const String& type, Object *object, const EventHandler3 &selector);
	bool AddEventListener(const String& type, Object *object, const EventHandler4 &selector);
	bool RemoveEventListener(const String& type, Object *object, const EventHandler &selector);
	bool RemoveEventListener(const String& type, Object *object, const EventHandler1 &selector);
	bool RemoveEventListener(const String& type, Object *object, const EventHandler2 &selector);
	bool RemoveEventListener(const String& type, Object *object, const EventHandler3 &selector);
	bool RemoveEventListener(const String& type, Object *object, const EventHandler4 &selector);
	void TriggerEvent(const String& type);
	void TriggerEvent(const String& type, const String& arg1);
	void TriggerEvent(const String& type, const String& arg1, const String& arg2);
	void TriggerEvent(const String& type, const String& arg1, const String& arg2, const String& arg3);
	void TriggerEvent(const String& type, const String& arg1, const String& arg2, const String& arg3, const String& arg4);

	void Clear();

private:
	void TriggerEventImpl(int flag, const String& type, const String& arg1, const String& arg2, const String& arg3, const String& arg4);

private:
	map_event_type _map_event;		//事件集合

	int	_dispatch_count = 0;		//可以解决循环派发，导致_is_dispatching失效问题
	bool _is_dispatching = false;	//防止派发事件的时候触发删除，导致迭代器失效
};
DC_END_NAMESPACE
