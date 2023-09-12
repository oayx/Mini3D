 
/*****************************************************************************************************/
// @author hannibal
// @date   2020/09/24
// @brief  
/*****************************************************************************************************/
#pragma once

#include "Physics_def.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class PhysicsDebugDraw Final : public btIDebugDraw
{
	DECLARE_ALLOCATOR;
	DEFAULT_CREATE(PhysicsDebugDraw);
	FRIEND_CONSTRUCT_DESTRUCT(PhysicsDebugDraw);
public:
	virtual void	drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
	virtual void	drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
	virtual void	reportErrorWarning(const char* warningString) override;
	virtual void	draw3dText(const btVector3& location, const char* textString) override;
	virtual void	setDebugMode(int debugMode) override;
	virtual int		getDebugMode() const override;
};
DC_END_NAMESPACE
