 
/*****************************************************************************
* Author： hannibal
* Date：2016/3/21
* Description：
*****************************************************************************/
#pragma once

#include "engine_header.h"

DC_BEGIN_NAMESPACE
class Transform;
class TestPanel;
/********************************************************************/
class TestMain : Object
{
public:
	bool Start();
	void Release();

public:
	void SetFocusNode(Transform* node) { _pFocusNode = node; }

private://立即模式
	void RegisterEvent();
	void UnRegisterEvent();
	void Update();

private:
	TestPanel* _testPanel = nullptr;
	Transform* _pFocusNode = nullptr;
	GameObject* _rootObject = nullptr;
};
DC_END_NAMESPACE
