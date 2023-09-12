
/*****************************************************************************
* Author： hannibal
* Description：单个项目
*****************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class Project : public Object
{
	friend class ProjectManager;
	DEFAULT_CREATE(Project);
	FRIEND_CONSTRUCT_DESTRUCT(Project);
	DISALLOW_COPY_ASSIGN(Project);
	DECLARE_OBJECT_SERIALIZE(Project);
	BEGIN_DERIVED_REFECTION_TYPE(Project, Object)
	END_DERIVED_REFECTION_TYPE;

	Project(const String& path) :_fullPath(path) {}
	~Project() { this->Close(); }

	virtual void Serialize()override;
	virtual void Deserialize()override;

public:
	bool Open();
	void Close();
	bool Create();

	void SetCurrScene(const String& scene_file) { _currScene = scene_file; }

	bool IsOpen()const { return _isOpen; }
	const String& GetFullPath()const { return _fullPath; }

private:
	bool	_isOpen = false;
	String	_fullPath = "";
	String	_currScene = "";
};
DC_END_NAMESPACE