 
/*****************************************************************************
* Author： hannibal
* Date：2020/7/30
* Description： 组件基类
*****************************************************************************/
#pragma once

#include "core/Object.h"
#include "core/time/Time.h"
#include "core/color/Color.h"

DC_BEGIN_NAMESPACE
class Camera;
class GameObject;
class Transform;
class Collision;
class Collision2D;
enum class ComponentAction : uint64
{
	Awake				= 1 << 0,
	Start				= 1 << 1,
	Enable				= 1 << 2,
	Disable				= 1 << 3,
	TransformChanged	= 1 << 4,

	CollisionEnter		= 1 << 5,
	CollisionStay		= 1 << 6,
	CollisionExit		= 1 << 7,
	TriggerEnter		= 1 << 8,
	TriggerStay			= 1 << 9,
	TriggerExit			= 1 << 10,
	CollisionEnter2D	= 1 << 11,
	CollisionStay2D		= 1 << 12,
	CollisionExit2D		= 1 << 13,
	TriggerEnter2D		= 1 << 14,
	TriggerStay2D		= 1 << 15,
	TriggerExit2D		= 1 << 16,

	Update				= 1 << 17,

	LateUpdate			= 1 << 18,
	PreRender			= 1 << 19,
	PostRender			= 1 << 20,

	DrawGizmos			= 1 << 21,
	GUI					= 1 << 22,
	UISize				= 1 << 23,

	Destroy				= 1 << 24,

	ApplicationFocus	= 1 << 25,
	ApplicationPause	= 1 << 26,
	ApplicationQuit		= 1 << 27,

	AddComponent		= 1 << 28,
	RemoveComponent		= 1 << 29,
};
DECLARE_ENUM_OPERATORS(ComponentAction);
#define DEFAULT_COMPONENT_ACTION  uint64(ComponentAction::Awake | ComponentAction::Start | ComponentAction::Enable | ComponentAction::Disable | ComponentAction::TransformChanged \
			| ComponentAction::PreRender | ComponentAction::PostRender | ComponentAction::DrawGizmos | ComponentAction::GUI | ComponentAction::UISize \
			| ComponentAction::Destroy| ComponentAction::AddComponent| ComponentAction::RemoveComponent)
/********************************************************************/
class ENGINE_DLL Component : public Object
{
	friend class GameObject;
	friend class EInspector_Hierarchy;
	FRIEND_CONSTRUCT_DESTRUCT(Component);
	DISALLOW_COPY_ASSIGN(Component);
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(Component);
	BEGIN_DERIVED_REFECTION_TYPE(Component, Object)
	END_REFECTION_TYPE;

protected:
	Component() {}
	virtual ~Component() { _gameObject = nullptr; }
	virtual void OnDrawEditor();
	virtual bool ShowInEditor() { return true; }
	virtual bool CanSerialize() { return true; }
	virtual bool CanAdd(GameObject* object) { return true; }
	virtual bool CanRemove() { return true; }
	virtual uint64 GetEditorAction()const { return DEFAULT_COMPONENT_ACTION; }

public:
	uint64 GetObjectInstanceId() const;
	virtual const String& GetInstanceName() const override;

	GameObject* GetGameObject() const { return _gameObject; }
	Transform* GetTransform() const;
	template <class T> T* GetComponent() const;

	void SetEnable(bool enable);
	bool IsEnable() const { return _enable; }

	bool CanExecute(ComponentAction type);

	void SetDrawGizmos(bool b) { _drawGizmos = b; }

protected:
	virtual void Awake() {}
	virtual void Start() {}

	virtual void OnEnable() {}
	virtual void OnDisable() {}
	virtual void OnTransformChanged() {}

	virtual void OnCollisionEnter(Collision* collisionInfo) {}
	virtual void OnCollisionStay(Collision* collisionInfo) {}
	virtual void OnCollisionExit(Collision* collisionInfo) {}
	virtual void OnTriggerEnter() {}
	virtual void OnTriggerStay() {}
	virtual void OnTriggerExit() {}
	virtual void OnCollisionEnter2D(Collision2D* collisionInfo) {}
	virtual void OnCollisionStay2D(Collision2D* collisionInfo) {}
	virtual void OnCollisionExit2D(Collision2D* collisionInfo) {}
	virtual void OnTriggerEnter2D() {}
	virtual void OnTriggerStay2D() {}
	virtual void OnTriggerExit2D() {}

	virtual void Update() {}
	virtual void LateUpdate() {}

	virtual void OnPreRender(Camera* camera) {}//OnPreRender is called before a camera starts rendering the scene.
	virtual void OnPostRender(Camera* camera) {}//OnPostRender is called after a camera finished rendering the scene.

	virtual void OnDrawGizmos(Camera* camera) {}
	virtual void OnGUI() {}
	virtual void OnUISize() {}

	virtual void OnMessage(const String& key, const String& value) {}
	virtual void OnDestroy() {}

	virtual void OnApplicationFocus() {}
	virtual void OnApplicationPause() {}
	virtual void OnApplicationQuit() {}

	virtual void OnAddComponent(Component* com) {}
	virtual void OnRemoveComponent(Component* com) {}

protected:
	bool _enable = true;
	bool _isStart = false;
	bool _drawGizmos = false;
	GameObject* _gameObject = nullptr;
};
DC_END_NAMESPACE
