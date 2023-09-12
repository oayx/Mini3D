
/*****************************************************************************
* Author： hannibal 
* Date：2009/11/20
* Description： 资源基类
*****************************************************************************/
#pragma once

#include "ResourcesDefine.h"

DC_BEGIN_NAMESPACE
class ResourcesManager;
/********************************************************************/
// ClassName：Resource
// Description：资源基类
class ENGINE_DLL Resource : public Object
{
	friend class ResourcesManager;
	DECLARE_OBJECT_CLONE;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Resource);
	BEGIN_DERIVED_REFECTION_TYPE(Resource, Object)
	END_DERIVED_REFECTION_TYPE;

protected:
	Resource(const ResourceType &type);
	virtual bool   LoadFromFile(const String& file) { return false; }

public:
	virtual ~Resource();
	static String	GetInternalDataPath();
	static String	GetFullDataPath(const String& file);
	static String	GetFullSavePath(const String& file);
	static String	GetRelativePath(const String& file);	//相对assets的目录

	static ResourceType GetResourceType(const String& file);
	static bool		IsInternal(const String& file) { return file.StartsWith("internal"); }

public:
	bool			Load(const String& file);

	const String&	GetResName()const { return _resName; }
	void			SetResName(const String& name) { _resName = name; }
	const String&	GetResFile()const { return _resFile; }
	void			SetResFile(const String& file);
	ResourceType	GetResType()const { return _type; }

protected:
	String			_resName = "";						//资源的唯一名称
	String			_resFile = "";						//资源文件
	ResourceType	_type = ResourceType::Undefined;	//资源类型

	static String	_internalPath;
};//Resource
DC_END_NAMESPACE
