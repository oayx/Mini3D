#include "Ray.h"
#include "Plane.h"
#include "Triangle.h"
#include "../math/Aabb.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(Ray);
std::pair<bool, float> Ray::Intersects(const Aabb& box)const noexcept
{
	if (box.IsNull()) return std::pair<bool, float>(false, 0.0f);
	if (box.IsInfinite()) return std::pair<bool, float>(true, 0.0f);
	
	float lowt = 0.0f;
	float t;
	bool hit = false;
	Vector3 hitpoint;
	const Vector3& min = box.GetMinimum();
	const Vector3& max = box.GetMaximum();
	const Vector3& rayorig = this->GetOrigin();
	const Vector3& raydir = this->GetDir();

	// Check origin inside first
	if (rayorig > min && rayorig < max)
	{
		return std::pair<bool, float>(true, 0.0f);
	}

	// Check each face in turn, only check closest 3
	// Min x
	if (rayorig.x <= min.x && raydir.x > 0)
	{
		t = (min.x - rayorig.x) / raydir.x;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Max x
	if (rayorig.x >= max.x && raydir.x < 0)
	{
		t = (max.x - rayorig.x) / raydir.x;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Min y
	if (rayorig.y <= min.y && raydir.y > 0)
	{
		t = (min.y - rayorig.y) / raydir.y;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Max y
	if (rayorig.y >= max.y && raydir.y < 0)
	{
		t = (max.y - rayorig.y) / raydir.y;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Min z
	if (rayorig.z <= min.z && raydir.z > 0)
	{
		t = (min.z - rayorig.z) / raydir.z;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.y >= min.y && hitpoint.y <= max.y &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Max z
	if (rayorig.z >= max.z && raydir.z < 0)
	{
		t = (max.z - rayorig.z) / raydir.z;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.y >= min.y && hitpoint.y <= max.y &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}

	return std::pair<bool, float>(hit, lowt);
}

std::pair<bool, float> Ray::Intersects(const Triangle& triangle, bool positiveSide, bool negativeSide)const noexcept
{
	Vector3 a = triangle.first;
	Vector3 b = triangle.secord;
	Vector3 c = triangle.third;
	Vector3 normal = (b - a).Cross(c - a);

	//
	// Calculate intersection with plane.
	//
	float t;
	{
		float denom = normal.Dot(this->GetDir());

		// Check intersect side
		if (denom > +std::numeric_limits<float>::epsilon())
		{
			if (!negativeSide)
				return std::pair<bool, float>(false, 0.0f);
		}
		else if (denom < -std::numeric_limits<float>::epsilon())
		{
			if (!positiveSide)
				return std::pair<bool, float>(false, 0.0f);
		}
		else
		{
			// Parallel or triangle area is close to zero when
			// the plane normal not normalised.
			return std::pair<bool, float>(false, 0.0f);
		}

		t = normal.Dot(a - this->GetOrigin()) / denom;
		if (t < 0)
		{
			// Intersection is behind origin
			return std::pair<bool, float>(false, 0.0f);
		}
	}

	//
	// Calculate the largest area projection plane in X, Y or Z.
	//
	size_t i0, i1;
	{
		float n0 = Math::Abs(normal[0]);
		float n1 = Math::Abs(normal[1]);
		float n2 = Math::Abs(normal[2]);

		i0 = 1; i1 = 2;
		if (n1 > n2)
		{
			if (n1 > n0) i0 = 0;
		}
		else
		{
			if (n2 > n0) i1 = 0;
		}
	}

	//
	// Check the intersection point is inside the triangle.
	//
	{
		float u1 = b[i0] - a[i0];
		float v1 = b[i1] - a[i1];
		float u2 = c[i0] - a[i0];
		float v2 = c[i1] - a[i1];
		float u0 = t * this->GetDir()[i0] + this->GetOrigin()[i0] - a[i0];
		float v0 = t * this->GetDir()[i1] + this->GetOrigin()[i1] - a[i1];

		float alpha = u0 * v2 - u2 * v0;
		float beta = u1 * v0 - u0 * v1;
		float area = u1 * v2 - u2 * v1;

		// epsilon to avoid float precision error
		float fEsp = 0.00001f;

		float tolerance = -fEsp * area;

		if (area > 0)
		{
			if (alpha < tolerance || beta < tolerance || alpha + beta > area - tolerance)
				return std::pair<bool, float>(false, 0.0f);
		}
		else
		{
			if (alpha > tolerance || beta > tolerance || alpha + beta < area - tolerance)
				return std::pair<bool, float>(false, 0.0f);
		}
	}

	return std::pair<bool, float>(true, t);
}

std::pair<bool, float> Ray::Intersects(const Plane& plane)const noexcept
{

	float denom = plane.normal.Dot(this->GetDir());
	if (Math::Abs(denom) < std::numeric_limits<float>::epsilon())
	{
		// Parallel
		return std::pair<bool, float>(false, 0.0f);
	}
	else
	{
		float nom = plane.normal.Dot(this->GetOrigin()) + plane.d;
		float t = -(nom / denom);
		return std::pair<bool, float>(t >= 0, t);
	}
}
DC_END_NAMESPACE
