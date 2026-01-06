 
/*****************************************************************************************************/
// @author hannibal
// @date   2020/09/11
// @brief  2d物理世界
/*****************************************************************************************************/
#pragma once

#include "Physics2d_def.h"

DC_BEGIN_NAMESPACE
class Collider2d;
/********************************************************************/
class ContactListener2d final : public b2ContactListener
{
	friend class Physics2d;
	DECLARE_ALLOCATOR;
	DEFAULT_CREATE(ContactListener2d);
	FRIEND_CONSTRUCT_DESTRUCT(ContactListener2d);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(ContactListener2d);
public:
	virtual void BeginContact(b2Contact* contact)override;
	virtual void EndContact(b2Contact* contact)override;
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold)override;
	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)override;
};
DC_END_NAMESPACE
