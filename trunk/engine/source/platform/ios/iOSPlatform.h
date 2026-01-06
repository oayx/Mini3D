
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once
#if defined(DC_PLATFORM_IOS)

#include "platform/PlatformBase.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL iOSPlatform final : public PlatformBase
{
	DEFAULT_CREATE(iOSPlatform);
	FRIEND_CONSTRUCT_DESTRUCT(iOSPlatform);
	DISALLOW_COPY_ASSIGN(iOSPlatform);
	BEGIN_DERIVED_REFECTION_TYPE(iOSPlatform, PlatformBase)
	END_REFECTION_TYPE;

public:
};
DC_END_NAMESPACE
#endif /*DC_PLATFORM_IOS*/
