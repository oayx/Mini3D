 
/*****************************************************************************
* Author： hannibal
* Date：2020/9/12
* Description：
*****************************************************************************/
#pragma once

#include "runtime/component/GameObject.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class UIViewBase : public Component
{
public:
	virtual void SetVisible(bool b);

protected:
	virtual void OnEnable()override;
	virtual void OnDisable()override;

	virtual void RegisterEvent() = 0;
	virtual void UnRegisterEvent() = 0;
};
DC_END_NAMESPACE
