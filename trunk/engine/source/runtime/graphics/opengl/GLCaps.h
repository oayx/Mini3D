
/*****************************************************************************
* Author： hannibal
* Date：2021年4月5日
* Description： 设备能力
*****************************************************************************/
#pragma once

#include "GLDefine.h"
#include "runtime/graphics/null/GraphicsCaps.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class GLCaps Final : public GraphicsCaps
{
	DEFAULT_CREATE(GLCaps);
	FRIEND_CONSTRUCT_DESTRUCT(GLCaps);
	BEGIN_DERIVED_REFECTION_TYPE(GLCaps, GraphicsCaps)
	END_DERIVED_REFECTION_TYPE;

public:
	virtual void Initialize()override;
};
DC_END_NAMESPACE