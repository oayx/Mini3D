#include "GameObject.h"
#include "Component.h"
#include "runtime/ui/UIView.h"
#include "runtime/ui/UICanvas.h"
#include "runtime/scene/LayerMask.h"
#include "runtime/scene/SceneManager.h"
#include "runtime/tween/GameObjectTween.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(GameObject, Object);
GameObject* GameObject::Instantiation(const String& name)
{
	GameObject* obj = GameObject::Create(name);
	obj->_transform = obj->AddComponent<Transform>();
	SceneManager::AddGameObject(obj);
	return obj;
}
GameObject* GameObject::Instantiation(const String& name, Transform* parent)
{
	GameObject* obj = GameObject::Create(name);
	obj->_transform = obj->AddComponent<Transform>();
	if(parent)obj->_transform->SetParent(parent);
	SceneManager::AddGameObject(obj);
	return obj;
}
GameObject* GameObject::InstantiationUI(const String& name)
{
	GameObject* obj = GameObject::Create(name);
	obj->_transform = obj->AddComponent<RectTransform>();
	obj->SetLayer(LayerMask::NameToLayer(LayerMask::UI));
	SceneManager::AddGameObject(obj);
	return obj;
}
GameObject* GameObject::InstantiationUI(const String& name, Transform* parent)
{
	GameObject* obj = GameObject::Create(name);
	obj->_transform = obj->AddComponent<RectTransform>();
	if (parent)obj->_transform->SetParent(parent);
	obj->SetLayer(LayerMask::NameToLayer(LayerMask::UI));
	SceneManager::AddGameObject(obj);
	return obj;
}
void GameObject::Destroy(GameObject* obj)
{
	if (obj == nullptr || obj->IsDestroying())return;
	obj->SetDestroy();
	obj->PostComponentAction(ComponentAction::Disable, true, true);
	obj->PostComponentAction(ComponentAction::Destroy, true, true);
	Transform* transform = obj->GetTransform();
	while(transform->GetChildCount() > 0)
	{
		GameObject::Destroy(transform->GetChild(0)->GetGameObject());
	}
	if(transform->GetParent())transform->GetParent()->RemoveChild(transform);
	SceneManager::RemoveGameObject(obj);
}
void GameObject::Destroy(Component* com)
{
	if (com != nullptr)
	{
		GameObject* obj = com->GetGameObject();
		if(obj != nullptr)obj->RemoveComponent(com);
	}
}
GameObject::GameObject(const String& name)
{
	this->SetInstanceName(name);
	_layer = LayerMask::NameToLayer(LayerMask::Default);
}
GameObject::~GameObject()
{
	for (auto com : _addedComponents)
	{
		SAFE_DELETE(com);
	}
	for (auto com : _removedComponents)
	{
		_components.Remove(com);//一个对象，有可能同时在_removedComponents和_components中
		SAFE_DELETE(com);
	}
	for (auto com : _components)
	{
		SAFE_DELETE(com);
	}
	_addedComponents.Clear();
	_removedComponents.Clear();
	_components.Clear();
	_transform = nullptr;
}
GameObject* GameObject::Find(const String& path)
{
	if (path.IsEmpty())
	{
		return nullptr;
	}
	String name = path;
	String childPath = "";
	int split_index = path.IndexOf('/');
	if (split_index >= 0)
	{
		name = path.Substring(0, split_index);
		childPath = path.Substring(split_index + 1);
	}

	if (SceneManager::GetCurrScene())
	{
		Transform* rootNode = SceneManager::GetCurrScene()->GetRootObject()->GetTransform();
		for (int i = 0; i < rootNode->GetChildCount(); ++i)
		{
			Transform* node = rootNode->GetChild(i);
			if (node)
			{
				if (node->GetGameObject()->GetInstanceName() == name)
				{
					if (!childPath.IsEmpty())
					{
						Transform* find = node->Find(childPath);
						if (find != nullptr)
						{
							return find->GetGameObject();
						}
					}
					else
					{
						return node->GetGameObject();
					}
				}
			}
		}
	}
	return nullptr;
}
Object* GameObject::Clone(Object* newObj)
{
	GameObject* obj = dynamic_cast<GameObject*>(newObj);
	if (!obj)return newObj;

	//基础属性
	obj->SetInstanceName(_instanceName);
	obj->InsertFlag(_flags);
	obj->SetLayer(_layer);
	obj->SetTag(_tag);
	obj->SetActive(_isActiveSelf);

	//Transform
	_transform->Clone(obj->GetTransform());

	//组件
	for (int i = 0; i < _components.Size(); ++i)
	{
		Component* com = _components[i];
		if (!com)continue;
		if (com->Is<Transform>() || com->Is<RectTransform>())continue;
		Component* new_com = obj->AddComponent(com->GetClassId());
		if (new_com)com->Clone(new_com);
	}

	//子
	for (int i = 0; i < _transform->GetChildCount(); ++i)
	{
		Transform* child_transfrom = _transform->GetChild(i);
		GameObject* child_obj = child_transfrom->GetGameObject();
		GameObject* child_new_obj = nullptr;
		if (child_transfrom->Is<RectTransform>())
			child_new_obj = GameObject::InstantiationUI(child_obj->GetInstanceName(), obj->GetTransform());
		else
			child_new_obj = GameObject::Instantiation(child_obj->GetInstanceName(), obj->GetTransform());
		child_obj->Clone(child_new_obj);
	}

	return obj;
}
Component* GameObject::AddComponent(const String& type_name)
{
	if (type_name == "Transform" || type_name == "RectTransform")return nullptr;

	const Type* type = Type::GetType(type_name.c_str());
	if (!type)
	{
		Debuger::Warning("Cannot find reflect type:%s", type_name.c_str());
		return nullptr;
	}
	if (!type->GetConstructor())
	{
		Debuger::Warning("Cannot find reflect constructor:%s", type_name.c_str());
		return nullptr;
	}
	if (!type->HasFlag(TypeFlag::AllowMultipleComponent) && GetComponent(type_name))
	{
		Debuger::Warning("Already has component:%s", type_name.c_str());
		return nullptr;
	}
	Object* obj = type->GetConstructor()->Invoke();
	if (!obj->IsSame(type_name))
	{
		Debuger::Warning("AddComponent type error:%s", type_name.c_str());
		return nullptr;
	}

	Component* com = obj->As<Component>();
	if (!AddComponent(com))return nullptr;
	return com;
}
bool GameObject::AddComponent(Component* com)
{
    CHECK_RETURN_PTR_FALSE(com);
	//销毁中不能添加组件
	if (this->IsDestroying())
	{
		Debuger::Warning("Cannot add Component while GameObject is destroying");
		SAFE_DELETE(com);
		return false;
	}

	if (!com->CanAdd(this))
	{
		Debuger::Warning("Add Component failed");
		SAFE_DELETE(com);
		return false;
	}

	if (com->Is<UIView>() || com->Is<UICanvas>())
	{//是UI组件
		if (!_transform->Is<RectTransform>())
		{
			Transform* parent = _transform->GetParent();

			//备份数据
			Vector3 pos = _transform->GetLocalPosition();
			Quaternion rotate = _transform->GetLocalRotation();
			Vector3 scale = _transform->GetLocalScale();
			Vector<Transform*> childrens;
			for (int i = 0; i < _transform->GetChildCount(); ++i)
			{
				childrens.Add(_transform->GetChild(i));
			}

			_transform->RemoveFromParent();
			_addedComponents.Remove(_transform);
			_components.Remove(_transform);
			SAFE_DELETE(_transform);

			_transform = this->AddComponent<RectTransform>();
			if (parent)_transform->SetParent(parent, false);
			_transform->SetLocalPosition(pos);
			_transform->SetLocalRotation(rotate);
			_transform->SetLocalScale(scale);
			for (auto child : childrens)
			{
				child->SetParent(_transform, false);
			}
			this->SetLayer(LayerMask::NameToLayer(LayerMask::UI));
		}
	}
	
	_addedComponents.Add(com);
	com->_gameObject = this;
	com->SetInstanceName(this->GetInstanceName());
	if(com->CanExecute(ComponentAction::Awake))com->Awake();
	if (ActiveInHierarchy())
	{
		if (com->CanExecute(ComponentAction::Enable))com->OnEnable();
	}
	PostComponentAction(ComponentAction::AddComponent, false, true, com);
	return true;
}
Component* GameObject::GetComponent(const String& type)const
{
	for (int i = 0; i < _addedComponents.Size(); ++i)
	{
		auto com = _addedComponents[i];
		String str = com->GetClassId();
		if (com->IsSame(type) && !_removedComponents.Contains(com))
		{
			return com;
		}
	}

	for (int i = 0; i < _components.Size(); ++i)
	{
		auto com = _components[i];
		if (com->IsSame(type) && !_removedComponents.Contains(com))
		{
			return com;
		}
	}
	return nullptr;
}
void GameObject::RemoveComponent(const String& type)
{
	bool is_remove_added = false, is_remove_removed = false;
	for (int i = _addedComponents.Size() - 1; i >= 0; --i)
	{
		auto com = _addedComponents[i];
		if (com != nullptr && com != _transform && com->IsSame(type))
		{
			if (!com->CanRemove())continue;
			_addedComponents.RemoveAt(i);
			is_remove_added = true;
			break;//只移除一个
		}
	}
	for (int i = _removedComponents.Size() - 1; i >= 0; --i)
	{
		auto com = _removedComponents[i];
		if (com != nullptr && com != _transform && com->IsSame(type))
		{
			if (!com->CanRemove())continue;
			is_remove_removed = true;
			break;//只移除一个
		}
	}

	if (!is_remove_added && !is_remove_removed)
	{
		for (int i = _components.Size() - 1; i >= 0; --i)
		{
			auto com = _components[i];
			if (com != nullptr && com != _transform && com->IsSame(type))
			{
				if (!com->CanRemove())continue;
				_removedComponents.Add(com);
				if (this->ActiveInHierarchy() && com->IsEnable() && com->CanExecute(ComponentAction::Disable))com->OnDisable();
				if (com->CanExecute(ComponentAction::Destroy))com->OnDestroy();
				PostComponentAction(ComponentAction::RemoveComponent, false, true, com);
				break;//只移除一个
			}
		}
	}
}
void GameObject::RemoveComponent(Component* com)
{
	if (!com)return;
	if (!com->CanRemove())return;
	if (!_addedComponents.Remove(com) && !_removedComponents.Contains(com))
	{
		_removedComponents.Add(com);
		if (this->ActiveInHierarchy() && com->IsEnable() && com->CanExecute(ComponentAction::Disable))com->OnDisable();
		if (com->CanExecute(ComponentAction::Destroy))com->OnDestroy();
		PostComponentAction(ComponentAction::RemoveComponent, false, true, com);
	}
}
void GameObject::OnTransformChanged()
{
	for (int i = 0; i < _addedComponents.Size(); ++i)
	{
		auto com = _addedComponents[i];
		if (com->IsEnable())
		{
			if (com->CanExecute(ComponentAction::TransformChanged))com->OnTransformChanged();
		}
	}

	for (int i = 0; i < _components.Size(); ++i)
	{
		auto com = _components[i];
		if (com->IsEnable())
		{
			if (com->CanExecute(ComponentAction::TransformChanged))com->OnTransformChanged();
		}
	}
}
void GameObject::SetLayer(int layer)
{
	_layer = layer;
}
bool GameObject::ActiveInHierarchy()const
{
	if (!_isActiveSelf)return false;
	if (_transform != nullptr && _transform->GetParent() != nullptr)
		return _transform->GetParent()->GetGameObject()->ActiveInHierarchy();
	return true;
}
void GameObject::SetActive(bool active)
{
	if (_isActiveSelf == active)return;
	_isActiveSelf = active;

	bool parent_active = active;
	if (_transform->GetParent() != nullptr)
		parent_active = _transform->GetParent()->GetGameObject()->ActiveInHierarchy();
	if (parent_active)
	{//只有父级以上都可见时，才执行
		PostComponentAction(active ? ComponentAction::Enable : ComponentAction::Disable, false, true);
	}
}
void GameObject::PostComponentAction(ComponentAction type, bool removed, bool child, void* data)
{
	Vector<Component*> list = _components;
	list.AddRange(_addedComponents);
	if (removed)list.AddRange(_removedComponents);
	for (int i = 0; i < list.Size(); ++i)
	{
		auto com = list[i];
		if (com->IsEnable() && com->CanExecute(type))
		{
			switch (type)
			{
			case ComponentAction::Start: com->Start(); break;
			case ComponentAction::Enable: com->OnEnable(); break;
			case ComponentAction::Disable: com->OnDisable(); break;
			case ComponentAction::TransformChanged: com->OnTransformChanged(); break;
			case ComponentAction::CollisionEnter: com->OnCollisionEnter((Collision*)data); break;
			case ComponentAction::CollisionStay: com->OnCollisionStay((Collision*)data); break;
			case ComponentAction::CollisionExit: com->OnCollisionExit((Collision*)data); break;
			case ComponentAction::TriggerEnter: com->OnTriggerEnter(); break;
			case ComponentAction::TriggerStay: com->OnTriggerStay(); break;
			case ComponentAction::TriggerExit: com->OnTriggerExit(); break;
			case ComponentAction::CollisionEnter2D: com->OnCollisionEnter2D((Collision2D*)data); break;
			case ComponentAction::CollisionStay2D: com->OnCollisionStay2D((Collision2D*)data); break;
			case ComponentAction::CollisionExit2D: com->OnCollisionExit2D((Collision2D*)data); break;
			case ComponentAction::TriggerEnter2D: com->OnTriggerEnter2D(); break;
			case ComponentAction::TriggerStay2D: com->OnTriggerStay2D(); break;
			case ComponentAction::TriggerExit2D: com->OnTriggerExit2D(); break;
			case ComponentAction::LateUpdate: com->LateUpdate(); break;
			case ComponentAction::PreRender: com->OnPreRender((Camera*)data); break;
			case ComponentAction::DrawGizmos: com->OnDrawGizmos((Camera*)data); break;
			case ComponentAction::PostRender: com->OnPostRender((Camera*)data); break;
			case ComponentAction::GUI: com->OnGUI(); break;
			case ComponentAction::UISize: com->OnUISize(); break;
			case ComponentAction::Destroy: com->OnDestroy(); break;
			case ComponentAction::ApplicationFocus: com->OnApplicationFocus(); break;
			case ComponentAction::ApplicationPause: com->OnApplicationPause(); break;
			case ComponentAction::ApplicationQuit: com->OnApplicationQuit(); break;
			case ComponentAction::AddComponent: com->OnAddComponent((Component*)data); break;
			case ComponentAction::RemoveComponent: com->OnRemoveComponent((Component*)data); break;
			default:break;
			}
		}
	}
	if (child && this->GetTransform())
	{
		int childCount = this->GetTransform()->GetChildCount();
		for (int i = 0; i < childCount; ++i)
		{
			auto child = this->GetTransform()->GetChild(i);
			if (child && child->GetGameObject()->ActiveSelf())
			{//对于子节点，只需要自身激活中就执行
				child->GetGameObject()->PostComponentAction(type, removed, child);
			}
		}
	}
}
void GameObject::SendMessage(const String& key, const String& value)
{
	Vector<Component*> list = _components;
	list.AddRange(_addedComponents);
	for (int i = 0; i < list.Size(); ++i)
	{
		auto com = list[i];
		if (com->IsEnable())
		{
			com->OnMessage(key, value);
		}
	}
}
void GameObject::SendMessageUpwards(const String& key, const String& value)
{
	SendMessage(key, value);
	if (_transform->GetParent() != nullptr)
		_transform->GetParent()->GetGameObject()->SendMessageUpwards(key, value);
}
void GameObject::SendMessageDownward(const String& key, const String& value)
{
	SendMessage(key, value);
	int childCount = this->GetTransform()->GetChildCount();
	for (int i = 0; i < childCount; ++i)
	{
		auto child = this->GetTransform()->GetChild(i);
		if (child->GetGameObject()->ActiveSelf())
		{
			child->GetGameObject()->SendMessageDownward(key, value);
		}
	}
}
void GameObject::Update()
{
	//添加
	if (_addedComponents.Size() > 0)
	{
		Vector<Component*> added = _addedComponents;
		_addedComponents.Clear();
		for (int i = 0; i < added.Size(); ++i)
		{
			_components.Add(added[i]);
		}
	}

	if (this->ActiveInHierarchy())
	{
		//Start
		for (int i = 0; i < _components.Size(); ++i)
		{
			auto com = _components[i];
			if (com->IsEnable() && com->CanExecute(ComponentAction::Start))
			{
				if (!com->_isStart)
				{
					com->_isStart = true;
					com->Start();
				}
			}
		}

		//Update
		for (int i = 0; i < _components.Size(); ++i)
		{
			auto com = _components[i];
			if (com->IsEnable() && com->CanExecute(ComponentAction::Update))
			{
				com->Update();
			}
		}
	}

	//移除
	if(_removedComponents.Size() > 0)
	{
		Vector<Component*> removed = _removedComponents;
		_removedComponents.Clear();
		for (int i = 0; i < removed.Size(); ++i)
		{
			auto com = removed[i];
			_components.Remove(com);
			SAFE_DELETE(com);
		}
	}
}
Tween* GameObject::DOBlink(float time, int count, bool defaultVisible)
{
	return BlinkTween::Create(this, time, count, defaultVisible);
}
Tween* GameObject::DOVisible(bool visible, float time)
{
	return VisibleTween::Create(this, visible, time);
}

/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(GameObject);
template<class TransferFunction> inline
void GameObject::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_ENUM(_flags);
	TRANSFER_SIMPLE(_layer);
	TRANSFER_SIMPLE(_isActiveSelf);

	if (transfer.IsRead())
	{
		//组件
		{
			Vector<String> components;
			{//先读取包括哪些组件
				if (transfer.Push("Components"))
				{
					TRANSFER_VECTOR(components, "Component");
					transfer.Pop();
				}
			}
			//再依次添加组件
			for (int i = 0; i < components.Size(); ++i)
			{
				Component* component = nullptr;
				const String& com_name = components[i];
				if (com_name == "Transform" || com_name == "RectTransform")
				{
					component = _transform;//Transform默认在创建GameObject时添加
				}
				else
				{
					component = AddComponent(com_name);
				}
				if (!component)
				{
					Debuger::Error("Cannot Add Component:%s", com_name.c_str());
					continue;
				}
				if (transfer.Push(component->GetClassId().c_str()))
				{
					component->Serialize(transfer, ptr);
					transfer.Pop();
				}
			}
		}

		//子
		{
			Vector<String> childrens;
			{//先读取包括哪些组件
				if (transfer.Push("Childrens"))
				{
					TRANSFER_VECTOR(childrens, "Children");
					transfer.Pop();
				}
			}

			//再依次添加组件
			for (int i = 0; i < childrens.Size(); ++i)
			{
				if (transfer.Push("GameObject", i))
				{
					bool is_ui_object = false;
					{//判断是否UI
						Vector<String> components;
						{//先读取包括哪些组件
							if (transfer.Push("Components"))
							{
								TRANSFER_VECTOR(components, "Component");
								transfer.Pop();
							}
						}
						for (int i = 0; i < components.Size(); ++i)
						{
							const String& com_name = components[i];
							if (com_name == "Transform" || com_name == "RectTransform")
							{
								if (com_name == "RectTransform")
								{
									is_ui_object = true;
									break;
								}
							}
						}
					}

					GameObject* object = nullptr;
					const String& child_name = childrens[i];
					if (is_ui_object)
						object = GameObject::InstantiationUI(child_name, _transform);
					else
						object = GameObject::Instantiation(child_name, _transform);

					object->Transfer(transfer, ptr);
					transfer.Pop();
				}
			}
		}
	}
	else
	{
		//组件
		{
			{//先写入组件类型
				Vector<String> components;
				for (int i = 0; i < _components.Size(); ++i)
				{
					if(!_components[i]->CanSerialize())continue;
					String type = _components[i]->GetType()->GetName();
					components.Add(type);
				}
				if (transfer.Push("Components"))
				{
					TRANSFER_VECTOR(components, "Component");
					transfer.Pop();
				}
			}
			//再写入细节，方便读取
			for (int i = 0; i < _components.Size(); ++i)
			{
				Component* component = _components[i];
				if (!_components[i]->CanSerialize())continue;
				if (transfer.Push(component->GetClassId().c_str()))
				{
					component->Deserialize(transfer, ptr);
					transfer.Pop();
				}
			}
		}

		//子
		{
			{//先写入名称

				Vector<String> childrens;
				for (int i = 0; i < _transform->GetChildCount(); ++i)
				{
					GameObject* child_object = _transform->GetChild(i)->GetGameObject();
					String name = child_object->GetInstanceName();
					childrens.Add(name);
				}

				if (transfer.Push("Childrens"))
				{
					TRANSFER_VECTOR(childrens, "Children");
					transfer.Pop();
				}
			}
			for (int i = 0; i < _transform->GetChildCount(); ++i)
			{
				GameObject* child_object = _transform->GetChild(i)->GetGameObject();
				if (transfer.Push("GameObject"))
				{
					child_object->Transfer(transfer, ptr);
					transfer.Pop();
				}
			}
		}
	}
}
DC_END_NAMESPACE