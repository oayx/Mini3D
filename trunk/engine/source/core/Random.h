 
/*****************************************************************************************************/
// @author hannibal
// @date   2015/05/28
// @brief  同步随机数(unity源代码)
/*****************************************************************************************************/
#pragma once

#include "BaseType.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL Random Final : public object
{
public:
	explicit Random(uint seed = 0)
	{
		SetSeed(seed);
	}

public:
	// random number between 0.0 and 1.0
	float GetFloat0_1()
	{
		return GetFloatFromInt(Get());
	}

	// random number between -1.0 and 1.0
	float GetSignedFloat_1_1()
	{
		return GetFloat0_1() * 2.0f - 1.0f;
	}

	int RandomRange(int min, int max)
	{
		int dif;
		if (min < max)
		{
			dif = max - min;
			int t = Get() % dif;
			t += min;
			return t;
		}
		else if (min > max)
		{
			dif = min - max;
			int t = Get() % dif;
			t = min - t;
			return t;
		}
		else
		{
			return min;
		}
	}

	float RandomRange(float min, float max)
	{
		float t = GetFloat0_1();
		t = min * t + (1.0f - t) * max;
		return t;
	}

	void SetSeed(uint seed)
	{
		x = seed;
		y = x * 1812433253U + 1;
		z = y * 1812433253U + 1;
		w = z * 1812433253U + 1;
	}

	uint GetSeed() const { return x; }

private:
	uint Get()
	{
		uint t;
		t = x ^ (x << 11);
		x = y; y = z; z = w;
		return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
	}

	inline static float GetFloatFromInt(uint value)
	{
		// take 23 bits of integer, and divide by 2^23-1
		return float(value & 0x007FFFFF) * (1.0f / 8388607.0f);
	}

	inline static byte GetByteFromInt(uint value)
	{
		// take the most significant byte from the 23-bit value
		return byte(value >> (23 - 8));
	}

private:
	uint x = 0, y = 0, z = 0, w = 0;
};
DC_END_NAMESPACE
