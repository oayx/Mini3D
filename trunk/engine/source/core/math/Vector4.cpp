#include "Vector4.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(Vector4);
const Vector4 Vector4::zero(0, 0, 0, 0);
const Vector4 Vector4::one(1, 1, 1, 1);

Vector4 Vector4::Lerp(const Vector4& from, const Vector4& to, float t, bool clamp_01)
{
	return Vector4(
		Math::Lerp(from.x, to.x, t, clamp_01),
		Math::Lerp(from.y, to.y, t, clamp_01),
		Math::Lerp(from.z, to.z, t, clamp_01),
		Math::Lerp(from.w, to.w, t, clamp_01));
}
DC_END_NAMESPACE
