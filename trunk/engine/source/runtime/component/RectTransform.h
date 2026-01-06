 
/*****************************************************************************
* Author： hannibal
* Date：2020/9/8
* Description： UI节点基类
*****************************************************************************/
#pragma once

#include "Transform.h"
#include "runtime/CommonDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
enum class StretchMode
{
	Null, 
	Horizontal,
	Vertical,
	All,
};
DECLARE_ENUM_OPERATORS(StretchMode);

class ENGINE_DLL RectTransform final : public Transform
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(RectTransform);
	DISALLOW_COPY_ASSIGN(RectTransform);
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(RectTransform);
	BEGIN_DERIVED_REFECTION_TYPE(RectTransform, Transform)
		CTORS(DEFAULT_CTOR(RectTransform))
		INSERT_TYPE_FLAGS(TypeFlag::ShowInEditor)
	END_REFECTION_TYPE;

protected:
	RectTransform();
	virtual ~RectTransform();
	virtual void OnEnable()override;
	virtual void OnDisable()override;
	virtual void OnTransformChanged()override;
	virtual void UpdateTransform(const Vector3& offset = Vector3::zero)override;
	virtual void OnDrawEditor()override;

public:
	void		SetAlige(AligeType alige);
	AligeType	GetAlige()const { return _alige; }

	void		SetStretchMode(StretchMode mode);
	StretchMode	GetStretchMode()const { return _stretchMode; }

	void		SetSize(float w, float h);
	const Size&	GetSize();

	virtual void SetPosition(const Vector3& pos)override;

private:
	Vector3		GetParentArchor();	//相对父节点的锚点
	void		SetNeedRebuild();

protected:
	Size		_size = Size(100.0f, 100.0f);
	Size		_originalSize = Size(100.0f, 100.0f);
	AligeType	_alige = AligeType::Middle;
	StretchMode	_stretchMode = StretchMode::Null;
	Vector4		_stretchValue = Vector4::zero;
};
DC_END_NAMESPACE