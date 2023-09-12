#include "Transform.h"
#include "runtime/component/GameObject.h"
#include "runtime/scene/LayerMask.h"
#include "runtime/scene/SceneManager.h"
#include "runtime/tween/TweenManager.h"
#include "runtime/tween/TransfromTween.h"
#include "runtime/Application.h"
#include "editor/EditorAppliction.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Transform, Component);
Transform::Transform()
{
	if (SceneManager::GetCurrScene())
	{
		_parentNode = SceneManager::GetCurrScene()->GetRootNode();
		_parentNode->AddChild(this);
	}
	_localBoundingBox.SetExtents(-Vector3::one, Vector3::one);
	_worldBoundingBox.SetNULL();
}
Transform::~Transform()
{
	TweenManager::Stop(this);
	_parentNode = nullptr;
	_childNodes.Clear();
}
void Transform::OnEnable()
{
	_isTransfromDirty = true;
	base::OnEnable();
}
void Transform::UpdateTransform(const Vector3& offset)
{
	if (_isTransfromDirty)
	{
		_localTransform = Matrix4(_localPosition + offset, _localRotation, _localScale);
		if (_parentNode)
		{
			_localToWorld = _localTransform * _parentNode->GetLocalToWorldMatrix();
			_worldPosition = _parentNode->GetLocalToWorldMatrix().TransformPoint(_localPosition + offset);
			_worldRotation = _parentNode->GetRotation() * _localRotation;
			_worldScale = _parentNode->GetScale() * _localScale;
		}
		else
		{
			_localToWorld = _localTransform;
			_worldRotation = _localRotation;
			_worldPosition = _localPosition + offset;
			_worldScale = _localScale;
		}
		_up = _localToWorld.TransformVector(Vector3(0, 1, 0));
		_right = _localToWorld.TransformVector(Vector3(1, 0, 0));
		_forward = _localToWorld.TransformVector(Vector3(0, 0, 1));
		_up.Normalize(); _right.Normalize(); _forward.Normalize();

		_worldToLocal = _localToWorld.Inverse();
		if (!_unscaleBoundingBox.IsNull())
		{
			_worldBoundingBox = _unscaleBoundingBox * _localToWorld;
		}

		_isTransfromDirty = false;
	}
}
void Transform::OnDrawEditor()
{
	Vector3 pos = this->GetLocalPosition();
	ImGuiEx::Label("Positin");
	const float width = ImGui::GetContentRegionAvail().x;
	const float reset_width = ImGui::CalcTextSize(ICON_FA_TRASH_ALT).x + 5.0f;
	ImGui::SetNextItemWidth(width - reset_width);
	if (ImGui::DragFloat3("##Positin", (float*)pos.ptr(), 0.1f, -10000.0f, 10000.0f))
	{
		this->SetLocalPosition(pos);
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(reset_width);
	if (ImGui::Selectable(ICON_FA_UNDO"##Reset Positin"))
	{
		this->SetLocalPosition(Vector3::zero);
	}
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("Reset Positin");

	OnDrawRotateScale(width, reset_width);
}
void Transform::OnDrawRotateScale(const float width, const float reset_width)
{
	//旋转
	Vector3 euler = this->GetLocalRotation().ToEuler();
	ImGuiEx::Label("Rotation");
	ImGui::SetNextItemWidth(width - reset_width);
	if (ImGui::DragFloat3("##Rotation", (float*)euler.ptr(), 0.1f, -10000.0f, 10000.0f))
	{
		this->SetLocalRotation(Quaternion(euler));
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(reset_width);
	if (ImGui::Selectable(ICON_FA_UNDO"##Reset Rotation"))
	{
		this->SetLocalRotation(Quaternion(Vector3::zero));
	}
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("Reset Rotation");

	//缩放
	Vector3 scale = this->GetLocalScale();
	ImGuiEx::Label("Scale");
	ImGui::SetNextItemWidth(width - reset_width);
	if (ImGui::DragFloat3("##Scale", (float*)scale.ptr(), 0.1f, -10000.0f, 10000.0f))
	{
		this->SetLocalScale(scale);
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(reset_width);
	if (ImGui::Selectable(ICON_FA_UNDO"##Reset Scale"))
	{
		this->SetLocalScale(Vector3::one);
	}
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("Reset Scale");
}
Object* Transform::Clone(Object* new_obj)
{
	base::Clone(new_obj);

	Transform* obj = dynamic_cast<Transform*>(new_obj);
	if (!obj)return new_obj;

	obj->SetLocalBoundingBox(_unscaleBoundingBox);

	obj->SetLocalPosition(_localPosition);
	obj->SetLocalRotation(_localRotation);
	obj->SetLocalScale(_localScale);

	obj->SetEditorCanTranslate(_editorCanTranslate);
	obj->SetEditorCanRotate(_editorCanRotate);
	obj->SetEditorCanScale(_editorCanScale);

	return obj;
}
Transform* Transform::GetChild(int index)const
{
	return _childNodes[index];
}
void Transform::SetAsFirstSibling()
{
	if (_parentNode == nullptr)return;
	for (auto it = _parentNode->_childNodes.begin(); it != _parentNode->_childNodes.end(); ++it)
	{
		if ((*it)->GetInstanceId() == this->GetInstanceId())
		{
			_parentNode->_childNodes.Remove(it);
			_parentNode->_childNodes.Insert(_parentNode->_childNodes.begin(), this);
			break;
		}
	}
}
void Transform::SetAsLastSibling()
{
	if (_parentNode == nullptr)return;
	for (auto it = _parentNode->_childNodes.begin(); it != _parentNode->_childNodes.end(); ++it)
	{
		if ((*it)->GetInstanceId() == this->GetInstanceId())
		{
			_parentNode->_childNodes.Remove(it);
			_parentNode->_childNodes.Add(this);
			break;
		}
	}
}
void Transform::SetSiblingIndex(int index)
{
	if (_parentNode == nullptr)return;
	for (auto it = _parentNode->_childNodes.begin(); it != _parentNode->_childNodes.end(); ++it)
	{
		if ((*it)->GetInstanceId() == this->GetInstanceId())
		{
			_parentNode->_childNodes.Remove(it);
			if (_parentNode->_childNodes.IsEmpty() || index >= _parentNode->_childNodes.Size())
			{
				_parentNode->_childNodes.Add(this);
			}
			else
			{
				it = _parentNode->_childNodes.begin();
				std::advance(it, index);
				_parentNode->_childNodes.Insert(it, this);
			}
			break;
		}
	}
}
void Transform::AddChild(Transform *node)
{
	if (node == nullptr)
		return;
	AssertEx(!this->GetGameObject()->IsDestroying(), "Cannot add child while GameObject is destroying");
	_childNodes.Add(node);
}
void Transform::RemoveChild(Transform *node)
{
	if (node == nullptr)
		return;
	for (auto it = _childNodes.begin(); it != _childNodes.end(); ++it)
	{
		if ((*it)->GetInstanceId() == node->GetInstanceId())
		{
			_childNodes.Remove(it);
			break;
		}
	}
}
void Transform::RemoveFromParent()
{
	if (_parentNode != nullptr)
	{
		_parentNode->RemoveChild(this);
		this->SetTransformDirty();
	}
}
void Transform::SetParent(Transform *parent, bool worldPositionStays)
{
	if (parent == this)return;

	Vector3 position = this->GetPosition();
	Quaternion rotation = this->GetRotation();
	Vector3 scale = this->GetScale();

	this->RemoveFromParent();
	if (parent == nullptr)
	{
		if (SceneManager::GetCurrScene())
		{
			_parentNode = SceneManager::GetCurrScene()->GetRootNode();
			_parentNode->AddChild(this);
		}
	}
	else
	{
		parent->AddChild(this);
	}
	_parentNode = parent;
	this->SetTransformDirty();
	if (worldPositionStays)
	{
		this->SetPosition(position);
		this->SetRotation(rotation);
		this->SetScale(scale);
	}

	//this->GetGameObject()->SetActive(this->GetGameObject()->ActiveSelf());
}
Transform *Transform::GetRoot()const
{
	if (_parentNode)
		return _parentNode->GetRoot();
	else
		return this->GetGameObject()->GetTransform();
}
Transform* Transform::Find(const String& path)const
{
	if (path.IsEmpty())
	{
		return nullptr;
	}

	String name = path;
	String child_path = "";
	int split_index = path.IndexOf('/');
	if (split_index >= 0)
	{
		name = path.Substring(0, split_index);
		child_path = path.Substring(split_index + 1);
	}

	if (name == "..")
	{
		if (_parentNode != nullptr)
		{
			if (child_path.IsEmpty())
				return _parentNode;
			else
				return _parentNode->Find(child_path);
		}
		else
		{
			return nullptr;
		}
	}

	for (int j = 0; j < (int)this->GetChildCount(); ++j)
	{
		Transform* child = this->GetChild(j);
		if (name == child->GetInstanceName())
		{
			if (!child_path.IsEmpty())
			{
				Transform* find = child->Find(child_path);
				if (find != nullptr)
				{
					return find;
				}
			}
			else
			{
				return child;
			}
		}
	}

	return nullptr;
}
void Transform::Update()
{
	UpdateTransform();
}
bool Transform::IsIntersects(Aabb &box)
{
	if (box.IsNull())
	{
		return false;
	}

	if (box.IsInfinite())
	{
		return true;
	}

	return (GetBoundingBox().Intersects(box) == IntersectResult::Intersect);
}
void Transform::Roll(float degree)
{
	RotateAxis(Vector3::forward, degree);
}
void Transform::Pitch(float degree)
{
	RotateAxis(Vector3::right, degree);
}
void Transform::Yaw(float degree)
{
	RotateAxis(Vector3::up, degree);
}
void Transform::RotateAxis(const Vector3& world_axis, float degree)
{
	Rotate(Quaternion(degree, world_axis));
}
void Transform::RotateAround(const Vector3& point, const Vector3& world_axis, float degree)
{
	Vector3 world_pos = this->GetPosition();
	Quaternion q(degree, world_axis);
	Vector3 dif = world_pos - point;
	dif = q * dif;
	world_pos = point + dif;
	this->SetPosition(world_pos);
	RotateAxis(world_axis, degree);
}
void Transform::Rotate(const Quaternion& q)
{
	Quaternion qnorm = q.Normalize();
	_localRotation = _localRotation * qnorm;
	_localRotation.Normalize();
	this->SetTransformDirty();
}
void Transform::Translate(const Vector3& d)
{
	_localPosition = _localPosition + d;
	this->SetTransformDirty();
}
void Transform::LookTo(const Vector3& dir, const Vector3& up)
{
	Matrix3 mat = Matrix3::LookTo(dir, up);
	Quaternion q = mat.ToRotate();
	_localRotation = q.Inverse();
	this->SetTransformDirty();
}
void Transform::LookAt(const Vector3& at, const Vector3& up)
{
	Matrix3 mat = Matrix3::LookAt(GetPosition(), at, up);
	Quaternion q = mat.ToRotate();
	_localRotation = q.Inverse();
	this->SetTransformDirty();
}
void Transform::Scale(const Vector3& scale)
{
	_localScale = _localScale * scale;
	this->SetTransformDirty();
}
void Transform::SetTransformDirty(bool object, bool child)
{
	_isTransfromDirty = true;
	if (object)
	{
		if (this->GetGameObject()->ActiveSelf())
		{
			this->GetGameObject()->OnTransformChanged();
			if (child)
			{
				for (uint i = 0; i < _childNodes.Size(); ++i)
				{
					_childNodes[i]->SetTransformDirty();
				}
			}
		}
	}
}
void Transform::SetPosition(const Vector3& pos)
{
	Vector3 local_position;
	if (_parentNode)
	{
		local_position = _parentNode->GetWorldToLocalMatrix().TransformPoint(pos);
	}
	else
	{
		local_position = pos;
	}
	this->SetLocalPosition(local_position);
}
void Transform::SetRotation(const Quaternion& rot)
{
	Quaternion local_rotation;
	if (_parentNode)
	{
		local_rotation = _parentNode->GetRotation().Inverse() * rot;
	}
	else
	{
		local_rotation = rot;
	}
	this->SetLocalRotation(local_rotation);
}
void Transform::SetScale(const Vector3& scale)
{
	Vector3 local_scale;
	if (_parentNode)
	{
		const Vector3& parent_scale = _parentNode->GetScale();
		local_scale = Vector3(scale.x / parent_scale.x, scale.y / parent_scale.y, scale.z / parent_scale.z);
	}
	else
	{
		local_scale = scale;
	}
	this->SetLocalScale(local_scale);
}
const Quaternion& Transform::GetRotation()
{
	UpdateTransform();
	return _worldRotation;
}
const Vector3& Transform::GetScale()
{
	UpdateTransform();
	return _worldScale;
}
const Vector3& Transform::GetPosition()
{
	UpdateTransform();
	return _worldPosition;
}
const Vector3& Transform::GetUp()
{
	UpdateTransform();
	return _up;
}
const Vector3& Transform::GetRight()
{
	UpdateTransform();
	return _right;
}
const Vector3& Transform::GetForward()
{
	UpdateTransform();
	return _forward;
}
void Transform::SetLocalBoundingBox(const Aabb& aabb) 
{
	_unscaleBoundingBox = aabb;
	_localBoundingBox = aabb;
	_localBoundingBox.Scale(_localScale);
	this->SetTransformDirty();
}
const Aabb& Transform::GetBoundingBox()
{
	UpdateTransform();
	return _worldBoundingBox;
}
const Matrix4& Transform::GetLocalMatrix()
{
	UpdateTransform();
	return _localTransform;
}
const Matrix4& Transform::GetLocalToWorldMatrix()
{
	UpdateTransform();
	return _localToWorld;
}
const Matrix4& Transform::GetWorldToLocalMatrix()
{
	UpdateTransform();
	return _worldToLocal;
}
Tween* Transform::DOMove(const Vector3& target_pos, float time)
{
	return MoveTween::To(this, target_pos, time, SimulationSpace::World);
}
Tween* Transform::DOLocalMove(const Vector3& target_pos, float time)
{
	return MoveTween::To(this, target_pos, time, SimulationSpace::Local);
}	
Tween* Transform::DoRotate(const Vector3& euler, float time)
{
	return RotateTween::To(this, euler, time, SimulationSpace::World);
}
Tween* Transform::DoRotateAround(const Vector3& point, const Vector3& axis, float degree)
{
	return RotateAroundTween::Create(this, point, axis, degree, SimulationSpace::World);
}
Tween* Transform::DoLocalRotate(const Vector3& euler, float time)
{
	return RotateTween::To(this, euler, time, SimulationSpace::Local);
}
Tween* Transform::DoScale(const Vector3& scale, float time)
{
	return ScaleTween::To(this, scale, time);
}
Tween* Transform::DoFlip(bool flipX, bool flipY, bool flipZ)
{
	return FlipTween::Create(this, flipX, flipY, flipZ);
}
Tween* Transform::DoPlace(const Vector3& pos)
{
	return PlaceTween::Create(this, pos);
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(Transform);
template<class TransferFunction> inline
void Transform::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_localPosition);
	TRANSFER_SIMPLE(_localRotation);
	TRANSFER_SIMPLE(_localScale);

	if (transfer.IsRead())
	{
		_isTransfromDirty = true;
	}
}
DC_END_NAMESPACE