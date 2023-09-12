 
/*****************************************************************************
* Author： hannibal
* Date：2021/8/21
* Description：属性-资源面板-文本
*****************************************************************************/
#pragma once

#include "EInspector_Project.h"

DC_BEGIN_NAMESPACE
class Material;
class TextureMeta;
struct ShaderGroup;
/********************************************************************/
class EInspector_Project_Material : public EInspector_Project_Base
{
	typedef EInspector_Project_Base base;

public:
	virtual void OnEnter()override;
	virtual void OnLeave()override;
	virtual void Render(AssetMeta* meta)override;

	static bool RenderMaterial(Material* material, Action action);

	static void ShowAddShader(std::function<void(String)> action);
	static void ShowAddShader(const ShaderGroup* parent_group, std::function<void(String)> action);

private:
	Material* _material = nullptr;
};
DC_END_NAMESPACE