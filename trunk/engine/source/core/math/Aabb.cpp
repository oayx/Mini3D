#include "Aabb.h"
#include "Matrix4.h"
#include "core/geometry/Sphere.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(Aabb);
Aabb::Aabb(const Aabb &aabb)
{
	if (aabb.IsNull())
	{
		SetNULL();
	}
	else if (aabb.IsInfinite())
	{
		SetInfinite();
	}
	else
	{
		SetExtents(aabb._minimum, aabb._maximum);
	}
}
bool Aabb::operator== (const Aabb& rhs) const noexcept
{
	if (this->_extent != rhs._extent)
	{
		return false;
	}

	if (!this->IsFinite())
	{
		return true;
	}

	return ((_minimum == rhs._minimum) && (_maximum == rhs._maximum));
}
bool Aabb::operator!= (const Aabb& rhs) const noexcept
{
	return !(*this == rhs);
}
Aabb Aabb::operator *(const Matrix4& mat)const noexcept
{ 
	if (_extent != EXTENT_FINITE)
		return *this;

	Vector3 _min(MAX_float, MAX_float, MAX_float);
	Vector3 _max(MIN_float, MIN_float, MIN_float);
	Vector3 corners[8];
	this->GetCorners(corners);
	for (int i = 0; i < 8; ++i)
	{
		corners[i] = mat.TransformPoint(corners[i]);
		_min = Vector3::Min(_min, corners[i]);
		_max = Vector3::Max(_max, corners[i]);
	}
	return Aabb(_min, _max);
}
Aabb Aabb::operator *(const Vector3& vec)const noexcept
{
	if (_extent != EXTENT_FINITE)
		return *this;

	Vector3 _min = _minimum + vec;
	Vector3 _max = _maximum + vec;
	return Aabb(_min, _max);
}
void Aabb::SetAbsExtents(const Vector3& min, const Vector3& max) noexcept
{
	if (min.SquareSize() <= max.SquareSize())
	{
		_extent = EXTENT_FINITE;
		_minimum = min;
		_maximum = max;
	}
	else
	{
		_extent = EXTENT_FINITE;
		_minimum = max;
		_maximum = min;
	}
}
Vector3 Aabb::GetCorner(CornerEnum cornerToGet) const noexcept
{
	switch (cornerToGet)
	{
	case FAR_LEFT_BOTTOM:
		return _minimum;
	case FAR_LEFT_TOP:
		return Vector3(_minimum.x, _maximum.y, _minimum.z);
	case FAR_RIGHT_TOP:
		return Vector3(_maximum.x, _maximum.y, _minimum.z);
	case FAR_RIGHT_BOTTOM:
		return Vector3(_maximum.x, _minimum.y, _minimum.z);
	case NEAR_RIGHT_BOTTOM:
		return Vector3(_maximum.x, _minimum.y, _maximum.z);
	case NEAR_LEFT_BOTTOM:
		return Vector3(_minimum.x, _minimum.y, _maximum.z);
	case NEAR_LEFT_TOP:
		return Vector3(_minimum.x, _maximum.y, _maximum.z);
	case NEAR_RIGHT_TOP:
		return _maximum;
	default:
		return Vector3::zero;
	}
}
void Aabb::GetCorners(Vector3* corners)const noexcept
{
	corners[0] = _minimum;
	corners[1].x = _minimum.x; corners[1].y = _maximum.y; corners[1].z = _minimum.z;
	corners[2].x = _maximum.x; corners[2].y = _maximum.y; corners[2].z = _minimum.z;
	corners[3].x = _maximum.x; corners[3].y = _minimum.y; corners[3].z = _minimum.z;

	corners[4] = _maximum;
	corners[5].x = _minimum.x; corners[5].y = _maximum.y; corners[5].z = _maximum.z;
	corners[6].x = _minimum.x; corners[6].y = _minimum.y; corners[6].z = _maximum.z;
	corners[7].x = _maximum.x; corners[7].y = _minimum.y; corners[7].z = _maximum.z;
}
void Aabb::Scale(const Vector3& s) noexcept
{
	if (_extent != EXTENT_FINITE)
	{
		return;
	}

	Vector3 min = _minimum * s;
	Vector3 max = _maximum * s;
	SetExtents(min, max);
}
void Aabb::Merge(const Vector3 &vec) noexcept
{
	switch (_extent)
	{
	case EXTENT_NULL: // if null, use this point
		SetExtents(vec, vec);
		return;

	case EXTENT_FINITE:
		_maximum.MakeCeil(vec);
		_minimum.MakeFloor(vec);
		return;

	case EXTENT_INFINITE: // if infinite, makes no difference
		return;
	}
}
void Aabb::Merge(const Aabb &aabb) noexcept
{
	if ((aabb._extent == EXTENT_NULL) || (_extent == EXTENT_INFINITE))
	{
		return;
	}
	// Otherwise if rhs is infinite, make this infinite, too
	else if (aabb._extent == EXTENT_INFINITE)
	{
		_extent = EXTENT_INFINITE;
	}
	// Otherwise if current null, just take rhs
	else if (_extent == EXTENT_NULL)
	{
		SetExtents(aabb._minimum, aabb._maximum);
	}
	// Otherwise merge
	else
	{
		Vector3 min = _minimum;
		Vector3 max = _maximum;
		max.MakeCeil(aabb._maximum);
		min.MakeFloor(aabb._minimum);

		SetExtents(min, max);
	}
}
bool Aabb::Contains(const Vector3& v) const noexcept
{
	if (IsNull())
		return false;
	if (IsInfinite())
		return true;

	return _minimum.x <= v.x && v.x <= _maximum.x &&
		_minimum.y <= v.y && v.y <= _maximum.y &&
		_minimum.z <= v.z && v.z <= _maximum.z;
}
bool Aabb::Contains(const Aabb& aabb) const noexcept
{
	//根据实际情况调整
	if (aabb.IsNull() || this->IsInfinite())
		return true;

	if (this->IsNull() || aabb.IsInfinite())
		return false;

	return this->_minimum.x <= aabb._minimum.x &&
		this->_minimum.y <= aabb._minimum.y &&
		this->_minimum.z <= aabb._minimum.z &&
		aabb._maximum.x <= this->_maximum.x &&
		aabb._maximum.y <= this->_maximum.y &&
		aabb._maximum.z <= this->_maximum.z;
}
IntersectResult Aabb::Intersects(const Aabb &aabb)const noexcept
{
	if (this->IsNull() || aabb.IsNull())
	{
		return IntersectResult::Outside;
	}

	if (this->IsInfinite())
	{
		return IntersectResult::Inside;
	}

	if (aabb.IsInfinite())
	{
		return IntersectResult::Intersect;
	}

	const Vector3 &vecFirMin = this->GetMinimum();
	const Vector3 &vecFirMax = this->GetMaximum();
	const Vector3 &vecSecMin = aabb.GetMinimum();
	const Vector3 &vecSecMax = aabb.GetMaximum();

	if (vecSecMax.x < vecFirMin.x || vecSecMin.x > vecFirMax.x ||
		vecSecMax.y < vecFirMin.y || vecSecMin.y > vecFirMax.y ||
		vecSecMax.z < vecFirMin.z || vecSecMin.z > vecFirMax.z
		)
	{
		return IntersectResult::Outside;
	}

	bool bFull = (vecSecMin.x > vecFirMin.x && vecSecMax.x < vecFirMax.x &&
		vecSecMin.y > vecFirMin.y && vecSecMax.y < vecFirMax.y &&
		vecSecMin.z > vecFirMin.z && vecSecMax.z < vecFirMax.z
		);

	if (bFull)
	{
		return IntersectResult::Inside;
	}
	else
	{
		return IntersectResult::Intersect;
	}
}
IntersectResult Aabb::Intersects(const Sphere &sphere)const noexcept
{
	Vector3 p = sphere.center;
	if (p.x < _minimum.x)
		p.x = _minimum.x;
	else if (p.x > _maximum.x)
		p.x = _maximum.x;

	if (p.y < _minimum.y)
		p.y = _minimum.y;
	else if (p.y > _maximum.y)
		p.y = _maximum.y;

	if (p.z < _minimum.z)
		p.z = _minimum.z;
	else if (p.z > _maximum.z)
		p.z = _maximum.z;

	if (!sphere.Contain(p))
		return IntersectResult::Outside;
	
	return IntersectResult::Intersect;
}
DC_END_NAMESPACE
