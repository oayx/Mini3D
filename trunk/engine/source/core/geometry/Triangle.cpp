#include "Triangle.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(Triangle);
Vector3 Triangle::GetNormal()
{
	Vector3 n = Vector3(secord - first).Cross(Vector3(third - first));
	n.Normalize();
	return n;
}
DC_END_NAMESPACE
