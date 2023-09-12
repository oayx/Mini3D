#include "EventDispatcher.h"
 
DC_BEGIN_NAMESPACE
/*****************************************************************************************************/
IMPL_DERIVED_REFECTION_TYPE(EventDispatcher, Object);
EventController EventDispatcher::_eventController;
bool EventDispatcher::AddEventListener(const String& type, Object *object, const EventHandler &selector)
{
	return _eventController.AddEventListener(type, object, selector);
}
bool EventDispatcher::AddEventListener(const String& type, Object *object, const EventHandler1 &selector)
{
	return _eventController.AddEventListener(type, object, selector);
}
bool EventDispatcher::AddEventListener(const String& type, Object *object, const EventHandler2 &selector)
{
	return _eventController.AddEventListener(type, object, selector);
}
bool EventDispatcher::AddEventListener(const String& type, Object *object, const EventHandler3 &selector)
{
	return _eventController.AddEventListener(type, object, selector);
}
bool EventDispatcher::AddEventListener(const String& type, Object *object, const EventHandler4 &selector)
{
	return _eventController.AddEventListener(type, object, selector);
}
bool EventDispatcher::RemoveEventListener(const String& type, Object *object, const EventHandler &selector)
{
	return _eventController.RemoveEventListener(type, object, selector);
}
bool EventDispatcher::RemoveEventListener(const String& type, Object *object, const EventHandler1 &selector)
{
	return _eventController.RemoveEventListener(type, object, selector);
}
bool EventDispatcher::RemoveEventListener(const String& type, Object *object, const EventHandler2 &selector)
{
	return _eventController.RemoveEventListener(type, object, selector);
}
bool EventDispatcher::RemoveEventListener(const String& type, Object *object, const EventHandler3 &selector)
{
	return _eventController.RemoveEventListener(type, object, selector);
}
bool EventDispatcher::RemoveEventListener(const String& type, Object *object, const EventHandler4 &selector)
{
	return _eventController.RemoveEventListener(type, object, selector);
}
void EventDispatcher::TriggerEvent(const String& type)
{
	_eventController.TriggerEvent(type);
}
void EventDispatcher::TriggerEvent(const String& type, const String& arg1)
{
	_eventController.TriggerEvent(type, arg1);
}
void EventDispatcher::TriggerEvent(const String& type, const String& arg1, const String& arg2)
{
	_eventController.TriggerEvent(type, arg1, arg2);
}
void EventDispatcher::TriggerEvent(const String& type, const String& arg1, const String& arg2, const String& arg3)
{
	_eventController.TriggerEvent(type, arg1, arg2, arg3);
}
void EventDispatcher::TriggerEvent(const String& type, const String& arg1, const String& arg2, const String& arg3, const String& arg4)
{
	_eventController.TriggerEvent(type, arg1, arg2, arg3, arg4);
}
void EventDispatcher::Clear()
{
	_eventController.Clear();
}
DC_END_NAMESPACE
