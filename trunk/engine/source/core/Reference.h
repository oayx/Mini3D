 
/*****************************************************************************************************/
// @author hannibal
// @date   2020/02/13
// @brief  引用计数
/*****************************************************************************************************/
#pragma once

#include "BaseType.h"

DC_BEGIN_NAMESPACE
/****************************************************************************************************/
class ENGINE_DLL Reference : public object
{
public:
	Reference()
	{
	}
	virtual ~Reference()
	{
	}

public:
	void Retain() 
	{
		++_refCount;
	}
	virtual void Release() 
	{
		if (_refCount > 0)
		{
			--_refCount;
		}
	}
	uint GetRefCount()const { return _refCount; }

protected:
	std::atomic_int _refCount = 0;
};
DC_END_NAMESPACE