 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/13
* Description： 2D向量
*****************************************************************************/
#pragma once

#include "Math.h"
#include "core/String.h"
#include "core/stl/List.h"
#include "core/stl/Vector.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
#pragma pack(push,4)
class ENGINE_DLL Vector2 final : public object
{
	BEGIN_REFECTION_TYPE(Vector2)
	END_FINAL_REFECTION_TYPE;

public:
	union
	{
		struct { float x, y; };
		float p[2] = { 0.0f };
	};

public:
	static const Vector2 zero;
	static const Vector2 xAxis;
	static const Vector2 yAxis;
	static const Vector2 one;
	static const Vector2 Infinity;

public:
	constexpr explicit Vector2() :x(0.0f), y(0.0f){ }
	constexpr Vector2(float x, float y) :x(x), y(y) { }
	constexpr Vector2(const Vector2& vec) :x(vec.x), y(vec.y) { }
	constexpr Vector2(Vector2&& other)noexcept = default;
	inline Vector2(const String& str)
	{
		Vector<float> vec = str.Split<float>( ',');
		if (vec.Size() == 2) { x = vec[0]; y = vec[1]; }
	}

public:
	//赋值，改变原始值
	Vector2& operator =(const Vector2 &vec) noexcept { x = vec.x; y = vec.y; return *this; }
	Vector2& operator =(Vector2&& other) noexcept { x = std::move(other.x); y = std::move(other.y); return *this; }

	//求负值，不改变原值
	constexpr Vector2 operator -()const noexcept { return Vector2(-x, -y); }

	constexpr bool operator ==(const Vector2 &vec)const noexcept { return this->Equals(vec); }
	constexpr bool operator !=(const Vector2 &vec)const noexcept { return !this->Equals(vec); }

	//重载算术运算符
	constexpr Vector2 operator +(const Vector2 &vec)const noexcept { return Vector2(x + vec.x, y + vec.y); }
	constexpr Vector2 operator -(const Vector2 &vec)const noexcept { return Vector2(x - vec.x, y - vec.y); }
	constexpr Vector2 operator *(float scale)const noexcept { return Vector2(x * scale, y * scale); }
	//参数为0返回最大值
	constexpr Vector2 operator /(float scale)const noexcept { return Vector2(x / scale, y / scale); }

	constexpr friend Vector2 operator *(float scale, const Vector2& vec) noexcept { return Vector2(vec.x * scale, vec.y * scale); }
	constexpr friend Vector2 operator /(const float scale, const Vector2& vec) noexcept {return Vector2(scale / vec.x, scale / vec.y);}

public:
	//求长度
	float Lenght()const noexcept { return Math::Sqrt(x * x + y * y); }
	//面积
	constexpr float SquareSize()const noexcept { return x * x + y * y; }

	//点积
	constexpr float Dot(const Vector2 &vec)const noexcept { return x * vec.x + y * vec.y; }

	//距离
	float Distance(const Vector2& vec)const noexcept { return Math::Sqrt(SqrDistance(vec)); }
	constexpr float SqrDistance(const Vector2& vec)const noexcept { return (x - vec.x) *(x - vec.x) + (y - vec.y) *(y - vec.y); }

	//单位化
	Vector2 Normalize() noexcept;
	Vector2 Normalize()const noexcept;

	//向量相加
	void Add(const Vector2 &vec2) noexcept { x += vec2.x; y += vec2.y; }
	//向量相减
	void Subtract(const Vector2 &vec2) noexcept { x -= vec2.x; y -= vec2.y; }

	//缩放，x, y方向同时
	void Scale(float scale) noexcept { x *= scale; y *= scale; }
	void Scale(float x_scale, float y_scale) noexcept { x *= x_scale; y *= y_scale; }
	void Set(float x, float y) noexcept { this->x = x; this->y = y; }

	constexpr bool Equals(const Vector2& target) const noexcept { return Math::FloatEqual(x, target.x) && Math::FloatEqual(y, target.y); }

	// Pointer accessor for direct copying
	float* ptr() noexcept { return p; }
	// Pointer accessor for direct copying
	const float* ptr() const noexcept { return p; }
	String ToString()const noexcept { char arr[128] = { 0 }; Snprintf(arr, sizeof(arr), "%f, %f", x, y); return String(arr); }

	//线性插值
	static Vector2 Lerp(const Vector2& from, const Vector2& to, float t, bool clamp_01 = true) noexcept;
public:
	//求两向量的夹角
	// @par：
	// @return：
	float Angle(const Vector2 &vec) noexcept { return Math::ACos((x * vec.x + y * vec.y) / (Lenght() * vec.Lenght())); }

	//求2D空间两个向量的旋转角(0~360)；坐标系为x轴向右，y轴向下；角度顺时针方向移动为正(与正常情况相反)
	// @arithmetic:  
	//             * 首先通过点乘和arccosine得到两个向量之间的夹角
	//             * 然后判断通过差乘来判断两个向量之间的位置关系
	//             * 如果p2在p1的顺时针方向, 返回arccose的角度值, 范围0 ~ 180.0(根据右手定理,可以构成正的面积)
	//             * 否则返回 360.0 - arecose的值, 返回180到360(根据右手定理,面积为负)
	// @par1：第一个向量的x坐标，坐标系为x轴向右，y轴向下
	// @par2：第一个向量的y坐标
	// @par3：第二个向量的x坐标
	// @par4：第二个向量的y坐标
	// @return：角度值
	float RotationAngle(const Vector2 &vec)const noexcept;

	//绕原点逆时针旋转
	// @par：
	// @return：
	Vector2 CounterClockwiseRotation(float radian) noexcept;

	//绕原点顺时针旋转
	// @par：
	// @return：
	Vector2 ClockwiseRotation(float radian) noexcept;
};//Vector2
#pragma pack(pop)
static_assert(sizeof(Vector2) == 2 * sizeof(float), "invalid bytes");
typedef Vector<Vector2> Vector2v;
DC_END_NAMESPACE
