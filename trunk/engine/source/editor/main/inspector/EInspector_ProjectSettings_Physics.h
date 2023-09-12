 
/*****************************************************************************
* Author： hannibal
* Date：2021/9/21
* Description：物理
*****************************************************************************/
#pragma once

#include "EInspector_Base.h"
#include "editor/EWindowBase.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class EInspector_ProjectSettings_Physics : public EInspector_Base
{
public:
	virtual void Render()override;

private:
	void RenderLayer();
	void RenderTag();
};
DC_END_NAMESPACE