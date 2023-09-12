﻿
/*****************************************************************************
* Author： hannibal
* Date：2021/5/11
* Description：智能指针
*****************************************************************************/
#pragma once

#include "BaseType.h"
#include "Assert.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
template <class T>
class AutoPointer Final : public object
{
public:
	AutoPointer (T* object = 0);
	AutoPointer (const AutoPointer& pointer);
	~AutoPointer ();
		
	T& operator* () const { return *_object; }
	T* operator-> () const { return _object; }
		
	AutoPointer& operator= (T* object);
	AutoPointer& operator= (const AutoPointer& ref);
		
	bool operator== (T* object) const { return (_object == object); }
	bool operator!= (T* object) const { return (_object != object); }
	bool operator== (const AutoPointer& ref) const { return (_object == ref._object); }
	bool operator!= (const AutoPointer& ref) const { return (_object != ref._object); }
	operator T*()const { return _object; }
		
	T * Get()const { return _object; }

private:
	T* _object = nullptr;
	std::mutex _mutex;
};

template <class T>
AutoPointer<T>::AutoPointer(T* object)
{
	AssertEx(!object->IsAutoRelease(), "AutoPointer cannot is a AutoRelease Object");
	_object = object;
	if (_object)
	{
		_object->Retain();
	}
}
template <class T>
AutoPointer<T>::AutoPointer(const AutoPointer& pointer)
{
	AssertEx(!pointer.Get()->IsAutoRelease(), "AutoPointer cannot is a AutoRelease Object");
	_object = pointer._object;
	if (_object)
	{
		_object->Retain();
	}
}
template <class T>
AutoPointer<T>::~AutoPointer()
{
	if (_object)
	{
		_object->Release();
		_object = NULL;
	}
}
template <class T>
AutoPointer<T>& AutoPointer<T>::operator= (T* object)
{
	if (_object != object)
	{
		if (object)
		{
			object->Retain();
		}

		if (_object)
		{
			_object->Release();
		}

		{
			thread_lock(_mutex);
			_object = object;
		}
	}
	return *this;
}
template <class T>
AutoPointer<T>& AutoPointer<T>::operator= (const AutoPointer& pointer)
{
	if (_object != pointer._object)
	{
		if (pointer._object)
		{
			pointer._object->Retain();
		}

		if (_object)
		{
			_object->Release();
		}

		{
			thread_lock(_mutex);
			_object = pointer._object;
		}
	}
	return *this;
}
DC_END_NAMESPACE