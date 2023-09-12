
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once
#if defined(DC_PLATFORM_LINUX)

#include "platform/PlatformBase.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class LinuxPlatform Final : public PlatformBase
{
	DEFAULT_CREATE(LinuxPlatform);
	FRIEND_CONSTRUCT_DESTRUCT(LinuxPlatform);
	DISALLOW_COPY_ASSIGN(LinuxPlatform);
	BEGIN_DERIVED_REFECTION_TYPE(LinuxPlatform, PlatformBase)
	END_DERIVED_REFECTION_TYPE;

public:
};
DC_END_NAMESPACE
#endif /*DC_PLATFORM_LINUX*/
