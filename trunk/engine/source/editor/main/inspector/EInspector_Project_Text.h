 
/*****************************************************************************
* Author： hannibal
* Date：2021/8/21
* Description：属性-资源面板-文本
*****************************************************************************/
#pragma once

#include "EInspector_Project.h"

DC_BEGIN_NAMESPACE
class TextureMeta;
/********************************************************************/
class EInspector_Project_Text : public EInspector_Project_Base
{
	typedef EInspector_Project_Base base;

public:
	virtual void OnEnter()override;
	virtual void Render(AssetMeta* meta)override;

private:
	String _content = "";
};
DC_END_NAMESPACE