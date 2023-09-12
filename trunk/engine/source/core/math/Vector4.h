 
/*****************************************************************************
* Author： hannibal
* Date：2010/1/3
* Description： 
*****************************************************************************/
#pragma once

#include "Vector3.h"
#include "core/stl/Vector.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
#pragma pack(push,4)
class ENGINE_DLL Vector4 Final : public object
{
	BEGIN_REFECTION_TYPE(Vector4)
	END_FINAL_REFECTION_TYPE;

public:
	union
	{
		struct{ float x, y, z, w; };
		float p[4] = { 0.0f };
	};

public:
	Vector4(): x(0), y(0), z(0), w(0){}
	Vector4(float fX, float fY, float fZ, float fW) : x(fX), y(fY), z(fZ), w(fW) {}
	Vector4(const Vector3& rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(1.0f) {}
	Vector4(const Vector4& rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}
	Vector4(const String& str)
	{
		Vector<float> vec = str.Split<float>(',');
		if (vec.Size() == 4)
		{
			x = vec[0]; y = vec[1]; z = vec[2]; w = vec[3];
		}
	}

	float operator [] (const size_t i) const { assert(i < 4); return *(&x + i); }
	float& operator [] ( const size_t i ){assert( i < 4 );return *(p+i);}

	Vector4& operator = ( const Vector4& vec ){x = vec.x;y = vec.y;z = vec.z;w = vec.w;return *this;}
	Vector4& operator = ( const float scale){x = scale;y = scale;z = scale;w = scale;return *this;}

	bool operator == ( const Vector4& vec ) const{return this->Equals(vec);}
	bool operator != ( const Vector4& vec ) const{return !this->Equals(vec);}

	Vector4& operator = (const Vector3& rhs){x = rhs.x;y = rhs.y;z = rhs.z;w = 1.0f;return *this;}
	Vector4 operator + ( const Vector4& vec ) const{return Vector4(x + vec.x,y + vec.y,z + vec.z,w + vec.w);}
	Vector4 operator - ( const Vector4& vec ) const{return Vector4(x - vec.x,y - vec.y,z - vec.z,w - vec.w);}
	Vector4 operator * ( const float scale ) const{return Vector4(x * scale,y * scale,z * scale,w * scale);}
	Vector4 operator * ( const Vector4& rhs) const{return Vector4(rhs.x * x,rhs.y * y,rhs.z * z,rhs.w * w);}
	Vector4 operator / ( const float scale ) const{assert( scale != 0.0f );float fInv = 1.0f / scale;return Vector4(x * fInv,y * fInv,z * fInv,w * fInv);}
	Vector4 operator / ( const Vector4& rhs) const{return Vector4(x / rhs.x,y / rhs.y,z / rhs.z,w / rhs.w);}

	const Vector4& operator + () const{return *this;}
	Vector4 operator - () const{return Vector4(-x, -y, -z, -w);}

	Vector4& operator += ( const Vector4& vec ){x += vec.x;y += vec.y;z += vec.z;w += vec.w;return *this;}
	Vector4& operator -= ( const Vector4& vec ){x -= vec.x;y -= vec.y;z -= vec.z;w -= vec.w;return *this;}
	Vector4& operator *= ( const float scale ){x *= scale;y *= scale;z *= scale;w *= scale;return *this;}
	Vector4& operator += ( const float scale ){x += scale;y += scale;z += scale;w += scale;return *this;}
	Vector4& operator -= ( const float scale ){x -= scale;y -= scale;z -= scale;w -= scale;return *this;}
	Vector4& operator *= ( const Vector4& vec ){x *= vec.x;y *= vec.y;z *= vec.z;w *= vec.w;return *this;}
	Vector4& operator /= ( const float scale ){assert( scale != 0.0f );float fInv = 1.0f / scale;x *= fInv;y *= fInv;z *= fInv;w *= fInv;return *this;}
	Vector4& operator /= ( const Vector4& vec ){x /= vec.x;y /= vec.y;z /= vec.z;w /= vec.w;return *this;}

	friend Vector4 operator * (const float scale, const Vector4& vec) { return Vector4(scale * vec.x, scale * vec.y, scale * vec.z, scale * vec.w); }
	friend Vector4 operator / (const float scale, const Vector4& vec) { return Vector4(scale / vec.x, scale / vec.y, scale / vec.z, scale / vec.w); }
	friend Vector4 operator + (const Vector4& lhs, const float rhs) { return Vector4(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs); }
	friend Vector4 operator + (const float lhs, const Vector4& rhs) { return Vector4(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w); }
	friend Vector4 operator - (const Vector4& lhs, float rhs) { return Vector4(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs); }
	friend Vector4 operator - (const float lhs, const Vector4& rhs) { return Vector4(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z, lhs - rhs.w); }

	/** Calculates the dot (scalar) product of this vector with another.
	@param
	vec Vector with which to calculate the dot product (together
	with this one).
	@returns
	A float representing the dot product value.
	*/
	float dotProduct(const Vector4& vec) const{return x * vec.x + y * vec.y + z * vec.z + w * vec.w;}

	//相等
	bool Equals(const Vector4& target) const { return Math::FloatEqual(x, target.x) && Math::FloatEqual(y, target.y) && Math::FloatEqual(z, target.z) && Math::FloatEqual(w, target.w); }

	operator uint()const{return ((uint)w << 24) | ((uint)x << 16) | ((uint)y << 8) | (uint)z;}

	float* ptr() { return p; }
	const float* ptr() const { return p; }
	String ToString()const { char arr[128] = { 0 }; Sprintf(arr, "%f, %f, %f, %f", x, y, z, w); return String(arr); }

	//线性插值
	static Vector4 Lerp(const Vector4& from, const Vector4& to, float t, bool clamp_01 = true);

	// special
	static const Vector4 zero;
	static const Vector4 one;
};
#pragma pack(pop)
static_assert(sizeof(Vector4) == 4 * sizeof(float), "invalid bytes");
typedef Vector<Vector4> Vector4v;
DC_END_NAMESPACE
