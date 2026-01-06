 
/*****************************************************************************************************/
// @author hannibal
// @date   2020/09/11
// @brief  
/*****************************************************************************************************/
#pragma once

#include "Physics2d_def.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class PhysicsDebugDraw2d final : public b2Draw
{
	friend class Physics2d;
	DECLARE_ALLOCATOR;
	DEFAULT_CREATE(PhysicsDebugDraw2d);
	FRIEND_CONSTRUCT_DESTRUCT(PhysicsDebugDraw2d);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(PhysicsDebugDraw2d);
public:
	virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)override;
	virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)override;
	virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override;
	virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)override;
	virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)override;
	virtual void DrawTransform(const b2Transform& xf)override;
};
DC_END_NAMESPACE
