#include "PhysicsDebugDraw2d.h"
#include "runtime/renderable/Gizmos.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
void PhysicsDebugDraw2d::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	Gizmos::SetMatrix(Matrix4::identity);
	Gizmos::SetColor(Color(color.r, color.g, color.b, 1));

	Vector3v list;
	for (int i = 0; i < vertexCount; i++)
	{
		list.Add(Vector3(vertices[i].x*PTM_RATIO, vertices[i].y*PTM_RATIO, 0));
	}
	if (!list.IsEmpty())list.Add(*list.begin());

	Gizmos::DrawPolygon(list);
}
void PhysicsDebugDraw2d::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	Gizmos::SetMatrix(Matrix4::identity);
	Gizmos::SetColor(Color(color.r, color.g, color.b, 1));

	Vector3v list;
	for (int i = 0; i < vertexCount; i++)
	{
		list.Add(Vector3(vertices[i].x*PTM_RATIO, vertices[i].y*PTM_RATIO, 0));
	}
	if (!list.IsEmpty())list.Add(*list.begin());

	Gizmos::DrawPolygon(list);
}
void PhysicsDebugDraw2d::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) 
{
	Gizmos::SetMatrix(Matrix4::identity);
	Gizmos::SetColor(Color(color.r, color.g, color.b, 1));

	Gizmos::DrawCircle(Vector3(center.x*PTM_RATIO, center.y*PTM_RATIO, 0), radius*PTM_RATIO);
}
void PhysicsDebugDraw2d::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	Gizmos::SetMatrix(Matrix4::identity);
	Gizmos::SetColor(Color(color.r, color.g, color.b, 1));

	Gizmos::DrawCircle(Vector3(center.x*PTM_RATIO, center.y*PTM_RATIO, 0), radius*PTM_RATIO);
}
void PhysicsDebugDraw2d::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	Gizmos::SetMatrix(Matrix4::identity);
	Gizmos::SetColor(Color(color.r, color.g, color.b, 1));

	Vector3v list;
	list.Add(Vector3(p1.x*PTM_RATIO, p1.y*PTM_RATIO, 0));
	list.Add(Vector3(p2.x*PTM_RATIO, p2.y*PTM_RATIO, 0));

	Gizmos::DrawPolygon(list);
}
void PhysicsDebugDraw2d::DrawTransform(const b2Transform& xf)
{
}
DC_END_NAMESPACE
