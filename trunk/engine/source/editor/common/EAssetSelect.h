 
/*****************************************************************************
* Author： hannibal
* Date：2021/8/27
* Description：资源选择
*****************************************************************************/
#pragma once

#include "editor/EWindowBase.h"
#include "runtime/resources/ResourcesDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class AssetMeta;
class EAssetSelect : public EWindowBase
{
	friend class EditorMain;

	static void Initialize();
	static void Destroy();

public:
	static void Render();
	static void Search(ResourceType type, std::function<void(const String&)> callback);

private:
	static bool IsShow;
	static ResourceType _resType;
	static Vector<AssetMeta*> _reslist;
	static std::function<void(const String&)> _callback;
};
DC_END_NAMESPACE