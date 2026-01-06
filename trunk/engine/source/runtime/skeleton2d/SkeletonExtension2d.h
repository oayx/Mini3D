
/*****************************************************************************
* Author： hannibal
* Date：2020/9/23
* Description：spine扩展
*****************************************************************************/
#pragma once

#include "core/Object.h"
#include <spine/spine.h>

namespace spine
{
	extern void releaseDefaultExtension();
}
DC_BEGIN_NAMESPACE
/********************************************************************/
class SpineTextureLoader final : public spine::TextureLoader
{
	virtual void load(spine::AtlasPage& page, const spine::String& path)override;
	virtual void unload(void* texture)override;
};

DC_END_NAMESPACE
