
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once
#if defined(DC_PLATFORM_WASM)

#include "platform/PlatformBase.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL WASMPlatform final : public PlatformBase
{
	DEFAULT_CREATE(WASMPlatform);
	FRIEND_CONSTRUCT_DESTRUCT(WASMPlatform);
	DISALLOW_COPY_ASSIGN(WASMPlatform);
	BEGIN_DERIVED_REFECTION_TYPE(WASMPlatform, PlatformBase)
	END_REFECTION_TYPE;

public:
};
DC_END_NAMESPACE
#endif /*DC_PLATFORM_WASM*/
