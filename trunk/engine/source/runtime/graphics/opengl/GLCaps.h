
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
class GLCaps final : public GraphicsCaps
{
	DEFAULT_CREATE(GLCaps);
	FRIEND_CONSTRUCT_DESTRUCT(GLCaps);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(GLCaps);
	BEGIN_DERIVED_REFECTION_TYPE(GLCaps, GraphicsCaps)
	END_REFECTION_TYPE;

public:
	virtual void Initialize()override;

	bool QueryExtension(const char* ext);

private:
	Set<const char*> _extensions;
};
DC_END_NAMESPACE