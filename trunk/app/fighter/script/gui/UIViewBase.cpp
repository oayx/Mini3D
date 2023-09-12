#include "UIViewBase.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
void UIViewBase::SetVisible(bool b)
{
	GetGameObject()->SetActive(b);
}
void UIViewBase::OnEnable()
{
	this->RegisterEvent();
}
void UIViewBase::OnDisable()
{
	this->UnRegisterEvent();
}
DC_END_NAMESPACE
