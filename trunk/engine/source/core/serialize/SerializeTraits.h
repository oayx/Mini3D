
/*****************************************************************************
* Author： hannibal
* Description：序列化
*****************************************************************************/
#pragma once
#include "core/String.h"
#include "core/Size.h"
#include "core/Rect.h"
#include "core/math/Vector2.h"
#include "core/math/Vector3.h"
#include "core/math/Vector4.h"
#include "core/math/Quaternion.h"
#include "core/color/Color.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
template<class T>
class SerializeTraitsBase
{
public:
	inline static bool IsBasicType()
	{
		return false;
	}

	template<class TransferFunction>
	inline static void TransferEnum(T& data, void* name, TransferFunction& transfer)
	{
		int v = (int)data;
		transfer.TransferBasicData(v, name);
		data = (T)v;
	}
};

template<typename T>
class SerializeTraits : public SerializeTraitsBase<T> 
{
public:
	typedef T value_type;

	template<class TransferFunction>
	inline static void Transfer(T& data, void* name, TransferFunction& transfer)
	{
		data.Transfer(transfer, name);
	}
};

template<class T>
class SerializeTraitsForBasicType : public SerializeTraitsBase<T>
{
public:
	inline static bool IsBasicType()
	{
		return true;
	}
	template<class TransferFunction>
	inline static void Transfer(T& data, void* name, TransferFunction& transfer)
	{
		transfer.TransferBasicData(data, name);
	}
};

template<>
class SerializeTraits<bool> : public SerializeTraitsForBasicType<bool> {};

template<>
class SerializeTraits<char> : public SerializeTraitsForBasicType<char> {};

template<>
class SerializeTraits<byte> : public SerializeTraitsForBasicType<byte> {};

template<>
class SerializeTraits<short> : public SerializeTraitsForBasicType<short> {};

template<>
class SerializeTraits<ushort> : public SerializeTraitsForBasicType<ushort> {};

template<>
class SerializeTraits<int> : public SerializeTraitsForBasicType<int> {};

template<>
class SerializeTraits<uint> : public SerializeTraitsForBasicType<uint> {};

template<>
class SerializeTraits<int64> : public SerializeTraitsForBasicType<int64> {};

template<>
class SerializeTraits<uint64> : public SerializeTraitsForBasicType<uint64> {};

template<>
class SerializeTraits<float> : public SerializeTraitsForBasicType<float> {};

template<>
class SerializeTraits<double> : public SerializeTraitsForBasicType<double> {};

template<>
class SerializeTraits<std::string> : public SerializeTraitsBase<std::string>
{
public:
	template<class TransferFunction>
	inline static void Transfer(std::string& data, void* name, TransferFunction& transfer)
	{
		transfer.TransferStringData(data, name);
	}
};
template<>
class SerializeTraits<String> : public SerializeTraitsBase<String>
{
public:
	template<class TransferFunction>
	inline static void Transfer(String& data, void* name, TransferFunction& transfer)
	{
		transfer.TransferStringData(data, name);
	}
};

template<>
class SerializeTraits<Color> : public SerializeTraitsBase<Color>
{
public:
	template<class TransferFunction>
	inline static void Transfer(Color& data, void* name, TransferFunction& transfer)
	{
		transfer.TransferColorData(data, name);
	}
};
template<>
class SerializeTraits<Vector2> : public SerializeTraitsBase<Vector2>
{
public:
	template<class TransferFunction>
	inline static void Transfer(Vector2& data, void* name, TransferFunction& transfer)
	{
		transfer.TransferVector2Data(data, name);
	}
};
template<>
class SerializeTraits<Vector3> : public SerializeTraitsBase<Vector3>
{
public:
	template<class TransferFunction>
	inline static void Transfer(Vector3& data, void* name, TransferFunction& transfer)
	{
		transfer.TransferVector3Data(data, name);
	}
};
template<>
class SerializeTraits<Vector4> : public SerializeTraitsBase<Vector4>
{
public:
	template<class TransferFunction>
	inline static void Transfer(Vector4& data, void* name, TransferFunction& transfer)
	{
		transfer.TransferVector4Data(data, name);
	}
};
template<>
class SerializeTraits<Quaternion> : public SerializeTraitsBase<Quaternion>
{
public:
	template<class TransferFunction>
	inline static void Transfer(Quaternion& data, void* name, TransferFunction& transfer)
	{
		transfer.TransferQuaternionData(data, name);
	}
};
template<>
class SerializeTraits<Size> : public SerializeTraitsBase<Size>
{
public:
	template<class TransferFunction>
	inline static void Transfer(Size& data, void* name, TransferFunction& transfer)
	{
		transfer.TransferSizeData(data, name);
	}
};
template<>
class SerializeTraits<iSize> : public SerializeTraitsBase<iSize>
{
public:
	template<class TransferFunction>
	inline static void Transfer(iSize& data, void* name, TransferFunction& transfer)
	{
		transfer.TransferiSizeData(data, name);
	}
};
template<>
class SerializeTraits<Rect> : public SerializeTraitsBase<Rect>
{
public:
	template<class TransferFunction>
	inline static void Transfer(Rect& data, void* name, TransferFunction& transfer)
	{
		transfer.TransferRectData(data, name);
	}
};
template<class T>
class SerializeTraits<Vector<T>> : public SerializeTraitsBase<Vector<T>>
{
public:
	template<class TransferFunction>
	inline static void Transfer(Vector<T>& data, void* name, TransferFunction& transfer)
	{
		transfer.TransferVectorData(data, name);
	}
};
DC_END_NAMESPACE