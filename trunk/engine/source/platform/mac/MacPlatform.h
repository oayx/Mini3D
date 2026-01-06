
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once
#if defined(DC_PLATFORM_MAC)

#include "platform/PlatformBase.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL MacPlatform final : public PlatformBase
{
	DEFAULT_CREATE(MacPlatform);
	FRIEND_CONSTRUCT_DESTRUCT(MacPlatform);
	DISALLOW_COPY_ASSIGN(MacPlatform);
	BEGIN_DERIVED_REFECTION_TYPE(MacPlatform, PlatformBase)
	END_REFECTION_TYPE;

public:
};
DC_END_NAMESPACE
#endif /*DC_PLATFORM_MAC*/
