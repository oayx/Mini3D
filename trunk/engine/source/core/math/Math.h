/*****************************************************************************
* Author： hannibal
* Date：2009/11/15
* Description： 数学库通用数据
*****************************************************************************/
#pragma once

#include "core/BaseType.h"

DC_BEGIN_NAMESPACE

// 相交结果
enum class IntersectResult
{
	//适用于有AABB或球参与
	Inside = 0,
	Outside,
	Intersect,

	//适用于平面参与
	Intersect_Plane = 10,// 经过平面
	Positive_Plane,
	Negative_Plane,

	//适用于直线与直线 或直线与射线 或射线与射线
	Parallel_Line = 20,//平行
	Intersect_Line,
	Outside_Line
};
DECLARE_ENUM_OPERATORS(IntersectResult);

/********************************************************************/
// ClassName：Math
// Description：基础数学函数
class ENGINE_DLL Math final : public object
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Math);
	BEGIN_REFECTION_TYPE(Math)
	END_FINAL_REFECTION_TYPE;
	
public:
	constexpr static float PI = 3.1415926f;	// Pi	
	constexpr static float PI2 = 6.2831853f;	// 2 * Pi
	constexpr static float INVPI = 0.3183098f;	// 1 / Pi
	constexpr static float Epsilon = 0.00001f;	// 浮点数精度

	constexpr static float Deg2Rad = 0.0174533f;
	constexpr static float Rad2Deg = 57.2958f;

public:
	constexpr static int IAbs(int i) noexcept
	{
		return (i >= 0 ? i : -i);
	}
	static int ICeil(float f) noexcept
	{
		return int(::ceil(f));
	}
	static int IFloor(float f) noexcept
	{
		return int(::floor(f));
	}
	constexpr static int ISign(int i) noexcept
	{
		return (i > 0 ? +1 : (i < 0 ? -1 : 0));
	}
	constexpr static float Abs(float f) noexcept
	{
		return f < 0 ? -f : f;
	}
	static float ACos(float f) noexcept
	{
		return float(::acos(f));
	}
	static float ASin(float f) noexcept
	{
		return float(::asin(f));
	}
	static float ATan(float f) noexcept
	{
		return float(::atan(f));
	}
	static float ATan2(float fY, float fX) noexcept
	{
		return float(::atan2(fY, fX));
	}
	//转换成角度
	//clamp0_360:是否夹取到[0,360)
	static float ATan2Degree(float fY, float fX, bool clamp0_360 = false) noexcept
	{
		float angle = float(::atan2(fY, fX));
		return angle * Math::Rad2Deg + ((angle >= 0 || !clamp0_360) ? 0 : 360);
	}
	static float Cos(float f) noexcept
	{
		return float(::cos(f));
	}
	//返回大于或者等于指定表达式的最小整数
	template<class T>
	static T Ceil(float f) noexcept
	{
		return T(::ceil(f));
	}
	//快速求出两点间距离
	constexpr static float Dist(float x0, float y0, float x1, float y1) noexcept
	{
		return Abs(x1 - x0) + Abs(y1 - y0);
	}
	static float Exp(float f) noexcept
	{
		return float(::exp(f));
	}
	//返回不大于指定表达式的最大整数
	static float Floor(float f) noexcept
	{
		return float(::floor(f));
	}
	//比较两个单精度浮点是否相等
	constexpr static bool FloatEqual(float a, float b) noexcept
	{ 
		return Abs(a - b) < Epsilon; 
	}
	constexpr static bool IsZero(float f) noexcept
	{
		return Abs(f) < Epsilon;
	}
	template<class T>
	constexpr static T Min(T a, T b) noexcept
	{
		return a > b ? b : a;
	}
	template<class T>
	constexpr static T Max(T a, T b) noexcept
	{
		return a < b ? b : a;
	}
	static float Min(float* arr, int length) noexcept
	{
		float minValue = MAX_float;
		for (int i = 0; i < length; ++i)
		{
			if (arr[i] < minValue)minValue = arr[i];
		}
		return minValue;
	}
	static float Max(float* arr, int length) noexcept
	{
		float maxValue = MIN_float;
		for (int i = 0; i < length; ++i)
		{
			if (arr[i] > maxValue)maxValue = arr[i];
		}
		return maxValue;
	}
	template<class T>
	constexpr static T Clamp(T value, T low, T high) noexcept
	{
		return Min<T>(Max<T>(value, low), high);
	}
	constexpr static float Clamp01(float value) noexcept
	{
		return Min<float>(Max<float>(value, 0), 1);
	}
	static float Log(float f) noexcept
	{
		return float(::logf(f));
	}
	static float Log2(float x) noexcept
	{ 
		return ::logf(x) / ::logf(2);
	}
	static float Lerp(float from, float to, float t, bool clamp_01 = true) noexcept
	{
		if (clamp_01)t = Math::Clamp01(t);
		return from + (to - from) * t;
	}
	static float Pow(float v, float exponent) noexcept
	{
		return float(::pow(v, exponent));
	}
	// 四舍五入到最邻近的整数
	template<class T>
	static T Round(float f) noexcept
	{
		float up = ::ceil(f);
		float down = ::floor(f);
		if (f - down < 0.5f)return (T)down;
		else if (up - f < 0.5f)return (T)up;
		else return ((((int)up) % 2) == 0 ? (T)up : (T)down);
	}
	//初始化随机数
	static void RandomInit() noexcept
	{
		::srand((uint)::time(NULL));
	}
	//[min,max)
	static float RandomRange(float min, float max) noexcept
	{
		long long rand_max = (long long)RAND_MAX + 1;
		double rand_01 = ::rand() / (double)rand_max;
		return (float)(min + rand_01 * (max - min));
	}
	static int RandomRange(int min, int max) noexcept
	{
		return (int)(min + RandomRange(0.0f, 1.0f) * (max - min));
	}
	static float Sin(float f) noexcept
	{
		return float(::sin(f));
	}
	constexpr static int Sign(float f) noexcept
	{
		return f < 0 ? -1 : 1;
	}
	constexpr static float Sqr(float f) noexcept
	{
		return f*f;
	}
	static float Sqrt(float f) noexcept
	{
		return float(::sqrt(f));
	}
	static float Tan(float f) noexcept
	{
		return float(::tan(f));
	}
	//一个整数是否2的n次方
	constexpr static bool Is2Power(uint v) noexcept
	{
		return ((v & (v - 1)) == 0);
	}
	//对给定角度取模(-PI,PI)
	static float ModAngle(float rad) noexcept
	{
		rad = rad + PI;
		float temp = fabsf(rad);
		temp = temp - (PI * static_cast<float>(static_cast<int32_t>(temp / PI)));
		temp = temp - PI;
		if (rad < 0.0f) 
			temp = -temp;
		return temp;
	}
	//2的n次方
	constexpr static bool IsPOT2(int number) noexcept
	{
		if ((number > 0) && (number&(number - 1)) == 0)
			return true;
		return false;
	}
};//Math
DC_END_NAMESPACE
