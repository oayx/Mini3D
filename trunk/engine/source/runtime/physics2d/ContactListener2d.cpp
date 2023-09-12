#include "ContactListener2d.h"
#include "Collider2d.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
void ContactListener2d::BeginContact(b2Contact* contact)
{
}
void ContactListener2d::EndContact(b2Contact* contact)
{

}
void ContactListener2d::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	const b2Manifold* manifold = contact->GetManifold();
	if (manifold->pointCount == 0)
	{
		return;
	}

	void* userdata_1 = contact->GetFixtureA()->GetBody()->GetUserData();
	void* userdata_2 = contact->GetFixtureB()->GetBody()->GetUserData();
	if (userdata_1 != nullptr && userdata_2 != nullptr)
	{
		Collider2d* collider1 = reinterpret_cast<Collider2d*>(userdata_1);
		Collider2d* collider2 = reinterpret_cast<Collider2d*>(userdata_2);

		b2WorldManifold worldManifold;
		contact->GetWorldManifold(&worldManifold);

		Collision2D cp;
		cp.contactCount = manifold->pointCount <= b2_maxManifoldPoints ? manifold->pointCount : b2_maxManifoldPoints;
		for (int i = 0; i < manifold->pointCount && i < b2_maxManifoldPoints; ++i)
		{
			cp.contacts[i].position = Vector2(worldManifold.points[i].x * PTM_RATIO, worldManifold.points[i].y * PTM_RATIO);
			cp.contacts[i].normal = Fromb2Vec2(worldManifold.normal);
			cp.contacts[i].normalImpulse = manifold->points[i].normalImpulse;
			cp.contacts[i].tangentImpulse = manifold->points[i].tangentImpulse;
			cp.contacts[i].separation = worldManifold.separations[i];
		}

		//状态1是持久化或删除，而状态2是添加或持久化。
		b2PointState state = b2_nullState;
		b2PointState state1[b2_maxManifoldPoints], state2[b2_maxManifoldPoints];
		b2GetPointStates(state1, state2, oldManifold, manifold);
		{
			for (int i = 0; i < b2_maxManifoldPoints; ++i)
			{
				if (state2[i] == b2_addState)
				{
					if (!collider1->HasContact(collider2->GetInstanceId()) || !collider2->HasContact(collider1->GetInstanceId()))
					{
						state = b2_addState;
					}
				}
				if (state == b2_nullState && state2[i] == b2_persistState)
				{
					state = b2_persistState;
				}
			}

			for (int i = 0; i < b2_maxManifoldPoints; ++i)
			{
				if (state1[i] == b2_removeState)
				{
					if (collider1->HasContact(collider2->GetInstanceId()) || collider2->HasContact(collider1->GetInstanceId()))
					{
						state = b2_removeState;
					}
				}
				if (state == b2_nullState && state1[i] == b2_persistState)
				{
					state = b2_persistState;
				}
			}
		}

		{

			cp.collider = collider2;
			cp.otherCollider = collider1;
			switch (state)
			{
			case b2_addState:
				collider2->GetGameObject()->PostComponentAction(ComponentAction::CollisionEnter2D, true, false, &cp);
				break;
			case b2_persistState:
				collider2->GetGameObject()->PostComponentAction(ComponentAction::CollisionStay2D, true, false, &cp);
				break;
			case b2_removeState:
				collider2->GetGameObject()->PostComponentAction(ComponentAction::CollisionExit2D, true, false, &cp);
				break;
			}

			cp.collider = collider1;
			cp.otherCollider = collider2;
			switch (state)
			{
			case b2_addState:
				collider1->GetGameObject()->PostComponentAction(ComponentAction::CollisionEnter2D, true, false, &cp);
				break;
			case b2_persistState:
				collider1->GetGameObject()->PostComponentAction(ComponentAction::CollisionStay2D, true, false, &cp);
				break;
			case b2_removeState:
				collider1->GetGameObject()->PostComponentAction(ComponentAction::CollisionExit2D, true, false, &cp);
				break;
			}
		}
	}
}
void ContactListener2d::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{

}
DC_END_NAMESPACE
