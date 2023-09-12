 
/*****************************************************************************
* Author： hannibal
* Date：2021/8/21
* Description：属性-资源面板-纹理
*****************************************************************************/
#pragma once

#include "EInspector_Project.h"

DC_BEGIN_NAMESPACE
class TextureMeta;
/********************************************************************/
class EInspector_Project_Texture : public EInspector_Project_Base
{
	typedef EInspector_Project_Base base;
public:
	virtual void Render(AssetMeta* meta)override;

private:
	void RenderGUI(TextureMeta* meta);
	void RenderModal(TextureMeta* meta);

private:
	TextureMeta _textureMeta;
};
DC_END_NAMESPACE