
/*****************************************************************************
* Author： hannibal
* Date：2021/9/23
* Description：质量设置
*****************************************************************************/
#pragma once

#include "runtime/graphics/RenderDefine.h"
#include "runtime/scene/SceneUtils.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL QualitySettings final : public Object
{
	friend class Application;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(QualitySettings);

public:
	static void Load();
	static void Save();

	static MSAAType		GetMSAAType() { return _mSAAType; }
	static void			SetMSAAType(MSAAType type) { _mSAAType = (MSAAType)Math::Clamp((int)type, 1, (int)MSAAType::Level_8); Save(); }

	static float		GetShadowDistance() { return _shadowDistance; }
	static void			SetShadowDistance(float distance) { _shadowDistance = distance; Save(); }
	static ShadowType	GetShadowType() { return _shadowType; }
	static void			SetShadowType(ShadowType type);
	static ShadowResolution	GetShadowResolution() { return _shadowResolution; }
	static void			SetShadowResolution(ShadowResolution resolution);

	static bool			IsEnableHDR() { return _enableHDR; }
	static void			SetEnableHDR(bool b) { _enableHDR = b; }
	static float		GetHDRExposure() { return _hDRExposure; }
	static void			SetHDRExposure(float exposure) { _hDRExposure = exposure; }
	static ColorFormat	GetHDRFormat() { return _hDRFormat; }
	static void			SetHDRFormat(ColorFormat format) { _hDRFormat = format; }

private:
	inline static MSAAType		_mSAAType = MSAAType::Disabled;

	inline static float			_shadowDistance = 200.0f;
	inline static ShadowType	_shadowType = ShadowType::Soft;
	inline static ShadowResolution _shadowResolution = ShadowResolution::Medium;

	inline static bool			_enableHDR = true;
	inline static float			_hDRExposure = 0.8f;	//曝光值
	inline static ColorFormat	_hDRFormat = ColorFormat::RGBA16F;
};//Light
DC_END_NAMESPACE
