
/*****************************************************************************
* Author： hannibal
* Date：2021/1/8
* Description：tween算法(参考https://github.com/jesusgollonet/ofpennereasing)
*****************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
enum class EasyType
{
	Linear,

	/* quadratic */
	QuadIn,
	QuadOut,
	QuadInOut,

	/* cubic */
	Cubicin,
	Cubicout,
	CubicInOut,

	/* quartic */
	QuartIn,
	QuartOut,
	QuartInOut,

	/* quintic */
	QuintIn,
	QuintOut,
	QuintInOut,

	/* sine */
	SineIn,
	SineOut,
	SineInOut,

	/* exponential */
	ExpoIn,
	ExpoOut,
	ExpoInOut,

	/* circular */
	CircIn,
	CircOut,
	CircInOut,

	/* elastic */
	ElasticIn,
	ElasticOut,
	ElasticInOut,

	/* back */
	BackIn,
	BackOut,
	BackInOut,

	/* bounce */
	BounceIn,
	BounceOut,
	BounceInOut,

	Max,
};
DECLARE_ENUM_OPERATORS(EasyType);

class ENGINE_DLL Easy final
{
public:
	/// <summary>
	/// 执行
	/// </summary>
	/// <param name="type">类型</param>
	/// <param name="from">源</param>
	/// <param name="to">目标</param>
	/// <param name="time">归一化的时间[0,1]</param>
	/// <returns></returns>
	static float Do(EasyType type, float from, float to, float time);
	static Vector3 Do(EasyType type, const Vector3& from, const Vector3& to, float time);
};

DC_END_NAMESPACE
