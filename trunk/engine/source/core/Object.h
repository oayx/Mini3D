 
/*****************************************************************************************************/
// @author hannibal
// @date   2020/02/13
// @brief  基础类
/*****************************************************************************************************/
#pragma once

#include "RTTI.h"
#include "Reference.h"
#include "AutoPointer.h"
#include "Numberic.h"
#include "Rect.h"
#include "Size.h"
#include "String.h"

#include "color/ColorConverter.h"

#include "file/Directory.h"
#include "file/File.h"
#include "file/Path.h"

#include "geometry/Plane.h"
#include "geometry/Ray.h"
#include "geometry/Sphere.h"
#include "geometry/Triangle.h"

#include "math/Aabb.h"
#include "math/Math.h"
#include "math/Matrix3.h"
#include "math/Matrix4.h"
#include "math/Quaternion.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

#include "stl/Array.h"
#include "stl/AVLTree.h"
#include "stl/BitsArray.h"
#include "stl/ByteArray.h"
#include "stl/HashMap.h"
#include "stl/HashTable.h"
#include "stl/List.h"
#include "stl/Map.h"
#include "stl/Queue.h"
#include "stl/RBTree.inl"
#include "stl/Set.h"
#include "stl/Stack.h"
#include "stl/Vector.h"

#include "serialize/SerializeRead.h"
#include "serialize/SerializeWrite.h"
#include "platform/PlatformDefine.h"

DC_BEGIN_NAMESPACE
#define DECLARE_OBJECT_CLONE \
public: \
	virtual Object* Clone(Object* newObj)override; \
private:

/****************************************************************************************************/
class ENGINE_DLL Object : public MetaObject, public Reference, public object
{
	DECLARE_SERIALIZE(Object);
	BEGIN_REFECTION_TYPE(Object)
	END_REFECTION_TYPE;

public:
	Object()
	{
		static std::atomic_uint64_t s_id = 0;
		_instanceId = ++s_id;
		_instanceName = std::string("Instance_") + std::to_string(_instanceId);
	}
	Object(const Object& obj)
	{
		//undo
	}
	virtual ~Object()
	{
	}
	Object& operator=(const Object& obj) noexcept
	{
		//undo
		return *this;
	}
	bool operator== (const Object& obj)const noexcept
	{
		return _instanceId == obj._instanceId;
	}
	bool operator!= (const Object& obj)const noexcept
	{
		return _instanceId != obj._instanceId;
	}

public:
	constexpr uint64 GetInstanceId() const noexcept { return _instanceId; }
	virtual const String& GetInstanceName() const { return _instanceName; }
	void SetInstanceName(const String& name) noexcept { if(!name.IsEmpty()) _instanceName = name; }
	virtual String GetClassId()const noexcept { return this->GetType()->GetName(); }

	virtual void Serialize(SerializeRead& transfer, void* ptr) {}
	virtual void Serialize(SerializeWrite& transfer, void* ptr) {}
	virtual void Deserialize(SerializeRead& transfer, void* ptr) {}
	virtual void Deserialize(SerializeWrite& transfer, void* ptr) {}
	virtual void Serialize() {}
	virtual void Deserialize() {}

	template<class T>
	bool Is()const noexcept { return this->GetType()->Is(typeof(T)); }
	template<class T>
	bool IsSame()const  noexcept { return this->GetType()->GetName() == typeof(T)->GetName(); }
	bool IsSame(const String& name) noexcept { return this->GetClassId() == name; }

	template<class T>
	T* As()const noexcept { if (this->Is<T>()) return (T*)this; return nullptr; }

	virtual String	ToString() { return ""; }
	virtual Object* Clone(Object* newObj) { newObj->SetInstanceName(_instanceName); return newObj; }

	virtual void	Release()override;
	void			AutoRelease() noexcept;
	bool			IsAutoRelease()const noexcept { return _isAutoRelease; }

public:

protected:
	uint64				_instanceId = 0;
	String				_instanceName = "";
	std::atomic_bool	_isAutoRelease = false;
	std::atomic_bool	_isAutoPointer = false;
};

//自动释放对象
class AutoreleasePool
{
	friend class Object;
	friend class Application;
	typedef List<Object*> Pools;

private:
	static void Destroy();
	static void AddObject(Object* object);
	static void DeleteObject(Object* object);
	static void Update();

private:
	static Pools*		_appendList;
	static Pools*		_releasePools;
	static std::mutex	_mutex;
};
DC_END_NAMESPACE
