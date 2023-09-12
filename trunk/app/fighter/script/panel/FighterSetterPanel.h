 
/*****************************************************************************
* Author： hannibal
* Date：2020/9/12
* Description：
*****************************************************************************/
#pragma once

#include "gui/UIViewBase.h"
#include "def/fighter_def.h"

DC_BEGIN_NAMESPACE
class UIImage;
/********************************************************************/
class FighterSetterPanel : public UIViewBase
{
	friend class GameObject;
	typedef UIViewBase base;
protected:
	virtual void Awake()override;
	virtual void RegisterEvent()override;
	virtual void UnRegisterEvent()override;
};
DC_END_NAMESPACE
