 
/*****************************************************************************************************/
// @author hannibal
// @date   2020/09/24
// @brief  
/*****************************************************************************************************/
#pragma once

#include "Physics_def.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ContactListener final : public object
{
	DEFAULT_CREATE(ContactListener);
	FRIEND_CONSTRUCT_DESTRUCT(ContactListener);
public:
	void CollisionDetection();
};
DC_END_NAMESPACE
