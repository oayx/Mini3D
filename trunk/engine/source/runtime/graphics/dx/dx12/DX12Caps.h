
/*****************************************************************************
* Author： hannibal
* Date：2021年4月5日
* Description： 设备能力
*****************************************************************************/
#pragma once

#include "DX12Define.h"
#include "runtime/graphics/null/GraphicsCaps.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DX12Caps final : public GraphicsCaps
{
	DEFAULT_CREATE(DX12Caps);
	FRIEND_CONSTRUCT_DESTRUCT(DX12Caps);
	BEGIN_DERIVED_REFECTION_TYPE(DX12Caps, GraphicsCaps)
	END_REFECTION_TYPE;

public:
	virtual void Initialize()override;

public:
	//MSAA
	static MSAALevel GetMaximumMSAAQualityLevel(DXGI_FORMAT format);		//获得提供的最高等级msaa
	static MSAALevel GetMSAAQualityLevel(int antiAlias, DXGI_FORMAT format);

private:
};
DC_END_NAMESPACE
