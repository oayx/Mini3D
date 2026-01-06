#include "Vector2.h"
#include <limits>
 

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(Vector2);
const Vector2 Vector2::zero(0.0f, 0.0f);
const Vector2 Vector2::xAxis(1.0f,0.0f);
const Vector2 Vector2::yAxis(0.0f,1.0f);
const Vector2 Vector2::one(1.0f, 1.0f);
const Vector2 Vector2::Infinity(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
Vector2 Vector2::Normalize()const noexcept
{
	Vector2 vec = *this;
	vec.Normalize();
	return vec;
}
Vector2 Vector2::Normalize() noexcept
{
	float fLenght = Lenght();
	if (Math::FloatEqual(fLenght, 0.0f))
	{
		x = y = 0.0f;
	}
	else
	{
		float fInv = 1.0f / fLenght;
		x = x * fInv;
		y = y * fInv;
	}
	return *this;
}
Vector2 Vector2::Lerp(const Vector2& from, const Vector2& to, float t, bool clamp_01) noexcept
{
	return Vector2(
		Math::Lerp(from.x, to.x, t, clamp_01),
		Math::Lerp(from.y, to.y, t, clamp_01));
}
float Vector2::RotationAngle(const Vector2 &vec)const noexcept
{
	const float epsilon = (float)(1.0e-6);
	const float nyPI = acos(-1.0f);
	float dist, dot, degree, angle;

	// normalize
	float x1 = x;
	float y1 = y;
	float x2 = vec.x;
	float y2 = vec.y;

	dist = (float)sqrt( (float)(x1 * x1 + y1 * y1) );
	if (fabs(dist) <= epsilon)
	{
		return 0;
	}
	x1 /= dist;
	y1 /= dist;
	dist = (float)sqrt( (float)(x2 * x2 + y2 * y2) );
	if (fabs(dist) <= epsilon)
	{
		return 0;
	}
	x2 /= dist;
	y2 /= dist;
	// dot product
	dot = x1 * x2 + y1 * y2;
	if ( fabs(dot-1.0) <= epsilon ) 
	{
		angle = 0.0;
	}
	else if ( fabs(dot+1.0) <= epsilon ) 
	{
		angle = nyPI;
	}
	else 
	{
		float cross;

		angle = acos(dot);
		//cross product
		cross = x1 * y2 - x2 * y1;

		if (cross < 0 ) 
		{ 
			angle = 2 * nyPI - angle;
		}    
	}
	degree = angle *  180.0f / nyPI;
	return degree;
}

/*============================================================================
* 函数名称： CounterClockwiseRotation
* 功    能： 绕原点逆时针旋转
			方法：
				等式：
					x1 = x0 * cosA - y0 * sinA
					y1 = x0 * sinA + y0 * cosA
				旋转矩阵：
					cosA   sinA
				   -sinA   cosA
* 参数列表： 
* 返 回 值： 
============================================================================*/
Vector2 Vector2::CounterClockwiseRotation(float radian) noexcept
{
	float fCos = Math::Cos(radian);
	float fSin = Math::Sin(radian);
	
	Vector2 vec_out;
	vec_out.x = x * fCos - y * fSin;
	vec_out.y = x * fSin + y * fCos;

	return vec_out;
}


/*============================================================================
* 函数名称： ClockwiseRotation
* 功    能： 
			方法：
				等式：
					x1 =  x0 * cosA + y0 * sinA
					y1 = -x0 * sinA + y0 * cosA
				旋转矩阵：
					cosA   -sinA
					sinA    cosA
* 参数列表： 
* 返 回 值： 
============================================================================*/
Vector2 Vector2::ClockwiseRotation(float radian) noexcept
{
	float fCos = Math::Cos(radian);
	float fSin = Math::Sin(radian);

	Vector2 vec_out;
	vec_out.x =  x * fCos + y * fSin;
	vec_out.y = -x * fSin + y * fCos;

	return vec_out;
}
DC_END_NAMESPACE
