#include "PhysicsDebugDraw.h"
#include "runtime/renderable/Gizmos.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
void PhysicsDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	Gizmos::SetMatrix(Matrix4::identity);
	Gizmos::SetColor(Color(color.getX(), color.getY(), color.getZ(), 1));
	Gizmos::DrawLine(FrombtVec3(from), FrombtVec3(to));
}
void PhysicsDebugDraw::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{

}
void PhysicsDebugDraw::reportErrorWarning(const char* warningString)
{

}
void PhysicsDebugDraw::draw3dText(const btVector3& location, const char* textString)
{

}
void PhysicsDebugDraw::setDebugMode(int debugMode)
{

}
int PhysicsDebugDraw::getDebugMode() const
{
	return btIDebugDraw::DBG_DrawAabb;
}
DC_END_NAMESPACE
