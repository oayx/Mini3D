
/*****************************************************************************
* Author： hannibal
* Date：2021年4月5日
* Description： 设备能力
*****************************************************************************/
#pragma once

#include "DX9Define.h"
#include "runtime/graphics/null/GraphicsCaps.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DX9Caps Final : public GraphicsCaps
{
	DEFAULT_CREATE(DX9Caps);
	FRIEND_CONSTRUCT_DESTRUCT(DX9Caps);
	BEGIN_DERIVED_REFECTION_TYPE(DX9Caps, GraphicsCaps)
	END_DERIVED_REFECTION_TYPE;

public:
	virtual void Initialize()override;
	virtual bool CheckTextureFormat(uint usage, ColorFormat format)override;
	virtual bool CheckCubeTextureFormat(uint usage, ColorFormat format)override;	
	virtual bool CheckRenderTextureFormat(ColorFormat format)override;
	virtual bool CheckSRGBTextureReadFormat(ColorFormat format)override;
	virtual bool CheckSRGBTextureWriteFormat(ColorFormat format)override;

public:
	//MSAA
	static MSAALevel GetMSAAQualityLevel(int antiAlias, D3DFORMAT backBufferFormat, bool windowed);

private:
	D3DCAPS9 _caps;
};
DC_END_NAMESPACE