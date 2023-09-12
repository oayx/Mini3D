 
/*****************************************************************************
* Author： hannibal
* Date：2021/9/8
* Description：属性-资源面板-字体
*****************************************************************************/
#pragma once

#include "EInspector_Project.h"

DC_BEGIN_NAMESPACE
class FontMeta;
/********************************************************************/
class EInspector_Project_Font : public EInspector_Project_Base
{
	typedef EInspector_Project_Base base;
public:
	virtual void Render(AssetMeta* meta)override;

private:
	void RenderGUI(FontMeta* meta);
	void RenderModal(FontMeta* meta);

private:
	FontMeta _fontMeta;
};
DC_END_NAMESPACE