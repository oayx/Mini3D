
/*****************************************************************************
* Author： hannibal
* Date：2020/9/12
* Description：
*****************************************************************************/
#pragma once

#include "UIViewBase.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class UIManager : public Object
{
	typedef Map<String, UIViewBase*> Panels;
	DEFAULT_CREATE(UIManager);
	FRIEND_CONSTRUCT_DESTRUCT(UIManager);
public:
	static UIManager* Instance();

	template<class T>
	void Show();

	template<class T>
	void Close();

	template<class T>
	String GetName();

	GameObject* GetUIRoot();
	UIViewBase* GetPanel(const String& name)const;
	
private:
	void RegisterEvent();
	void UnRegisterEvent();

private:
	static UIManager* _instance;
	GameObject*	_uIRootNode = nullptr;
	Panels		_allPanels;
};
template<class T>
void UIManager::Show()
{
	String name = GetName<T>();
	Debuger::Log("open panel %s", name.c_str());

	UIViewBase* panel = GetPanel(name);
	if (panel != nullptr)
	{
		panel->SetVisible(true);
		return;
	}

	GameObject* obj = GameObject::InstantiationUI("FighterMainPanel");
	obj->GetTransform()->SetParent(GetUIRoot()->GetTransform(), false);
	UIViewBase* component = (UIViewBase*)obj->AddComponent<T>();
	AssertEx(_allPanels.Add(name, component),"");
}
template<class T>
void UIManager::Close()
{
	String name = GetName<T>();
	Debuger::Log("close panel %s", name.c_str());

	UIViewBase* panel = GetPanel(name);
	if (panel != nullptr)
	{
		_allPanels.Remove(_allPanels.Find(name));
		GameObject::Destroy(panel->GetGameObject());
	}
} 
template<class T>
String UIManager::GetName()
{
	String name = typeid(T).name();
	return name;
}
DC_END_NAMESPACE
