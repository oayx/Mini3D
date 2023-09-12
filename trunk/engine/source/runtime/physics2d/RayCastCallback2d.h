 
/*****************************************************************************************************/
// @author hannibal
// @date   2020/09/11
// @brief  2d物理世界
/*****************************************************************************************************/
#pragma once

#include "Physics2d_def.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class RayCastClosestCallback Final : public b2RayCastCallback
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(RayCastClosestCallback);
public:
	RayCastClosestCallback(const RaycastHit2D&& info, uint layerMask)
	{
		_info = std::move(info);
		_layerMask = layerMask;
	}

	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction);

	bool _hit = false;
	RaycastHit2D _info;
	uint _layerMask = 0;
};

class RayCastAnyCallback Final : public b2RayCastCallback
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(RayCastAnyCallback);
public:
	RayCastAnyCallback(const RaycastHit2D&& info, uint layerMask)
	{
		_info = std::move(info);
		_layerMask = layerMask;
	}

	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction);

	bool _hit = false;
	RaycastHit2D _info;
	uint _layerMask = 0;
};

class RayCastMultipleCallback Final : public b2RayCastCallback
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(RayCastMultipleCallback);
public:
	RayCastMultipleCallback(const Vector<RaycastHit2D>&& list, uint layerMask)
	{
		_list = std::move(list);
		_layerMask = layerMask;
	}

	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction);

	Vector<RaycastHit2D> _list;
	uint _layerMask = 0;
};
DC_END_NAMESPACE
