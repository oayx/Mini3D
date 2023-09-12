#include "ContactListener.h"
#include "Collider.h"
#include "Physics.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
void ContactListener::CollisionDetection()
{
	//printf("--------------------\n");
	Physics::GetWorld()->performDiscreteCollisionDetection();
	int numManifolds = Physics::GetWorld()->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = Physics::GetWorld()->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* objA = (btCollisionObject*)(contactManifold->getBody0());
		btCollisionObject* objB = (btCollisionObject*)(contactManifold->getBody1());
		Component* colliderA = reinterpret_cast<Component*>(objA->getUserPointer());
		Component* colliderB = reinterpret_cast<Component*>(objB->getUserPointer());

		//printf("%s\n", colliderA->GetName().c_str()); // 碰撞点
		//printf("%s\n", colliderB->GetName().c_str());

		int numContacts = contactManifold->getNumContacts();

		Collision cp;
		cp.collider = colliderA;
		cp.otherCollider = colliderB;
		cp.contactCount = numContacts <= bt_maxManifoldPoints ? numContacts : bt_maxManifoldPoints;
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			cp.contacts[j].position = FrombtVec3(pt.getPositionWorldOnA());
			cp.contacts[j].normal = FrombtVec3(pt.m_normalWorldOnB);
		}
		colliderA->GetGameObject()->PostComponentAction(ComponentAction::CollisionStay, false, false, &cp);

		cp.collider = colliderB;
		cp.otherCollider = colliderA;
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			cp.contacts[j].position = FrombtVec3(pt.getPositionWorldOnB());
			cp.contacts[j].normal = FrombtVec3(pt.m_normalWorldOnB);
		}
		colliderB->GetGameObject()->PostComponentAction(ComponentAction::CollisionStay, false, false, &cp);
	}
}
DC_END_NAMESPACE
