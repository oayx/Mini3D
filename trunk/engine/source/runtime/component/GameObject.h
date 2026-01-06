 
/*****************************************************************************
* Author： hannibal
* Date：2020/7/30
* Description： 组件基类
*****************************************************************************/
#pragma once

#include "Component.h"
#include "RectTransform.h"

DC_BEGIN_NAMESPACE
class Tween;
class SceneManager;
enum class GameObjectFlag
{
	Null = 0x0,
	//引擎内部使用
	EditorCamera = 0x1,				//编辑器相机
	VisibleEditorCamera = 0x2,		//编辑器相机可见:编辑器下有效

	//外部可以使用
	DonotPutInScene	= 0x1000,		//不放入场景管理器
	AlwaysFindInScene = 0x2000,		//场景查询时一直找的到
};
DECLARE_ENUM_OPERATORS(GameObjectFlag);
/********************************************************************/
class ENGINE_DLL GameObject : public Object
{
	friend class Transform;
	friend class SceneManager;
	DEFAULT_CREATE(GameObject);
	FRIEND_CONSTRUCT_DESTRUCT(GameObject);
	DISALLOW_COPY_ASSIGN(GameObject);
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(GameObject);
	BEGIN_DERIVED_REFECTION_TYPE(GameObject, Object)
	END_REFECTION_TYPE;
	using base::base;

private:
	GameObject(const String& name);
	virtual ~GameObject();

public:
	static GameObject* Instantiation(const String& name = "");
	static GameObject* Instantiation(const String& name, Transform* parent);
	static GameObject* InstantiationUI(const String& name = "");
	static GameObject* InstantiationUI(const String& name, Transform* parent);
	static void Destroy(GameObject* obj);//下一帧才会销毁内存
	static void Destroy(Component* com);
	static GameObject* Find(const String& path);

public:
	Component* AddComponent(const String& type_name);
	template <class T, typename ...ARGS> T* AddComponent(ARGS... args);
	template <class T> T* GetComponent() const;
	template <class T> List<T*> GetComponents() const;
	template <class T> T* GetComponentInChildren(bool self = true, bool inactive = false) const;
	template <class T> List<T*> GetComponentsInChildren(bool self = true) const;//是否包括自身，如果为true，包括自身和子节点
	template <class T> T* GetComponentInParent(bool self = true, bool inactive = false) const;
	const Vector<Component*>& GetAllComponent()const { return _components; }
	Component* GetComponent(const String& type)const;
	template <class T> void RemoveComponent();
	void RemoveComponent(const String& type);
	void RemoveComponent(Component* com);

	Transform* GetTransform() const { return _transform; }
	RectTransform* GetRectTransform() const { return dynamic_cast<RectTransform*>(_transform); }
	void OnTransformChanged();

	int  GetLayer() const { return _layer; }
	void SetLayer(int layer);

	const String& GetTag()const { return _tag; }
	void  SetTag(const String& tag) { _tag = tag; }

	bool HasFlag(GameObjectFlag flag) { return _flags & flag; }
	void InsertFlag(GameObjectFlag flag) { _flags |= flag; }
	void RemoveFlag(GameObjectFlag flag) { _flags ^= flag; }

	bool ActiveSelf() const { return _isActiveSelf; }
	bool ActiveInHierarchy()const;
	void SetActive(bool active);
	//通知组件内部回调
	void PostComponentAction(ComponentAction type, bool removed, bool child, void* data = nullptr);

	void SendMessage(const String& key, const String& value);//通知当前物体下
	void SendMessageUpwards(const String& key, const String& value);//发送给自身的所有脚本以及自身父物体，父物体的父物体等身上所有脚本
	void SendMessageDownward(const String& key, const String& value);//发送给自身的所有脚本以及自身子物体，子物体的子物体等身上所有脚本

	void Update();

	Tween* DOBlink(float time, int count, bool defaultVisible = true);
	Tween* DOVisible(bool visible, float time);

private:
	bool AddComponent(Component* com);
	bool IsDestroying()const { return _isDestroying; }
	void SetDestroy() { _isDestroying = true; }

private:
	Transform* _transform = nullptr;
	Vector<Component*> _components;
	Vector<Component*> _addedComponents;
	Vector<Component*> _removedComponents;
	GameObjectFlag _flags = GameObjectFlag::Null;
	int _layer = 0;
	String _tag = "";
	bool _isActiveSelf = true;
	bool _isDestroying = false;
};

template <class T, typename ...ARGS>
T* GameObject::AddComponent(ARGS... args)
{
	if (!typeof(T)->HasFlag(TypeFlag::AllowMultipleComponent) && GetComponent<T>() != nullptr)
	{
		Debuger::Error("Already has component");
		return nullptr;
	}
	T* com = Memory::New<T>(args...);
	if (!AddComponent(com))return nullptr;
	return com;
}

template <class T>
T* GameObject::GetComponent() const
{
	for (int i = 0; i < _addedComponents.Size(); ++i)
	{
		Component* com = _addedComponents[i];
		if (_removedComponents.Contains(com))continue;
		T* t = dynamic_cast<T*>(com);
		if (t)return t;
	}

	for (int i = 0; i < _components.Size(); ++i)
	{
		Component* com = _components[i];
		if (_removedComponents.Contains(com))continue;
		T* t = dynamic_cast<T*>(com);
		if (t)return t;
	}
	return nullptr;
}

template <class T>
List<T*> GameObject::GetComponents() const
{
	List<T*> coms;
	for (int i = 0; i < _addedComponents.Size(); ++i)
	{
		auto com = _addedComponents[i];
		auto t = dynamic_cast<T*>(com);
		if (t)
		{
			coms.Add(t);
		}
	}

	for (int i = 0; i < _components.Size(); ++i)
	{
		auto com = _components[i];
		auto t = dynamic_cast<T*>(com);
		if (t)
		{
			coms.Add(t);
		}
	}

	return coms;
}
template <class T>
T* GameObject::GetComponentInChildren(bool self, bool inactive) const
{
	T* com;
	if (self)com = this->GetComponents<T>();
	if (com && (inactive || (!inactive && com->IsEnable())))return com;

	int childCount = this->GetTransform()->GetChildCount();
	for (int i = 0; i < childCount; ++i)
	{
		auto child = this->GetTransform()->GetChild(i);
		T* child_com = child->GetGameObject()->GetComponentInChildren<T>(true);
		if(child_com)
		{
			return child_com;
		}
	}

	return nullptr;
}
template <class T>
List<T*> GameObject::GetComponentsInChildren(bool self) const
{
	List<T*> coms;
	if(self)coms = this->GetComponents<T>();

	int childCount = this->GetTransform()->GetChildCount();
	for (int i = 0; i < childCount; ++i)
	{
		auto child = this->GetTransform()->GetChild(i);
		List<T*> child_coms = child->GetGameObject()->GetComponentsInChildren<T>(true);
		for (auto c : child_coms)
		{
			coms.Add(c);
		}
	}

	return coms;
}
template <class T>
T* GameObject::GetComponentInParent(bool self, bool inactive) const
{
	T* com = nullptr;
	if (self)com = this->GetComponent<T>();
	if (com && (inactive || (!inactive && com->IsEnable())))return com;

	Transform* parant = this->GetTransform()->GetParent();
	if(parant)
	{
		T* com = parant->GetGameObject()->GetComponentInParent<T>(true);
		if (com)
		{
			return com;
		}
	}

	return nullptr;
}
template <class T> 
void GameObject::RemoveComponent()
{
	T* com = this->GetComponent<T>();
	if (com)this->RemoveComponent(com);
}
DC_END_NAMESPACE
