
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
class ENGINE_DLL QualitySettings Final : public Object
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
	static MSAAType		_mSAAType;

	static float		_shadowDistance;
	static ShadowType	_shadowType;
	static ShadowResolution _shadowResolution;

	static bool			_enableHDR;
	static float		_hDRExposure;	//曝光值
	static ColorFormat	_hDRFormat;
};//Light
DC_END_NAMESPACE
