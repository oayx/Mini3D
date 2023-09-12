
/*****************************************************************************
* Author： hannibal
* Date：2009/12/8
* Description： 场景节点基类
*****************************************************************************/
#pragma once

#include "Component.h"

DC_BEGIN_NAMESPACE
class Tween;
/********************************************************************/
// ClassName：Transform
// Description：场景节点基类
class ENGINE_DLL Transform : public Component
{
	friend class GameObject;
	typedef Vector<Transform*> Nodes;
	typedef List<Transform*> ListNodes;
	FRIEND_CONSTRUCT_DESTRUCT(Transform);
	DISALLOW_COPY_ASSIGN(Transform);
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(Transform);
	BEGIN_DERIVED_REFECTION_TYPE(Transform, Component)
		CTORS(DEFAULT_CTOR(Transform))
		INSERT_TYPE_FLAGS(TypeFlag::ShowInEditor)
	END_DERIVED_REFECTION_TYPE;

protected:
	Transform();
	virtual ~Transform();
	virtual void OnEnable()override;
	virtual bool CanRemove()override { return false; }
	virtual void UpdateTransform(const Vector3& offset = Vector3::zero);
	virtual void OnDrawEditor()override;
			void OnDrawRotateScale(const float width, const float reset_width);
	virtual uint64 GetEditorAction()const override { return DEFAULT_COMPONENT_ACTION | uint64(ComponentAction::Update); }

public://节点相关
	Transform*		GetChild(int index)const;
	uint			GetChildCount() const { return (uint)_childNodes.Size(); }
	void			SetAsFirstSibling();
	void			SetAsLastSibling();
	void			SetSiblingIndex(int index);

	//设置父节点，如果worldPositionStays传false，则当前节点的世界坐标不变；如果传true，则世界坐标受父节点影响
	void			SetParent(Transform *parent, bool worldPositionStays = false);
	Transform*		GetParent()const { return _parentNode; }
	Transform*		GetRoot()const;

	Transform*		Find(const String& path)const;

	//更新场景节点，这个方法是一个递归调用的，先场景节点调用，接着遍历子节点都会调用
	void			Update()override;

	//相交判断
	bool			IsIntersects(Aabb &box);

public://变换
	//绕Z轴旋转
	void			Roll(float degree);

	//绕X轴旋转
	void			Pitch(float degree);

	//绕Y轴旋转
	void			Yaw(float degree);

	//绕任意轴旋转
	void			Rotate(const Quaternion& q);
	void			RotateAxis(const Vector3& world_axis, float degree);
	void			RotateAround(const Vector3& point, const Vector3& world_axis, float degree);

	//本地移动
	void			Translate(const Vector3& d);
	void			LookTo(const Vector3& dir, const Vector3& up);
	void			LookAt(const Vector3& at, const Vector3& up);

	//本地缩放
	void			Scale(const Vector3& scale);

	void			SetTransformDirty(bool object = true, bool child = true);

	//本地坐标
	virtual void	 SetLocalPosition(const Vector3& position) { _localPosition = position; this->SetTransformDirty(); }
	virtual void	 SetLocalRotation(const Quaternion& rot) { _localRotation = rot; this->SetTransformDirty(); }
	virtual void	 SetLocalScale(const Vector3& scale) { _localScale = scale; this->SetTransformDirty(); }

	const Vector3&	 GetLocalPosition()const { return _localPosition; }
	const Quaternion&GetLocalRotation()const { return _localRotation; }
	const Vector3&	 GetLocalScale()const { return _localScale; }

	//世界坐标
	virtual void	 SetPosition(const Vector3& pos);
	virtual void	 SetRotation(const Quaternion& rot);
	virtual void	 SetScale(const Vector3& scale);

	const Quaternion&GetRotation();
	const Vector3&	 GetScale();
	const Vector3&	 GetPosition();

	const Vector3&	 GetUp();
	const Vector3&	 GetRight();
	const Vector3&	 GetForward();

	const Aabb&		 GetUnscaleBoundingBox()const { return _unscaleBoundingBox; }
	const Aabb&		 GetLocalBoundingBox()const { return _localBoundingBox; }//本地坐标系包围盒
	void			 SetLocalBoundingBox(const Aabb& aabb);
	const Aabb&		 GetBoundingBox();

	const Matrix4&	 GetLocalMatrix();
	const Matrix4&	 GetLocalToWorldMatrix();
	const Matrix4&	 GetWorldToLocalMatrix();

public:
	Tween*			 DOMove(const Vector3& target_pos, float time);
	Tween*			 DOLocalMove(const Vector3& target_pos, float time);
	Tween*			 DoRotate(const Vector3& euler, float time);
	Tween*			 DoRotateAround(const Vector3& point, const Vector3& axis, float degree);
	Tween*			 DoLocalRotate(const Vector3& euler, float time);
	Tween*			 DoScale(const Vector3& scale, float time);
	Tween*			 DoFlip(bool flipX, bool flipY, bool flipZ);
	Tween*			 DoPlace(const Vector3& pos);

public:
	bool			 GetEditorCanTranslate()const { return _editorCanTranslate; }
	bool			 GetEditorCanRotate()const { return _editorCanRotate; }
	bool			 GetEditorCanScale()const { return _editorCanScale; }
	void			 SetEditorCanTranslate(bool b) { _editorCanTranslate = b; }
	void			 SetEditorCanRotate(bool b) { _editorCanRotate = b; }
	void			 SetEditorCanScale(bool b) { _editorCanScale = b; }

private:
	void			 AddChild(Transform* node);
	void			 RemoveChild(Transform* node);
	void			 RemoveFromParent();

protected:
	Transform*		 _parentNode = nullptr;	//父节点
	Nodes			 _childNodes;				//子节点，用于场景更新         

protected://场景更新
	bool			_isTransfromDirty = true;

	Aabb			_worldBoundingBox;
	Aabb            _localBoundingBox;
	Aabb            _unscaleBoundingBox;

	//从根节点起的变换组合
	Quaternion		_worldRotation = Quaternion::identity;
	Vector3			_worldPosition = Vector3::zero;
	Vector3			_worldScale = Vector3::one;

	//相对父节点的变化
	Quaternion		_localRotation = Quaternion::identity;
	Vector3			_localPosition = Vector3::zero;
	Vector3			_localScale = Vector3::one;

	Vector3			_forward = Vector3::forward;
	Vector3			_right = Vector3::right;
	Vector3			_up = Vector3::up;

	Matrix4			_localTransform;
	Matrix4			_localToWorld;
	Matrix4			_worldToLocal;

protected:
	bool			_editorCanTranslate = true;
	bool			_editorCanRotate = true;
	bool			_editorCanScale = true;
};
DC_END_NAMESPACE