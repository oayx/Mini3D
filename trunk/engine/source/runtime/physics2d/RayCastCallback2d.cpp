#include "RayCastCallback2d.h"
#include "Collider2d.h"
#include "runtime/scene/LayerMask.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
float32 RayCastClosestCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
{
	b2Body* body = fixture->GetBody();
	void* userData = body->GetUserData();
	if (userData)
	{
		Collider2d* collider = reinterpret_cast<Collider2d*>(userData);
		if (_layerMask == 0)
		{
			_info.gameobject = collider->GetGameObject();
		}
		else
		{
			uint mask = LayerMask::GetMask(collider->GetGameObject()->GetLayer());
			if (mask & _layerMask)
			{
				_info.gameobject = collider->GetGameObject();
			}
		}
	}
	if (_info.gameobject == nullptr)
	{
		return -1.0f;
	}

	_hit = true;
	_info.point = Vector2(point.x*PTM_RATIO, point.y*PTM_RATIO);
	_info.normal = Vector2(normal.x, normal.y);

	// By returning the current fraction, we instruct the calling code to clip the ray and
	// continue the ray-cast to the next fixture. WARNING: do not assume that fixtures
	// are reported in order. However, by clipping, we can always get the closest fixture.
	return fraction;
}

/********************************************************************/
float32 RayCastAnyCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
{
	b2Body* body = fixture->GetBody();
	void* userData = body->GetUserData();
	if (userData)
	{
		Collider2d* collider = reinterpret_cast<Collider2d*>(userData);
		if (_layerMask == 0)
		{
			_info.gameobject = collider->GetGameObject();
		}
		else
		{
			uint mask = LayerMask::GetMask(collider->GetGameObject()->GetLayer());
			if (mask & _layerMask)
			{
				_info.gameobject = collider->GetGameObject();
			}
		}
	}
	if (_info.gameobject == nullptr)
	{
		return -1.0f;
	}

	_hit = true;
	_info.point = Vector2(point.x*PTM_RATIO, point.y*PTM_RATIO);
	_info.normal = Vector2(normal.x, normal.y);

	// At this point we have a hit, so we know the ray is obstructed.
	// By returning 0, we instruct the calling code to terminate the ray-cast.
	return 0.0f;
}

/********************************************************************/
float32 RayCastMultipleCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
{
	b2Body* body = fixture->GetBody();
	void* userData = body->GetUserData();
	RaycastHit2D info;
	if (userData)
	{
		Collider2d* collider = reinterpret_cast<Collider2d*>(userData);
		if (_layerMask == 0)
		{
			info.gameobject = collider->GetGameObject();
		}
		else
		{
			uint mask = LayerMask::GetMask(collider->GetGameObject()->GetLayer());
			if (mask & _layerMask)
			{
				info.gameobject = collider->GetGameObject();
			}
		}
	}
	if (info.gameobject == nullptr)
	{
		return -1.0f;
	}

	info.point = Vector2(point.x*PTM_RATIO, point.y*PTM_RATIO);
	info.normal = Vector2(normal.x, normal.y);
	_list.Add(info);

	// By returning 1, we instruct the caller to continue without clipping the ray.
	return 1.0f;
}
DC_END_NAMESPACE
