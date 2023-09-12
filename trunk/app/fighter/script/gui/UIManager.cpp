#include "UIManager.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
UIManager* UIManager::_instance = nullptr;
UIManager* UIManager::Instance()
{
	if (_instance == nullptr)
		_instance = UIManager::Create();
	return _instance;
}
GameObject* UIManager::GetUIRoot()
{
	if (_uIRootNode == nullptr)
		_uIRootNode = GameObject::Find("UIRoot");
	return _uIRootNode;
} 
UIViewBase* UIManager::GetPanel(const String& name)const
{
	auto it = _allPanels.Find(name);
	if (it == _allPanels.end())
		return nullptr;
	return it->second;
}
void UIManager::RegisterEvent()
{
}
void UIManager::UnRegisterEvent()
{
}
DC_END_NAMESPACE