#include "TransfromTween.h"
#include "TweenManager.h"
#include "runtime/component/Transform.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(MoveTween, Tween);
MoveTween* MoveTween::To(Transform* target, const Vector3& target_pos, float time, SimulationSpace space)
{
	CHECK_RETURN_PTR_NULL(target);
	Vector3 position = space == SimulationSpace::World ? target->GetPosition() : target->GetLocalPosition();
	MoveTween *tween = Memory::New<MoveTween>();
	tween->AutoRelease();
	tween->_startValue = position;
	tween->_targetValue = target_pos;
	tween->_transformTime = time;
	tween->_space = space;
	tween->_target = target;
	return tween;
}
MoveTween* MoveTween::Speed(Transform* target, const Vector3& speed, float time, SimulationSpace space)
{
	CHECK_RETURN_PTR_NULL(target);
	Vector3 position = space == SimulationSpace::World ? target->GetPosition() : target->GetLocalPosition();
	MoveTween* tween = Memory::New<MoveTween>();
	tween->AutoRelease();
	tween->_startValue = position;
	tween->_targetValue = position + speed * time;
	tween->_transformTime = time;
	tween->_space = space;
	tween->_target = target;
	return tween;
}
void MoveTween::OnUpdate(float currTime)
{
	Vector3 pos;
	if (currTime >= _endTime)
	{
		pos = _targetValue;
	}
	else
	{
		float elapasedTime = currTime - _startTime;
		pos = Easy::Do(_easyType, _startValue, _targetValue, elapasedTime / _transformTime);
	}
	Transform* transform = dynamic_cast<Transform*>(_target);
	if (_space == SimulationSpace::World)
		transform->SetPosition(pos);
	else
		transform->SetLocalPosition(pos);
}
Tween* MoveTween::OnFrom(bool is_relative)
{
	Transform* transform = dynamic_cast<Transform*>(_target);
	Vector3 src_pos = _startValue;
	if (is_relative)
	{
		if (_space == SimulationSpace::World)
			_startValue = transform->GetPosition() + _targetValue;
		else
			_startValue = transform->GetLocalPosition() + _targetValue;
	}
	else
	{
		_startValue = _targetValue;
	}
	_targetValue = src_pos;

	if (_space == SimulationSpace::World)
		transform->SetPosition(_startValue);
	else
		transform->SetLocalPosition(_startValue);
	return this;
}

/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(RotateTween, Tween);
RotateTween* RotateTween::To(Transform* target, const Vector3& euler, float time, SimulationSpace space)
{
	CHECK_RETURN_PTR_NULL(target);
	Vector3 rotate = space == SimulationSpace::World ? target->GetRotation().ToEuler() : target->GetLocalRotation().ToEuler();
	RotateTween* tween = Memory::New<RotateTween>();
	tween->AutoRelease();
	tween->_startValue = rotate;
	tween->_targetValue = rotate + euler;
	tween->_transformTime = time;
	tween->_space = space;
	tween->_target = target;
	return tween;
}
RotateTween* RotateTween::Speed(Transform* target, const Vector3& speed, float time, SimulationSpace space)
{
	CHECK_RETURN_PTR_NULL(target);
	Vector3 rotate = space == SimulationSpace::World ? target->GetRotation().ToEuler() : target->GetLocalRotation().ToEuler();
	RotateTween* tween = Memory::New<RotateTween>();
	tween->AutoRelease();
	tween->_startValue = rotate;
	tween->_targetValue = rotate + speed * time;
	tween->_transformTime = time;
	tween->_space = space;
	tween->_target = target;
	return tween;
}
void RotateTween::OnUpdate(float currTime)
{
	Transform* transform = dynamic_cast<Transform*>(_target);

	Vector3 euler;
	if (currTime >= _endTime)
	{
		euler = _targetValue;
	}
	else
	{
		float elapasedTime = currTime - _startTime;
		euler = Easy::Do(_easyType, _startValue, _targetValue, elapasedTime / _transformTime);
	}

	Quaternion rotate = Quaternion(euler);
	if (_space == SimulationSpace::World)
		transform->SetRotation(rotate);
	else
		transform->SetLocalRotation(rotate);
}
Tween* RotateTween::OnFrom(bool is_relative)
{
	Transform* transform = dynamic_cast<Transform*>(_target);

	Vector3 euler = _startValue;
	if (is_relative)
	{
		if (_space == SimulationSpace::World)
			_startValue = transform->GetRotation().ToEuler() + _targetValue;
		else
			_startValue = transform->GetLocalRotation().ToEuler() + _targetValue;
	}
	else
	{
		_startValue = _targetValue;
	}
	_targetValue = euler;

	Quaternion rotate = Quaternion(_startValue);
	if (_space == SimulationSpace::World)
		transform->SetRotation(rotate);
	else
		transform->SetLocalRotation(rotate);
	return this;
}

/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(RotateAroundTween, Tween);
RotateAroundTween* RotateAroundTween::Create(Transform* target, const Vector3& point, const Vector3& axis, float degree, SimulationSpace space)
{
	CHECK_RETURN_PTR_NULL(target);
	RotateAroundTween* tween = Memory::New<RotateAroundTween>();
	tween->AutoRelease();
	tween->_point = point;
	tween->_axis = axis;
	tween->_degree = degree;
	tween->_space = space;
	tween->_target = target;
	tween->_transformTime = MAX_float;
	return tween;
}
void RotateAroundTween::OnUpdate(float currTime)
{
	Transform* transform = dynamic_cast<Transform*>(_target);

	float delta_time = TweenManager::GetDeltaTime();
	if (_space == SimulationSpace::World)
		transform->RotateAround(_point, _axis, _degree * delta_time);
	else
		transform->RotateAround(_point, _axis, _degree* delta_time);
}
Tween* RotateAroundTween::OnFrom(bool is_relative)
{
	Transform* transform = dynamic_cast<Transform*>(_target);

	float delta_time = TweenManager::GetDeltaTime();
	if (_space == SimulationSpace::World)
		transform->RotateAround(_point, _axis, -_degree * delta_time);
	else
		transform->RotateAround(_point, _axis, -_degree * delta_time);
	return this;
}

/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(ScaleTween, Tween);
ScaleTween* ScaleTween::To(Transform* target, const Vector3& target_scale, float time)
{
	CHECK_RETURN_PTR_NULL(target);
	const Vector3& scale = target->GetLocalScale();

	ScaleTween* tween = Memory::New<ScaleTween>();
	tween->AutoRelease();
	tween->_startValue = scale;
	tween->_targetValue = target_scale;
	tween->_transformTime = time;
	tween->_target = target;
	return tween;
}
void ScaleTween::OnUpdate(float currTime)
{
	Transform* transform = dynamic_cast<Transform*>(_target);
	Vector3 scale;
	if (currTime >= _endTime)
	{
		scale = _targetValue;
	}
	else
	{
		float elapasedTime = currTime - _startTime;
		scale = Easy::Do(_easyType, _startValue, _targetValue, elapasedTime / _transformTime);
	}
	transform->SetLocalScale(scale);
}
Tween* ScaleTween::OnFrom(bool is_relative)
{
	Transform* transform = dynamic_cast<Transform*>(_target);
	Vector3 src_scale = _startValue;
	if (is_relative)
		_startValue = transform->GetLocalScale() + _targetValue;
	else
		_startValue = _targetValue;
	_targetValue = src_scale;

	transform->SetLocalScale(_startValue);
	return this;
}

/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(FlipTween, Tween);
FlipTween* FlipTween::Create(Transform* target, bool flipX, bool flipY, bool flipZ)
{
	CHECK_RETURN_PTR_NULL(target);
	FlipTween *tween = Memory::New<FlipTween>();
	tween->AutoRelease();
	tween->_flipX = flipX;
	tween->_flipY = flipY;
	tween->_flipZ = flipZ;
	tween->_target = target;
	return tween;
}
void FlipTween::OnStarted()
{
	Transform* transform = dynamic_cast<Transform*>(_target);
	Vector3 scale = transform->GetLocalScale();
	if (_flipX)scale.x = -scale.x;
	if (_flipY)scale.y = -scale.y;
	if (_flipZ)scale.z = -scale.z;
	transform->SetLocalScale(Vector3(scale.x, scale.y, scale.z));
}

/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(PlaceTween, Tween);
PlaceTween* PlaceTween::Create(Transform* target, const Vector3& pos)
{
	CHECK_RETURN_PTR_NULL(target);
	PlaceTween *tween = Memory::New<PlaceTween>();
	tween->AutoRelease();
	tween->_position = pos;
	tween->_target = target;
	return tween;
}
void PlaceTween::OnStarted()
{
	Transform* transform = dynamic_cast<Transform*>(_target);
	transform->SetLocalPosition(_position);
}
DC_END_NAMESPACE