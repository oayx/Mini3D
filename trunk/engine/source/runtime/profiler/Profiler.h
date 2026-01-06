
/*****************************************************************************
* Author： hannibal
* Description：性能分析
*****************************************************************************/
#pragma once

#include "core/BaseType.h"
#include "tracy/tracy/Tracy.hpp"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL Profiler : public object
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Profiler);
	BEGIN_REFECTION_TYPE(Profiler)
	END_FINAL_REFECTION_TYPE;

};
DC_END_NAMESPACE