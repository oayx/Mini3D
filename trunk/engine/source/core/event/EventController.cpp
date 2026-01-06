#include "EventController.h"

DC_BEGIN_NAMESPACE
/*****************************************************************************************************/
IMPL_DERIVED_REFECTION_TYPE(EventController, Object);
bool EventController::AddEventListener(const String& type, Object *object, const EventHandler &selector)
{
	map_range_type it_range = _mapEventes.equal_range(type);
	for (map_it it = it_range.first; it != it_range.second; ++it)
	{
		EventItem& item = it->second;
		if (item.object == object && item.handler == selector)
		{
			Debuger::Error("EventController::AddEventListener - already existed:%s\n", type.c_str());
			return false;
		}
	}

	EventItem item(object, selector);
	_mapEventes.insert(map_event_type::value_type(type, item));

	return true;
}
bool EventController::AddEventListener(const String& type, Object *object, const EventHandler1 &selector)
{
	map_range_type it_range = _mapEventes.equal_range(type);
	for (map_it it = it_range.first; it != it_range.second; ++it)
	{
		EventItem& item = it->second;
		if (item.object == object && item.handler1 == selector)
		{
			Debuger::Error("EventController::AddEventListener - already existed:%s\n", type.c_str());
			return false;
		}
	}

	EventItem item(object, selector);
	_mapEventes.insert(map_event_type::value_type(type, item));

	return true;
}
bool EventController::AddEventListener(const String& type, Object *object, const EventHandler2 &selector)
{
	map_range_type it_range = _mapEventes.equal_range(type);
	for (map_it it = it_range.first; it != it_range.second; ++it)
	{
		EventItem& item = it->second;
		if (item.object == object && item.handler2 == selector)
		{
			Debuger::Error("EventController::AddEventListener - already existed:%s\n", type.c_str());
			return false;
		}
	}

	EventItem item(object, selector);
	_mapEventes.insert(map_event_type::value_type(type, item));

	return true;
}
bool EventController::AddEventListener(const String& type, Object *object, const EventHandler3 &selector)
{
	map_range_type it_range = _mapEventes.equal_range(type);
	for (map_it it = it_range.first; it != it_range.second; ++it)
	{
		EventItem& item = it->second;
		if (item.object == object && item.handler3 == selector)
		{
			Debuger::Error("EventController::AddEventListener - already existed:%s\n", type.c_str());
			return false;
		}
	}

	EventItem item(object, selector);
	_mapEventes.insert(map_event_type::value_type(type, item));

	return true;
}
bool EventController::AddEventListener(const String& type, Object *object, const EventHandler4 &selector)
{
	map_range_type it_range = _mapEventes.equal_range(type);
	for (map_it it = it_range.first; it != it_range.second; ++it)
	{
		EventItem& item = it->second;
		if (item.object == object && item.handler4 == selector)
		{
			Debuger::Error("EventController::AddEventListener - already existed:%s\n", type.c_str());
			return false;
		}
	}

	EventItem item(object, selector);
	_mapEventes.insert(map_event_type::value_type(type, item));

	return true;
}
bool EventController::RemoveEventListener(const String& type, Object *object, const EventHandler &selector)
{
	bool ret = false;
	map_range_type it_range = _mapEventes.equal_range(type);
	map_it it = it_range.first;
	while (it != it_range.second)
	{
		EventItem& item = it->second;
		if (item.object == object && item.handler == selector)
		{
			_mapEventes.erase(it);
			ret = true;
			break;
		}
		else
		{
			++it;
		}
	}
	return ret;
}
bool EventController::RemoveEventListener(const String& type, Object *object, const EventHandler1 &selector)
{
	bool ret = false;
	map_range_type it_range = _mapEventes.equal_range(type);
	map_it it = it_range.first;
	while (it != it_range.second)
	{
		EventItem& item = it->second;
		if (item.object == object && item.handler1 == selector)
		{
			_mapEventes.erase(it);
			ret = true;
			break;
		}
		else
		{
			++it;
		}
	}
	return ret;
}
bool EventController::RemoveEventListener(const String& type, Object *object, const EventHandler2 &selector)
{
	bool ret = false;
	map_range_type it_range = _mapEventes.equal_range(type);
	map_it it = it_range.first;
	while (it != it_range.second)
	{
		EventItem& item = it->second;
		if (item.object == object && item.handler2 == selector)
		{
			_mapEventes.erase(it);
			ret = true;
			break;
		}
		else
		{
			++it;
		}
	}
	return ret;
}
bool EventController::RemoveEventListener(const String& type, Object *object, const EventHandler3 &selector)
{
	bool ret = false;
	map_range_type it_range = _mapEventes.equal_range(type);
	map_it it = it_range.first;
	while (it != it_range.second)
	{
		EventItem& item = it->second;
		if (item.object == object && item.handler3 == selector)
		{
			_mapEventes.erase(it);
			ret = true;
			break;
		}
		else
		{
			++it;
		}
	}
	return ret;
}
bool EventController::RemoveEventListener(const String& type, Object *object, const EventHandler4 &selector)
{
	bool ret = false;
	map_range_type it_range = _mapEventes.equal_range(type);
	map_it it = it_range.first;
	while (it != it_range.second)
	{
		EventItem& item = it->second;
		if (item.object == object && item.handler4 == selector)
		{
			_mapEventes.erase(it);
			ret = true;
			break;
		}
		else
		{
			++it;
		}
	}
	return ret;
}
void EventController::TriggerEvent(const String& type)
{
	TriggerEventImpl(0, type, "", "", "", "");
}
void EventController::TriggerEvent(const String& type, const String& arg1)
{
	TriggerEventImpl(1, type, arg1, "", "", "");
}
void EventController::TriggerEvent(const String& type, const String& arg1, const String& arg2)
{
	TriggerEventImpl(2, type, arg1, arg2, "", "");
}
void EventController::TriggerEvent(const String& type, const String& arg1, const String& arg2, const String& arg3)
{
	TriggerEventImpl(3, type, arg1, arg2, arg3,"");
}
void EventController::TriggerEvent(const String& type, const String& arg1, const String& arg2, const String& arg3, const String& arg4)
{
	TriggerEventImpl(4, type, arg1, arg2, arg3, arg4);
}
void EventController::TriggerEventImpl(int flag, const String& type, const String& arg1, const String& arg2, const String& arg3, const String& arg4)
{
	++_dispatchCount;
	//派发事件
	bool has_notify = true;
	while (has_notify)
	{
		has_notify = false;
		map_range_type it_range = _mapEventes.equal_range(type);
		for (map_it it = it_range.first; it != it_range.second; ++it)
		{
			EventItem& item = it->second;
			if (!item.is_notify)
			{
				has_notify = true;
				item.is_notify = true;
				if (item.object)
				{
					switch (flag)
					{
					case 0:(item.object->*(item.handler))(); break;
					case 1:(item.object->*(item.handler1))(arg1); break;
					case 2:(item.object->*(item.handler2))(arg1, arg2); break;
					case 3:(item.object->*(item.handler3))(arg1, arg2, arg3); break;
					case 4:(item.object->*(item.handler4))(arg1, arg2, arg3, arg4); break;
					}
				}
				break;
			}
		}
	}

	--_dispatchCount;
	if (_dispatchCount == 0)
	{
		map_range_type it_range = _mapEventes.equal_range(type);
		for (map_it it = it_range.first; it != it_range.second; ++it)
		{
			EventItem& item = it->second;
			item.is_notify = false;
		}
	}
}
void EventController::Clear()
{
	_mapEventes.clear();
}
DC_END_NAMESPACE
