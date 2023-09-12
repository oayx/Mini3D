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
class ENGINE_DLL Math Final : public object
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Math);
	BEGIN_REFECTION_TYPE(Math)
	END_FINAL_REFECTION_TYPE;
	
public:
	static const float PI;			// Pi	
	static const float PI2;			// 2 * Pi
	static const float INVPI;		// 1 / Pi
	static const float Epsilon;		// 浮点数精度

	static const float Deg2Rad;
	static const float Rad2Deg;

public:
	static int IAbs(int i)
	{
		return (i >= 0 ? i : -i);
	}
	static int ICeil(float f)
	{
		return int(::ceil(f));
	}
	static int IFloor(float f)
	{
		return int(::floor(f));
	}
	static int ISign(int i)
	{
		return (i > 0 ? +1 : (i < 0 ? -1 : 0));
	}
	static float Abs(float f)
	{
		return float(::fabs(f));
	}
	static float ACos(float f)
	{
		return float(::acos(f));
	}
	static float ASin(float f)
	{
		return float(::asin(f));
	}
	static float ATan(float f)
	{
		return float(::atan(f));
	}
	static float ATan2(float fY, float fX)
	{
		return float(::atan2(fY, fX));
	}
	//转换成角度
	//clamp0_360:是否夹取到[0,360)
	static float ATan2Degree(float fY, float fX, bool clamp0_360 = false)
	{
		float angle = float(::atan2(fY, fX));
		return angle * Math::Rad2Deg + ((angle >= 0 || !clamp0_360) ? 0 : 360);
	}
	static float Cos(float f)
	{
		return float(::cos(f));
	}
	//返回大于或者等于指定表达式的最小整数
	template<class T>
	static T Ceil(float f)
	{
		return T(::ceil(f));
	}
	template<class T>
	static T Clamp(T value, T low, T high)
	{
		return Min<T>(Max<T>(value, low), high);
	}
	static float Clamp01(float value)
	{
		return Min<float>(Max<float>(value, 0), 1);
	}
	//快速求出两点间距离
	static float Dist(float x0, float y0, float x1, float y1)
	{
		return ::fabs(x1 - x0) + ::fabs(y1 - y0);
	}
	static float Exp(float f)
	{
		return float(::exp(f));
	}
	//返回不大于指定表达式的最大整数
	static float Floor(float f)
	{
		return float(::floor(f));
	}
	//比较两个单精度浮点是否相等
	static bool FloatEqual(float a, float b) 
	{ 
		return Abs(a - b) < Epsilon; 
	}
	static bool IsZero(float f)
	{
		return Abs(f) < Epsilon;
	}
	template<class T>
	static T Min(T a, T b)
	{
		return a > b ? b : a;
	}
	template<class T>
	static T Max(T a, T b)
	{
		return a < b ? b : a;
	}
	static float Min(float* arr, int length)
	{
		float min_value = MAX_float;
		for (int i = 0; i < length; ++i)
		{
			if (arr[i] < min_value)min_value = arr[i];
		}
		return min_value;
	}
	static float Max(float* arr, int length)
	{
		float max_value = MIN_float;
		for (int i = 0; i < length; ++i)
		{
			if (arr[i] > max_value)max_value = arr[i];
		}
		return max_value;
	}
	static float Log(float f)
	{
		return float(::logf(f));
	}
	static float Log2(float x) 
	{ 
		return ::logf(x) / ::logf(2);
	}
	static float Lerp(float from, float to, float t, bool clamp_01 = true)
	{
		if (clamp_01)t = Math::Clamp01(t);
		return from + (to - from) * t;
	}
	static float Pow(float v, float exponent)
	{
		return float(::pow(v, exponent));
	}
	// 四舍五入到最邻近的整数
	template<class T>
	static T Round(float f)
	{
		float up = ::ceil(f);
		float down = ::floor(f);
		if (f - down < 0.5f)return (T)down;
		else if (up - f < 0.5f)return (T)up;
		else return ((((int)up) % 2) == 0 ? (T)up : (T)down);
	}
	//初始化随机数
	static void RandomInit()
	{
		::srand((uint)::time(NULL));
	}
	//[min,max)
	static float RandomRange(float min, float max)
	{
		long long rand_max = (long long)RAND_MAX + 1;
		double rand_01 = ::rand() / (double)rand_max;
		return (float)(min + rand_01 * (max - min));
	}
	static int RandomRange(int min, int max)
	{
		return (int)(min + RandomRange(0.0f, 1.0f) * (max - min));
	}
	static float Sin(float f)
	{
		return float(::sin(f));
	}
	static int Sign(float f)
	{
		return f < 0 ? -1 : 1;
	}
	static float Sqr(float f)
	{
		return f*f;
	}
	static float Sqrt(float f)
	{
		return float(::sqrt(f));
	}
	static float Tan(float f)
	{
		return float(::tan(f));
	}
	//一个整数是否2的n次方
	static bool Is2Power(uint v)
	{
		return ((v & (v - 1)) == 0);
	}
	//对给定角度取模(-PI,PI)
	static float ModAngle(float rad)
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
	static bool IsPOT2(int number)
	{
		if ((number > 0) && (number&(number - 1)) == 0)
			return true;
		return false;
	}
};//Math
DC_END_NAMESPACE
