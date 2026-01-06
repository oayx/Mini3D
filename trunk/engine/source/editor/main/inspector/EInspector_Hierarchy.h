 
/*****************************************************************************
* Author： hannibal
* Date：2021/8/10
* Description：属性-层级面板
*****************************************************************************/
#pragma once

#include "EInspector_Base.h"

DC_BEGIN_NAMESPACE
class Type;
class GameObject;
/********************************************************************/
class EInspector_Hierarchy : public EInspector_Base
{
public:
	virtual void Render()override;

private:
	//标题
	void ShowHeader(GameObject* select_obj);
	//Transform
	void ShowTransform(GameObject* select_obj);

private://组件
	void ShowComponent(GameObject* select_obj);
	//增加组件按钮
	void ShowAddComponent(GameObject* select_obj);
	void ShowSearchAddComponent(std::function<void(String)> action, const EComponents& parentComponent, const char* filter);

private:
};
DC_END_NAMESPACE