
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL Template : public Object
{
	DEFAULT_CREATE(Template);
	FRIEND_CONSTRUCT_DESTRUCT(Template);
	DISALLOW_COPY_ASSIGN(Template);
	BEGIN_DERIVED_REFECTION_TYPE(Template, Object)
	END_REFECTION_TYPE;

public:
	Template() {}
	~Template() {}
};
DC_END_NAMESPACE