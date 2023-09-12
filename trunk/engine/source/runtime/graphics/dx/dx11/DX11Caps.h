
/*****************************************************************************
* Author： hannibal
* Date：2021年4月5日
* Description： 设备能力
*****************************************************************************/
#pragma once

#include "DX11Define.h"
#include "runtime/graphics/null/GraphicsCaps.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DX11Caps Final : public GraphicsCaps
{
	DEFAULT_CREATE(DX11Caps);
	FRIEND_CONSTRUCT_DESTRUCT(DX11Caps);
	BEGIN_DERIVED_REFECTION_TYPE(DX11Caps, GraphicsCaps)
	END_DERIVED_REFECTION_TYPE;

public:
	virtual void Initialize()override;

	virtual bool IsGenerateMipsSupported(TextureType type, ColorFormat format)override;
	virtual bool CheckTextureFormat(uint usage, ColorFormat format)override;
	virtual bool CheckCubeTextureFormat(uint usage, ColorFormat format)override;

public:
	//MSAA
	static MSAALevel GetMaximumMSAAQualityLevel(DXGI_FORMAT format);		//获得提供的最高等级msaa
	static MSAALevel GetMSAAQualityLevel(int antiAlias, DXGI_FORMAT format);

private:
};
DC_END_NAMESPACE