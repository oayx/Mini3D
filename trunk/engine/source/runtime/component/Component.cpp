#include "Component.h"
#include "GameObject.h"
#include "Component.inl"
#include "runtime/Application.h"
#include "editor/EditorDefine.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Component, Object);
uint64 Component::GetObjectInstanceId() const
{
	return _gameObject->GetInstanceId();
}
const String& Component::GetInstanceName() const
{ 
	return _gameObject->GetInstanceName(); 
}
Transform* Component::GetTransform() const
{
	return GetGameObject()->GetTransform();
}
void Component::SetEnable(bool enable)
{
	if (_enable != enable)
	{
		_enable = enable;
		if (enable)
			this->OnEnable();
		else
			this->OnDisable();
	}
}
bool Component::CanExecute(ComponentAction type) 
{
	if (!Application::IsEditor())return true;
	return (Application::IsPlay() || (GetEditorAction() & uint64(type))) && !Application::IsPause();
}
Object* Component::Clone(Object* newObj)
{
	Component* obj = dynamic_cast<Component*>(newObj);
	if (!obj)return newObj;

	obj->SetEnable(_enable);

	return obj;
}
void Component::OnDrawEditor()
{
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(Component);
template<class TransferFunction> inline
void Component::Transfer(TransferFunction& transfer, void* ptr)
{
	TRANSFER_SIMPLE(_enable);
}
DC_END_NAMESPACE
