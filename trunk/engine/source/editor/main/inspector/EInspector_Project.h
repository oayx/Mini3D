 
/*****************************************************************************
* Author： hannibal
* Date：2021/8/21
* Description：属性-资源面板
*****************************************************************************/
#pragma once

#include "EInspector_Base.h"
#include "runtime/resources/AssetMeta.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class AssetMeta;
class EInspector_Project_Base;
class EInspector_Project : public EInspector_Base
{
	friend class EMain_Inspector;
	FRIEND_CONSTRUCT_DESTRUCT(EInspector_Project);
	EInspector_Project();
	~EInspector_Project();

public:
	virtual void Render()override;

private:
	String _currAssetFile = "";
	EInspector_Project_Base* _currInspector = nullptr;
	EInspector_Project_Base* _inspectors[(int)ResourceType::Max] = { nullptr };
};

/********************************************************************/
class EInspector_Project_Base : public EInspector_Base
{
public:
	virtual void Render()override {}
	virtual void Render(AssetMeta* meta);

protected:
	String _assetGUID = "";
};
DC_END_NAMESPACE