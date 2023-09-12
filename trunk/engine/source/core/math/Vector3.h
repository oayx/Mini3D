 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/13
* Description： 3D向量
*****************************************************************************/
#pragma once

#include "Math.h"
#include "core/String.h"
#include "core/stl/List.h"
#include "core/stl/Vector.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
#pragma pack(push,4)
class ENGINE_DLL Vector3 Final : public object
{
	BEGIN_REFECTION_TYPE(Vector3)
	END_FINAL_REFECTION_TYPE;

public:
	union
	{
		struct { float x, y, z; };
		float p[3] = {0.0f};
	};

public:
	static const Vector3 zero;
	static const Vector3 right;
	static const Vector3 up;
	static const Vector3 forward;
	static const Vector3 one;
	static const Vector3 infinity;

public:
	explicit Vector3() { x = 0.0f; y = 0.0f;  z = 0.0f; }
	Vector3(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }
	Vector3(const Vector3 &vec) { x = vec.x; y = vec.y; z = vec.z; }
	Vector3(const String& str)
	{
		Vector<float> vec = str.Split<float>(',');
		if (vec.Size() == 3)
		{
			x = vec[0]; y = vec[1]; z = vec[2];
		}
	}

public:
	//赋值，改变原始值
	Vector3 &operator =(const Vector3 &vec) { x = vec.x; y = vec.y; z = vec.z; return *this; }
	//求负值，不改变原值
	Vector3 operator -()const { return Vector3(-x, -y, -z); }
	//重载算术运算符
	Vector3 operator +(const Vector3 &vec)const { return Vector3(x + vec.x, y + vec.y, z + vec.z); }
	Vector3 operator -(const Vector3 &vec)const { return Vector3(x - vec.x, y - vec.y, z - vec.z); }
	Vector3 operator *(float scale)const { return Vector3(x * scale, y * scale, z * scale); }
	Vector3 operator *(const Vector3 &vec)const { return Vector3(x * vec.x, y * vec.y, z * vec.z); }
	Vector3 operator /(const Vector3& vec) const { return Vector3(x / vec.x, y / vec.y, z / vec.z); }
	Vector3 operator +=(const Vector3 &vec) { *this = Vector3(x + vec.x, y + vec.y, z + vec.z); return *this; }
	//参数为0返回最大值
	Vector3 operator /(float scale)const { return Vector3(x / scale, y / scale, z / scale); }

	float operator [] (const size_t i) const { assert(i < 3); return *(p + i); }
	float& operator [] (const size_t i) { assert(i < 3); return *(p + i); }

	//重载关系运算符
	bool operator ==(const Vector3 &vec)const { return this->Equals(vec); }
	bool operator !=(const Vector3 &vec)const { return !this->Equals(vec); }

	bool operator < (const Vector3& rhs) const { if (x < rhs.x && y < rhs.y && z < rhs.z)return true; return false; }
	bool operator > (const Vector3& rhs) const { if (x > rhs.x && y > rhs.y && z > rhs.z)return true; return false; }

	friend Vector3 operator * (const float scale, const Vector3& vec){ return Vector3(scale * vec.x,scale * vec.y,scale * vec.z);}
	friend Vector3 operator / (const float scale, const Vector3& vec){ return Vector3(scale / vec.x,scale / vec.y,scale / vec.z);}

public:
	//求长度
	float Lenght()const { return Math::Sqrt(x * x + y * y + z * z); }
	//面积
	float SquareSize()const { return x * x + y * y + z * z; }

	//点积
	float Dot(const Vector3 &vec)const { return x * vec.x + y * vec.y + z * vec.z; }
	//每个分量的绝对值点积
	float AbsDot(const Vector3 &vec)const { return Math::Abs(x * vec.x) + Math::Abs(y * vec.y) + Math::Abs(z * vec.z); }
	//叉积
	Vector3 Cross(const Vector3 &vec)const { return Vector3(y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x); }
	
	//距离
	float Distance(const Vector3& vec)const { return Math::Sqrt(SqrDistance(vec)); }
	float SqrDistance(const Vector3& vec)const { return (x - vec.x) *(x - vec.x) + (y - vec.y) *(y - vec.y) + (z - vec.z) *(z - vec.z); }

	//单位化,改变原值
	Vector3 Normalize();
	Vector3 Normalize(float len);
	//单位化,不改变原值
	Vector3 Normalize()const { Vector3 vec = *this; vec.Normalize(); return vec; }

	//向量相加
	void Add(const Vector3 &vec) { x += vec.x; y += vec.y; z += vec.z; }
	//向量相减
	void Subtract(const Vector3 &vec) { x -= vec.x; y -= vec.y; z -= vec.z; }

	//缩放，x, y，z方向同时
	void Scale(float scale) { x *= scale; y *= scale; z *= scale; }
	void Scale(float x_scale, float y_scale, float fZScale) { x *= x_scale; y *= y_scale; z *= fZScale; }
	void Set(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }

	//不大于
	void MakeFloor(const Vector3& vec) { if (vec.x < x) x = vec.x; if (vec.y < y) y = vec.y; if (vec.z < z) z = vec.z; }
	//不小于
	void MakeCeil(const Vector3& vec) { if (vec.x > x) x = vec.x; if (vec.y > y) y = vec.y; if (vec.z > z) z = vec.z; }

	//0向量
	void SetZero() { x = y = z = 0; }
	//是否为0向量
	bool IsZero()const { return (Math::IsZero(x) && Math::IsZero(y) && Math::IsZero(z)); }

	//中间点
	Vector3 MidPoint()const { return Vector3(x * 0.5f, y  * 0.5f, z * 0.5f); }
	//中间点
	Vector3 MidPoint(const Vector3& vec)const { return Vector3((x + vec.x) * 0.5f, (y + vec.y) * 0.5f, (z + vec.z) * 0.5f); }

	//求两向量的夹角
	float Angle(const Vector3 &vec) { return Math::ACos((x * vec.x + y * vec.y + z * vec.z) / (Lenght() * vec.Lenght())); }

	//投影
	void Projection(const Vector3 &vec, Vector3 &normal, Vector3 &vecVertical)const;

	//相等
	bool Equals(const Vector3& target) const { return Math::FloatEqual(x, target.x) && Math::FloatEqual(y, target.y) && Math::FloatEqual(z, target.z); }

	float Max()const { return x > y ? x > z ? x : z : y > z ? y : z; }

	//取得指针
	float *ptr() { return p; }
	const float *ptr()const { return p; }
	String ToString()const { char arr[128] = { 0 }; Sprintf(arr, "%f, %f, %f", x, y, z); return String(arr); }

	static Vector3 Min(const Vector3& lhs, const Vector3& rhs) { return Vector3(Math::Min<float>(lhs.x, rhs.x), Math::Min<float>(lhs.y, rhs.y), Math::Min<float>(lhs.z, rhs.z)); }
	static Vector3 Max(const Vector3& lhs, const Vector3& rhs) { return Vector3(Math::Max<float>(lhs.x, rhs.x), Math::Max<float>(lhs.y, rhs.y), Math::Max<float>(lhs.z, rhs.z)); }

	//线性插值
	static Vector3 Lerp(const Vector3& from, const Vector3& to, float t, bool clamp_01 = true);
};
#pragma pack(pop)
static_assert(sizeof(Vector3) == 3 * sizeof(float), "invalid bytes");
typedef Vector<Vector3> Vector3v;
DC_END_NAMESPACE
