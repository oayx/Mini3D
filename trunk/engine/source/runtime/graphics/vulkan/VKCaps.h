
/*****************************************************************************
* Author： hannibal
* Date：2021年4月5日
* Description： 设备能力
*****************************************************************************/
#pragma once
#include "VKDefine.h"
#include "runtime/graphics/null/GraphicsCaps.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class VKCaps final : public GraphicsCaps
{
	DEFAULT_CREATE(VKCaps);
	FRIEND_CONSTRUCT_DESTRUCT(VKCaps);
	BEGIN_DERIVED_REFECTION_TYPE(VKCaps, GraphicsCaps)
	END_REFECTION_TYPE;

public:
	virtual void Initialize()override;

public:

private:
};
DC_END_NAMESPACE