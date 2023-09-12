
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once
#if defined(DC_PLATFORM_ANDROID)

#include "platform/PlatformBase.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL AndroidPlatform Final : public PlatformBase
{
	DEFAULT_CREATE(AndroidPlatform);
	FRIEND_CONSTRUCT_DESTRUCT(AndroidPlatform);
	DISALLOW_COPY_ASSIGN(AndroidPlatform);
	BEGIN_DERIVED_REFECTION_TYPE(AndroidPlatform, PlatformBase)
	END_DERIVED_REFECTION_TYPE;

public:
};
DC_END_NAMESPACE
#endif /*DC_PLATFORM_ANDROID*/
