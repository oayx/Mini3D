#include "Easy.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
#define value from + (to - from)
float LinearFun(float from, float to, float time)
{
	return value * time;
}
float QuadInFun(float from, float to, float time)
{
	return value * time * time;
}
float QuadOutFun(float from, float to, float time)
{
	return value * time * (2.0f - time);
}
float QuadBothFun(float from, float to, float time)
{
	if (time < 0.5f)
	{
		return value * time * time * 2.0f;
	}
	else
	{
		return value * (2.0f * time * (2.0f - time) - 1.0f);
	}
}
float CubicInFun(float from, float to, float time)
{
	return value * time * time * time;
}
float CubicOutFun(float from, float to, float time)
{
	time -= 1.0f;
	return value * (time * time * time + 1.0f);
}
float CubicBothFun(float from, float to, float time)
{
	if (time < 0.5f)
	{
		return value * 4.0f * time * time * time;
	}
	else
	{
		time -= 1.0f;
		return value * (4.0f * time * time * time + 1.0f);
	}
}
float QuartInFun(float from, float to, float time)
{
	return value * time * time * time * time;
}
float QuartOutFun(float from, float to, float time)
{
	time -= 1.0f;
	return value * (time * time * time * (-time) + 1.0f);
}
float QuartBothFun(float from, float to, float time)
{
	if (time < 0.5f)
	{
		return value * 8.0f * time * time * time * time;
	}
	else
	{
		time -= 1.0f;
		return value * (-8.0f * time * time * time * time + 1.0f);
	}
}
float QuintInFun(float from, float to, float time)
{
	return value * time * time * time * time * time;
}
float QuintOutFun(float from, float to, float time)
{
	time -= 1.0f;
	return value * (time * time * time * time * time + 1.0f);
}
float QuintBothFun(float from, float to, float time)
{
	if (time < 0.5f)
	{
		return value * 16.0f * time * time * time * time * time;
	}
	else
	{
		time -= 1.0f;
		return value * (16.0f * time * time * time * time * time + 1.0f);
	}
}
float SineInFun(float from, float to, float time)
{
	return value * (1.0f - cosf(time * Math::PI *0.5f));
}
float SineOutFun(float from, float to, float time)
{
	return value * sinf(time * Math::PI *0.5f);
}
float SineBothFun(float from, float to, float time)
{
	return value * 0.5f * (1.0f - cosf(time * Math::PI));
}
float ExpoInFun(float from, float to, float time)
{
	if (time == 0.0f)
	{
		return from;
	}
	else
	{
		return value * powf(2.0f, 10.0f * (time - 1.0f));
	}
}
float ExpoOutFun(float from, float to, float time)
{
	if (time == 1.0f)
	{
		return to;
	}
	else
	{
		return value * (1.0f - powf(2.0f, -10.0f * time));
	}
}
float ExpoBothFun(float from, float to, float time)
{
	if (time == 0.0f)
	{
		return from;
	}

	if (time == 1.0f)
	{
		return to;
	}

	if (time < 0.5f)
	{
		return value * 0.5f * powf(2.0f, 20.0f * time - 10.0f);
	}
	else
	{
		return value * 0.5f * (2.0f - powf(2.0f, -20.0f * time + 10.0f));
	}
}
float CircInFun(float from, float to, float time)
{
	return value * (1.0f - sqrtf(1.0f - time * time));
}
float CircOutFun(float from, float to, float time)
{
	return value * sqrtf((2.0f - time) * time);
}
float CircBothFun(float from, float to, float time)
{
	if (time < 0.5f)
	{
		return value * 0.5f * (1.0f - sqrtf(1.0f - 4.0f * time * time));
	}
	else
	{
		time = time * 2.0f - 2.0f;
		return value * 0.5f * (sqrtf(1.0f - time * time) + 1);
	}
}
float ElasticInFun(float from, float to, float time)
{
	if (time == 0.0f)
	{
		return from;
	}

	if (time == 1.0f)
	{
		return to;
	}

	return value * -powf(2.0f, 10.0f * time - 10.0f) * sinf((3.33f * time - 3.58f) * Math::PI2);
}
float ElasticOutFun(float from, float to, float time)
{
	if (time == 0.0f)
	{
		return from;
	}

	if (time == 1.0f)
	{
		return to;
	}

	return value * (powf(2.0f, -10.0f * time) * sinf((3.33f * time - 0.25f) * Math::PI2) + 1.0f);
}
float ElasticBothFun(float from, float to, float time)
{
	if (time == 0.0f)
	{
		return from;
	}

	if (time == 1.0f)
	{
		return to;
	}

	if (time < 0.5f)
	{
		return value * -0.5f * powf(2.0f, 20.0f * time - 10.0f) * sinf((4.45f * time - 2.475f) * Math::PI2);
	}
	else
	{
		return value * (powf(2.0f, -20.0f * time + 10.0f) * sinf((4.45f * time - 2.475f) * Math::PI2) * 0.5f + 1.0f);
	}
}
float BackInFun(float from, float to, float time)
{
	return value * time * time * (2.70158f * time - 1.70158f);
}
float BackOutFun(float from, float to, float time)
{
	time -= 1.0f;
	return value * (time * time * (2.70158f * time + 1.70158f) + 1.0f);
}
float BackBothFun(float from, float to, float time)
{
	if (time < 0.5f)
	{
		return value * time * time * (14.379636f * time - 5.189818f);
	}
	else
	{
		time -= 1.0f;
		return value * (time * time * (14.379636f * time + 5.189818f) + 1.0f);
	}
}
float BounceOutFun(float from, float to, float time)
{
	if (time < 0.363636f)
	{
		return value * 7.5625f * time * time;
	}
	else if (time < 0.72727f)
	{
		time -= 0.545454f;
		return value * (7.5625f * time * time + 0.75f);
	}
	else if (time < 0.909091f)
	{
		time -= 0.818182f;
		return value * (7.5625f * time * time + 0.9375f);
	}
	else
	{
		time -= 0.954545f;
		return value * (7.5625f * time * time + 0.984375f);
	}
}
float BounceInFun(float from, float to, float time)
{
	if (time > 0.636364f)
	{
		time = 1.0f - time;
		return value * (1.0f - 7.5625f * time * time);
	}
	else if (time > 0.27273f)
	{
		time = 0.454546f - time;
		return value * (0.25f - 7.5625f * time * time);
	}
	else if (time > 0.090909f)
	{
		time = 0.181818f - time;
		return value * (0.0625f - 7.5625f * time * time);
	}
	else
	{
		time = 0.045455f - time;
		return value * (0.015625f - 7.5625f * time * time);
	}
}
float BounceBothFun(float from, float to, float time)
{
	if (time < 0.5f)
	{
		// bounce in
		if (time > 0.318182f)
		{
			time = 1.0f - time * 2.0f;
			return value * (0.5f - 3.78125f * time * time);
		}
		else if (time > 0.136365f)
		{
			time = 0.454546f - time * 2.0f;
			return value * (0.125f - 3.78125f * time * time);
		}
		else if (time > 0.045455f)
		{
			time = 0.181818f - time * 2.0f;
			return value * (0.03125f - 3.78125f * time * time);
		}
		else
		{
			time = 0.045455f - time * 2.0f;
			return value * (0.007813f - 3.78125f * time * time);
		}
	}

	// bounce out
	if (time < 0.681818f)
	{
		time = time * 2.0f - 1.0f;
		return value * (3.78125f * time * time + 0.5f);
	}
	else if (time < 0.863635f)
	{
		time = time * 2.0f - 1.545454f;
		return value * (3.78125f * time * time + 0.875f);
	}
	else if (time < 0.954546f)
	{
		time = time * 2.0f - 1.818182f;
		return value * (3.78125f * time * time + 0.96875f);
	}
	else
	{
		time = time * 2.0f - 1.954545f;
		return value * (3.78125f * time * time + 0.992188f);
	}
}
#undef value

typedef float (*EasyFun)(float from, float to, float time);
EasyFun EasyArray[(int)EasyType::Max] =
{
	LinearFun,

	QuadInFun,
	QuadOutFun,
	QuadBothFun,

	CubicInFun,
	CubicOutFun,
	CubicBothFun,

	QuartInFun,
	QuartOutFun,
	QuartBothFun,

	QuintInFun,
	QuintOutFun,
	QuintBothFun,

	SineInFun,
	SineOutFun,
	SineBothFun,

	ExpoInFun,
	ExpoOutFun,
	ExpoBothFun,

	CircInFun,
	CircOutFun,
	CircBothFun,

	ElasticInFun,
	ElasticOutFun,
	ElasticBothFun,

	BackInFun,
	BackOutFun,
	BackBothFun,

	BounceInFun,
	BounceOutFun,
	BounceBothFun,
};

float Easy::Do(EasyType type, float from, float to, float time)
{
	int t = int(type);
	CHECK_RETURN_PTR_NUMBER(t >= 0 && t < int(EasyType::Max));
	time = Math::Clamp01(time);
	return EasyArray[t](from, to, time);
}
Vector3 Easy::Do(EasyType type, const Vector3& from, const Vector3& to, float time)
{
	int t = int(type);
	AssertEx(t >= 0 && t < int(EasyType::Max),"");
	time = Math::Clamp01(time);
	Vector3 ret(EasyArray[t](from.x, to.x, time), EasyArray[t](from.y, to.y, time), EasyArray[t](from.z, to.z, time));
	return ret;
}
DC_END_NAMESPACE
