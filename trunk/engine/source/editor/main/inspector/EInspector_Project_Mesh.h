 
/*****************************************************************************
* Author： hannibal
* Date：2021/8/21
* Description：属性-资源面板-模型
*****************************************************************************/
#pragma once

#include "EInspector_Project.h"

DC_BEGIN_NAMESPACE
class MeshMeta;
/********************************************************************/
class EInspector_Project_Mesh : public EInspector_Project_Base
{
	typedef EInspector_Project_Base base;
public:
	virtual void Render(AssetMeta* meta)override;

private:
	void RenderGUI(MeshMeta* meta);
	void RenderModal(MeshMeta* meta);

private:
	MeshMeta _meshMeta;
};
DC_END_NAMESPACE