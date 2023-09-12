 
/*****************************************************************************
* Author： hannibal
* Date：2009/12/14
* Description： 颜色(默认使用RGBA模式)
*****************************************************************************/
#pragma once

#include "ColorConverter.h"
#include "core/String.h"
#include "core/math/Vector4.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
#pragma pack(push,4)
class ENGINE_DLL Color Final : public object
{
	BEGIN_REFECTION_TYPE(Color)
	END_FINAL_REFECTION_TYPE;
	
public:
	Color(): r(0), g(0), b(0), a(0)
	{
	}
	Color(float fr, float fg, float fb, float fa = 1.0f): r(fr), g(fg), b(fb), a(fa)
	{
	}
	Color(uint v)
	{
		a = ((v >> 24) & 0xff) / 255.0f;
		r = ((v >> 16) & 0xff) / 255.0f;
		g = ((v >> 8) & 0xff) / 255.0f;
		b = ((v) & 0xff) / 255.0f;
	}
	Color(const Vector4& vec)
	{
		r = vec.x; g = vec.y; b = vec.z; a = vec.w;
	}
	Color(const Color& color)
	{
		r = color.r; g = color.g; b = color.b; a = color.a;
	}
	Color(const String& str)
	{
		Vector<float> vec = str.Split<float>(',');
		if (vec.Size() == 4)
		{
			r = vec[0]; g = vec[1]; b = vec[2]; a = vec[3];
		}
	}
	Color& operator=(const Vector4& vec)
	{
		r = vec.x; g = vec.y; b = vec.z; a = vec.w;
		return *this;
	}
	Color& operator=(const Color& color)
	{
		r = color.r; g = color.g; b = color.b; a = color.a;
		return *this;
	}
	bool operator ==(const Color& c) const
	{
		return Math::FloatEqual(r, c.r) && Math::FloatEqual(g, c.g) && Math::FloatEqual(b, c.b) && Math::FloatEqual(a, c.a);
	}
	bool operator !=(const Color& c) const
	{
		return !(*this == c);
	}
	Color operator *(const Color& c) const
	{
		return Color(r * c.r, g * c.g, b * c.b, a * c.a);
	}
	Color& operator *=(const Color& c)
	{
		r *= c.r;
		g *= c.g;
		b *= c.b;
		a *= c.a;
		return *this;
	}
	Color operator *(float v) const
	{
		return Color(r * v, g * v, b * v, a * v);
	}
	Color operator /(float v) const
	{
		return *this * (1 / v);
	}
	operator uint()const
	{
		uint dwR = GetRed();
		uint dwG = GetGreen();
		uint dwB = GetBlue();
		uint dwA = GetAlpha();
		return (dwA << 24) | (dwB << 16) | (dwG << 8) | dwR;
	}
public:
	void Set(float fr = 0.0f, float fg = 0.0f, float fb = 0.0f, float fa = 1.0f)
	{
		r = fr; g = fg; b = fb; a = fa;
	}

	byte GetAlpha() const 
	{ 
		return (a >= 1.0f ? 0xff : a <= 0.0f ? 0x00 : byte(a * 255));
	}
	byte GetRed()	const 
	{ 
		return (r >= 1.0f ? 0xff : r <= 0.0f ? 0x00 : byte(r * 255));
	}
	byte GetGreen() const 
	{ 
		return (g >= 1.0f ? 0xff : g <= 0.0f ? 0x00 : byte(g * 255));
	}
	byte GetBlue()	const 
	{ 
		return (b >= 1.0f ? 0xff : b <= 0.0f ? 0x00 : byte(b * 255)); 
	}
	//DX9默认使用BGRA颜色模式，OpenGL+OpenGLES+D3D11是RGBA
	uint ToVertexColor()
	{
#if defined(DC_GRAPHICS_API_DX9)
		return (uint(a * 255) << 24) | (uint(r * 255) << 16) | (uint(g * 255) << 8) | uint(b * 255);
#else
		return (uint(a * 255) << 24) | (uint(b * 255) << 16) | (uint(g * 255) << 8) | uint(r * 255);
#endif
	}

	String ToString()const { char arr[128] = { 0 }; Sprintf(arr, "%f, %f, %f, %f", r, g, b, a); return String(arr); }

	float* ptr()
	{
		return p;
	}
	const float* ptr() const
	{
		return p;
	}

public:
	static Color Black;
	static Color White;
	static Color Red;
	static Color Green;
	static Color Blue;
	static Color Gray;
	static Color Yellow;
	static Color Clear;
	static Color Lerp(const Color& from, const Color& to, float t, bool clamp_01 = true)
	{
		return Color(
			Math::Lerp(from.r, to.r, t, clamp_01),
			Math::Lerp(from.g, to.g, t, clamp_01),
			Math::Lerp(from.b, to.b, t, clamp_01),
			Math::Lerp(from.a, to.a, t, clamp_01));
	}
public:
	union
	{
		struct{ float r, g, b, a; };
		float p[4];
	};
};//Color
#pragma pack(pop)
DC_END_NAMESPACE
