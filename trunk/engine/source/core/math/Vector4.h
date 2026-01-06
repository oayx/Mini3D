 
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
class ENGINE_DLL Vector4 final : public object
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
	static const Vector4 zero;
	static const Vector4 one;

public:
	constexpr Vector4(): x(0), y(0), z(0), w(0){}
	constexpr Vector4(float fX, float fY, float fZ, float fW) : x(fX), y(fY), z(fZ), w(fW) {}
	constexpr Vector4(const Vector3& rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(1.0f) {}
	constexpr Vector4(const Vector4& rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}
	constexpr Vector4(Vector4&& other)noexcept = default;
	Vector4(const String& str)
	{
		Vector<float> vec = str.Split<float>(',');
		if (vec.Size() == 4)
		{
			x = vec[0]; y = vec[1]; z = vec[2]; w = vec[3];
		}
	}

public:
	constexpr Vector4& operator = (const Vector4& vec) noexcept { x = vec.x; y = vec.y; z = vec.z; w = vec.w; return *this; }
	constexpr Vector4& operator = (const float scale) noexcept { x = scale; y = scale; z = scale; w = scale; return *this; }
	constexpr Vector4& operator = (Vector4&& other) noexcept { x = std::move(other.x); y = std::move(other.y); z = std::move(other.z); w = std::move(other.w); return *this; }

	constexpr float operator [] (const size_t i) const noexcept { assert(i < 4); return *(&x + i); }
	constexpr float& operator [] (const size_t i) noexcept { assert(i < 4); return *(p + i); }

	constexpr bool operator == (const Vector4& vec) const noexcept { return this->Equals(vec); }
	constexpr bool operator != (const Vector4& vec) const noexcept { return !this->Equals(vec); }

	Vector4& operator = (const Vector3& rhs) noexcept {x = rhs.x;y = rhs.y;z = rhs.z;w = 1.0f;return *this;}
	constexpr Vector4 operator + ( const Vector4& vec ) const noexcept {return Vector4(x + vec.x,y + vec.y,z + vec.z,w + vec.w);}
	constexpr Vector4 operator - ( const Vector4& vec ) const noexcept {return Vector4(x - vec.x,y - vec.y,z - vec.z,w - vec.w);}
	constexpr Vector4 operator * ( const float scale ) const noexcept {return Vector4(x * scale,y * scale,z * scale,w * scale);}
	constexpr Vector4 operator * ( const Vector4& rhs) const noexcept {return Vector4(rhs.x * x,rhs.y * y,rhs.z * z,rhs.w * w);}
	constexpr Vector4 operator / ( const float scale ) const noexcept {assert( scale != 0.0f );float fInv = 1.0f / scale;return Vector4(x * fInv,y * fInv,z * fInv,w * fInv);}
	constexpr Vector4 operator / ( const Vector4& rhs) const noexcept {return Vector4(x / rhs.x,y / rhs.y,z / rhs.z,w / rhs.w);}

	const Vector4& operator + () const noexcept {return *this;}
	Vector4 operator - () const noexcept {return Vector4(-x, -y, -z, -w);}

	Vector4& operator += ( const Vector4& vec ) noexcept {x += vec.x;y += vec.y;z += vec.z;w += vec.w;return *this;}
	Vector4& operator -= ( const Vector4& vec ) noexcept {x -= vec.x;y -= vec.y;z -= vec.z;w -= vec.w;return *this;}
	Vector4& operator *= ( const float scale ) noexcept {x *= scale;y *= scale;z *= scale;w *= scale;return *this;}
	Vector4& operator += ( const float scale ) noexcept {x += scale;y += scale;z += scale;w += scale;return *this;}
	Vector4& operator -= ( const float scale ) noexcept {x -= scale;y -= scale;z -= scale;w -= scale;return *this;}
	Vector4& operator *= ( const Vector4& vec ) noexcept {x *= vec.x;y *= vec.y;z *= vec.z;w *= vec.w;return *this;}
	Vector4& operator /= ( const float scale ) noexcept {assert( scale != 0.0f );float fInv = 1.0f / scale;x *= fInv;y *= fInv;z *= fInv;w *= fInv;return *this;}
	Vector4& operator /= ( const Vector4& vec ) noexcept {x /= vec.x;y /= vec.y;z /= vec.z;w /= vec.w;return *this;}

	constexpr friend Vector4 operator * (const float scale, const Vector4& vec) noexcept { return Vector4(scale * vec.x, scale * vec.y, scale * vec.z, scale * vec.w); }
	constexpr friend Vector4 operator / (const float scale, const Vector4& vec) noexcept { return Vector4(scale / vec.x, scale / vec.y, scale / vec.z, scale / vec.w); }
	constexpr friend Vector4 operator + (const Vector4& lhs, const float rhs) noexcept { return Vector4(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs); }
	constexpr friend Vector4 operator + (const float lhs, const Vector4& rhs) noexcept { return Vector4(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w); }
	constexpr friend Vector4 operator - (const Vector4& lhs, float rhs) noexcept { return Vector4(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs); }
	constexpr friend Vector4 operator - (const float lhs, const Vector4& rhs) noexcept { return Vector4(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z, lhs - rhs.w); }

	//点积
	constexpr float dotProduct(const Vector4& vec) const noexcept {return x * vec.x + y * vec.y + z * vec.z + w * vec.w;}

	//相等
	constexpr bool Equals(const Vector4& target) const noexcept { return Math::FloatEqual(x, target.x) && Math::FloatEqual(y, target.y) && Math::FloatEqual(z, target.z) && Math::FloatEqual(w, target.w); }

	constexpr operator uint()const noexcept {return ((uint)w << 24) | ((uint)x << 16) | ((uint)y << 8) | (uint)z;}

	float* ptr() noexcept { return p; }
	const float* ptr() const noexcept { return p; }
	String ToString()const noexcept { char arr[128] = { 0 }; Snprintf(arr, sizeof(arr), "%f, %f, %f, %f", x, y, z, w); return String(arr); }

	//线性插值
	static Vector4 Lerp(const Vector4& from, const Vector4& to, float t, bool clamp_01 = true) noexcept;
};
#pragma pack(pop)
static_assert(sizeof(Vector4) == 4 * sizeof(float), "invalid bytes");
typedef Vector<Vector4> Vector4v;
DC_END_NAMESPACE
