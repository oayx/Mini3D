#include "Object.h" 
#include "Point.h"
#include "Size.h"
#include "Rect.h"
#include "Numberic.h"
#include "geometry/Plane.h"
#include "geometry/Ray.h"
#include "geometry/Sphere.h"
#include "geometry/Triangle.h"
#include "stl/BitsArray.h"
#include "stl/ByteArray.h"
#include "time/ClockTime.h"
#include "time/IntervalTime.h"
#include "runtime/graphics/Material.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(object);
IMPL_REFECTION_TYPE(Object); 
IMPL_REFECTION_TYPE(Sphere);
IMPL_REFECTION_TYPE(Triangle);
IMPL_REFECTION_TYPE(ByteOrder);
IMPL_REFECTION_TYPE(Bool);
IMPL_REFECTION_TYPE(BitsArray);
IMPL_REFECTION_TYPE(tPoint);
IMPL_REFECTION_TYPE(tSize);
IMPL_REFECTION_TYPE(tRect);
IMPL_DERIVED_REFECTION_TYPE(ClockTime, Object);
IMPL_DERIVED_REFECTION_TYPE(LogicIntervalTime, Object);
IMPL_DERIVED_REFECTION_TYPE(IntervalTime,Object);
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(Object);
template<class TransferFunction> inline
void Object::Transfer(TransferFunction& transfer, void* name)
{
	TRANSFER_SIMPLE(_instanceName);
}
void Object::Release()
{
	Reference::Release();
	if (_refCount == 0)
	{
		AutoreleasePool::DeleteObject(this);
		Object* object = this;
		SAFE_DELETE(object);
	}
}
void Object::AutoRelease() noexcept
{
	_isAutoRelease = true;
	AutoreleasePool::AddObject(this);
}
/********************************************************************/
AutoreleasePool::Pools* AutoreleasePool::_appendList = Memory::New<AutoreleasePool::Pools>();
AutoreleasePool::Pools* AutoreleasePool::_releasePools = Memory::New<AutoreleasePool::Pools>();
std::mutex AutoreleasePool::_mutex;
void AutoreleasePool::Destroy()
{
	SAFE_DELETE(_appendList);
	SAFE_DELETE(_releasePools);
}
void AutoreleasePool::AddObject(Object* object)
{
	AssertEx(std::find(_appendList->begin(), _appendList->end(), object) == _appendList->end(), "have already added");
	{
		LOCK(_mutex);
		_appendList->Add(object);
	}
}
void AutoreleasePool::DeleteObject(Object* object)
{
	{
		LOCK(_mutex);
		_appendList->Remove(object);
	}
}
void AutoreleasePool::Update()
{
	DC_PROFILE_FUNCTION;
	if (!_appendList->IsEmpty())
	{
		auto runList = _appendList;
		{
			LOCK(_mutex);
			_appendList = _releasePools;
			_releasePools = runList;
		}

		for (auto it = runList->begin(); it != runList->end(); ++it)
		{
			Object* obj = *it;
			if (obj->GetRefCount() == 0)//引用次数初始为0
			{
				SAFE_RELEASE(obj);
			}
		}
		runList->Clear();
	}
}
DC_END_NAMESPACE